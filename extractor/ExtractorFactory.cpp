#include "ExtractorFactory.h"

#include "extractors/FFT.h"

ExtractorFactory::ExtractorFactory()
{
}

ExtractorInterface *ExtractorFactory::getExtractor(const QString &name,
                                                   const QStringList &params)
{
    ExtractorInterface *extractor = NULL;
    if (name == "fourier") {
        extractor = new FFT();
    } else {
        qCritical("no such extractor \"%s\"\n", name.toStdString().c_str());
    }
    if ((extractor != NULL) && (!extractor->init(params))) {
        delete extractor;
        return NULL;
    }
    return extractor;
}
