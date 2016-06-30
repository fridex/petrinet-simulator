/**
 * \file     browsewindow.cpp
 * \brief    Subor s oknom popisu programu.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     mar 10 2012
 */

#include <QtGui>

#include <pn/client/browsewindow.h>
#include <pn/client/connection.h>
#include <pn/client/mainwindow.h>
#include <pn/client/project.h>

#include "ui_browsewindow.h"

/**
 * Poradie poloziek v zozname v okne.
 */
const int VER_VER_IDX       = 0;
const int VER_USER_IDX      = 1;
const int VER_TIME_IDX      = 2;
const int VER_DESC_IDX      = 3;
const int PROJ_NAME_IDX     = 0;

/**
 * Identifikacia polozky projekt alebo verzia.
 */
enum {
    IDENT_PROJ,
    IDENT_VER
};

/**
 * \brief Konstruktor pre okno so zoznamom projektov v repozitari.
 */
browseWindow::browseWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::browseWindow)
{
    ui->setupUi(this);

    my_error = false;

    // Identifikacia projeku / verzie.
    ui->projectView->setColumnHidden(5, true);

    // Prvotne naplnenie projektov.
    get_projects();

    // Pripravenie signalov pre pracu s projektami.
    connect(ui->projectView, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
            this, SLOT(get_version(QTreeWidgetItem *, int)));

    connect(ui->searchEdit, SIGNAL(editingFinished()),
            this, SLOT(search_projects()));
}

/**
 * \brief Destruktor pre okno so zoznamom projektov v repozitari.
 */
browseWindow::~browseWindow()
{
    delete ui;
}

/**
 * \brief Predikat pre zistenie chyboveho stavu.
 */
bool browseWindow::error() const
{
    return my_error;
}

/**
 * \brief Naplnenie okna s informaciami o repozitari.
 */
void browseWindow::get_projects()
{
    ui->progressBar->setMaximum(0); // Zapnutie progressbaru.

    // Prebratie zoznamu.
    Connection::instance()->req_list();
    if (Connection::instance()->error()) {
        my_error = true;
        QMessageBox msgBox(QMessageBox::Critical, tr("Failed"),
                           Connection::instance()->msg());
        msgBox.exec();
        return;
    }

    // Vymazanie zoznamov.
    ui->projectView->clear();
    ui->projectView->reset();

    // Prebranie projektov
    for (ProjectRecord_iter it = Connection::instance()->req_list_begin();
            it != Connection::instance()->req_list_end(); ++it) {
        QTreeWidgetItem * item = new QTreeWidgetItem(ui->projectView);

        item->setText(PROJ_NAME_IDX, it->name);
        item->setText(5, QString::number(IDENT_PROJ));

        ui->projectView->addTopLevelItem(item);

        QApplication::processEvents(); // aktualizuj progressbar
    }

    // Prebranie verzii
    int count = ui->projectView->topLevelItemCount();
    for (int i = 0; i < count; ++i) {
        get_versionView(ui->projectView->topLevelItem(i));;
        QApplication::processEvents(); // aktualizuj progressbar
    }

    ui->progressBar->setMaximum(100); // Vypnutie progressbaru.
}

/**
 * \brief Signal pre onovenie obsahu verzii konkretneho projektu.
 * \param item polozka pre ktoru sa maju verzie prevziat
 * \param column identifikacia v Qt
 */
