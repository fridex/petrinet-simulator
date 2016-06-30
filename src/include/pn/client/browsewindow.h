/**
 * \file     browsewindow.h
 * \brief    Okno s popisom programu.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     mar 11 2012
 */

#ifndef PN_CLIENT_BROWSEWINDOW_H_
#define PN_CLIENT_BROWSEWINDOW_H_

#include <QtGui>

namespace Ui {
    class browseWindow;
}

/**
 * \brief Trieda reprezentujuca okno s repozitarom.
 */
class browseWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit browseWindow(QWidget *parent = 0);
    ~browseWindow();

    bool error() const;

private:
    bool my_error;
    Ui::browseWindow *ui;

private slots:
    void get_projects();
    void get_version(QTreeWidgetItem * item, int column);
    void get_versionView(QTreeWidgetItem * item, int column = 0);
    void search_projects(void);
}; // browseWindow

#endif // PN_CLIENT_BROWSEWINDOW_H_

