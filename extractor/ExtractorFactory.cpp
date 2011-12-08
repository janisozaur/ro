#include "ExtractorFactory.h"

#include "extractors/FFT.h"

ExtractorFactory::ExtractorFactory()
{
}

ExtractorInterface *ExtractorFactory::getExtractor(const QString &name,
                                                   const QStringList &params)
{
    ExtractorInterface *extractor = nullptr;
    if (name == "fourier") {
        extractor = new FFT();
    }
    if ((extractor != nullptr) && (!extractor->init(params))) {
        delete extractor;
        return nullptr;
    }
    return extractor;
}
