/**
 * \file     simlogwindow.cpp
 * \brief    subor s oknom s informacie o logoch na strane serveru
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     mar 10 2012
 */

#include <QMessageBox>
#include <QtGui>

#include <pn/client/connection.h>
#include <pn/client/simlogwindow.h>

#include "ui_simlogwindow.h"

/**
 * \brief Konstruktor.
 */
simlogWindow::simlogWindow(QString pname, unsigned version, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::simlogWindow)
{
    my_error = false;

    ui->setupUi(this);

    emit refresh(pname, version);
}

/**
 * \brief Destruktor.
 */
simlogWindow::~simlogWindow()
{
    delete ui;
}

/**
 * \brief Predikat pre zistenie informacii, ci bola pri zistovani udajov zo
 * strany serveru.
 * \retval true v pripade chyby
 */
bool simlogWindow::error()
{
    return my_error;
}

/**
 * Aktualuzacia vypis zoznamu logu zo strany serveru.
 * \param pname nazov projektu pre ktory sa ma simlog stiahnut
 * \param version verzia projektu pre specifikovanie simlogu
 */
void simlogWindow::refresh(QString & pname, unsigned version)
{
    Connection::instance()->req_simlog(pname, version);

    if (Connection::instance()->error()) {
        my_error = true;

        QMessageBox msgBox(QMessageBox::Critical, tr("Failed"),
                           Connection::instance()->msg());
        msgBox.exec();

        return;
    }

    ui->treeWidgetSimlog->setRootIsDecorated(false);
    for (SimlogRecord_iter it = Connection::instance()->req_simlog_begin();
            it != Connection::instance()->req_simlog_end(); ++it) {
        QDateTime time;
        time.setTime_t(it->time);

        QTreeWidgetItem * item = new QTreeWidgetItem(ui->treeWidgetSimlog);

        item->setText(0, time.toString(Qt::SystemLocaleShortDate));
        item->setText(1, it->user);

        ui->treeWidgetSimlog->addTopLevelItem(item);
    }
}

