#ifndef APU_H
#define APU_H

#include <QIODevice>



class APU : public QIODevice
{
    Q_OBJECT

public:
    explicit APU(double freq, int sampleRate, QObject *parent = nullptr);

    qint64 readData(char *data, qint64 maxlen) override;

    void start() { open(QIODevice::ReadOnly); }
    void stop()  { close(); }

    qint64 writeData(const char *, qint64) override { return 0; }
    qint64 bytesAvailable() const override { return 4096 + QIODevice::bytesAvailable(); }

private:
    double m_freq = 440.0;
    int m_sampleRate = 48000;
    double m_phase = 0.0;

};

#endif // APU_H
