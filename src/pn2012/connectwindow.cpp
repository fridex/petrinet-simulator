/**
 * \file     connectwindow.cpp
 * \brief    Okno pripojenia na server.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     mar 10 2012
 */

#include <QMessageBox>

#include <pn/client/connectwindow.h>
#include <pn/client/connection.h>

#include "ui_connectwindow.h"

/**
 * \brief Konstruktor okna pripojenia na server.
 * \param parent Predchodca okna.
 */
connectWindow::connectWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::connectWindow)
{
    ui->setupUi(this);

    ui->lineEditServer->setText(Connection::instance()->host());
    ui->lineEditUsername->setText(Connection::instance()->username());
    ui->lineEditPassword->setText(Connection::instance()->password());
    ui->spinBoxPort->setValue(Connection::instance()->port());
}

/**
  * \brief Destruktor okna pripojenia na server.
  */
connectWindow::~connectWindow()
{
    delete ui;
}

/**
 * \brief Slot spusteny pri potvrdeni spojenia so serverom.
 */
void connectWindow::accept()
{
    // Nastav spojenie.
    Connection::instance()->set_host(ui->lineEditServer->text(),
                                     ui->spinBoxPort->value());

    Connection::instance()->set_username(ui->lineEditUsername->text(),
                                         ui->lineEditPassword->text());

    // Zasli zostavenu poziadavku na server.
    if (ui->checkBoxRegister->isChecked())
        Connection::instance()->req_register();
    else
        Connection::instance()->req_auth();

    // Informacia pre uzivatela.
    if (! Connection::instance()->error()) {
        QMessageBox msgBox(QMessageBox::Information, tr("Success"),
                           Connection::instance()->msg());
        msgBox.exec();
        this->close();
    } else {
        QMessageBox msgBox(QMessageBox::Critical, tr("Failed"),
                           Connection::instance()->msg());
        msgBox.exec();
    }
}

