#ifndef FFT_H
#define FFT_H

#include "../ExtractorInterface.h"
#include "ComplexArray.h"

class FFT : public ExtractorInterface
{
public:
    struct QImages {
        QImage phase, magnitude;
    };
    FFT();
    ~FFT();
    virtual bool init(const QStringList &params);
    virtual QString name() const;
    void perform(ComplexArray *ca, bool inverse = false);
    QVector<float> extract(const QImage &image, const int &x, const int &y);
    int size() const;
//    const QImages complexToImages(const ComplexArray *ca, QImage::Format format) const;

private:
    void rearrange(QVector<Complex> &elements);
    void transform(QVector<Complex> &elements, bool inverse);
    void oneDFftH(ComplexArray *ca, int idx, int idx1, int idx2, bool inverse);
    void oneDFftV(ComplexArray *ca, int idx, int idx1, int idx2, bool inverse);

    QSize mSize;
    ComplexArray *mCA;
    QImage mImg;
};

#endif // FFT_H
