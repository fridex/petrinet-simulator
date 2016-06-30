/**
 * \file     user.cpp
 * \brief    Implementacia triedy reprezentujucu uzivatela.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     feb 26 2012
 */

#include <QString>

#include <pn/server/user.h>

/**
 * \brief Konstruktor pre uzivatela.
 * \param username Uzivatelske meno.
 * \param password Uzivatelske heslo.
 */
User::User(const QString & username,
                  const QString & password) {
    my_username = username;
    my_password = password;
}

/**
 * \brief Destruktor uzivatela.
 */
User::~User() {
}

/**
 * \brief Spristupnenie mena konkretneho uzivatela.
 * \return Meno uzivatela.
 * \retval const QString & Meno pouzivatela.
 */
const QString & User::username() {
    return my_username;
}

/**
 * \brief Spristupnenie hesla konkretneho uzivatela.
 * \return Heslo pouzivatela.
 * \retval const QString & Meno pouzivatela.
 */
const QString & User::password() {
    return my_password;
}

/**
 * \brief Zmena hesla pouzivatela.
 * \param password Nove heslo uzivatela.
 * \retval void
 */
void User::set_password(const QString & password) {
    my_password = password;
}

/**
 * \brief Zmena uzivatelskeho mena
 * \param username Nove meno pouzivatela.
 * \retval void
 * \warning Pozor moze narusit konzistenciu s ProjectVersion, kde sa uchovava
 * meno pouzivatela priamo.
 */
void User::set_username(const QString & username) {
    my_username = username;
}

