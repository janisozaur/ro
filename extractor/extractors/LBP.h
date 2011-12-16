#ifndef LBP_H
#define LBP_H

#include "ExtractorInterface.h"

class LBP : public ExtractorInterface
{
public:
    LBP();
    virtual QVector<float> extract(const QImage &image, const int &x, const int &y) const;
    virtual bool init(const QStringList &params);
    virtual int size() const;
    virtual void preprocess(const QImage &img);
    virtual QVector<QVector<float> > postprocess(const QImage &img) const;
    virtual void preprocessTest(const QImage &data, const QImage &label);
    virtual QVector<LabelledData> postprocessTest(const QImage &data, const QImage &label) const;
    virtual bool extracts() const;

private:
    QPoint getPointDelta(const int &i) const;

    QVector<QRgb> mGreyscaleColorTable;
    QVector<int> mUniforms;
    QVector<QVector<float> > mHistogram;
    QImage mLBPImage;
};

#endif // LBP_H
