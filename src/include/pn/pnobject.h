/**
 * \file     pnobject.h
 * \brief    Abstraktna trieda pre prvky petriho sieti.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     apr 07 2012
 */

#ifndef PN_PNOBJECT_H_
#define PN_PNOBJECT_H_

// forward
class QString;
class QPointF;

/**
 * \brief Abstraktna trieda pre prvky petriho sieti.
 */
class PNObject {
  public:
    PNObject();
    virtual ~PNObject();

    const QString & name() const;
    const QString & value() const;
    virtual void xml(QString & data) const = 0;

    void set_name(const QString & new_name);
    void set_value(const QString & new_value);

  private:
    QString my_name;       //!< Nazov objektu.
    QString my_value;      //!< Hodnota, ktoru objekt nesie.
}; // PNObject

#endif  // PN_PNOBJECT_H_

