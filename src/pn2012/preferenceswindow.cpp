/**
 * \file     preferenceswindow.cpp
 * \brief    okno s nastaveniami aplikacie
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     apr 09 2012
 */

#include <QMessageBox>

#include <pn/client/preferenceswindow.h>
#include <pn/client/settings.h>
#include <pn/place.h>
#include <pn/transition.h>
#include <pn/arrow.h>

#include "ui_preferenceswindow.h"

/**
 * \brief Konstuktor.
 */
preferencesWindow::preferencesWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::preferencesWindow)
{
    ui->setupUi(this);

    ui->passwordEdit->setEchoMode(QLineEdit::Password);
    ui->passView->setCheckState(Qt::Checked);
    ui->arrowStyleBox->setCurrentIndex(static_cast<int>(Settings::instance()->arrow_style()));
    ui->placeStyleBox->setCurrentIndex(static_cast<int>(Settings::instance()->place_style()));
    ui->transitionStyleBox->setCurrentIndex(static_cast<int>(Settings::instance()->transition_style()));

    ui->fontComboBox->setCurrentFont(Settings::instance()->font());

    ui->hostEdit->setText(Settings::instance()->host());
    ui->userEdit->setText(Settings::instance()->username());
    ui->passwordEdit->setText(Settings::instance()->password());

    connect(ui->passView, SIGNAL(stateChanged(int)), this, SLOT(change_echo(int)));
}

/**
 * \brief Destruktor.
 */
preferencesWindow::~preferencesWindow()
{
    delete ui;
}

/**
 * \brief Slot volany pre zobrazenie hesla v nastaveniach.
 * \param echo priznak, ci sa ma zobrazit heslo
 */
void preferencesWindow::change_echo(int echo) {
    if (echo)
        ui->passwordEdit->setEchoMode(QLineEdit::Password);
    else
        ui->passwordEdit->setEchoMode(QLineEdit::Normal);
}

/**
 * \brief Slot volany pri potvrdeny nastaveni.
 */
void preferencesWindow::accept() {
    Settings::instance()->set_arrow_style(ui->arrowStyleBox->currentIndex());
    Settings::instance()->set_transition_style(ui->transitionStyleBox->currentIndex());
    Settings::instance()->set_place_style(ui->placeStyleBox->currentIndex());

    Settings::instance()->set_font(ui->fontComboBox->currentFont());

    Settings::instance()->set_host(ui->hostEdit->text());
    Settings::instance()->set_username(ui->userEdit->text());
    Settings::instance()->set_password(ui->passwordEdit->text());

    Arrow::set_style(ui->arrowStyleBox->currentIndex());
    Place::set_style(ui->placeStyleBox->currentIndex());
    Transition::set_style(ui->transitionStyleBox->currentIndex());

    QMessageBox msgBox(QMessageBox::Information, tr("Settings configuration"),
                   tr("Please restart application to apply settings."));
    msgBox.exec();
    this->close();
}

