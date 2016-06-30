/**
 * \file     mainwindow.cpp
 * \brief    subor s hlavnym oknom edioru
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     mar 06 2012
 */

#include <QMessageBox>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QSignalMapper>

#include <pn/client/mainwindow.h>
#include <pn/client/aboutwindow.h>
#include <pn/client/connectwindow.h>
#include <pn/client/helpwindow.h>
#include <pn/client/simlogwindow.h>
#include <pn/client/browsewindow.h>
#include <pn/client/preferenceswindow.h>
#include <pn/client/commitwindow.h>
#include <pn/client/connection.h>
#include <pn/client/project.h>
#include <pn/arrow.h>
#include <pn/transition.h>
#include <pn/place.h>
#include <pn/pnlist.h>

#include "ui_mainwindow.h"

/**
 * \brief Konstruktor hlavneho okna.
 * \param parent Predchodca okna.
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    my_current_mode = Project::MNONE;

    connect(ui->tabWidget, SIGNAL(currentChanged(int)),
            this, SLOT(setTabMode()));

    // namapovanie signalov na slot
    setMapper(my_signal_mapper);

    // vytvori project, ktory dedi QGraphicsScene
    Project *project = new Project(this);
    project->setMode(my_current_mode);

    // nastavim velkost priestoru sceny
    project->setSceneRect(QRect(0,0,1000,1000));

    // vytvorime zobrazenie sceny projektu
    QGraphicsView *view = new QGraphicsView(project);

    // vytvorime tab a vlozime obraz sceny s projektom
    ui->tabWidget->addTab(view, tr("Project"));
}

/**
 * \brief Destruktor hlavneho okna.
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
  * \brief Zobrazi okno Preferences.
  */
void MainWindow::on_action_Pref_triggered()
{
    preferencesWindow pref_window;
    pref_window.setModal(true);
    int result = pref_window.exec();
    if (result == QDialog::Accepted) {
        this->redraw_scenes();
    }

    this->redraw_scenes();
}

/**
  * \brief Zobrazi okno About.
  */
void MainWindow::on_action_About_triggered()
{
    aboutWindow *about_window = new aboutWindow(this);
    about_window->setModal(true);
    // about_window->show();
    about_window->exec();
}

/**
  * \brief Zobrazi okno Connect.
  */
void MainWindow::on_action_Connect_triggered()
{
    connectWindow * connect_window = new connectWindow(this);
    connect_window->setModal(true);
    connect_window->exec();
}

/**
  * \brief Zobrazi okno s napovedou.
  */
void MainWindow::on_action_Help_triggered()
{
    helpWindow * help_window = new helpWindow(this);
    help_window->show();
}

/**
 * \brief Slot pri vybrani volby otvorit lokalny projekt.
 */
void MainWindow::on_action_Open_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                tr("Open project"), QDir::currentPath(),
                                tr("Petri Net file(*.pn);;All files (*)"));

    if (! filename.isEmpty()) {
        QFile fin(filename);
        // Pozor na chybu pri otvoreni.
        if (! fin.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox msgBox(QMessageBox::Critical, tr("Failed"),
                               fin.errorString());
            msgBox.exec();
            return;
        }

        QString xml;
        xml = fin.readAll();
        fin.close();

        /* vytvorenie sceny z xml */
        this->xml_to_scene(xml, filename);
    }
}

/**
 * \brief Slot pre zobrazenie okna simlogu.
 */
void MainWindow::on_actionSimlog_triggered()
{
    Project *project;
    QWidget *widget;
    QGraphicsView *view;

    /* ziskame ukazatel na aktualny projekt */
    if ((widget = ui->tabWidget->currentWidget())) {
        if ((view = dynamic_cast<QGraphicsView *>(widget))) {
            if (! (project = dynamic_cast<Project *>(view->scene())))
                return;
        } else {
            return;
        }
    } else {
        return; //Ziadny tab.
    }

    if (! project->from_server()) {
        QMessageBox msgBox(QMessageBox::Critical, tr("Failed"),
                           tr("Project not from server!"));
            msgBox.exec();
        return;
    }

    simlogWindow * simlog_window = new simlogWindow(project->servername(),
                                                    project->version(), this);
    if (! simlog_window->error())
        simlog_window->show();
    else
        simlog_window->close();
}

/**
 * \brief Slot pri vybrani volby ulozit lokalne projekt.
 */
