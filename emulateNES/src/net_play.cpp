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

    Data d;
    d.header = "ready";
    d.controller = 0;
    d.frame = 0;
    d.data = QHostInfo::localHostName().toUtf8();

    writeDatagram(d);
}

void NetPlay::onDisconnected()
{
    is_connect = false;
}


void NetPlay::sendMessage(const QByteArray& payload)
{
    if (socket->state() != QAbstractSocket::ConnectedState)
        return;

    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);

    out << static_cast<quint32>(payload.size());

    packet.push_back(payload);

    QMetaObject::invokeMethod(socket.get(), [this, packet]()
    {
        socket->write(packet);
        socket->flush();

    }, Qt::QueuedConnection);
}

void NetPlay::writeDatagram(const Data& d)
{
    QByteArray payload;
    QDataStream out(&payload, QIODevice::WriteOnly);

    out << d;
    sendMessage(payload);
}


void NetPlay::onReadyRead()
{
    recvBuffer.push_back(socket->readAll());
    processMessages();
}

void NetPlay::processMessages()
{
    while (true)
    {
        if (recvBuffer.size() < 4)
            return;

        QDataStream in(recvBuffer);

        quint32 msgSize = 0;
        in >> msgSize;

        if (recvBuffer.size() < msgSize + 4)
            return;

        QByteArray payload = recvBuffer.mid(4, msgSize);
        recvBuffer.remove(0, 4 + msgSize);

        QDataStream payloadIn(payload);

        Data d;
        payloadIn >> d;

#ifdef DEBUG_ON
        qDebug() << "Получено: " << d.header << " " << d.controller << " frame: " << d.frame << " data: " << d.data;
#endif

        if (d.header == "ready")
        {
            QMessageBox box(QMessageBox::Icon::Information, "info", QString("Подключился пользователь %1").arg(QString::fromUtf8(d.data)), QMessageBox::StandardButton::Ok);
            box.exec();

            emit selectGameForNet();
            continue;
        }

        if (!d.header.isEmpty())
        {
            if (d.header == "load_sucsess" || d.header == "load_fail")
            {
                load_sucsess_callback();
            }
            else if (d.header == "load")
            {
                max_frame_now = 0;

                clearBuffers();

                load_callback(d.data);
            }
            else if(d.header == "new_game")
            {
                max_frame_now = 0;
                emit startGameForNet(d.data);
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
}
