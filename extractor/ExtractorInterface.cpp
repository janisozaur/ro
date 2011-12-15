#include "ExtractorInterface.h"

ExtractorInterface::ExtractorInterface()
{
}

ExtractorInterface::~ExtractorInterface()
{
}

void ExtractorInterface::preprocess(const QImage &/*img*/)
{
}

QVector<QVector<float> > ExtractorInterface::postprocess(const QImage &/*img*/) const
{
    return QVector<QVector<float> >();
}
