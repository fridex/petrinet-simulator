/**
 * \file     project.cpp
 * \brief    Implementacia triedy pre vykreslovanie objektov petriho siete.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     apr 09 2012
 */

#include <QtGui>

#include <pn/client/project.h>
#include <pn/pnobject.h>
#include <pn/place.h>
#include <pn/transition.h>
#include <pn/arrow.h>
#include <pn/client/pnobjectwindow.h>

/**
 * \brief Implicitny nazov suboru projektu.
 */
const char * DEFAULT_FILENAME = "/new_project.pn";

/**
 * \brief - Konstruktor pre vytvorenie projektu
 */
Project::Project(QObject *parent) : QGraphicsScene(parent) {
    // Pocitadla pre generovanie unikatnych mien objektov.
    my_line = 0;
    my_pcnt = 1;
    my_tcnt = 1;
    my_acnt = 1;

    my_saved = false;
    my_version = 0;

    // Nastavenie implicitnej cesty a nazvu suboru.
    my_fileinfo = QDir::currentPath() + DEFAULT_FILENAME;
}

/**
 * \brief Vygeneruje xml Petriho siete.
 * \param data Premenna pre ulozenie xml petriho siete.
 */
void Project::xml(QString & data) {
    PNObject *object;

    data.clear();
    data.append("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n<pn>\n");
    foreach(QGraphicsItem *item, items()) {
        if ((object = dynamic_cast<PNObject *>(item))) {
            object->xml(data);
        }
    }
    data.append("</pn>\n");
}

/**
 * \brief Nastavi mod editovania.
 * \param mode Mod pre nastavenie.
 */
void Project::setMode(Mode mode) {
    my_mode = mode;
}

/**
 * \brief Reimplementacia metody pre udalost stlacenia tlacidla mysi.
 * \param mouseEvent Udalost operacie s mysou.
 */
void Project::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if (mouseEvent->button() != Qt::LeftButton)
        return; // potrebujeme len lave tlacidlo

    QGraphicsItem *item;
    Place *place;
    Transition *transition;
    // na zaklade modu sa rozhodneme aku akciu vykoname
    switch (my_mode) {
        case MMOVE:
            item = itemAt(mouseEvent->scenePos());
            if (item && ! dynamic_cast<Arrow *>(item)) {
                item->setFlag(QGraphicsItem::ItemIsMovable, true);
            }
            break;
        case MPLACE:    // my favorite ;)
            place = new Place();
            /* nastavime meno objektu */
            set_object_name(place);
            addItem(place);
            place->setPos(mouseEvent->scenePos());
            this->update();
            break;
        case MTRANS:
            transition = new Transition();
            /* nastavime meno objektu */
            set_object_name(transition);
            addItem(transition);
            transition->setPos(mouseEvent->scenePos());
            this->update();
            break;
        case MARROW:
            my_line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),
                                                   mouseEvent->scenePos()));
            addItem(my_line);
            break;
        case MEDIT:
            item = itemAt(mouseEvent->scenePos());
            if (item) {
                PNObjectWindow window(this, item);
                window.setModal(true);
                window.exec();
                this->update();
            }
            break;
        case MDEL:
            item = itemAt(mouseEvent->scenePos());
            if (item)
                remove_item(item);
                this->update();
            break;
        default:
            break;
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}

/**
 * \brief Reimplementacia metody pre udalost pohybu mysi.
 * \param mouseEvent Udalost operacie s mysou.
 */
void Project::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if (my_mode == MARROW && my_line != 0) {
        QLineF newLine(my_line->line().p1(), mouseEvent->scenePos());
        my_line->setLine(newLine);
    } else if (my_mode == MMOVE) {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
        //this->invalidate(sceneRect(), QGraphicsScene::BackgroundLayer);
        this->update();
    }
}

/**
 * \brief Reimplementacia metody pre udalost pustenia tlacidla mysi.
 * \param mouseEvent Udalost operacie s mysou.
 */
