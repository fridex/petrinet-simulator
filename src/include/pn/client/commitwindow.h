/**
 * \file     commitwindow.h
 * \brief    Okno pre ulozenie projektu do repozitara na servri.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     apr 15 2012
 */

#ifndef PN_CLIENT_COMMITWINDOW_H_
#define PN_CLIENT_COMMITWINDOW_H_

#include <QDialog>
#include <pn/client/project.h>

namespace Ui {
class commitWindow;
}

/**
 * \brief Okno pre ulozenie projektu do repozitara na servri.
 */
class commitWindow : public QDialog
{
    Q_OBJECT

public:
    explicit commitWindow(Project * project, QWidget *parent = 0);
    ~commitWindow();

private:
    Ui::commitWindow *ui;
    Project * my_project;

private slots:
    void accept();
}; // commitWindow

#endif // PN_CLIENT_COMMITWINDOW_H_

