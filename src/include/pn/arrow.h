/**
 * \file     arrow.h
 * \brief    Trieda pre reprezentaciu sipky.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     apr 07 2012
 */

#ifndef PN_ARROW_H_
#define PN_ARROW_H_

#include <QPointF>
#include <QGraphicsLineItem>
#include <QGraphicsItem>

#include <pn/pnobject.h>

// forward
class QString;

/**
 * \brief Trieda reprezentujuca sipku.
 */
class Arrow : public QGraphicsLineItem, public PNObject {
  public:
    /**
     * \brief Styl vykreslovaneho prvku.
     */
    enum ArrowStyle {
        Classic,
        Colored,
        Bold,
        BoldColored
    };

    Arrow(QGraphicsItem *start = 0, QGraphicsItem *end = 0,
          QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
    virtual ~Arrow();

    QGraphicsItem * start_object();
    QGraphicsItem * end_object();
    void updatePosition();
    QRectF boundingRect() const;

    void set_start_object(PNObject * new_start);
    void set_end_object(PNObject * new_end);
    PNObject * start_pnobject();
    PNObject * end_pnobject();

    static ArrowStyle style();
    virtual void xml(QString & data) const;

    static void set_style(ArrowStyle style);
    static void set_style(int style);
    void apply_style();

    QPainterPath shape() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                      QWidget *);

  private:
      static ArrowStyle my_style;

      QPolygonF my_arrow_head;
      QGraphicsItem *my_start_object;
      QGraphicsItem *my_end_object;
      qreal my_arrow_size;

      QPointF get_intersect_point(const QLineF & line, QGraphicsItem *item);
}; // Arrow

#endif  // PN_ARROW_H_

