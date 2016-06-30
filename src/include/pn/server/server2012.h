/**
 * \file     server2012.h
 * \brief    hlavny subor pre server2012
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     feb 29 2012
 */

#ifndef PN_SERVER_SERVER2012_H_
#define PN_SERVER_SERVER2012_H_

#include <QTcpServer>
#include <QSemaphore>

#include <pn/server/projectdb.h>
#include <pn/server/userdb.h>

// forwards
class QTcpSocket;

/**
 * \brief Trieda reprezentujuca server.
 */
class Server : public QTcpServer {
    Q_OBJECT

  private:
    // Potrebne databazy
    ProjectDB my_projects;
    UserDB my_users;
    QSemaphore my_sem_projdb, my_sem_userdb, my_sem_simlog;
    // Nazov suboru, ktory sa pouzije pre ukladanie uzivatelov a ich prvotne
    // nacitanie.
    const char * my_userdb;

  protected:
    void incomingConnection(int socketDescriptor);

  public:
    Server(unsigned port, const char * userdb,
           const char * projectdb, QObject * parent = 0);
    virtual ~Server();
    bool loop();
    bool update_userdb(const QString & username, const QString & password);
    void load_userdb();

    bool exist_user(const QString & username);
    bool verify_user(const QString & username, const QString & password);
    bool exist_project(const QString & pname, unsigned version = 1);
    bool add_user(const QString & username, const QString & password);
    ProjectDB & projects();
    bool add_project(const QString & pname,
                     const QString & username,
                     const QString & desc,
                     const QString & xml);

    bool update_simlog(const QString & username,
                       const QString & pname,
                       unsigned version);

  private:
    /**
     * \brief DISABLE_COPY_AND_ASSIGN
     */
    Server(const Server &);
    /**
     * \brief DISABLE_COPY_AND_ASSIGN
     */
    void operator=(const Server &);
}; // Server

void server2012(unsigned port, const char * userdb, const char * projectdb);

#endif // PN_SERVER_SERVER2012_H_

