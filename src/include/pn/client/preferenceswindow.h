/**
 * \file     preferenceswindow.h
 * \brief    okno s nastaveniami aplikacie
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     apr 09 2012
 */

#ifndef PN_CLIENT_PREFERENCESWINDOW_H_
#define PN_CLIENT_PREFERENCESWINDOW_H_

#include <QDialog>

namespace Ui {
class preferencesWindow;
}

/**
 * \brief Trieda pre okno s nastaveniami aplikacie.
 */
class preferencesWindow : public QDialog
{
    Q_OBJECT

public:
    explicit preferencesWindow(QWidget *parent = 0);
    ~preferencesWindow();

private:
    Ui::preferencesWindow *ui;

private slots:
    void accept();
    void change_echo(int echo);
}; // preferencesWindow

#endif // PN_CLIENT_PREFERENCESWINDOW_H_

