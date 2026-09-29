#include "net_play.h"
#include <QNetworkDatagram>
#include <QtNetwork/qudpsocket.h>
#include <QHostInfo>
#include <QMessageBox>



NetPlay::NetPlay()
{
    socket.reset(new QUdpSocket());

    connect(socket.get(), &QUdpSocket::readyRead, this, &NetPlay::readyRead);
}

void NetPlay::writeDatagram(const Data &data)
{   
    QByteArray array;
    QDataStream out(&array, QIODevice::WriteOnly);

    out << data;

    socket->writeDatagram(array, Address, port);
}

void NetPlay::connecting(const std::string &host, quint16 _port)
{
    if(is_connect)
    {
        socket->close();
        is_connect = false;
    }

    port = _port;

    if(firstPlayer)
    {
        is_connect = socket->bind(QHostAddress::Any, port);
    }
    else
    {
        Address.setAddress(host.c_str());

        is_connect = socket->bind();

        QByteArray hello = QString("ready%1").arg(QHostInfo::localHostName()).toStdString().c_str();
        socket->writeDatagram(hello, Address, port);
    }


#ifdef DEBUG_ON
    if (!is_connect)
    {
        qDebug() << "Ошибка:" << socket->errorString();
        qDebug() << "Код ошибки:" << socket->error();
    }
#endif
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

void NetPlay::readyRead()
{
    QByteArray bytes = "";

    QHostAddress _sender;
    quint16 senderPort = 0;

    while (socket->hasPendingDatagrams())
    {
        bytes.resize(int(socket->pendingDatagramSize()));
        socket->readDatagram(bytes.data(), bytes.size(), &_sender, &senderPort);
    }

    if(bytes[0] == 'r' && bytes[1] == 'e' && bytes[2] == 'a'
        && bytes[3] == 'd' && bytes[4] == 'y')
    {
        QMessageBox box(QMessageBox::Icon::Information, "info", QString("Подключился пользователь %1").arg(bytes.mid(5, -1).toStdString().c_str()), QMessageBox::StandardButton::Ok);
        box.exec();

        Address = _sender;
        port = senderPort;

        emit selectGameForNet();
        return;
    }

    QDataStream in(bytes);

    Data d;
    in >> d;

#ifdef DEBUG_ON
        qDebug() << "Получено:" << d.controller
                 << "от" << _sender.toString()
                 << ":" << senderPort;
#endif

    if(d.startGame.count() > 0)
    {
        emit startGameForNet(d.startGame);
    }
    else
    {
        max_frame_now = d.frame;
        data[d.frame] = d;
    }
}
