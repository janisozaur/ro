#include "ExtractorFactory.h"

#include "extractors/FFT.h"
#include "extractors/FFTSum.h"
#include "extractors/LBP.h"
#include "extractors/AutoCorrelation.h"

ExtractorFactory::ExtractorFactory()
{
}

ExtractorInterface *ExtractorFactory::getExtractor(const QString &name,
                                                   const QStringList &params)
{
    ExtractorInterface *extractor = NULL;
    if (name == "fourier") {
        extractor = new FFT();
    } else if (name == "fouriersum") {
        extractor = new FFTSum();
    } else if (name == "lbp") {
        extractor = new LBP();
    } else if (name == "ac") {
        extractor = new AutoCorrelation();
    } else {
        qCritical("no such extractor \"%s\"\n", name.toStdString().c_str());
    }
    if ((extractor != NULL) && (!extractor->init(params))) {
        delete extractor;
        return NULL;
    }
    return extractor;
}