void MainWindow::on_action_Save_as_triggered()
{
    Project *project;
    QWidget *widget;
    QGraphicsView *view;

    /* ziskame ukazatel na aktualny projekt */
    if ((widget = ui->tabWidget->currentWidget())) {
        if ((view = dynamic_cast<QGraphicsView *>(widget))) {
            if (! (project = dynamic_cast<Project *>(view->scene())))
                return;
        } else {
            return;
        }
    } else {
        return; //Ziadny tab.
    }

    QString filename = QFileDialog::getSaveFileName(this,
                                tr("Save as"), QDir::currentPath(),
                                tr("Petri Net file(*.pn);;All files (*)"));
    if (! filename.isEmpty()) {
        if (! filename.endsWith(".pn"))
            filename += ".pn";

        QFile fout(filename);
        // Pozor na chybu pri zapise.
        if (! fout.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox msgBox(QMessageBox::Critical, tr("Failed"),
                               fout.errorString());
            msgBox.exec();
            return;
        }

        QString xml;
        project->xml(xml);
        // Zapis celeho xml.
        fout.write(xml.toAscii());
        project->set_filepath(filename);

        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(),
                                  project->filename());

        fout.close();
    }
}

/**
 * \brief Slot pri vybrani volby ulozit lokalne projekt.
 */
void MainWindow::on_action_Save_triggered()
{
    Project *project;
    QWidget *widget;
    QGraphicsView *view;

    /* ziskame ukazatel na aktualny projekt */
    if ((widget = ui->tabWidget->currentWidget())) {
        if ((view = dynamic_cast<QGraphicsView *>(widget))) {
            if (! (project = dynamic_cast<Project *>(view->scene())))
                return;
        } else {
            return;
        }
    } else {
        return; //Ziadny tab.
    }



    if (! project->was_saved()) {
        // Este nebola zadana cesta, kam sa ma ukladat
        emit on_action_Save_as_triggered();
        return;
    }

    QString filename = project->fileinfo().absoluteFilePath();

    QFile fout(filename);

    // Pozor na chybu pri zapise.
    if (! fout.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox msgBox(QMessageBox::Critical, tr("Failed"),
                           fout.errorString());
        msgBox.exec();
        return;
    }

    QString xml;
    project->xml(xml);
    // Zapis celeho xml.
    fout.write(xml.toAscii());

    fout.close();
}

/**
 * \brief Slot pri vybrani volby prezerania repoziatara.
 */
void MainWindow::on_action_Browse_triggered()
{
    browseWindow * browse_window = new browseWindow(this);

    if (! browse_window->error())
        browse_window->show();
    else
        browse_window->close();
}

/**
 * \brief Slot pri vybrani volby ulozit lokalne projekt - prepisat existujuci.
 */
void MainWindow::on_action_Remote_Save_triggered()
{
    Project *project = 0;
    QWidget *widget;
    QGraphicsView *view;

    // Je vobec uzivatel pripojeny?
    if (! Connection::instance()->connected()) {
        QMessageBox msgBox(QMessageBox::Critical, tr("Failed"),
                           tr("Not connected!"));
            msgBox.exec();
        return;
    }

    /* ziskame ukazatel na aktualny projekt */
    if ((widget = ui->tabWidget->currentWidget())) {
        if ((view = dynamic_cast<QGraphicsView *>(widget))) {
            if (! (project = dynamic_cast<Project *>(view->scene())))
                return;
        }

    } else {
        QMessageBox msgBox(QMessageBox::Critical, tr("Failed"),
                           tr("No project is opened."));
            msgBox.exec();
        return;
    }

    commitWindow * comwin = new commitWindow(project, this);
    comwin->setModal(true);
    comwin->exec();
}

/**
 * \brief Slot pri vytvoreni noveho projektu.
 */
void MainWindow::on_action_New_triggered()
{
    int index;
    Project *project = new Project(this);
    project->setMode(my_current_mode);
    project->setSceneRect(QRect(0,0,1000,1000));
    QGraphicsView *view = new QGraphicsView(project);

    index = ui->tabWidget->addTab(view, "Project");
    ui->tabWidget->setCurrentIndex(index);
}

/**
 * \brief Slot pre prevedenie kroku simulacie.
 */
void MainWindow::on_actionStep_triggered()
{
    Project *project = 0;
    QWidget *widget;
    QGraphicsView *view;

    // Je vobec uzivatel pripojeny?
    if (! Connection::instance()->connected()) {
        QMessageBox msgBox(QMessageBox::Critical, tr("Failed"),
                           tr("Not connected!"));
            msgBox.exec();
        return;
    }

    /* ziskame ukazatel na aktualny projekt */
    if ((widget = ui->tabWidget->currentWidget())) {
        if ((view = dynamic_cast<QGraphicsView *>(widget))) {
            if (! (project = dynamic_cast<Project *>(view->scene())))
                return;
        }

    }

    QString xml;
    project->xml(xml);
    Connection::instance()->req_step(xml,
                                     project->servername(),
                                     project->version());


    if (Connection::instance()->error()) {
        QMessageBox msgBox(QMessageBox::Critical, tr("Failed"),
                           Connection::instance()->msg());
        msgBox.exec();
        return;
    }

    this->xml_to_scene(Connection::instance()->xml(), project->filename(),
                       project);

}