void browseWindow::get_versionView(QTreeWidgetItem * item, int column)
{
    // Ak je riadok verzia, otvor ju v editore.
    if (item->text(5).toInt() == IDENT_VER) {
        get_version(item, column);
        return;
    }

    // Ak su verzie uz nacitane, netreba ich znovu preberat zo serveru.
    if (item->childCount() > 0)
        return;

    QString pname = item->text(0);
    Connection::instance()->req_vlist(pname);
    if (Connection::instance()->error()) {
        my_error = true;
        QMessageBox msgBox(QMessageBox::Critical, tr("Failed"),
                           Connection::instance()->msg());
        msgBox.exec();
        return;
    }

    unsigned i = Connection::instance()->req_vlist_size();
    QDateTime date;
    for (VersionRecord_iter it = Connection::instance()->req_vlist_begin();
            it != Connection::instance()->req_vlist_end(); ++it, --i) {
        QTreeWidgetItem * child = new QTreeWidgetItem(item);

        child->setText(VER_VER_IDX, QString::number(i));
        child->setText(VER_USER_IDX, it->user);

        date.setTime_t(it->time);
        child->setText(VER_TIME_IDX, date.toString());
        child->setText(VER_DESC_IDX, it->desc);

        // Identifikator.
        child->setText(5, QString::number(IDENT_VER));

        item->addChild(child);

        QApplication::processEvents(); // aktualizuj progressbar
    }

    ui->progressBar->setMaximum(100); // Vypnutie progressbaru.
}

/**
 * \brief Signal pre otvorenie konkretneho projektu.
 * \param item polozka pre identifikaciu projeku a verzie
 * \param column identifikacia v Qt
 */
void browseWindow::get_version(QTreeWidgetItem * item, int column)
{
    column ^= column; // nepouzita, z dovodu kompatibility s Qt.
    if (item->text(5).toInt() != IDENT_VER)
        return; // Nie je verzia.

    ui->progressBar->setMaximum(0); //Spustenie progrressbaru.

    QString pname = item->parent()->text(PROJ_NAME_IDX);
    Connection::instance()->req_get(pname,
                                    item->text(VER_VER_IDX).toUInt());
    if (Connection::instance()->error()) {
        my_error = true;
        QMessageBox msgBox(QMessageBox::Critical, tr("Failed"),
                           Connection::instance()->msg());
        msgBox.exec();
        return;
    } else {
        // Vykreslenie vybrateho projektu zo serveru
        QObject *o = this->parent();
        MainWindow *mw;
        if ((mw = dynamic_cast<MainWindow *>(o))) {
            mw->xml_to_scene(Connection::instance()->xml(),
                             item->parent()->text(PROJ_NAME_IDX),
                             0,
                             (item->text(VER_VER_IDX)).toUInt());
        }
    }

    this->close();
    ui->progressBar->setMaximum(100); // Vypnutie progressbaru.
}

/**
 * \brief Slot pre vyhladavanie verzie v repozitari.
 * \param text text podla ktoreho sa ma vyhladavat
 */
void browseWindow::search_projects() {
    ui->progressBar->setMaximum(0); // Zapnutie progressbaru.

    ui->searchEdit->setDisabled(true);
    QString text = ui->searchEdit->text();

    int count = ui->projectView->topLevelItemCount();
    for (int i = 0; i < count; ++i) {
        bool matches_parent = false;
        bool matches_child = false;

        QTreeWidgetItem * project = ui->projectView->topLevelItem(i);
        QApplication::processEvents(); // aktualizuj progressbar

        if (project->text(PROJ_NAME_IDX).contains(text, Qt::CaseInsensitive)
                || text.isEmpty()) {
            matches_parent = true;
            project->setHidden(false);
        } else {
            project->setHidden(true);
        }

        int vcount = project->childCount();
        for (int i = 0; i < vcount; ++i) {
            QTreeWidgetItem * version = project->child(i);

            if (version->text(VER_DESC_IDX).contains(text, Qt::CaseInsensitive)
                    || version->text(VER_USER_IDX)
                            .contains(text, Qt::CaseInsensitive)
                    || text.isEmpty() || matches_parent) {
                version->setHidden(false);
                project->setHidden(false);
                project->setExpanded(true);
                matches_child = true;
            } else
                version->setHidden(true);

            QApplication::processEvents(); // aktualizuj progressbar
        }

        if (matches_child)
            project->setHidden(false);

    }

    ui->searchEdit->setDisabled(false);
    ui->progressBar->setMaximum(100); // Vypnutie progressbaru.
}

