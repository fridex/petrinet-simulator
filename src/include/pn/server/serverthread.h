/**
 * \file     serverthread.h
 * \brief    Implementacia vlakna pre poziadavok na serveri.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     mar 15 2012
 */

#ifndef PN_SERVER_SERVERTHREAD_H_
#define PN_SERVER_SERVERTHREAD_H_

#include <QThread>

#include <pn/server/server2012.h>

/**
 * \brief Trieda pre vlakno spravujuce spojenie na servri.
 */
class ServerThread : public QThread {
    Q_OBJECT

  private:
    int my_socket_desc;
    Server * my_server;

    void handle_request(QTcpSocket & socket);

  public:
    ServerThread(int socket_desc, QObject * parent);
    virtual ~ServerThread();

    void run();

  private:
    /**
     * \brief DISABLE_COPY_AND_ASSIGN
     */
    ServerThread(const ServerThread &);
    /**
     * \brief DISABLE_COPY_AND_ASSIGN
     */
    void operator=(const ServerThread &);
}; // ServerThread

#endif // PN_SERVER_SERVERTHREAD_H_

