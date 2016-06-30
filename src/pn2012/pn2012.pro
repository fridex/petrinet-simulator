# ICP2012 pn2012 file
# Fridolin Pokorny <fridex.devel@gmail.com>

TEMPLATE = app
TARGET = pn2012
DEPENDPATH += .
INCLUDEPATH += ../include/
SOURCES +=  main.cpp \
            pn2012.cpp \
            connection.cpp \
            mainwindow.cpp \
            aboutwindow.cpp \
            connectwindow.cpp \
            helpwindow.cpp \
            simlogwindow.cpp \
            browsewindow.cpp \
            pnobjectwindow.cpp \
            preferenceswindow.cpp \
            commitwindow.cpp \
            settings.cpp \
            ../proto.cpp \
            ../pnobject.cpp \
            ../arrow.cpp \
            ../place.cpp \
            ../transition.cpp \
            ../pnlist.cpp \
            project.cpp

HEADERS +=  ../include/pn/client/pn2012.h \
            ../include/pn/client/connection.h \
            ../include/pn/client/mainwindow.h \
            ../include/pn/client/aboutwindow.h \
            ../include/pn/client/connectwindow.h \
            ../include/pn/client/helpwindow.h \
            ../include/pn/client/simlogwindow.h \
            ../include/pn/client/browsewindow.h \
            ../include/pn/client/pnobjectwindow.h \
            ../include/pn/client/preferenceswindow.h \
            ../include/pn/client/commitwindow.h \
            ../include/pn/client/settings.h \
            ../include/pn/pnobject.h \
            ../include/pn/arrow.h \
            ../include/pn/place.h \
            ../include/pn/transition.h \
            ../include/pn/pnlist.h \
            ../include/pn/client/project.h \

QT += network gui core xml
QMAKE_CXXFLAGS += -std=c++98 -Wall -Wextra

FORMS +=    mainwindow.ui \
            aboutwindow.ui \
            connectwindow.ui \
            helpwindow.ui \
            simlogwindow.ui \
            browsewindow.ui \
            pnobjectwindow.ui \
            preferenceswindow.ui \
            commitwindow.ui

RESOURCES +=  resources.qrc

