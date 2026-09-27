#include "net_play.h"
#include <QNetworkDatagram>
#include <QtNetwork/qudpsocket.h>




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

    socket->writeDatagram(array, hostAddress, port);
}

void NetPlay::connecting(const std::string &host, quint16 port)
{
    is_connect = socket->bind(QHostAddress(host.c_str()), port);

#ifdef DEBUG_ON
    if (!is_connect)
    {
        qDebug() << "Ошибка:" << udp->errorString();
        qDebug() << "Код ошибки:" << udp->error();
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

    while (socket->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = socket->receiveDatagram();

        bytes += datagram.data();

#ifdef DEBUG_ON
        qDebug() << "Получено:" << datagram.data()
                 << "от" << datagram.senderAddress().toString()
                 << ":" << datagram.senderPort();
#endif

    }

    QDataStream in(bytes);

    Data d;

    in >> d;


    if(d.startGame.count() > 0)
    {
        emit startGameForNet(d.startGame);
    }
}
