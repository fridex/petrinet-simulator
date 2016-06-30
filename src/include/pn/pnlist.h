/**
 * \file     pnlist.h
 * \brief    Trieda pre transparentnu pracu s objektami petriho siete.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     apr 07 2012
 */

#ifndef PN_PNLIST_H_
#define PN_PNLIST_H_

#include <QList>
#include <QString>

// forward
class PNObject;
class QXmlStreamAttributes;
class Arrow;

/**
 * Iterator zoznamu PNObject.
 */
typedef QList<PNObject *>::iterator PNList_iter;

/**
 * Zoznam PNObject.
 */
typedef QList<PNObject *> PNList_items;

/**
 * \brief Trieda pre transparentnu pracu s objektami petriho siete.
 */
class PNList {
  private:
    struct ArrowDep {
        Arrow * arrow;
        QString from;
        QString to;
    };
    typedef QList<ArrowDep *> ADepList;
    typedef QList<ArrowDep *>::iterator ADepList_iter;

    QList<PNObject *> my_list;
    QString my_error;
    unsigned my_transition_count;

    bool parse_arrow(ADepList & alist, const QXmlStreamAttributes & attributes);
    bool parse_place(const QXmlStreamAttributes & attributes, bool server);
    bool parse_transition(const QXmlStreamAttributes & attributes);

  public:
      PNList();
      ~PNList();

      void push_back(PNObject *);
      void push_front(PNObject *);

      PNList_iter begin();
      PNList_iter end();

      bool from_xml(const QString & xml, bool server = false);
      void xml(QString & xml);
      const QString & error() const;
      unsigned transition_count();

      PNList_items & items();

      bool compose(ADepList & alist);

      void clear();
}; // PNList

#endif // PN_PNLIST_H_

