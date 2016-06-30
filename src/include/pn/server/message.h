/**
 * \file     message.h
 * \brief    Rozparsovanie poziadavku od klienta a spristupnenie jednotlivych
 *           poloziek
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     mar 03 2012
 */

#ifndef PN_SERVER_MESSAGE_H_
#define PN_SERVER_MESSAGE_H_

#include <pn/proto.h>

// forwards
class QTcpSocket;
class QString;
class QByteArray;

/**
 * \brief Trieda reprezentujuca rozparsovanu spravu z daneho socketu.
 */
class Message {
  private:
    enum Req_type my_type;
    QString my_username;

    QString my_password;
    QString my_project;
    QString my_desc;
    unsigned my_version;
    unsigned my_version_stated;  // Len pre dodatocnu kontrolu v case parsovania.
    QString my_xml;

    QString my_error;

    bool parse(QTcpSocket * socket);
    bool check();

  public:
    Message();
    virtual ~Message();

    enum Req_type type() const;
    const QString & username() const;
    const QString & password() const;
    const QString & project() const;
    unsigned version() const;
    const QString & desc() const;
    const QString & xml() const;
    const QString & error() const;

    bool socket(QTcpSocket * socket);

  private:
    /**
     * \brief DISABLE_COPY_AND_ASSIGN
     */
    Message(const Message &);
    /**
     * \brief DISABLE_COPY_AND_ASSIGN
     */
    void operator=(const Message &);
}; // Message

#endif // PN_SERVER_MESSAGE_H_

