# ICP2012 server2012 file
# Fridolin Pokorny <fridex.devel@gmail.com>

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += ../include

SOURCES =   main.cpp \
            answer.cpp \
            message.cpp \
            server2012.cpp \
            projectdb.cpp \
            simulation.cpp \
            userdb.cpp \
            user.cpp \
            serverthread.cpp\
            debug.cpp\
            ../proto.cpp \
            ../pnobject.cpp \
            ../arrow.cpp \
            ../place.cpp \
            ../transition.cpp \
            ../pnlist.cpp


HEADERS +=  ../include/pn/proto.h \
            ../include/pn/pnobject.h \
            ../include/pn/arrow.h \
            ../include/pn/place.h \
            ../include/pn/transition.h \
            ../include/pn/pnlist.h \
            ../include/pn/server/user.h \
            ../include/pn/server/userdb.h \
            ../include/pn/server/projectdb.h \
            ../include/pn/server/answer.h \
            ../include/pn/server/message.h \
            ../include/pn/server/server2012.h \
            ../include/pn/server/simulation.h \
            ../include/pn/server/serverthread.h \
            ../include/pn/server/debug.h

QMAKE_CXXFLAGS += -std=c++98 -Wall -Wextra -Wswitch-enum

QT += network xml script

