/**
 * \file     helpwindow.h
 * \brief    Okno s napovedou.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     mar 11 2012
 */

#ifndef PN_CLIENT_HELPWINDOW_H_
#define PN_CLIENT_HELPWINDOW_H_

#include <QMainWindow>

namespace Ui {
    class helpWindow;
}

/**
 * \brief Trieda reprezentujuca okno s napovedou.
 */
class helpWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit helpWindow(QWidget *parent = 0);
    ~helpWindow();

private:
    Ui::helpWindow *ui;
}; // helpWindow

#endif // PN_CLIENT_HELPWINDOW_H_

