#ifndef AUTOCORRELATION_H
#define AUTOCORRELATION_H

#include "../ExtractorInterface.h"
#include "ComplexArray.h"

class AutoCorrelation : public ExtractorInterface
{
public:
    AutoCorrelation();
    virtual bool init(const QStringList &params);
    virtual QString name() const;
    QVector<float> extract(const QImage &image, const int &x, const int &y) const;
    int size() const;

private:
    void perform(ComplexArray *ca, bool inverse = false) const;
    void rearrange(QVector<Complex> &elements) const;
    void transform(QVector<Complex> &elements, bool inverse) const;
    void oneDFftH(ComplexArray *ca, int idx1, int idx2, bool inverse) const;
    void oneDFftV(ComplexArray *ca, int idx1, int idx2, bool inverse) const;

    QSize mSize;
};

#endif // AUTOCORRELATION_H
