/**
 * \file     place.cpp
 * \brief    Trieda pre reprezentaciu miesta.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     apr 07 2012
 */

#include <QString>
#include <QPointF>
#include <QtGui>

#include <cmath>

#include <pn/place.h>
#include <pn/arrow.h>
#include <pn/transition.h>

/**
 * Nastavenie triedneho atributu pre styl.
 */
Place::PlaceStyle Place::my_style = Place::Classic;

/**
 * \brief Konstruktor.
 */
Place::Place(QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsEllipseItem(parent, scene) {

    set_value("");
    setToolTip("Tokens: " + value());
    my_place_size = 40;
    my_place.setRect(0,0,40,40);
    setRect(my_place);
    apply_style();

    my_active_tokens = 0;
    my_passive_tokens = 0;
}

/**
 * \brief Destruktor.
 */
Place::~Place() {
    if (my_active_tokens)
        delete my_active_tokens;

    if (my_passive_tokens)
        delete my_passive_tokens;
}


/**
 * \brief Odstranenie sipky miesta.
 * \param arrow sipka
 */
void Place::removeArrow(Arrow *arrow) {
    int index = my_arrows.indexOf(arrow);

    if (index != -1)
        my_arrows.removeAt(index);
}

/**
 * \brief Pridanie sipky prechodu.
 * \param arrow sipka
 */
void Place::addArrow(Arrow *arrow) {
    my_arrows.append(arrow);
}

/**
 * \brief Spristupnenie miesta.
 * \return vracia rovnobeznik v ktorom sa nachadza miesto
 */
const QRectF & Place::rect() const {
    return my_place;
}

/**
 * \brief Spristupnenie stylu miesta.
 * \return Aktualne nastaveny styl vykreslovania miesta.
 */
Place::PlaceStyle Place::style() {
    return my_style;
}

/**
 * \brief Vytvorenie XML reprezentacie miesta.
 * \param data XML reprezentacia miesta
 */
void Place::xml(QString & data) const {
    QXmlStreamWriter writer(&data);

    data.append("  ");
    writer.writeStartElement("place");
    writer.writeAttribute("point_x", QString::number(this->x()));
    writer.writeAttribute("point_y", QString::number(this->y()));
    writer.writeAttribute("name", this->name());

    if (my_active_tokens) {
    // Ide o simulaciu na serveri, je nutne zapisat vysledok
        QString val;

        for (QVector<int>::iterator it = my_active_tokens->begin();
                it != my_active_tokens->end();
                ++it) {
            val.append(QString::number((*it)));
            if (it + 1 != my_active_tokens->end())
                val.append(",");
        }

        writer.writeAttribute("value", val);

    } else {
        writer.writeAttribute("value", this->value());
    }

    writer.writeEndElement();  // place
    data.append('\n');
}

/**
 * \brief Nastavenie noveho stylu miesta pre vykreslovanie.
 * \param new_style novy styl pre vykreslovanie
 */
void Place::set_style(PlaceStyle new_style) {
    my_style = new_style;
}

/**
 * \brief Nastavenie noveho stylu miesta pre vykreslovanie.
 * \param new_style novy styl pre vykreslovanie
 */
void Place::set_style(int new_style) {
    my_style = static_cast<PlaceStyle>(new_style);
}

/**
 * \brief Spristupni zoznam sipiek.
 * \return Zoznam sipiek.
 */
const QList<Arrow *> & Place::get_arrows() const {
    return my_arrows;
}

/**
 * \brief Aplikuje nastavenia podla stylu.
 */
void Place::apply_style() {
    QColor color = Qt::black;
    qreal pen_width = 2;
    setBrush(Qt::white);

    if (my_style == Colored || my_style == BoldColored) {
        setBrush(Qt::cyan);
    }

    if (my_style == Bold || my_style == BoldColored) {
        pen_width = 5;
    }

    setPen(QPen(color, pen_width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

/**
 * \brief Prida token do zoznamu aktivnych tokenov.
 * \param token token pre pridanie
 */
void Place::add_active_token(int token) {
    if (! my_active_tokens)
        my_active_tokens = new QVector<int>;

    my_active_tokens->push_back(token);
}

/**
 * \brief Prida token do zoznamu neaktivnych tokenov.
 * \param token token pre pridanie
 */
void Place::add_passive_token(int token) {
    if (! my_passive_tokens)
        my_passive_tokens = new QVector<int>;

    my_passive_tokens->push_back(token);
}

/**
 * \brief Spristupnenie tokenov v simulacii - tokeny, ktore su dostupne v
 * simulacii.
 */
QVector<int> * Place::active_tokens() {
    if (! my_active_tokens)
        my_active_tokens = new QVector<int>;

    return my_active_tokens;
}

/**
 * \brief Spristupnenie tokenov v simulacii - tokeny, ktore su nedostupne v
 * simulacii.
 */
QVector<int> * Place::passive_tokens() {
    if (! my_active_tokens)
        my_passive_tokens = new QVector<int>;

    return my_passive_tokens;
}

/**
 * \brief Premiestnenie vsetkych pasivnych tokenov k pasivnym tokenom.
 */
void Place::flush_tokens() {
    if (! my_passive_tokens || my_passive_tokens->size() == 0)
        return;

    if (! my_active_tokens)
        my_active_tokens = new QVector<int>;

    while (my_passive_tokens->size() != 0) {
        my_active_tokens->push_back(my_passive_tokens->first());
        my_passive_tokens->pop_front();
    }
}

/**
 * \brief Pri zmene objektu sa zavola funkcie na prekreslenie.
 * \param change Typ zmeny.
 * \param value Datatyp Qt
 * \return Datatyp Qt.
 */
QVariant Place::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change > 0) {
        foreach (Arrow *arrow, my_arrows) {
            arrow->updatePosition();
        }
    }

    return value;
}

/**
 * \brief Reimplementacia metody pre vykreslenie miesta.
 * \param painter Ukazatel na kresliaci nastroj.
 */
void Place::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                  QWidget *) {
    /* nastavenie pera pre painter */
    painter->setPen(pen());
    painter->setBrush(brush());


    QRectF bdrec = boundingRect();
    QPointF center_point = bdrec.center();
    QPointF plus_point = QPointF(40/sqrt(8), 40/sqrt(8));
    QRectF rect = QRectF(center_point - plus_point, center_point + plus_point);


    QPointF pos = QPointF(my_place_size + 5.0, my_place_size/2.0);
    painter->drawText(pos, name());
    painter->drawEllipse(this->rect());
    painter->drawText(rect, Qt::AlignCenter | Qt::TextWordWrap, value());
}

/**
 * \brief Spristupni rozmer miesta.
 * \return Hodnota hrany obalujuceho stvorca miesta.
 */
qreal Place::place_size() const {
    return my_place_size;
}

