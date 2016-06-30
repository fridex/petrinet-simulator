/**
 * \file     pnlist.cpp
 * \brief    Trieda pre transparentnu pracu s objektami petriho siete.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     apr 07 2012
 */
#include <QList>
#include <QString>
#include <QPointF>
#include <QDebug>
#include <QXmlStreamReader>
#include <QGraphicsTextItem>

#include <pn/pnlist.h>
#include <pn/pnobject.h>
#include <pn/arrow.h>
#include <pn/place.h>
#include <pn/transition.h>

const char * XML_START  = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<pn>\n";
const char * XML_END    = "</pn>\n";

/**
 * \brief Atributy a nazvy tagov v XML.
 */
const char * XML_START_TOKEN  = "pn";
const char * XML_ARROW        = "arrow";
const char * XML_PLACE        = "place";
const char * XML_TRANSITION   = "transition";

const char * XML_START_X      = "point_x";
const char * XML_START_Y      = "point_y";
const char * XML_NAME         = "name";
const char * XML_FROM         = "from";
const char * XML_MODE         = "mode";
const char * XML_COND         = "condition";
const char * XML_PRIOR        = "priority";
const char * XML_TO           = "to";
const char * XML_VALUE        = "value";

/**
 * \brief Konstruktor.
 */
PNList::PNList() {
    my_transition_count = 0;
}

/**
 * \brief Destruktor.
 */
PNList::~PNList() {
    this->clear();
}

/**
 * \brief Pridanei prvku petriho siete na koniec zoznamu.
 * \param obj objekt, ktory ma byt pridany do zoznamu
 */
void PNList::push_back(PNObject * obj) {
    if (obj)
        my_list.push_back(obj);
}

/**
 * \brief Spristupnenie zoznamu.
 * \retval zoznam PNObjektov.
 */
PNList_items & PNList::items() {
    return my_list;
}

PNList_iter PNList::begin() {
    return my_list.begin();
}

PNList_iter PNList::end() {
    return my_list.end();
}

/**
 * \brief Metoda pre spristupnenie chybovej hlasky.
 * \return spristupnena chybova hlaska
 */
const QString & PNList::error() const {
    return my_error;
}

/**
 * \brief Pridanie prvku pretruho siete na zaciatok zoznamu.
 * \param obj objekt, ktory ma byt pridany do zoznamu
 */
void PNList::push_front(PNObject * obj) {
    if (obj)
        my_list.push_front(obj);
}

/**
 * \brief Metoda pre spracovanie atributov sipky a vlozenie rozparsovaneho prvku
 * do zoznamu objektov petriho sieti.
 * \param attributes atributy sipky, ktore sa maju spracovat
 * \return false v pripade chybnych atributov
 */
bool PNList::parse_arrow(ADepList & alist,
                         const QXmlStreamAttributes & attributes) {
    Arrow * arrow = new Arrow();

    my_list.push_back(arrow);

    /*
     * Pri parsovani sipky je problem so zavislostou. Pokial je meno na
     * odkazujuci objekt vo from alebo to este nenacitane, dojde k chybe.
     * Preto treba nacitat cely dokument a zavislosti na sipkach dorobit.
     */

    arrow->set_name(attributes.value(XML_NAME).toString());
    if (arrow->name().isEmpty())
        return false;

    ArrowDep * list_item = new ArrowDep;

    list_item->arrow = arrow;
    list_item->from = attributes.value(XML_FROM).toString();
    list_item->to = attributes.value(XML_TO).toString();

    alist.push_back(list_item);

    if (list_item->from.isEmpty())
        return false;

    if (list_item->to.isEmpty())
        return false;

    return true;
}

/**
 * \brief Metoda pre spracovanie atributov miesta a vlozenie rozparsovaneho
 * prvku do zoznamu objektov petriho sieti.
 * \param attributes atributy sipky, ktore sa maju spracovat
 * \return false v pripade chybnych atributov
 */
bool PNList::parse_place(const QXmlStreamAttributes & attributes, bool server) {
    bool ok;
    Place * place = new Place();

    my_list.push_back(place);

    place->setX(attributes.value(XML_START_X).toString().toInt(&ok));
    if (! ok)
        return false;
    place->setY(attributes.value(XML_START_Y).toString().toInt(&ok));
    if (! ok)
        return false;

    place->set_name(attributes.value(XML_NAME).toString());
    if (place->name().isEmpty())
        return false;

    place->set_value(attributes.value(XML_VALUE).toString());
    place->setToolTip("Tokens: " + place->value());

    // Prevod tokenov do pola.
    if (server) {
        QStringList list = place->value().split(",");
        QString num;
        bool ok;

        foreach (const QString &str, list) {
            if (str.isEmpty())
                continue;

            num = str.simplified();
            place->add_active_token(num.toInt(&ok));

            if (! ok) // Zly token - nie je cislo.
                return false;
        }

        place->set_value("");
    }

    return true;
}

/**
 * \brief Metoda pre spracovanie atributov prechodu a vlozenie rozparsovaneho
 * prvku do zoznamu objektov petriho sieti.
 * \param attributes atributy sipky, ktore sa maju spracovat
 * \return false v pripade chybnych atributov
 */
