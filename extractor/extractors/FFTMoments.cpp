#include "FFTMoments.h"
#include "../../common/CommonDefines.h"
#include "Moments.h"

#include <QImage>

#include <QDebug>

// http://www.librow.com/articles/article-10

FFTMoments::FFTMoments()
{
}

FFTMoments::~FFTMoments()
{
}

bool FFTMoments::init(const QStringList &params)
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

// return average value of fft power iterating over r values
QVector<float> FFTMoments::extract(const QImage &data, const int &x, const int &y) const
{
    // the array created here might be bigger than image size - it has to be
    // a square of side length 2^n
    const int w = mSize.width();
    const int h = mSize.height();
    const int dw = data.width();
    const int dh = data.height();
    QVector<float> timeHistogram(256);
    ComplexArray *ca = new ComplexArray(boost::extents[w][h]);

    // fill only the data that exists in the image
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
            timeHistogram[d[px]]++;
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
    QVector<float> result;
    QVector<float> hist;
    const int maxR = w / 2;
#ifdef HAS_VECTOR_RESERVE
    result.reserve(size());
#endif
    for (int r = 1; r < maxR; r++) {
        int count = 0;
        float sum = 0;
        for (int i = 0; i < 360; i += 5) {
            const int x = r * cos(i) + maxR;
            const int y = r * sin(i) + maxR;
            float p = (*ca)[x][y].abs();
            p = c * log(1.0 + p);
            sum += p;
            count++;
        }
        hist.append(sum / count);
    }
    Moments m;
    {
        const float mean = m.mean(hist);
        const float wmean = m.weightedMean(hist);
        const float variance = m.variance(hist, mean);
        const float skewness = m.skewness(hist, mean, variance);
        const float kurtosis = m.kurtosis(hist, mean, variance);
        result << mean << wmean << variance << skewness << kurtosis;
    }
    {
        const float mean = m.mean(timeHistogram);
        const float wmean = m.weightedMean(timeHistogram);
        const float variance = m.variance(timeHistogram, mean);
        const float skewness = m.skewness(timeHistogram, mean, variance);
        const float kurtosis = m.kurtosis(timeHistogram, mean, variance);
        result << mean << wmean << variance << skewness << kurtosis;
    }

    delete ca;

    return result;
}

QString FFTMoments::name() const
{
    return "FFTMoments";
}

int FFTMoments::size() const
{
    return 10;
}

void FFTMoments::rearrange(QVector<Complex> &elements) const
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

void FFTMoments::oneDFftH(ComplexArray *ca, int idx1, int idx2, bool inverse) const
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

void FFTMoments::oneDFftV(ComplexArray *ca, int idx1, int idx2, bool inverse) const
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

void FFTMoments::perform(ComplexArray *ca, bool inverse) const
{
    Q_ASSERT(ca->num_dimensions() == 2);
    oneDFftH(ca, 0, 1, inverse);
    oneDFftV(ca, 1, 0, inverse);
}

void FFTMoments::transform(QVector<Complex> &elements, bool inverse) const
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
