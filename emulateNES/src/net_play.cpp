#include "net_play.h"
#include <QHostInfo>
#include <QMessageBox>
#include <QDebug>
#include <QMetaObject>
#include <QDataStream>

NetPlay::NetPlay()
{
    server.reset(new QTcpServer());
    socket.reset(new QTcpSocket());

    connect(server.get(), &QTcpServer::newConnection, this, &NetPlay::onNewConnection);
    connect(socket.get(), &QTcpSocket::readyRead,    this, &NetPlay::onReadyRead);
    connect(socket.get(), &QTcpSocket::connected,    this, &NetPlay::onConnected);
    connect(socket.get(), &QTcpSocket::disconnected, this, &NetPlay::onDisconnected);
}

void NetPlay::connecting(const std::string& host, quint16 _port)
{
    if (is_connect)
    {
        socket->disconnectFromHost();
        socket->close();
        server->close();
        is_connect = false;
    }

    recvBuffer.clear();
    port = _port;

    if (firstPlayer)
    {
        server->listen(QHostAddress::Any, port);
        emit connected(true);
    }
    else
    {
        socket->connectToHost(QString::fromStdString(host), port);
    }
}

void NetPlay::onNewConnection()
{
    while (server->hasPendingConnections())
    {
        auto client = server->nextPendingConnection();
        socket.reset(client);

        socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);

        connect(socket.get(), &QTcpSocket::readyRead,    this, &NetPlay::onReadyRead);
        connect(socket.get(), &QTcpSocket::disconnected, this, &NetPlay::onDisconnected);

        is_connect = true;
        emit connected(true);
    }
}

void NetPlay::onConnected()
{
    socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);

    is_connect = true;
    emit connected(is_connect);

    QByteArray hello = "ready" + QHostInfo::localHostName().toUtf8();
    sendMessage(hello);          // теперь сообщение тоже проходит через framing
}

void NetPlay::onDisconnected()
{
    is_connect = false;
}

// ------------------ отправка ------------------

void NetPlay::sendMessage(const QByteArray& payload)
{
    if (socket->state() != QAbstractSocket::ConnectedState)
        return;

    // length-prefix: 4 байта размера полезной нагрузки
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << static_cast<quint32>(payload.size());
    packet.append(payload);

    QTcpSocket* sock = socket.get();
    QMetaObject::invokeMethod(sock, [sock, packet]()
                              {
                                  if (sock->state() != QAbstractSocket::ConnectedState)
                                      return;
                                  sock->write(packet);
                                  sock->flush();
                              }, Qt::QueuedConnection);
}

void NetPlay::writeDatagram(const Data& d)
{
    QByteArray payload;
    QDataStream out(&payload, QIODevice::WriteOnly);
    out << d;
    sendMessage(payload);
}

// ------------------ приём ------------------

void NetPlay::onReadyRead()
{
    recvBuffer.push_back(socket->readAll());
    processMessages();   // НЕ очищаем recvBuffer здесь!
}

void NetPlay::processMessages()
{
    constexpr int kHeaderSize = sizeof(quint32);

    while (true)
    {
        if (recvBuffer.size() < kHeaderSize)
            return;

        QDataStream in(recvBuffer);
        quint32 msgSize = 0;
        in >> msgSize;

        if (static_cast<quint32>(recvBuffer.size()) < kHeaderSize + msgSize)
            return; // ещё не всё пришло

        QByteArray payload = recvBuffer.mid(kHeaderSize, msgSize);
        recvBuffer.remove(0, kHeaderSize + msgSize);

        // --- служебное сообщение "ready" ---
        if (payload.startsWith("ready"))
        {
            QMessageBox box(QMessageBox::Icon::Information, "info",
                            QString("Подключился пользователь %1")
                                .arg(QString::fromUtf8(payload.mid(5))),
                            QMessageBox::StandardButton::Ok);
            box.exec();

            emit selectGameForNet();
            continue;
        }

        // --- обычный Data ---
        QDataStream payloadIn(payload);
        Data d;
        payloadIn >> d;

#ifdef DEBUG_ON
        qDebug() << "Получено:" << d.controller << "frame:" << d.frame
                 << "startGame size:" << d.startGame.size();
#endif

        if (!d.startGame.isEmpty())
        {
            if (d.startGame.startsWith("load_sucsess") ||
                d.startGame.startsWith("load_fail"))
            {
                if (load_sucsess_callback)
                {
                    auto cb = load_sucsess_callback;   // снимок, чтобы избежать гонок
                    load_sucsess_callback = nullptr;
                    cb();
                }
            }
            else if (d.startGame.startsWith("load"))
            {
                max_frame_now = 0;
                {
                    std::lock_guard<std::mutex> lock(map_mutex);
                    data.clear();
                    localData.clear();
                }
                if (load_callback)
                    load_callback(d.startGame.mid(4));
            }
            else
            {
                max_frame_now = 0;
                emit startGameForNet(d.startGame);
            }
        }
        else
        {
            max_frame_now = d.frame;
            std::lock_guard<std::mutex> lock(map_mutex);
            data[d.frame] = d;
        }
    }
}

// ------------------ доступ к буферам ------------------

std::optional<Data> NetPlay::getData(uint64_t frame)
{
    std::lock_guard<std::mutex> lock(map_mutex);
    auto it = data.find(frame);
    if (it != data.end())
    {
        auto res = std::optional<Data>(it->second);
        data.erase(it);
        return res;
    }
    return std::nullopt;
}

std::optional<uint8_t> NetPlay::getLocalData(uint64_t frame)
{
    std::lock_guard<std::mutex> lock(map_mutex);
    auto it = localData.find(frame);
    if (it != localData.end())
    {
        auto res = std::optional<uint8_t>(it->second);
        localData.erase(it);
        return res;
    }
    return std::nullopt;
}

void NetPlay::setLocalData(uint64_t frame, uint8_t value)
{
    std::lock_guard<std::mutex> lock(map_mutex);
    localData[frame] = value;
}

void NetPlay::clearBuffers()
{
    std::lock_guard<std::mutex> lock(map_mutex);
    data.clear();
    localData.clear();
    // ВАЖНО: recvBuffer НЕ трогаем — его парсит processMessages()
    //         и его нельзя обнулять посреди цикла.
}