/**
 * \brief Slot pre odsimulovanie petriho siete.
 */
void MainWindow::on_actionRun_triggered()
{
    Project *project = 0;
    QWidget *widget;
    QGraphicsView *view;

    // Je vobec uzivatel pripojeny?
    if (! Connection::instance()->connected()) {
        QMessageBox msgBox(QMessageBox::Critical, tr("Failed"),
                           tr("Not connected!"));
            msgBox.exec();
        return;
    }

    /* ziskame ukazatel na aktualny projekt */
    if ((widget = ui->tabWidget->currentWidget())) {
        if ((view = dynamic_cast<QGraphicsView *>(widget))) {
            if (! (project = dynamic_cast<Project *>(view->scene())))
                return;
        }

    }

    QString xml;
    project->xml(xml);
    Connection::instance()->req_run(xml,
                                     project->servername(),
                                     project->version());


    if (Connection::instance()->error()) {
        QMessageBox msgBox(QMessageBox::Critical, tr("Failed"),
                           Connection::instance()->msg());
        msgBox.exec();
        return;
    }

    this->xml_to_scene(Connection::instance()->xml(), project->filename(),
                       project);
}

/**
 * \brief Slot pre zatvorenie tabu s projektom.
 */
void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    ui->tabWidget->removeTab(index);
}

/**
 * \brief Nastavi mapper signalov na slot setMode.
 * \param mapper Mapper signalov.
 */
void MainWindow::setMapper(QSignalMapper* & mapper) {
    // signal mapper pre zistenie, od ktoreho objektu prisiel signal
    mapper = new QSignalMapper();
    // namapovanie objektov na signaly
    mapper->setMapping(ui->actionMove, Project::MMOVE);
    mapper->setMapping(ui->actionPlaces, Project::MPLACE);
    mapper->setMapping(ui->actionTransitions, Project::MTRANS);
    mapper->setMapping(ui->actionArrows, Project::MARROW);
    mapper->setMapping(ui->actionEditObject, Project::MEDIT);
    mapper->setMapping(ui->actionDelete, Project::MDEL);

    // spojenie signalov so slotom mappru
    connect(ui->actionMove, SIGNAL(triggered()),
            mapper, SLOT(map()));
    connect(ui->actionPlaces, SIGNAL(triggered()),
            mapper, SLOT(map()));
    connect(ui->actionTransitions, SIGNAL(triggered()),
            mapper, SLOT(map()));
    connect(ui->actionArrows, SIGNAL(triggered()),
            mapper, SLOT(map()));
    connect(ui->actionEditObject, SIGNAL(triggered()),
            mapper, SLOT(map()));
    connect(ui->actionDelete, SIGNAL(triggered()),
            mapper, SLOT(map()));

    // spojenie singalu mappru so slotom pre nastavenie modu
    connect(mapper, SIGNAL(mapped(int)),
            this, SLOT(setMode(int)));
}

/**
 * \brief Slot pre nastavenia modu editoru.
 * \param id identifikator modu
 */
void MainWindow::setMode(int id) {
    QWidget *widget;
    QGraphicsView *view;
    QGraphicsScene *scene;
    Project *project;
    Project::Mode mode;

    mode = static_cast<Project::Mode>(id);

    setActionChecked(my_current_mode, mode);

    widget = ui->tabWidget->currentWidget();
    if ((view = dynamic_cast<QGraphicsView *>(widget))) {
        scene = view->scene();
        if ((project = dynamic_cast<Project *>(scene))) {
            if (my_current_mode == mode) {
                project->setMode(Project::MNONE);
                my_current_mode = Project::MNONE;
            } else {
                project->setMode(mode);
                my_current_mode = mode;
            }
        }
    } else {
        /* pri ziadnom stlacene tlacidlo vypneme */
        QObject *object = my_signal_mapper->mapping(mode);
        if (object->inherits("QAction")) {
            QAction *action = dynamic_cast<QAction *>(object);
            action->setChecked(false);
        }
    }

}

/**
 * \brief Slot pre nastavenie Modu pri zmene tabu
 */
void MainWindow::setTabMode(void) {
    QWidget *widget;
    QGraphicsView *view;
    QGraphicsScene *scene;
    Project *project;

    widget = ui->tabWidget->currentWidget();
    /* pri zavreti Widgetu vypneme tlacidlo, ak je nejake stlacene */
    if (! widget && my_current_mode != Project::MNONE ) {
        QObject *object = my_signal_mapper->mapping(my_current_mode);
        if (object->inherits("QAction")) {
            QAction *action = dynamic_cast<QAction *>(object);
            action->setChecked(false);
        }
        my_current_mode = Project::MNONE;
        return;
    }

    if ((view = dynamic_cast<QGraphicsView *>(widget))) {
        scene = view->scene();
        if ((project = dynamic_cast<Project *>(scene))) {
            project->setMode(my_current_mode);
        }
    }

}