void Project::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    QGraphicsItem *item;
    Place *p;
    Transition *t;
    Arrow *arrow;

    if (my_mode == MMOVE) {
        // nastavime polozku nepresuvatelnou
        item = itemAt(mouseEvent->scenePos());
        if (item) {
            item->setFlag(QGraphicsItem::ItemIsMovable, false);
            this->update();
        }
    } else if (my_mode == MARROW && my_line != 0) {
        /* vytvorenie objektu sipky */
        /**
         * Vyberame objekty zo sceny na koncovych bodoch ciary.
         * Ciaru zo zoznamov objektov vyberieme a potom ju zrusime.
         */
        QList<QGraphicsItem *> start_objects = items(my_line->line().p1());
        if (start_objects.count() && start_objects.first() == my_line)
            start_objects.removeFirst();
        QList<QGraphicsItem *> end_objects = items(my_line->line().p2());
        if (end_objects.count() && end_objects.first() == my_line)
            end_objects.removeFirst();
        removeItem(my_line);
        delete my_line;

        if (start_objects.count() > 0 && end_objects.count() > 0 &&
                no_duplicity(start_objects.first(), end_objects.first())) {
            /*
             * treba pretypovat kvoli addArrow + porovnavat ci sa na oboch
             * koncoch sipky nenachadzaju rovnake objekty
             */
            p = qgraphicsitem_cast<Place *>(start_objects.first());
            t = qgraphicsitem_cast<Transition *>(end_objects.first());
            if (p && t) {
                arrow = new Arrow(p, t);
                set_object_name(arrow);
                arrow->setZValue(-1000.0);
                p->addArrow(arrow);
                t->addArrow(arrow);
                /* vlozenie sipky do sceny a aktualizovanie pozicie */
                addItem(arrow);
                arrow->updatePosition();
            }
            t = qgraphicsitem_cast<Transition *>(start_objects.first());
            p = qgraphicsitem_cast<Place *>(end_objects.first());
            if (t && p) {
                arrow = new Arrow(t, p);
                set_object_name(arrow);
                arrow->setZValue(-1000.0);
                p->addArrow(arrow);
                t->addArrow(arrow);
                /* vlozenie sipky do sceny a aktualizovanie pozicie */
                addItem(arrow);
                arrow->updatePosition();
            }
         }

    }
    my_line = 0;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

/**
 * \brief Odstrani polozku zo sceny.
 * \param item Polozka na odstranenie.
 */
void Project::remove_item(QGraphicsItem * item) {
    Arrow * arrow;
    Place * place, * p;
    Transition * trans, *t;

    if ((arrow = dynamic_cast<Arrow *>(item))) {
            if ((p = dynamic_cast<Place *>(arrow->start_object())))
                p->removeArrow(arrow);
            else if ((t = dynamic_cast<Transition *>(arrow->start_object())))
                t->removeArrow(arrow);
            if ((p = dynamic_cast<Place *>(arrow->end_object())))
                p->removeArrow(arrow);
            else if ((t = dynamic_cast<Transition *>(arrow->end_object())))
                t->removeArrow(arrow);
    } else if ((place = dynamic_cast<Place *>(item))) {
        foreach (Arrow *arrow, place->get_arrows()) {
            if ((p = dynamic_cast<Place *>(arrow->start_object())))
                p->removeArrow(arrow);
            else if ((t = dynamic_cast<Transition *>(arrow->start_object())))
                t->removeArrow(arrow);
            if ((p = dynamic_cast<Place *>(arrow->end_object())))
                p->removeArrow(arrow);
            else if ((t = dynamic_cast<Transition *>(arrow->end_object())))
                t->removeArrow(arrow);

            removeItem(arrow);
            delete arrow;
        }
    } else if ((trans = dynamic_cast<Transition *>(item))) {
        foreach (Arrow *arrow, trans->get_arrows()) {
            if ((p = dynamic_cast<Place *>(arrow->start_object())))
                p->removeArrow(arrow);
            else if ((t = dynamic_cast<Transition *>(arrow->start_object())))
                t->removeArrow(arrow);
            if ((p = dynamic_cast<Place *>(arrow->end_object())))
                p->removeArrow(arrow);
            else if ((t = dynamic_cast<Transition *>(arrow->end_object())))
                t->removeArrow(arrow);

            removeItem(arrow);
            delete arrow;
        }
    }
    removeItem(item);
    delete item;
}

/**
 * \brief Kontrola duplicity sipok.
 * \param start Zaciatocny objekt.
 * \param end Koncovy objekt.
 * \return Informacia o duplicite.
 * \retval True v pripade nenajdenia duplicity.
 */
bool Project::no_duplicity(QGraphicsItem *start, QGraphicsItem *end) {
    Place *p;
    Transition *t;

    p = dynamic_cast<Place *>(start);
    t = dynamic_cast<Transition *>(start);
    if (p) {
        foreach(Arrow *arrow, p->get_arrows()) {
           if (arrow->end_object() == end)
               return false;
        }
    } else if (t) {
        foreach(Arrow *arrow, t->get_arrows()) {
           if (arrow->end_object() == end)
               return false;
        }
    }

    return true;
}

