#include "apu.h"
#include "qmath.h"
#include <cmath>


APU::APU(double freq, int sampleRate, QObject *parent) : QIODevice(parent), m_freq(freq), m_sampleRate(sampleRate)
{

}

qint64 APU::readData(char *data, qint64 maxlen)
{
    // 16-bit mono
    const int bytesPerSample = 2;
    qint64 samplesToWrite = maxlen / bytesPerSample;

    qint16 *out = reinterpret_cast<qint16*>(data);
    const double twoPi = 2.0 * M_PI;
    const double phaseInc = twoPi * m_freq / double(m_sampleRate);
    const double amp = 0.2;

    for (qint64 i = 0; i < samplesToWrite; ++i)
    {
        double v = std::sin(m_phase) * amp;
        out[i] = qint16(std::lrint(v * 32767.0));
        m_phase += phaseInc;

        if (m_phase >= twoPi)
            m_phase -= twoPi;
    }

    return samplesToWrite * bytesPerSample;
}
