#ifndef NET_PLAY_H
#define NET_PLAY_H

#include <memory>
#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <map>
#include <QDataStream>
#include <optional>
#include <mutex>
#include <atomic>

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

    void writeDatagram(const Data& data);   // имя оставил для совместимости с PPU

    void connecting(const std::string& host, quint16 port);
    bool isConnnection() { return is_connect; }

    std::optional<Data> getData(uint64_t frame);
    std::optional<uint8_t> getLocalData(uint64_t frame);
    void setLocalData(uint64_t frame, uint8_t value);

    bool isFirstPlayer() { return firstPlayer; }
    void set_player(bool val) { firstPlayer = val; }

    quint64 getMaxFrameNow() { return max_frame_now.load(); }

    void clearBuffers();

    void set_load_callback(std::function<void(QByteArray&& array)> fun) { load_callback = fun; };
    void set_load_sucsess_callback(std::function<void()> fun) { load_sucsess_callback = fun; };

private slots:
    void onNewConnection();
    void onConnected();
    void onReadyRead();
    void onDisconnected();

private:
    std::unique_ptr<QTcpServer> server;   // только у хоста
    std::unique_ptr<QTcpSocket> socket;   // у клиента — свой; у хоста — подключённый

    quint16 port = 0;
    QByteArray recvBuffer;   // накопитель для фрейминга

    std::map<uint64_t, Data> data;
    std::map<uint64_t, uint8_t> localData;

    std::atomic<bool> firstPlayer = true;
    std::atomic<bool> is_connect = false;
    std::atomic<quint64> max_frame_now{0};

    std::mutex map_mutex;

    std::function<void(QByteArray&& array)> load_callback;
    std::function<void()> load_sucsess_callback;

    void sendMessage(const QByteArray& payload);
    void processMessages();

signals:
    void startGameForNet(QByteArray&);
    void selectGameForNet();
    void connected(bool value);
};

#endif // NET_PLAY_H
