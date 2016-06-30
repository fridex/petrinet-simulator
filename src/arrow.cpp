/**
 * \file     arrow.cpp
 * \brief    Trieda pre reprezentaciu sipky.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     apr 07 2012
 */

#include <QString>
#include <QPointF>
#include <QtGui>
#include <QTransform>

#include <pn/arrow.h>
#include <pn/place.h>
#include <pn/transition.h>
#include <cmath>

const qreal Pi = 3.14;

/**
 * Nastavenie triedneho atributu pre styl.
 */
Arrow::ArrowStyle Arrow::my_style = Arrow::Classic;

/**
 * \brief Konstruktor.
 */
Arrow::Arrow(QGraphicsItem *start, QGraphicsItem *end,
             QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsLineItem(parent, scene) {
    my_start_object = start;
    my_end_object = end;
    apply_style();
}

/**
 * \brief Destruktor.
 */
Arrow::~Arrow(){
}

/**
 * \brief Spristupnenie pociatocneho objektu sipky.
 * \return Ukazatel na pociatocny objekt sipky.
 */
QGraphicsItem * Arrow::start_object() {
    return my_start_object;
}

/**
 * \brief Spristupnenie koncoveho objektu sipky.
 * \return Ukazatel na koncovy objekt sipky.
 */
QGraphicsItem * Arrow::end_object() {
    return my_end_object;
}

/**
 * \brief Nastavenie tvaru hlavicky sipky.
 * \return Tvar sipky.
 */
QPainterPath Arrow::shape() const {
    QPainterPath path = QGraphicsLineItem::shape();
    path.addPolygon(my_arrow_head);
    return path;
}

/**
 * \brief Reimplementacia obalujuceho obdlznika sipky.
 * \return Obdlznik obalujuci sipku.
 */
QRectF Arrow::boundingRect() const {
    qreal extra = (pen().width() + 20) / 2.0;

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                      line().p2().y() - line().p1().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}


/**
 * \brief Aktualizuje vykreslenie sipky.
 */
void Arrow::updatePosition() {
    QLineF line(mapFromItem(my_start_object, 0, 0),
                mapFromItem(my_end_object, 0, 0));
    setLine(line);
}

/**
 * \brief Nastavenie pociatocneho objektu sipky.
 * \param new_start pociatocny objekt sipky.
 */
void Arrow::set_start_object(PNObject * new_start) {
    my_start_object = dynamic_cast<QGraphicsItem *>(new_start);
}

/**
 * \brief Nastavenie koncoveho objektu sipky.
 * \param new_end koncovy objekt sipky.
 */
void Arrow::set_end_object(PNObject * new_end) {
    my_end_object = dynamic_cast<QGraphicsItem *>(new_end);
}

/**
 * \brief Spristupnenie pociatocneho objektu sipky.
 * \return Pociatocny objekt sipky.
 */
PNObject * Arrow::start_pnobject() {
    return dynamic_cast<PNObject *>(my_start_object);
}

/**
 * \brief Spristupnenie koncoveho objektu sipky.
 * \return Koncovy objekt sipky
 */
PNObject * Arrow::end_pnobject() {
    return dynamic_cast<PNObject *>(my_end_object);
}

/**
 * \brief Spristupnenie aktualne nastaveneho stylu sipky pre vykreslovanie.
 * \return Nastaveny styl vykreslovania
 */
Arrow::ArrowStyle Arrow::style() {
    return my_style;
}

/**
 * \brief Vytvorenie XML reprezentacie z aktualnych parametrov.
 * \param data XML reprezentacia sipky
 */
void Arrow::xml(QString & data) const {
    QXmlStreamWriter writer(&data);

    PNObject * from = dynamic_cast<PNObject *>(my_start_object);
    PNObject * to = dynamic_cast<PNObject *>(my_end_object);

    data.append("  ");
    writer.writeStartElement("arrow");
    writer.writeAttribute("from", from->name());
    writer.writeAttribute("to", to->name());
    writer.writeAttribute("name", this->name());
    writer.writeEndElement();  // arrow
    data.append('\n');
}

/**
 * \brief Zmena stylu sipky.
 * \param style Novy styl sipky.
 */
void Arrow::set_style(Arrow::ArrowStyle style) {
    my_style = style;
}

/**
 * \brief Zmena stylu sipky.
 * \param style Novy styl sipky.
 */
void Arrow::set_style(int style) {
    set_style(static_cast<ArrowStyle>(style));
}

/**
 * \brief Reimplementacia metody pre vykreslenie sipky.
 * \param painter Ukazatel na kresliaci nastroj.
 */
void Arrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                 QWidget *) {

    /* ak objekty koliduju, nevykreslujeme lebo sipku nie je vydiet */
    if (my_start_object->collidesWithItem(my_end_object))
        return;

    QPointF arrow_p1, arrow_p2; // body pre vykreslenie sipok
    double angle;
    QPointF p1, p2;

    /* nastavenie pera pre painter */
    painter->setPen(pen());
    painter->setBrush(pen().color());

    /**
     * Nasleduje kod pre ziskanie pociatocneho a koncoveho bodu sipky
     */
    /* ziskame obdlzniky v ktorych sa nachadzaju vykreslene objekty */
    QRectF r1 = my_start_object->sceneBoundingRect();
    QRectF r2 = my_end_object->sceneBoundingRect();

    /* vytvorime spojnicu stredov obdlznikov */
    QLineF center_line(r1.center(), r2.center());

    /* ziskame body pre vykreslenie sipky */
    p1 = get_intersect_point(center_line, my_start_object);
    p2 = get_intersect_point(center_line, my_end_object);

    setLine(QLineF(p2, p1));

    if (line().length() != 0)
        angle = ::acos(line().dx() / line().length());
    else
        angle = 0;

    if (line().dy() >= 0)
        angle = (Pi * 2) - angle;

    arrow_p1 = line().p1() + QPointF(sin(angle + Pi / 3) * my_arrow_size,
                                     cos(angle + Pi / 3) * my_arrow_size);
    arrow_p2 = line().p1() + QPointF(sin(angle + Pi - Pi / 3) * my_arrow_size,
                                     cos(angle + Pi - Pi / 3) * my_arrow_size);

    my_arrow_head.clear();
    my_arrow_head << line().p1() << arrow_p1 << arrow_p2;

    /* vypocet pozicie popisku s nazvom a vykreslenie */
    if (line().length() != 0) {
        QPointF pos = (line().p2() + line().p1()) / 2.0;
        painter->drawText(pos.x() + 5.0, pos.y() - 5.0, name());
    }

    painter->drawLine(line());
    painter->drawPolygon(my_arrow_head);
}

