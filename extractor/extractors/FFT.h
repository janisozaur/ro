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
    void perform(ComplexArray *ca, bool inverse = false) const;
    QVector<float> extract(const QImage &image, const int &x, const int &y) const;
    int size() const;
//    const QImages complexToImages(const ComplexArray *ca, QImage::Format format) const;

private:
    void rearrange(QVector<Complex> &elements) const;
    void transform(QVector<Complex> &elements, bool inverse) const;
    void oneDFftH(ComplexArray *ca, int idx1, int idx2, bool inverse) const;
    void oneDFftV(ComplexArray *ca, int idx1, int idx2, bool inverse) const;

    QSize mSize;
};

#endif // FFT_H
