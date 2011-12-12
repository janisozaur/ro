#include "FFT.h"

#include <QImage>

#include "QDebug"

// http://www.librow.com/articles/article-10

FFT::FFT()
{
}

FFT::~FFT()
{
}

bool FFT::init(const QStringList &params)
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
QVector<float> FFT::extract(const QImage &data, const int &x, const int &y) const
{
    if (data.format() != QImage::Format_Indexed8) {
        qCritical("Image is not greyscale!");
    }

    // the array created here might be bigger than image size - it has to be
    // a square of side length 2^n
    const int w = mSize.width();
    const int h = mSize.height();
    ComplexArray *ca = new ComplexArray(boost::extents[w][h]);

    // fill only the data that exists in the image
    for (int ay = 0; ay < mSize.height(); ay++) {
        for (int ax = 0; ax < mSize.width(); ax++) {
            const int px = ((x - mSize.width() / 2 + ax) + data.width()) % data.width();
            const int py = ((y - mSize.height() / 2 + ay) + data.height()) % data.height();
            (*ca)[ay][ax] = Complex(data.pixelIndex(px, py), 0);
        }
    }
    perform(ca);

    qreal minm = 0;
    qreal maxm = 0;
    for (unsigned int j = 0; j < ca->shape()[0]; j++) {
        for (unsigned int k = 0; k < ca->shape()[1]; k++) {
            qreal magnitude = (*ca)[j][k].abs();
            if (magnitude > maxm) {
                maxm = magnitude;
            } else if (magnitude < minm) {
                minm = magnitude;
            }
        }
    }

    qreal c = 255.0 / log(1.0 + abs(maxm - minm));
    QVector<float> result;
    const int maxR = mSize.width() / 2;
    result.reserve(maxR);
    for (int r = 1; r < maxR; r++) {
        int count = 0;
        qreal sum = 0;
        for (int i = 0; i < 360; i += 5) {
            const int x = r * cos(i) + maxR;
            const int y = r * sin(i) + maxR;
            qreal p = (*ca)[x][y].abs();
            p = c * log(1.0 + p);
            sum += p;
            count++;
        }
        result.append(sum / count);
    }

    return result;
}

QString FFT::name() const
{
    return "FFT";
}

int FFT::size() const
{
    return mSize.width();
}

void FFT::rearrange(QVector<Complex> &elements) const
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

void FFT::oneDFftH(ComplexArray *ca, int idx1, int idx2, bool inverse) const
{
    for (unsigned int j = 0; j < ca->shape()[idx2]; j++) {
        QVector<Complex> elements;
        elements.reserve(ca->shape()[idx1]);
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

void FFT::oneDFftV(ComplexArray *ca, int idx1, int idx2, bool inverse) const
{
    for (unsigned int j = 0; j < ca->shape()[idx2]; j++) {
        QVector<Complex> elements;
        elements.reserve(ca->shape()[idx1]);
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

void FFT::perform(ComplexArray *ca, bool inverse) const
{
    Q_ASSERT(ca->num_dimensions() == 2);
    oneDFftH(ca, 0, 1, inverse);
    oneDFftV(ca, 1, 0, inverse);
}

void FFT::transform(QVector<Complex> &elements, bool inverse) const
{
    const unsigned int N = elements.count();
    const qreal pi = inverse ? M_PI : -M_PI;
    for (unsigned int step = 1; step < N; step <<= 1) {
        const unsigned int jump = step << 1;
        const qreal delta = pi / qreal(step);
        const qreal sine = sin(delta * .5);
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
        qreal scale = 1.0 / elements.size();
        for (int i = 0; i < elements.size(); i++) {
            elements[i] *= scale;
        }
    }
}

//const FFT::QImages FFT::complexToImages(const ComplexArray *ca, QImage::Format format) const
//{
//    QSize size(ca->shape()[1], ca->shape()[2]);
//    QImage phaseImage = QImage(size, format);
//    QImage magnitudeImage = QImage(size, format);
//    if (format == QImage::Format_Indexed8) {
//        QVector<QRgb> colors;
//        colors.reserve(256);
//        for (int i = 0; i < 256; i++) {
//            colors << qRgb(i, i, i);
//        }
//        phaseImage.setColorTable(colors);
//        magnitudeImage.setColorTable(colors);
//        phaseImage.fill(0);
//        magnitudeImage.fill(0);
//    } else {
//        phaseImage.fill(Qt::black);
//        magnitudeImage.fill(Qt::black);
//    }
//    for (unsigned int i = 0; i < ca->shape()[0]; i++) {
//        qreal minp = 0;
//        qreal maxp = 0;
//        qreal minm = 0;
//        qreal maxm = 0;
//        for (unsigned int j = 0; j < ca->shape()[1]; j++) {
//            for (unsigned int k = 0; k < ca->shape()[2]; k++) {
//                qreal phase = (*ca)[i][j][k].phase();
//                qreal magnitude = (*ca)[i][j][k].abs();
//                if (phase > maxp) {
//                    maxp = phase;
//                } else if (phase < minp) {
//                    minp = phase;
//                }
//                if (magnitude > maxm) {
//                    maxm = magnitude;
//                } else if (magnitude < minm) {
//                    minm = magnitude;
//                }
//            }
//        }

//        ColorParser cp(format);
//        for (unsigned int j = 0; j < ca->shape()[1]; j++) {
//            for (unsigned int k = 0; k < ca->shape()[2]; k++) {
//                qreal p = ((*ca)[i][j][k].phase() - minp) / (maxp - minp) * 255.0;
//                {
//                    QVector3D oldPixel = cp.pixel(k, j, phaseImage);
//                    QVector3D newPixel;
//                    switch (i) {
//                        case 0:
//                            newPixel.setX(p);
//                            break;
//                        case 1:
//                            newPixel.setY(p);
//                            break;
//                        case 2:
//                            newPixel.setZ(p);
//                            break;
//                        default:
//                            break;
//                    }
//                    cp.setPixel(k, j, phaseImage, cp.merge(oldPixel, newPixel));
//                }

//                // logarithmic scale
//                // implementaion: http://homepages.inf.ed.ac.uk/rbf/HIPR2/pixlog.htm
//                // idea: http://homepages.inf.ed.ac.uk/rbf/HIPR2/fourier.htm#guidelines
//                p = (*ca)[i][j][k].abs();
//                {
//                    qreal c = 255.0 / log(1.0 + abs(maxm - minm));
//                    p = c * log(1.0 + p);
//                    QVector3D oldPixel = cp.pixel(k, j, magnitudeImage);
//                    QVector3D newPixel;
//                    switch (i) {
//                        case 0:
//                            newPixel.setX(p);
//                            break;
//                        case 1:
//                            newPixel.setY(p);
//                            break;
//                        case 2:
//                            newPixel.setZ(p);
//                            break;
//                        default:
//                            break;
//                    }
//                    cp.setPixel(k, j, magnitudeImage, cp.merge(oldPixel, newPixel));
//                }
//            }
//        }
//    }
//    rearrangeQuadrants(phaseImage, magnitudeImage);
//    QImages images;
//    images.magnitude = magnitudeImage;
//    images.phase = phaseImage;
//    return images;
//}
