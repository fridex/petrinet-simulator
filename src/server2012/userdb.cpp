/**
 * \file     userdb.cpp
 * \brief    Implementacia uzivatelskej databazy.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     feb 26 2012
 */

#include <map>

#include <QString>

#include <pn/server/user.h>
#include <pn/server/userdb.h>

/**
 * \brief Konstruktor databazy uzivatelov.
 */
UserDB::UserDB() {
}

/**
 * \brief Destruktor databazy uzivatelov pre uvolnenie systemovych zdrojov.
 */
UserDB::~UserDB() {
    for (std::map<QString, User*>::iterator it = my_users.begin();
         it != my_users.end(); ++it) {
        delete it->second;
    }
}

/**
 * \brief Metoda pre pridanie uzivatela do databazy.
 * \param username Meno novopridaneho uzivatela.
 * \param password Heslo novopridavajuceho uzivatela.
 * \return Do databazy nie je mozne pridat uz existujuceho uzivatela.
 * \retval true ak je mozne pridat uzivatela do databazy a nedojde k duplicite.
 */
bool UserDB::add_user(const QString & username, const QString & password) {
    if (this->exist(username))
        return false;

    User * new_user = new User(username, password);
    my_users[username] = new_user;
    return true;
}

/**
 * \brief Zistenie, ci sa zadany uzivatel s uzivatelskym menom v databaze
 *        nachadza.
 * \param username Meno uzivatela pre zistenie existencie v databaze.
 * \return Informacia o existencii zaznamu uzivatela v databaze.
 * \retval true ak uzivatel sa v databaze nachadza,
 */
bool UserDB::exist(const QString & username) {
    return my_users.count(username) == 1;
}

#if 0
/**
 * \brief Zmena mena uzivatela v databaze uzivatelov.
 * \param username_old Stare meno uzivatela.
 * \param username_new Nove meno uzivatela.
 * \retval void
 * \throws const char * Pokial sa uzivatel v databaze nenachadza, je generovana
 *         vynimka.
 */
void UserDB::set_username(const QString & username_old,
                                 const QString & username_new) {
    if (my_users.count(username_old) == 0)
        throw "Unable to set change username for unknown user!";

    my_users[username_old]->set_username(username_new);
}
#endif // 0

/**
 * \brief Zmena hesla uzivatela.
 * \param username Meno uzivatela, ktoremu sa zmeni heslo.
 * \param password Nove heslo pre uzivatela.
 * \retval void
 * \throws const char * Pokial sa uzivatel v databaze nenachadza, je generovana
 *         vynimka.
 */
void UserDB::set_password(const QString & username,
                                 const QString & password) {
    if (my_users.count(username) == 0)
        throw "Unable to set password for unknown user!";

    my_users[username]->set_password(password);
}

/**
 * \brief Zistenie hesla uzivatela z databaze. Heslo je ulozene v otvorenej
 *        podobe.
 * \param username Meno uzivatela na ktoreho heslo sa dotazuje,
 * \throws const char * Pokial sa uzivatel v databaze nenachadza, je generovana
 *         vynimka.
 */
const QString & UserDB::password(const QString & username) {
    if (my_users.count(username) == 0)
        throw "Unable to change password for unknown user!";

    return my_users[username]->password();
}

/**
 * \brief Overenie platnosti hesla pre zadaneho uzivatela.
 * \param username Meno uzivatela na ktoreho heslo sa ma overit.
 * \param password Overovane heslo uzivatela.
 * \return Informacia o tom, ci zadane heslo suhlasi s ulozenym.
 * \retval bool true ak sa hesla zhoduju.
 * \throws const char * Pokial sa uzivatel v databaze nenachadza, je generovana
 *         vynimka.
 */
bool UserDB::verify(const QString & username,
                           const QString & password) {
    if (my_users.count(username) == 0)
        throw "Unable to verify unknown user!";

    return my_users[username]->password() == password;
}

