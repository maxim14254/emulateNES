#include "apu.h"
#include "qmath.h"
#include <cmath>
#include "global.h"


APU::APU(double freq, int sampleRate, QObject *parent) : QIODevice(parent), m_freq(freq), m_sampleRate(sampleRate)
{

}

qint64 APU::readData(char *data, qint64 maxlen)
{
    // 16-bit mono
    int bytesPerSample = 2;
    qint64 samplesToWrite = maxlen / bytesPerSample;

    qint16 *out = reinterpret_cast<qint16*>(data);
    double twoPi = 2.0 * M_PI;
    double phaseInc = twoPi * m_freq / double(m_sampleRate);
    double amp = 0.2;
    double p = 0.5 * 2.0 * M_PI;

    for (qint64 i = 0; i < samplesToWrite; ++i)
    {
        double v1 = 0;
        double v2 = 0;
        for(int n = 1; n < harmonics; ++n)
        {
            v1 += std::sin(m_phase * n) / n;
            v2 += std::sin((m_phase - p) * n) / n;
        }

        double eee = (2.0 / M_PI) * (v1 - v2);
        eee *= amp;

        out[i] = qint16(std::lrint(eee * 32767.0));;
        m_phase += phaseInc;

        if (m_phase >= twoPi)
            m_phase -= twoPi;
    }

    return samplesToWrite * bytesPerSample;
}
