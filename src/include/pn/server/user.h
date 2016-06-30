/**
 * \file     user.h
 * \brief    Implementacia triedy reprezentujucu uzivatela.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     feb 26 2012
 */

#ifndef PN_USER_H_
#define PN_USER_H_

// forward
class QString;

/**
 * \brief Trieda reprezentujuca uzivatela a uchovanie dolezitych informacii.
 */
class User {
  private:
    QString my_username;    //!< Uzivatelske meno.
    QString my_password;    //!< Uzivatelove meno.

  public:
    User(const QString & username, const QString & password);
    virtual ~User();

    void set_password(const QString & passwd);
    void set_username(const QString & username) __attribute__ ((deprecated));

    const QString & username();
    const QString & password();

  private:
    /**
     * \brief DISABLE_COPY_AND_ASSIGN
     */
    User(const User &);
    /**
     * \brief DISABLE_COPY_AND_ASSIGN
     */
    void operator=(const User &);
}; // User

#endif // PN_USER_H_

