#ifndef NET_PLAY_H
#define NET_PLAY_H

#include <memory>
#include <QObject>
#include <QHostAddress>
#include <QUdpSocket>
#include <map>
#include <QDataStream>
#include <optional>
#include <mutex>


struct Data
{
    quint64 frame;
    uint8_t controller;
    QByteArray startGame;

    friend QDataStream &operator<<(QDataStream &out, const Data &d)
    {
        out << d.frame;
        out << d.controller;
        out << d.startGame;

        return out;
    }

    friend QDataStream &operator>>(QDataStream &in, Data &d)
    {
        in >> d.frame;
        in >> d.controller;
        in >> d.startGame;

        return in;
    }
};


class NetPlay : public QObject
{
    Q_OBJECT

public:
    NetPlay();

    void writeDatagram(const Data& data);

    void connecting(const std::string& host, quint16 port);
    bool isConnnection() { return is_connect; }

    std::optional<Data> getData(uint64_t frame);

    std::optional<uint8_t> getLocalData(uint64_t frame);
    void setLocalData(uint64_t frame, uint8_t value);

    bool isFirstPlayer() { return firstPlayer; }
    void set_player( bool val) { firstPlayer = val; }

    quint64 getMaxFrameNow() { return max_frame_now; }


private:
    std::unique_ptr<QUdpSocket> socket;
    quint16 port;
    QHostAddress hostAddress;

    std::map<uint64_t, Data> data;          // данные сетевого игрока
    std::map<uint64_t, uint8_t> localData;  // мои данные

    std::atomic<bool> firstPlayer = true;     // какой я игрок
    std::atomic<bool> is_connect = false;
    std::atomic<quint64> max_frame_now;

    std::mutex map_mutex;

    void readyRead();

signals:
    void startGameForNet(QByteArray&);
    void selectGameForNet();
};

#endif // NET_PLAY_H
