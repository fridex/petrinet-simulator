/**
 * \file     connectwindow.h
 * \brief    Okno s moznostou prihlasenia / pripojenia k servru.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     mar 11 2012
 */

#ifndef PN_CLIENT_CONNECTWINDOW_H_
#define PN_CLIENT_CONNECTWINDOW_H_

#include <QDialog>

namespace Ui {
    class connectWindow;
}

/**
 * \brief Trieda reprezentujuca okno s moznostou prihlasenia / pripojenia k
 * servru.
 */
class connectWindow : public QDialog
{
    Q_OBJECT

public:
    explicit connectWindow(QWidget *parent = 0);
    ~connectWindow();

private:
    Ui::connectWindow *ui;

private slots:
    void accept();
}; // connectWindow

#endif // PN_CLIENT_CONNECTWINDOW_H_

