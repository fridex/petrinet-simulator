/**
 * \file     place.h
 * \brief    Trieda pre reprezentaciu miesta.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     apr 07 2012
 */

#ifndef PN_PLACE_H_
#define PN_PLACE_H_

#include <QPointF>
#include <QGraphicsEllipseItem>
#include <QList>
#include <QVector>

#include <pn/pnobject.h>
#include <pn/arrow.h>

// forward
class QString;

/**
 * \brief Trieda pre reprezentaciu miesta.
 */
class Place : public QGraphicsEllipseItem, public PNObject {
  public:
    /**
     * \brief Styl vykreslovaneho prvku.
     */
    enum PlaceStyle {
        Classic,
        Colored,
        Bold,
        BoldColored
    };

    Place(QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
    virtual ~Place();

    qreal place_size() const;

    void removeArrow(Arrow *arrow);
    void removeArrows();
    void addArrow(Arrow *arrow);
    const QRectF & rect() const;
    const QList<Arrow *> & get_arrows() const;

    static PlaceStyle style();
    virtual void xml(QString & data) const;

    void add_active_token(int token);
    void add_passive_token(int token);

    QVector<int> * active_tokens();
    QVector<int> * passive_tokens();

    void flush_tokens();

    static void set_style(PlaceStyle style);
    static void set_style(int style);
    void apply_style();
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                      QWidget *);
  protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

  private:
      static PlaceStyle my_style;
      QRectF my_place;              //<! Stvorec s miestom.
      qreal my_place_size;          //<! Rozmer hrany stvorca obalujuceho miesto
      QList<Arrow *> my_arrows;     //<! Zoznam ukazatelov na sipky.
      QVector<int> * my_active_tokens;
      QVector<int> * my_passive_tokens;

}; // Place

#endif  // PN_PLACE_H_

