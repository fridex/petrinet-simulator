/**
 * \file     aboutwindow.h
 * \brief    Okno s popisom programu.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     mar 11 2012
 */

#ifndef PN_CLIENT_ABOUTWINDOW_H_
#define PN_CLIENT_ABOUTWINDOW_H_

#include <QDialog>

namespace Ui {
    class aboutWindow;
}

/**
 * \brief Trieda reprezentujuca okno s informaciami o aplikacii.
 */
class aboutWindow : public QDialog
{
    Q_OBJECT

public:
    explicit aboutWindow(QWidget *parent = 0);
    ~aboutWindow();

private:
    Ui::aboutWindow *ui;
}; // aboutWindow

#endif // PN_CLIENT_ABOUTWINDOW_H_