/**
 * \brief Nastavi meno objektu.
 * \param object Objekt, ktory sa ma nastavit.
 * \param name Nazov objektu ktory sa ma nastavit. Ak nie je nastavi sa
 * implicitny nazov.
 * \return Potvrdenie nastavenia objektu.
 * \retval True v pripade uspesneho nastavenia, inak False.
 */
bool Project::set_object_name(PNObject * object, QString name) {
    QList <QGraphicsItem *> list = items();
    // ak bolo zadane meno, uzivatel editoval object
    if (! name.isEmpty()) {
        if (name_in_list(object, list, name)) {
            return false;
        } else {
            object->set_name(name);
            return true;
        }
    }

    /**
     * Meno nie je zadane, objekt sa vytvoril. Nastavime implicitne
     * meno. Musime kontrolovat, ci vytvorene meno existuje, ak ano vytvorime
     * dalsie. Mena sa vytvaraju pomocou pismena a pocitadla.
     * Cyklus konci, ked sa vytvorene meno nenachdaza v zozname.
     */
    for (;;) {
        name = create_name(object);
        if (! name_in_list(object, list, name)) {
            object->set_name(name);
            return true;
        }
    }
}

/**
 * \brief Zisti ci sa nachadza objekt so zadanym menom v zozname.
 * \param object objekt, ktory sa nastavuje.
 * \param list Zoznam poloziek.
 * \param name Nazov objektu.
 * \return Potvrdenie najdenia nazvu objektu.
 * \retval True v pripade najdenia nazvu inak False.
 */
bool Project::name_in_list(PNObject * object, QList<QGraphicsItem *> & list,
                  const QString name) {
    if (list.isEmpty())
        return false; //  v zozname nie su objekty

    PNObject *o;
    foreach(QGraphicsItem *item, list) {
        if ((o = dynamic_cast<PNObject *>(item)) && o != object) {
            if (name == o->name())
                return true;
        }
    }

    return false;
}

/**
 * \brief Vytvori nazov pre objekt podla typu objektu.
 * \param object Object pre ktory sa vytvara nazov.
 * \return Nazov objektu.
 * \retval QString Nazov objektu.
 */
const QString Project::create_name(PNObject * object) {
    QString name;
    if (dynamic_cast<Place *>(object)) {
        name = "p" + QString::number(my_pcnt);
        my_pcnt++;
    } else if (dynamic_cast<Transition *>(object)) {
        name = "t" + QString::number(my_tcnt);
        my_tcnt++;
    } else if (dynamic_cast<Arrow *>(object)) {
        name = "x" + QString::number(my_acnt);
        my_acnt++;
    }
    return name;
}

/**
 * \brief Nastavi cestu k ulozenemu suboru.
 * \param path Cesta k suboru.
 */
void Project::set_filepath(const QString & path) {
    my_fileinfo.setFile(path);
    my_saved = true;
}

/**
 * \brief Ziskanie nazvu projektu, pod ktorym bol ulozeny. Nazov je bez suffixu.
 * \return nazov projektu
 */
QString Project::filename() const {
    return my_fileinfo.baseName();
}

/**
 * \brief Ziskanie plnej informacie o ceste k projektu.
 * \return informacie o ceste projektu
 */
const QFileInfo & Project::fileinfo() const {
    return my_fileinfo;
}

/**
 * \brief Ziskanie adresara v ktorom bol projekt ulozeny.
 * \return adresar s projektom
 */
QString Project::filedir() const {
    return my_fileinfo.absolutePath();
}

/**
 * \brief Informacia o tom, ci bol projekt ulozeny (pre Save).
 * \return true, ak projekt uz bol ulozeny.
 */
bool Project::was_saved() const {
    return my_saved;
}

/**
 * \brief Nastavenie verzie projektu podla verzie zo serveru.
 * \param new_version nove cislo verzie.
 */
void Project::set_version(unsigned new_version) {
    my_version = new_version;
}

/**
 * \brief Nastavenie mena projektu podla mena zo serveru.
 * \param new_name nove meno projektu.
 */
void Project::set_servername(const QString & new_name) {
    my_servername = new_name;
}

/**
 * \brief Ziskanie cisla verzie aktualneho projektu.
 * \return cislo verzie
 */
unsigned Project::version() const {
    return my_version;
}

/**
 * \brief Ziskanie mena projektu podla mena zo serveru.
 * \return meno projektu podla servru
 */
const QString & Project::servername() const {
    return my_servername;
}

/**
 * \brief Informacia o tom, ci projekt bol zo servru.
 * \return true ak projekt je zo serveru
 */
bool Project::from_server() const {
    return my_version != 0 && ! my_servername.isEmpty();
}

