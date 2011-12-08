#ifndef EXTRACTORFACTORY_H
#define EXTRACTORFACTORY_H

#include "ExtractorInterface.h"

#include <QStringList>

class ExtractorFactory
{
public:
    static ExtractorInterface *getExtractor(const QString &name,
                                            const QStringList &params);

private:
    ExtractorFactory();
};

#endif // EXTRACTORFACTORY_H
