/**
 * \file     simlogwindow.h
 * \brief    Okno s logom simulacii.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     mar 11 2012
 */

#ifndef PN_CLIENT_SIMLOGWINDOW_H_
#define PN_CLIENT_SIMLOGWINDOW_H_

#include <QMainWindow>

namespace Ui {
    class simlogWindow;
}

/**
 * \brief Trieda pre simlog.
 */
class simlogWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit simlogWindow(QString pname, unsigned version,
                          QWidget *parent = 0);
    ~simlogWindow();
    bool error();

private:
    Ui::simlogWindow *ui;
    bool my_error;

private slots:
    void refresh(QString &pname, unsigned version);
}; // simlogWindow

#endif // PN_CLIENT_SIMLOGWINDOW_H_

