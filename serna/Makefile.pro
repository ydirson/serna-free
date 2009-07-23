
TEMPLATE    = serna
SUBDIRS     += \
                i18n \
                docutils \
                utils \
                genui \
                docview:docutils \
                structeditor:docview \
                plaineditor:docview \
                core:structeditor;plaineditor

debug:EXCLUDE_FROM_BUILD = i18n
release:win32_SUBDIRS *= dll
win32:SUBDIRS *= $$win32_SUBDIRS
SUBDIRS    *= \
                sapi:structeditor \
                pyapi:sapi \
                plugins:core;sapi \
                app:plugins
                
options     = package=serna

CONFIG *= sceneareaset
