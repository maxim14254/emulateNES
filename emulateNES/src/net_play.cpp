#include "net_play.h"
#include <QHostInfo>
#include <QMessageBox>
#include <QDebug>
#include <QMetaObject>


NetPlay::NetPlay()
{
    server.reset(new QTcpServer());
    socket.reset(new QTcpSocket());

    connect(server.get(), &QTcpServer::newConnection, this, &NetPlay::onNewConnection);
    connect(socket.get(), &QTcpSocket::readyRead, this, &NetPlay::onReadyRead);
    connect(socket.get(), &QTcpSocket::connected, this, &NetPlay::onConnected);
    connect(socket.get(), &QTcpSocket::disconnected, this, &NetPlay::onDisconnected);
}

void NetPlay::connecting(const std::string& host, quint16 _port)
{
    if(is_connect)
    {
        socket->disconnectFromHost();
        socket->close();
        server->close();
        is_connect = false;
    }

    recvBuffer.clear();
    port = _port;

    if(firstPlayer)
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
    while(server->hasPendingConnections())
    {
        auto client = server->nextPendingConnection();

        socket.reset(client);

        socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);

        connect(socket.get(), &QTcpSocket::readyRead, this, &NetPlay::onReadyRead);
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
    sendMessage(hello);
}

void NetPlay::onDisconnected()
{
    is_connect = false;
}

void NetPlay::sendMessage(const QByteArray& payload)
{
    if(socket->state() != QAbstractSocket::ConnectedState)
        return;

    QByteArray packet;
    packet.append(payload);

    QMetaObject::invokeMethod(socket.get(), [&, packet]()
    {
        socket->write(packet);
        socket->flush();

    }, Qt::QueuedConnection);
}

void NetPlay::writeDatagram(const Data &d)
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

    recvBuffer.clear();
}

void NetPlay::processMessages()
{
    if(recvBuffer.size() < 0)
        return;


    if(recvBuffer.startsWith("ready"))
    {
        QMessageBox box(QMessageBox::Icon::Information, "info", QString("Подключился пользователь %1").arg(QString::fromUtf8(recvBuffer.mid(5))), QMessageBox::StandardButton::Ok);
        box.exec();

        emit selectGameForNet();
    }
    else
    {
        QDataStream in(recvBuffer);

        Data d;
        in >> d;

#ifdef DEBUG_ON
        qDebug() << "Получено:" << d.controller << "frame:" << d.frame;
#endif

        if(d.startGame.size() > 0)
        {
            max_frame_now = 0;

            if(d.startGame.startsWith("load"))
            {
                clearBuffers();
                load_callback(d.startGame.mid(4));
            }
            else
                emit startGameForNet(d.startGame);
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
    if(it != data.end())
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
    if(it != localData.end())
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
    max_frame_now = 0;
    data.clear();
    localData.clear();
    recvBuffer.clear();
}