/**
 * \brief Vrati priesecnik spojnice stredov a hrany prechodu
 * \param line Spojnica stredov.
 * \param rect Ohranicujuci obdlznik.
 */
QPointF Arrow::get_intersect_point(const QLineF & line, QGraphicsItem *item) {
    Place *place;
    QPointF cpoint;
    QPointF p1, p2;
    QLineF rect_line;
    QLineF::IntersectType intersect_type;
    QPointF intersect_point;

    if ((place = dynamic_cast<Place *>(item))) {
        qreal size = place->place_size() / 2.0;
        cpoint = item->sceneBoundingRect().center();
        if (line.length() != 0) {
            qreal cos_val = size * (line.dx() / line.length());
            qreal sin_val = size * (line.dy() / line.length());
            if (my_end_object == item) {
                cos_val *= -1;
                sin_val *= -1;
            }
            return QPointF(cpoint.x() + cos_val,
                           cpoint.y() + sin_val);
        } else
            return cpoint;
    } else {
        QPolygonF polygon = QPolygonF(item->sceneBoundingRect());
        p1 = polygon.first();

        for (int i = 1; i < polygon.count(); ++i) {
            p2 = polygon.at(i);
            rect_line = QLineF(p1, p2);
            intersect_type = rect_line.intersect(line, &intersect_point);
            if (intersect_type == QLineF::BoundedIntersection)
                return intersect_point;
            p1 = p2;
        }
        return p1;
    }
}

/**
 * \brief Aplikuje nastavenia podla stylu.
 */
void Arrow::apply_style() {
    QColor color = Qt::black;
    qreal pen_width = 2;
    my_arrow_size = 10;

    if (my_style == Colored || my_style == BoldColored) {
        color = Qt::darkGray;
    }
    if (my_style == Bold || my_style == BoldColored) {
        pen_width = 5;
        my_arrow_size = 20;
    }

    setPen(QPen(color, pen_width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

