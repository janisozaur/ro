TEMPLATE = subdirs

message("found Qt version $${QT_VERSION}.")

SUBDIRS += \
    classifier \
    extractor \
    trainer

OTHER_FILES += \
    common/projectsCommon.pro
