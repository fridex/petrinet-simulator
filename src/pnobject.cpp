/**
 * \file     pnobject.cpp
 * \brief    Abstraktna trieda pre prvky petriho sieti.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     apr 07 2012
 */

#include <QString>

#include <pn/pnobject.h>

/**
 * \brief Konstruktor.
 */
PNObject::PNObject() {
}

/**
 * \brief Destruktor.
 */
PNObject::~PNObject() {
}

/**
 * \brief Spristupnenie mena objektu.
 * \return meno objektu
 */
const QString & PNObject::name() const {
    return my_name;
}

/**
 * \brief Spristupnenie hodnoty, ktoru objekt nesie.
 * \return hodnota objektu
 */
const QString & PNObject::value() const {
    return my_value;
}

/**
 * \brief Nastavenie noveho mena objektu.
 * \param new_name nove meno objektu
 */
void PNObject::set_name(const QString & new_name) {
    my_name = new_name;
}

/**
 * \brief Nastavenie novej hodnoty objektu.
 * \param new_value nova hodnota objektu
 */
void PNObject::set_value(const QString & new_value) {
    my_value = new_value;
}

