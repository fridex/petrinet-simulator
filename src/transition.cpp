/**
 * \file     transition.cpp
 * \brief    Trieda pre reprezentaciu prechodu.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     apr 07 2012
 */

#include <QtGui>
#include <QString>
#include <QPointF>

#include <pn/transition.h>
#include <pn/arrow.h>
#include <pn/place.h>

/**
 * Nastavenie triedneho atributu pre styl.
 */
Transition::TransitionStyle Transition::my_style = Transition::Classic;

/**
 * \brief Konstruktor.
 * \param parent Predchodca triedy Transition.
 * \param scene Scena v ktorej sa nachadza.
 */
Transition::Transition(QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsRectItem(parent, scene) {
    my_width = 100;
    my_height = 50;
    my_transition.setRect(0, 0, my_width, my_height);
    setRect(my_transition);
    apply_style();
    my_priority = 1;
    set_condition("");
    set_mode("");
    setToolTip("Condition: " + condition() + "\nMode: " + mode() +
               "\nPriority: " + QString::number(priority()));
}

/**
 * \brief Destruktor.
 */
Transition::~Transition() {
}

/**
 * \brief Vrati sirku obdlznika prechodu.
 * \return Hodnota sirky prechodu.
 */
const qreal & Transition::width() const {
    return my_width;
}

/**
 * \brief Vyska obdlznika prechodu.
 * \return Hodnota vysky prechodu.
 */
const qreal & Transition::height() const {
    return my_height;
}

/**
 * \brief Spristupnenie prechodu.
 * \return Rovnobeznik prechodu.
 */
const QRectF & Transition::rect() const {
    return my_transition;
}

/**
 * \brief Odstranenie sipky prechodu.
 * \param arrow sipka
 */
void Transition::removeArrow(Arrow *arrow) {
    int index = my_arrows.indexOf(arrow);

    if (index != -1)
        my_arrows.removeAt(index);
}

/**
 * \brief Pridanie sipky prechodu.
 * \param arrow sipka
 */
void Transition::addArrow(Arrow *arrow) {
    my_arrows.append(arrow);
}

/**
 * \brief Spristupnenie zoznamu sipok.
 * \return zoznam sipok
 */
QList<Arrow *> & Transition::arrows() {
    return my_arrows;
}

/**
 * \brief Spristupnenie stylu prechodu.
 * \return nastaveny styl prechodu
 */
Transition::TransitionStyle Transition::style() {
    return my_style;
}

/**
 * \brief Vytvorenie XML reprezentacie prechodu.
 * \param data XML reprezentacia prechodu.
 */
void Transition::xml(QString & data) const {
    QXmlStreamWriter writer(&data);

    data.append("  ");
    writer.writeStartElement("transition");
    writer.writeAttribute("point_x", QString::number(this->x()));
    writer.writeAttribute("point_y", QString::number(this->y()));
    writer.writeAttribute("name", this->name());
    writer.writeAttribute("condition", this->my_condition);
    writer.writeAttribute("mode", this->my_mode);
    writer.writeAttribute("priority", QString::number(this->my_priority));
    writer.writeEndElement();  // transition

    data.append('\n');
}

/**
 * \brief Spristupnenie priority.
 * \return Hodnota priority.
 */
int Transition::priority() const {
    return my_priority;
}

/**
 * \brief Nastavenie priority prechodu.
 * \param prior priorita prechodu
 */
void Transition::set_priority(int prior) {
    my_priority = prior;
}

/**
 * \brief Nastavenie stylu prechodu.
 * \brief new_style Styl pre nastavenie.
 */
void Transition::set_style(TransitionStyle new_style) {
    my_style = new_style;
}

/**
 * \brief Nastavenie stylu prechodu.
 * \param new_style Styl pre nastavenie.
 */
void Transition::set_style(int new_style) {
    my_style = static_cast<TransitionStyle>(new_style);
}

/**
 * \brief Spristupni zoznam sipiek.
 *
 */
QList<Arrow *> & Transition::get_arrows() {
    return my_arrows;
}

/**
 * \brief Spristupni zoznam sipiek.
 * \return Zoznam sipiek.
 */
const QList<Arrow *> & Transition::get_arrows() const {
    return my_arrows;
}

/**
 * \brief Vrati hodnotu podmienky.
 * \return Retazec s hodnotou podmienky.
 */
const QString & Transition::condition() const {
    return my_condition;
}

/**
 * \brief Vrati hodnotu modu.
 * \return Retazec s hodnotou modu prechodu.
 */
const QString & Transition::mode() const {
    return my_mode;
}

/**
 * \brief Nastavi hodnotu podmienky.
 * \param con Hodnota podmienky.
 */
void Transition::set_condition(const QString & con) {
    my_condition = con;
}

/**
 * \brief Nastavi hodnotu modu.
 * \param mode Hodnota modu.
 */
void Transition::set_mode(const QString & mode) {
    my_mode = mode;
}

/**
 * \brief Aplikuje nastavenia podla stylu.
 */
void Transition::apply_style() {
    QColor color = Qt::black;
    qreal pen_width = 2;
    setBrush(Qt::white);

    if (my_style == Colored || my_style == BoldColored) {
        setBrush(Qt::yellow);
    }

    if (my_style == Bold || my_style == BoldColored) {
        pen_width = 5;
    }

    setPen(QPen(color, pen_width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

/**
 * \brief Metoda pre zistenie, ci bol prechod odsimulovany.
 * \return Potvrdenie odsimulovanie.
 * \retval True v pripade, ze bol prechod odsimulovany, inak False.
 */
bool Transition::active() {
    return my_active;
}

/**
 * \brief Metoda pre nastavenie informacie o odsimulovani prechodu.
 * \param act booleovska hodnota s informaciou o simulacii
 */
void Transition::set_active(bool act) {
    my_active = act;
}

/**
 * \brief Pri zmene objektu sa zavola metoda pre aktualiaciu sipky.
 * \param change Typ zmeny.
 * \param value Qt Datatyp.
 * \return Qt Datatyp.
 */
QVariant Transition::itemChange(GraphicsItemChange change,
        const QVariant &value) {
    if (change > 0) {
        foreach (Arrow *arrow, my_arrows) {
            arrow->updatePosition();
        }
    }

    return value;
}

/**
 * \brief Reimplementacia metody pre vykreslenie prechodu.
 * \param painter Ukazatel na kresliaci nastroj.
 */
void Transition::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                  QWidget *) {

    /* nastavenie pera pre painter */
    painter->setPen(pen());
    painter->setBrush(brush());
    painter->drawRect(rect());

    /* pero pre vykreslenie ciary oddelujucu podmienku a mod */
    QPen pen(Qt::black, 2);
    painter->setPen(pen);
    QLineF line(5, 25, 95, 25);
    painter->drawLine(line);

    QRectF rec_con = QRectF(1,1, 99, 24);
    QRectF rec_mod = QRectF(1,26, 99, 24);
    painter->drawText(rec_con, Qt::AlignCenter, condition());
    painter->drawText(rec_mod, Qt::AlignCenter, mode());

    /* nastavenie pozicie popisku */
    QPointF pos = QPointF(my_width + 5.0, my_height / 2.0);
    painter->drawText(pos, name());
}

