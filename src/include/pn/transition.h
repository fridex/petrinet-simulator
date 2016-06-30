/**
 * \file     transition.h
 * \brief    Trieda pre reprezentaciu prechodu.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     apr 07 2012
 */

#ifndef PN_TRANSITION_H_
#define PN_TRANSITION_H_

#include <QPointF>
#include <QGraphicsRectItem>
#include <QList>

#include <pn/pnobject.h>
#include <pn/arrow.h>
#include <pn/transition.h>

// forward
class QString;

/**
 * \brief Trieda pre reprezentaciu prechodu.
 */
class Transition : public QGraphicsRectItem, public PNObject {
  public:
    /**
     * \brief Styl vykreslovaneho prvku.
     */
    enum TransitionStyle {
        Classic,
        Colored,
        Bold,
        BoldColored
    };

    Transition(QGraphicsItem *parent = 0,
               QGraphicsScene *scene = 0);
    virtual ~Transition();

    const QRectF & rect() const;
    void removeArrow(Arrow *arrow);
    void addArrow(Arrow *arrow);
    QList<Arrow *> & get_arrows();
    const QList<Arrow *> & get_arrows() const;
    QList<Arrow *> & arrows();

    const QString & condition() const;
    const QString & mode() const;
    const qreal & width() const;
    const qreal & height() const;
    static TransitionStyle style();
    virtual void xml(QString & data) const;

    int priority() const;
    void set_priority(int prior);

    bool active();
    void set_active(bool act);

    static void set_style(TransitionStyle style);
    static void set_style(int style);
    void set_condition(const QString & con);
    void set_mode(const QString & mode);
    void apply_style();
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                       QWidget *);

  protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

  private:
    static TransitionStyle my_style;
    QRectF my_transition;
    qreal my_width;             //<! Sirka obdlznika prechodu
    qreal my_height;            //<! Vyska obdlznika prechodu.
    QList<Arrow *> my_arrows;   //<! Zoznam ukazatelov na sipky.
    QString my_condition;       //<! Podmienka prechodu.
    QString my_mode;            //<! Mod prechodu.
    int my_priority;            //<! Priorita prechodu pre simulaciu.
    bool my_active;             //<! Informacia, ci prechod uz bol odsimulovany.

}; // Transition

#endif  // PN_TRANSITION_H_

