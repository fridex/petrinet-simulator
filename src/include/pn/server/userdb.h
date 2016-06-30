/**
 * \file     userdb.h
 * \brief    Implementacia uzivatelskej databazy.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     feb 26 2012
 */

#ifndef PN_USERDB_H_
#define PN_USERDB_H_

// forward
class QString;
class User;

/**
 * \brief Objekt uzivatelska databaza.
 */
class UserDB {
  private:
    std::map<QString, User*> my_users;  //!< Zaznam o jednotlivych uzivateloch.

  public:
    UserDB();
    virtual ~UserDB();


    bool add_user(const QString & username, const QString & password);

    bool exist(const QString & username);
#if 0
    void set_username(const QString & username_old, const QString & username_new);
#endif // 0
    void set_password(const QString & username, const QString & password);

    const QString & password(const QString & username);

    bool verify(const QString & username, const QString & password);

  private:
    /**
     * \brief DISABLE_COPY_AND_ASSIGN
     */
    UserDB(const UserDB &);
    /**
     * \brief DISABLE_COPY_AND_ASSIGN
     */
    void operator=(const User &);

}; // UserDB

#endif // PN_USERDB_H_