/**
 * \brief Nastavi novu akciu na aktivnu staru na neaktivnu.
 * \param old_mode Predosly editacny mod.
 * \param new_mode Aktualny editacny mod.
 */
void MainWindow::setActionChecked(Project::Mode old_mode, Project::Mode new_mode) {
    if (old_mode == 0)
        return;

    QObject *object;
    QAction *action;

    object = my_signal_mapper->mapping(old_mode);
    if (object->inherits("QAction")) {
        action = dynamic_cast<QAction *>(object);
        action->setChecked(false);
    }

    if (old_mode != new_mode) {
        object = my_signal_mapper->mapping(new_mode);
        if (object->inherits("QAction")) {
            action = dynamic_cast<QAction *>(object);
            action->setChecked(true);
        }
    }
}

/**
 * \brief Slot pre zatvorenie tabu klavesovou skratkou
 */
void MainWindow::on_action_CloseTab_triggered()
{
    int index = ui->tabWidget->currentIndex();
    if (index != -1)
        ui->tabWidget->removeTab(index);
}

/**
 * \brief Aplikuje styl vzhladu na kazdy objek vo vsetkych scenach.
 */
void MainWindow::redraw_scenes() {
    QWidget *widget;
    QGraphicsView *view;
    QGraphicsScene *scene;
    Arrow *arrow;
    Place *place;
    Transition *transition;

    int count = ui->tabWidget->count();
    for (int i = 0; i < count; i++) {
        widget = ui->tabWidget->widget(i);
        if ((view = dynamic_cast<QGraphicsView *>(widget))) {
            scene = view->scene();
            foreach(QGraphicsItem *item, scene->items()) {
                if ((arrow = dynamic_cast<Arrow *>(item)))
                    arrow->apply_style();
                else if((place = dynamic_cast<Place *>(item)))
                    place->apply_style();
                else if ((transition = dynamic_cast<Transition *>(item)))
                    transition->apply_style();
            }
        }
    }
}

/**
 * \brief Vytvori z xml novu scenu s projektom.
 * \param xml XML subor s petriho sietou.
 * \param proj_name Nazov projektu.
 * \param old_project Ukazatel na scenu s projektom.
 * \param version Verzia siete / projektu.
 */
void MainWindow::xml_to_scene(const QString & xml, QString proj_name,
                              Project *old_project,
                              unsigned version) {
    PNList *pnlist;                 //<! Zoznam PN objektov.
    Project *project;               //<! Novy project.
    QGraphicsScene *scene;          //<! Nova scena.
    QGraphicsItem *item;            //<! Polozka sceny.
    QGraphicsView *view;            //<! Zobrazenie sceny.
    int index;                      //<! Index vytvoreneho tabu.

    /* vytvorime pnlist */
    pnlist = new PNList();

    /* rozparsovanie xml suboru */
    if (! pnlist->from_xml(xml)) {
        QMessageBox msgBox(QMessageBox::Critical, tr("Failed"),
                           pnlist->error());
        msgBox.exec();
        return;
    }

    /* ak bola zadana scena nevytvarame novu, ale vkladame do starej */
    if (old_project) {
        if ((scene = dynamic_cast<QGraphicsScene *>(old_project))) {
            /* zrusenie poloziek v scene */
            scene->clear();

            /* vlozenie poloziek do sceny */
            foreach(PNObject *pnobject, pnlist->items()) {
                if ((item = dynamic_cast<QGraphicsItem *>(pnobject))) {
                    scene->addItem(item);
                }
            }
            return; // scena sa aktualizoval
        }
    }

    /* vytvorime novy projekt */
    project = new Project(this);
    project->setMode(my_current_mode);
    project->setSceneRect(QRect(0,0,1000,1000));
    project->set_filepath(proj_name);
    project->set_version(version);
    project->set_servername(proj_name);

    /* pridavanie objektov do sceny */
    if ((scene = dynamic_cast<QGraphicsScene *>(project))) {
        foreach(PNObject *pnobject, pnlist->items()) {
            if ((item = dynamic_cast<QGraphicsItem *>(pnobject))) {
                scene->addItem(item);
            }
        }
    }

    /* vytvorenie zobrazenia sceny a pridanie do tabu */
    view = new QGraphicsView(project);
    index = ui->tabWidget->addTab(view, project->filename());
    ui->tabWidget->setCurrentIndex(index);
}
