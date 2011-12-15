#ifndef EXTRACTORINTERFACE_H
#define EXTRACTORINTERFACE_H

#include "../common/LabelledData.h"

#include <QVector>
#include <QImage>
#include <QStringList>

class ExtractorInterface
{
public:
    ExtractorInterface();
    virtual ~ExtractorInterface();
    virtual QVector<float> extract(const QImage &image, const int &x, const int &y) const = 0;
    virtual bool init(const QStringList &params) = 0;
    virtual int size() const = 0;
    virtual void preprocess(const QImage &img);
    virtual QVector<QVector<float> > postprocess(const QImage &img) const;
    virtual void preprocessTest(const QImage &data, const QImage &label);
    virtual QVector<LabelledData> postprocessTest(const QImage &data, const QImage &label) const;
    virtual bool extracts() const;
};

#endif // EXTRACTORINTERFACE_H
