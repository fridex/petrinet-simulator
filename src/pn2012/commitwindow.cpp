/**
 * \file     commitwindow.cpp
 * \brief    Okno pre ulozenie projektu do repozitara na servri.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     apr 15 2012
 */

#include <QWidget>
#include <QGraphicsView>
#include <QMessageBox>

#include <pn/client/commitwindow.h>
#include <pn/client/project.h>
#include <pn/client/connection.h>

#include "ui_commitwindow.h"

/**
 * \brief Konstruktor.
 * \param project projekt, ktory sa bude ukladat
 * \param parent nasradeny QWidget
 */
commitWindow::commitWindow(Project * project, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::commitWindow)
{


    ui->setupUi(this);

    my_project = project;

    if (! my_project->from_server())
        ui->nameEdit->setText(my_project->filename());
    else
        ui->nameEdit->setText(my_project->servername());

    ui->userEdit->setText(Connection::instance()->username());
    ui->userEdit->setDisabled(true);
}

/**
 * Destruktor.
 */
commitWindow::~commitWindow()
{
    delete ui;
}

/**
 * \brief Slot volani pri potvrdeni vyberu.
 */
void commitWindow::accept() {
    QString data;

    // Overenie vstupu.
    if (ui->nameEdit->text().isEmpty()) {
        QMessageBox msgBox(QMessageBox::Critical, tr("Failed"),
                           tr("Name is empty!"));
        msgBox.exec();
        return;
    }

    if (ui->descEdit->text().isEmpty()) {
        QMessageBox msgBox(QMessageBox::Critical, tr("Failed"),
                           tr("Description is empty!"));
        msgBox.exec();
        return;
    }

    // Ziskanie XML projektu.
    my_project->xml(data);

    // Zaslanie poziadavku.
    Connection::instance()->req_add(ui->nameEdit->text(),
                                    ui->descEdit->text(),
                                    data);

    if (! Connection::instance()->error()) {
        // Nastavenie parametrov projektu.
        my_project->set_version(Connection::instance()->version());
        my_project->set_servername(ui->nameEdit->text());

        QMessageBox msgBox(QMessageBox::Information, tr("Success"),
                           Connection::instance()->msg());
        msgBox.exec();
    } else {
        QMessageBox msgBox(QMessageBox::Critical, tr("Failed"),
                           Connection::instance()->msg());
        msgBox.exec();
    }

    this->close();
}

