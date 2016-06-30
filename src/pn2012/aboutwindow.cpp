/**
 * \file     aboutwindow.cpp
 * \brief    subor s oknom popisu programu
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     mar 10 2012
 */

#include <pn/client/aboutwindow.h>

#include "ui_aboutwindow.h"

/**
 * \brief Konstruktor okna s popisom programu.
 * \param parent Predchodca okna.
 */
aboutWindow::aboutWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::aboutWindow)
{
    ui->setupUi(this);
}

/**
  * \brief Destruktor okna s popisom programu.
  */
aboutWindow::~aboutWindow()
{
    delete ui;
}

