/**
 * \file     helpwindow.cpp
 * \brief    Okno s napovedou.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     mar 11 2012
 */

#include <QShortcut>

#include <pn/client/helpwindow.h>

#include "ui_helpwindow.h"

/**
 * \brief Konstruktor okna s napovedou.
 * \param parent Predchodca okna.
 */
helpWindow::helpWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::helpWindow)
{
    ui->setupUi(this);
    (void) new QShortcut(QKeySequence(tr("Escape")), this, SLOT(close()));
}

/**
  * \brief Destruktor okna s napovedou.
  */
helpWindow::~helpWindow()
{
    delete ui;
}

