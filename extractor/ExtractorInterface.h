#ifndef EXTRACTORINTERFACE_H
#define EXTRACTORINTERFACE_H

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
};

#endif // EXTRACTORINTERFACE_H