bool PNList::parse_transition(const QXmlStreamAttributes & attributes) {
    bool ok;
    Transition * transition = new Transition();

    my_list.push_back(transition);

    transition->setX(attributes.value(XML_START_X).toString().toInt(&ok));
    if (! ok)
        return false;
    transition->setY(attributes.value(XML_START_Y).toString().toInt(&ok));
    if (! ok)
        return false;

    transition->set_priority(attributes.value(XML_PRIOR).toString().toInt(&ok));
    if (! ok)
        return false;

    transition->set_name(attributes.value(XML_NAME).toString());
    if (transition->name().isEmpty())
        return false;

    transition->set_condition(attributes.value(XML_COND).toString());

    transition->set_mode(attributes.value(XML_MODE).toString());

    transition->set_active(true);


    transition->setToolTip("Condition: " + transition->condition() +
                           "\nMode: " + transition->mode() + "\nPriority: " +
                           QString::number(transition->priority()));

    return true;
}

/**
 * \brief Rozparsovanie a vytvorenie prvkov petriho siete, ktore budu vlozene do
 * zoznamu na zaklade XML suboru.
 * \param data XML reprezentacia petriho siete
 * \param server informacia o tom, ci parsovanie je na serveri
 * \return v pripade chybnej petriho siete false
 */
bool PNList::from_xml(const QString & data, bool server) {
    QXmlStreamReader xml(data);
    ADepList alist;

    my_error.clear();

    while (! xml.atEnd() && ! xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartDocument)
            continue;

        if (xml.name() == XML_START_TOKEN)
            continue;

        if (xml.isStartElement()) {
            if (xml.name() == XML_ARROW) {
                if (! parse_arrow(alist, xml.attributes())) {
                    this->clear();
                    qDebug() << "Parsing arrow failed.";
                    my_error = QObject::tr("Parsing arrow failed.");
                    return false;
                }
            } else if (xml.name() == XML_PLACE) {
                if (! parse_place(xml.attributes(), server)) {
                    this->clear();
                    qDebug() << "Parsing place failed.";
                    my_error = QObject::tr("Parsing place failed.");
                    return false;
                }
            } else if (xml.name() == XML_TRANSITION) {
                my_transition_count++;
                if (! parse_transition(xml.attributes())) {
                    this->clear();
                    qDebug() << "Parsing transition failed.";
                    my_error = QObject::tr("Parsing transition failed.");
                    return false;
                }
            } else {
                this->clear();
                qDebug() << "Unknown xml element type.";
                my_error = QObject::tr("Unknown xml element type.");
                return false;
            }
        }
        else if (xml.hasError()) {
            this->clear();
            qDebug() << xml.lineNumber() << ":" << xml.columnNumber();
            qDebug() << xml.errorString();
            my_error = xml.errorString();
            return false;
        }
    }

    /*
     * Zoznam je vytvoreny z XML, ale treba nastavit zavislosti medzi sipkou a
     * ostatnymi objektami v svene.
     */
    return this->compose(alist);
}


/**
 * \brief Zistenie a vytvorenie zavislosti medzi sipkou a ostatnymi objektami.
 * \return false v pripade, ze XML ma nekonzistentne zavislosti
 */
bool PNList::compose(ADepList & alist) {
    Place * p;
    Transition *t;

    // Prehlada vsetky objekty a pokial sa nazov rovna referencovanemu, nastavi
    // from a to objekt sipky.
    for (ADepList_iter it = alist.begin(); it != alist.end(); ++it) {
        for (PNList_iter jt = my_list.begin();
                jt != my_list.end(); ++jt) {

            if ((*jt)->name() == (*it)->from) {
                (*it)->arrow->set_start_object((*jt));

                if ((p = dynamic_cast<Place*>(*jt))) {
                    p->addArrow((*it)->arrow);
                } else if ((t = dynamic_cast<Transition*>(*jt))) {
                    t->addArrow((*it)->arrow);
                } else {
                    // Sipka je smerovana z sipky.
                    qDebug() << "Name error in objects - arrow name!";
                    return false;
                }

            } else if ((*jt)->name() == (*it)->to) {
                (*it)->arrow->set_end_object((*jt));

                if ((p = dynamic_cast<Place*>(*jt))) {
                    p->addArrow((*it)->arrow);
                } else if ((t = dynamic_cast<Transition*>(*jt))) {
                    t->addArrow((*it)->arrow);
                } else {
                    // Sipka je smerovana do sipky.
                    qDebug() << "Name error in objects - arrow name!";
                    return false;
                }
            }

        }

        // Nenasli sa objekty, ktore boli odkazovane -> chyba.
        if (! (*it)->arrow->start_pnobject()
                || ! (*it)->arrow->end_pnobject()) {
            qDebug() << "Name error in objects - name not found.";
            return false;
        }
    }

    return true;
}

/**
 * \brief Vytvorenie XML reprezentacie aktualne ulozenej petriho siete v
 * zozname.
 * \param data XML reprezentacia petriho siete.
 */
void PNList::xml(QString & data) {
    QString xml;

    data.clear();
    data.append(XML_START);
    for (PNList_iter it = my_list.begin(); it != my_list.end(); ++it) {
        (*it)->xml(data);
    }

    data.append(XML_END);
}

/**
 * \brief Odstranenie a dealokacia vsetkych prvkov petriho siete v zozname.
 */
void PNList::clear() {
    for (PNList_iter it = my_list.begin(); it != my_list.end();) {
        delete * it;
        it = my_list.erase(it);
    }
}

/**
 * \brief Pocet prechodov v zozname.
 * \return pocet prechodov v zozname.
 */
unsigned PNList::transition_count() {
    return my_transition_count;
}

