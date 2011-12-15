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

bool ExtractorInterface::extracts() const
{
    return true;
}

void ExtractorInterface::preprocessTest(const QImage &/*data*/, const QImage &/*label*/)
{
}

QVector<LabelledData> ExtractorInterface::postprocessTest(const QImage &/*data*/,
                                                          const QImage &/*label*/) const
{
    return QVector<LabelledData>();
}
