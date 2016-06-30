/**
 * \file     debug.cpp
 * \brief    Implementacia vypisu informacii o behu serveru.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     mar 15 2012
 */

#include <QTime>
#include <QDebug>
#include <QThread>
#include <QSemaphore>

#include <pn/server/debug.h>

/**
 * \brief Thread-safe vypis o behu servru.
 * \param msg Sprava pre vypis
 * \retval void
 */
void debug(const char * msg) {
    static QSemaphore my_sem(1);

    my_sem.acquire();

    qDebug() << "[" << QThread::currentThreadId() << "]"
             <<QTime::currentTime().toString() << msg;

    my_sem.release();
}

