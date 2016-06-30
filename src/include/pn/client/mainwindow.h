/**
 * \file     mainwindow.h
 * \brief    hlavickovy subor pre mainwindow.cpp
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     mar 06 2012
 */

#ifndef PN_CLIENT_MAINWINDOW_H_
#define PN_CLIENT_MAINWINDOW_H_

#include <QMainWindow>
#include <QSignalMapper>
#include <pn/client/project.h>

/**
 * \brief Menny priestor grafickeho uzivatelskeho rozhrania.
 */
namespace Ui {
    class MainWindow;
}

/**
 * \brief Trieda pre hlavne okno aplikacie.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void xml_to_scene(const QString & xml, QString proj_name,
                      Project *old_project = 0,
                      unsigned version = 0);

private slots:
    void on_action_About_triggered();

    void on_action_Pref_triggered();

    void on_action_Connect_triggered();

    void on_action_Help_triggered();

    void on_action_Open_triggered();

    void on_actionSimlog_triggered();

    void on_action_Save_as_triggered();

    void on_action_Save_triggered();

    void on_action_Browse_triggered();

    void on_action_Remote_Save_triggered();

    void on_action_New_triggered();

    void on_actionStep_triggered();

    void on_actionRun_triggered();

    void on_tabWidget_tabCloseRequested(int index);

    void setMapper(QSignalMapper* & mapper);

    void setMode(int id);

    void setTabMode(void);

    void setActionChecked(Project::Mode old_mode, Project::Mode new_mode);

    void on_action_CloseTab_triggered();

    void redraw_scenes();

private:
    Ui::MainWindow *ui;
    QSignalMapper *my_signal_mapper;
    Project::Mode my_current_mode;
}; // mainWindow

#endif // PN_CLIENT_MAINWINDOW_H_

