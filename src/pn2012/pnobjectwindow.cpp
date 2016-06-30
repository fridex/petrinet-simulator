/**
 * \file     pnobjectwindow.cpp
 * \brief    Trieda pre okno s nastavenim vlastnosti prvkov petriho siete.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     apr 07 2012
 */

#include <QMessageBox>

#include <pn/client/pnobjectwindow.h>
#include <pn/place.h>
#include <pn/transition.h>
#include <pn/arrow.h>
#include <pn/pnobject.h>
#include <pn/client/project.h>

#include "ui_pnobjectwindow.h"

class Project;

/**
 * \brief Konstruktor okna pre editaciu objektov petriho siete.
 * \param project Scena s projektom/sietou.
 * \param item Nastavovany objekt petriho siete.
 * \param parent Predchodca okna.
 */
PNObjectWindow::PNObjectWindow(Project *project, QGraphicsItem *item,
                               QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PNObjectWindow)
{
    ui->setupUi(this);
    my_item = item;
    my_project = project;
    Place *p;
    Transition *t;
    Arrow *a;

    /* nastavenia okna podla typu editovaneho objektu */
    if ((p = dynamic_cast<Place *>(my_item))) {
        ui->labelCondition->setVisible(false);
        ui->conditionEdit->setVisible(false);
        ui->modeEdit->setVisible(false);
        ui->labelMode->setVisible(false);
        ui->priorityBox->setVisible(false);
        ui->labelPriority->setVisible(false);

        ui->nameEdit->setText(p->name());
        ui->tokensEdit->setText(p->value());
    } else if ((t = dynamic_cast<Transition *>(my_item))) {
        ui->labelTokens->setVisible(false);
        ui->tokensEdit->setVisible(false);

        ui->nameEdit->setText(t->name());
        ui->modeEdit->setText(t->mode());
        ui->conditionEdit->setText(t->condition());
        ui->priorityBox->setValue(t->priority());
    } else if ((a = dynamic_cast<Arrow *>(my_item))) {
        ui->labelTokens->setVisible(false);
        ui->tokensEdit->setVisible(false);

        ui->labelCondition->setVisible(false);
        ui->conditionEdit->setVisible(false);
        ui->modeEdit->setVisible(false);
        ui->labelMode->setVisible(false);
        ui->priorityBox->setVisible(false);
        ui->labelPriority->setVisible(false);

        ui->nameEdit->setText(a->name());
    }
}

/**
 * \brief Destrktor okna pre editaciu objektu petriho siete.
 */
PNObjectWindow::~PNObjectWindow()
{
    delete ui;
}

/**
 * \brief Slot spusteny pri potvrdeni editacneho okna objektu.
 */
void PNObjectWindow::accept() {
    Place *p;
    Transition *t;
    Arrow *a;
    PNObject *object;

    /* osetrenie prazdneho nazvu */
    if (ui->nameEdit->text().isEmpty()) {
        QMessageBox msgBox(QMessageBox::Critical, tr("Failed set Name."),
                           "Name of the object must be entered!");
        msgBox.exec();
        return;
    }

    /* kontrola jedinecneho nazvu */
    if ((object = dynamic_cast<PNObject *>(my_item))) {
        if ((! my_project->set_object_name(object, ui->nameEdit->text()))) {
            QMessageBox msgBox(QMessageBox::Critical, tr("Failed set Name."),
                               "Name of the object must be unique!");
            msgBox.exec();
            return;
        }
    }

    /* nastavenie objektu petriho siete */
    if ((p = dynamic_cast<Place *>(my_item))) {
        p->set_name(ui->nameEdit->text());
        p->set_value(ui->tokensEdit->text());
        p->setToolTip("Tokens: " + p->value());
    } else if ((t = dynamic_cast<Transition *>(my_item))) {
        t->set_name(ui->nameEdit->text());
        t->set_condition(ui->conditionEdit->text());
        t->set_mode(ui->modeEdit->text());
        t->set_priority(ui->priorityBox->value());
        t->setToolTip("Condition: " + t->condition() + "\nMode: " + t->mode() +
                      "\nPriority: " + QString::number(t->priority()));
    } else if ((a = dynamic_cast<Arrow *>(my_item))) {
        a->set_name(ui->nameEdit->text());
    }
    this->close();
}

