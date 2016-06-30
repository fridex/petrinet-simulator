/**
 * \file     pnobjectwindow.h
 * \brief    Trieda pre okno s nastavenim vlastnosti prvkov petriho siete.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     apr 07 2012
 */

#ifndef PNOBJECTWINDOW_H
#define PNOBJECTWINDOW_H

#include <QDialog>
#include <QGraphicsItem>
#include <pn/client/project.h>


namespace Ui {
    class PNObjectWindow;
}

/**
 * \brief Trieda pre okno s nastavenim vlastnosti prvkov petriho siete.
 */
class PNObjectWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PNObjectWindow(Project *project, QGraphicsItem *item,
                            QWidget *parent = 0);
    ~PNObjectWindow();

private slots:
    void accept();

private:
    Ui::PNObjectWindow *ui;
    QGraphicsItem *my_item;
    Project *my_project;
}; // PNObjectWindow

#endif // PNOBJECTWINDOW_H

