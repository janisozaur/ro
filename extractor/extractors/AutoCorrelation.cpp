#include "AutoCorrelation.h"
#include "../../common/CommonDefines.h"

#include <QImage>
#include <boost/multi_array.hpp>

#include <QDebug>

typedef boost::multi_array<float, 2> TwoDArray;

AutoCorrelation::AutoCorrelation()
{
}

bool AutoCorrelation::init(const QStringList &params)
{
    bool ok;
    if (params.size() < 1) {
        qCritical() << "this extractor requires param: size";
        return false;
    }
    const int size = params.at(0).toInt(&ok);
    if (!ok) {
        qCritical() << "cannot parse param 0:" << params.at(0) << "as int";
        return false;
    }
    bool powerOfTwo = false;
    int currentPower = 1;
    for (int i = 0; i <= size; i++) {
        if (currentPower == size) {
            powerOfTwo = true;
            break;
        }
        currentPower *= 2;
    }
    if (!powerOfTwo) {
        qCritical() << "fft requires its size to be power of two!" << size
                    << "is not a power of two";
        return false;
    }
    mSize = QSize(size, size);
    return true;
}

QString AutoCorrelation::name() const
{
    return "AutoCorrelation";
}

int AutoCorrelation::size() const
{
    return mSize.width();
}

QVector<float> AutoCorrelation::extract(const QImage &data, const int &x, const int &y) const
{
    const int w = mSize.width();
    const int h = mSize.height();
    const int dw = data.width();
    const int dh = data.height();
    ComplexArray *ca = new ComplexArray(boost::extents[w][h]);

    for (int ay = 0; ay < h; ay++) {
        int py = (y - h / 2 + ay) + dh;
        while (py >= dh) {
            py -= dh;
        }
        #ifdef HAS_IMAGE_CONSTSCANLINE
            const uchar *d = data.constScanLine(py);
        #else
            const uchar *d = data.scanLine(py);
        #endif
        for (int ax = 0; ax < w; ax++) {
            int px = (x - w / 2 + ax) + dw;
            while (px >= dw) {
                px -= dw;
            }
            (*ca)[ay][ax] = Complex(d[px], 0);
        }
    }
    perform(ca);

    float minm = 0;
    float maxm = 0;
    for (int j = 0; j < w; j++) {
        for (int k = 0; k < h; k++) {
            float magnitude = (*ca)[j][k].abs();
            if (magnitude > maxm) {
                maxm = magnitude;
            } else if (magnitude < minm) {
                minm = magnitude;
            }
        }
    }

    float c = 255.0 / log(1.0 + abs(maxm - minm));

    TwoDArray *img = new TwoDArray(boost::extents[w][h]);
    TwoDArray *flip = new TwoDArray(boost::extents[w][h]);

    for (int ay = 0; ay < h; ay++) {
        int py = (y - h / 2 + ay) + dh;
        while (py >= dh) {
            py -= dh;
        }
        #ifdef HAS_IMAGE_CONSTSCANLINE
            const uchar *d = data.constScanLine(py);
        #else
            const uchar *d = data.scanLine(py);
        #endif
        for (int ax = 0; ax < w; ax++) {
            int px = (x - w / 2 + ax) + dw;
            while (px >= dw) {
                px -= dw;
            }
            const float f = c * log(1.0 + (*ca)[ay][ax].abs());
            //const float f = d[px];
            (*img)[ay][ax] = f;
            (*flip)[ay][w - ax - 1] = f;
        }
    }

    QVector<float> result(h);
    float *resultPtr = result.data();
    for (int i = 0; i < h; i++) {
        for (int ay = 0; ay < h; ay++) {
            for (int ax = 0; ax < w; ax++) {
                const float f = float((*img)[ay][ax] * (*flip)[ay][(ax + i + w - 1) % w]) / float(65535);
                resultPtr[i] += f;
            }
        }
    }

    delete ca;
    delete img;
    delete flip;

    return result;
}

void AutoCorrelation::rearrange(QVector<Complex> &elements) const
{
    int target = 0;
    for (int position = 0; position < elements.count(); position++) {
        if (target > position) {
            Complex temp = elements.at(position);
            elements[position] = elements.at(target);
            elements[target] = temp;
        }

        unsigned int mask = elements.count();

        while (target & (mask >>= 1)) {
            target &= ~mask;
        }

        target |= mask;
    }
}

void AutoCorrelation::oneDFftH(ComplexArray *ca, int idx1, int idx2, bool inverse) const
{
    for (unsigned int j = 0; j < ca->shape()[idx2]; j++) {
        QVector<Complex> elements;
#ifdef HAS_VECTOR_RESERVE
        elements.reserve(ca->shape()[idx1]);
#endif
        for (unsigned int k = 0; k < ca->shape()[idx1]; k++) {
            elements << (*ca)[k][j];
        }
        rearrange(elements);
        transform(elements, inverse);
        for (unsigned int k = 0; k < ca->shape()[idx1]; k++) {
            (*ca)[k][j] = elements.at(k);
        }
    }
}

void AutoCorrelation::oneDFftV(ComplexArray *ca, int idx1, int idx2, bool inverse) const
{
    for (unsigned int j = 0; j < ca->shape()[idx2]; j++) {
        QVector<Complex> elements;
#ifdef HAS_VECTOR_RESERVE
        elements.reserve(ca->shape()[idx1]);
#endif
        for (unsigned int k = 0; k < ca->shape()[idx1]; k++) {
            elements << (*ca)[j][k];
        }
        rearrange(elements);
        transform(elements, inverse);
        for (unsigned int k = 0; k < ca->shape()[idx1]; k++) {
            (*ca)[j][k] = elements.at(k);
        }
    }
}

void AutoCorrelation::perform(ComplexArray *ca, bool inverse) const
{
    Q_ASSERT(ca->num_dimensions() == 2);
    oneDFftH(ca, 0, 1, inverse);
    oneDFftV(ca, 1, 0, inverse);
}

void AutoCorrelation::transform(QVector<Complex> &elements, bool inverse) const
{
    const unsigned int N = elements.count();
    const float pi = inverse ? M_PI : -M_PI;
    for (unsigned int step = 1; step < N; step <<= 1) {
        const unsigned int jump = step << 1;
        const float delta = pi / float(step);
        const float sine = sin(delta * .5);
        const Complex multiplier(-2. * sine * sine, sin(delta));
        Complex factor(1, 0);

        for (unsigned int group = 0; group < step; ++group) {
            for (unsigned int pair = group; pair < N; pair += jump) {
                const unsigned int match = pair + step;
                const Complex product(factor * elements.at(match));
                elements[match] = elements.at(pair) - product;
                elements[pair] += product;
            }
            factor = multiplier * factor + factor;
        }
    }
    if (inverse) {
        const float scale = 1.0 / elements.size();
        for (int i = 0; i < elements.size(); i++) {
            elements[i] *= scale;
        }
    }
}
