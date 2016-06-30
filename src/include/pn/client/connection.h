/**
 * \file     connection.h
 * \brief    Posielanie poziadavkov od klienta na server a spracovanie odpovedi.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     mar 06 2012
 */

#ifndef PN_CLIENT_CONNECTION_H_
#define PN_CLIENT_CONNECTION_H_

#include <QList>
#include <QString>
#include <QTcpSocket>

#include <pn/proto.h>

/**
 * \brief Struktura uklada informacie o jednotlivych projektoch z poziadavku
 * LIST.
 */
struct ProjectRecord {
    QString name;
};

/**
 * \brief Struktura uklada informacie o jednotlivych verziach z poziadavku
 * VLIST.
 */
struct VersionRecord {
    QString desc;
    QString user;
    unsigned time;
};

/**
 * \brief Struktura uklada informacie z logov z poziadavku SIMLOG.
 */
struct SimlogRecord {
    QString user;
    unsigned time;
};

/**
 * Navratove hodnoty iteratorov pre spristupnenie hodnot.
 */
typedef QList<ProjectRecord>::iterator ProjectRecord_iter;
typedef QList<VersionRecord>::iterator VersionRecord_iter;
typedef QList<SimlogRecord>::iterator  SimlogRecord_iter;

/**
 * \brief Singleton zabezpecujucI komunikaciu so serverom - zostavovanie
 * poziadavkov a spracovanie odpovedi zo strany servru.
 */
class Connection {
  public:
    void singleton_destroy();

    static Connection * instance();

    void set_host(const QString & host, unsigned port);
    void set_username(const QString & username, const QString & password);

    const QString & host() const;
    unsigned port() const;
    unsigned version() const;
    const QString & username() const;
    const QString & password() const;
    const QString & msg() const;
    const QString & xml() const;
    bool connected() const;

    void req_clear();
    void all_clear();
    bool error() const;

    unsigned req_list_size();
    ProjectRecord_iter req_list_begin();
    ProjectRecord_iter req_list_end();

    unsigned req_vlist_size();
    VersionRecord_iter req_vlist_begin();
    VersionRecord_iter req_vlist_end();

    unsigned req_simlog_size();
    SimlogRecord_iter  req_simlog_begin();
    SimlogRecord_iter  req_simlog_end();

    bool req_auth();
    bool req_logout();
    bool req_register();
    bool req_list();
    bool req_vlist(QString & pname);
    bool req_get(const QString & name, unsigned version);
    bool req_add(const QString & name,
                 const QString & desc,
                 const QString & xml);
    bool req_step(const QString & xml);
    bool req_step(const QString & xml, const QString & name, unsigned version);
    bool req_run(const QString & xml);
    bool req_run(const QString & xml, const QString & name, unsigned version);
    bool req_simlog(const QString & pname, unsigned version);

  private:
    Connection();
    ~Connection();

    static Connection * my_instance;

    QString my_username;
    QString my_password;
    QString my_host;
    unsigned my_port;
    QString my_msg;
    bool my_error;
    bool my_connected;
    unsigned my_version;

    QString my_request;
    QString my_xml;
    QTcpSocket my_socket;
    QList<ProjectRecord> my_list;
    QList<VersionRecord> my_vlist;
    QList<SimlogRecord> my_simlog;


    bool parse();
    bool send();
}; // Connection

#endif // PN_CLIENT_CONNECTION_H_

