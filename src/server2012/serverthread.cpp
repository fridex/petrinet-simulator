/**
 * \file     serverthread.cpp
 * \brief    Implementacia vlakna pre poziadavok na serveri.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     mar 15 2012
 */

#include <QThread>
#include <QTcpSocket>

#include <pn/server/answer.h>
#include <pn/server/message.h>
#include <pn/server/simulation.h>
#include <pn/server/debug.h>

#include <pn/server/serverthread.h>

/**
 * \brief Konstruktor pre vlakno spracovavajuce poziadavok na serveri.
 * \param socket_desc Socket deskriptor pre spracovanie odpovedi
 * \param parent Ukazatel na server.
 */
ServerThread::ServerThread(int socket_desc, QObject * parent)
    : QThread(parent) {
    my_socket_desc = socket_desc;
    my_server = static_cast<Server*>(parent);
}

/**
 * \brief Destruktor pre vlakno spracovavajuce poziadavok na serveri.
 */
ServerThread::~ServerThread() {
}

/**
 * \brief Spustenie samostatneho vlakna na serveri.
 * \retval void
 */
void ServerThread::run() {
    QTcpSocket socket;
     if (!socket.setSocketDescriptor(my_socket_desc)) {
         return;
     }

     debug("Waiting for client to ready");
     if (socket.waitForReadyRead()) {
        // Vybavenie dotazovanej poziadavky
        this->handle_request(socket);
    } else {
        debug("Connection time out");
    }

     debug("Request handled");
     socket.disconnectFromHost();
     //socket.waitForDisconnected();
}

/**
 * \brief Metoda pre rozparsovanie a vybavenie poziadavku od klienta.
 * \param socket Socket z ktoreho sa zadana poziadavka bude parsovat.
 * \retval void
 */
void ServerThread::handle_request(QTcpSocket & socket) {
    QString result; // vysledok v pripade simulacie.
    Simulation * sim;
    unsigned version;

    Message * msg = new Message();
    Answer * msg_back = new Answer();

    debug("Parsing request...");
    if (msg->socket(&socket)) {
        debug("Request parsed");
        if (msg->type() != REQ_REGISTER
            && (! my_server->exist_user(msg->username())
                || ! my_server->verify_user(msg->username(), msg->password()))) {
            msg_back->set_standard(ANSWER_BAD_AUTH);
            debug("Bad AUTH");
        } else {
            switch (msg->type()) {
                case REQ_AUTH:
                    msg_back->set_standard(ANSWER_OK_AUTH);
                    debug("AUTH");
                    break;

                case REQ_LOGOUT:
                    msg_back->set_standard(ANSWER_OK_LOGOUT);
                    debug("LOGOUT");
                    break;

                case REQ_REGISTER:
                    if (! my_server->exist_user(msg->username())) {
                        msg_back->set_standard(ANSWER_OK_REGISTER);
                        my_server->add_user(msg->username(), msg->password());
                        my_server->update_userdb(msg->username(),
                                                 msg->password());
                        debug("REG");
                    } else {
                        msg_back->set_standard(ANSWER_BAD_REGISTER);
                        debug("Bad REG");
                    }
                    break;

                case REQ_LIST:
                    msg_back->set_list(my_server->projects());
                    debug("LIST");
                    break;

                case REQ_VLIST:
                    if (! my_server->exist_project(msg->project())) {
                        msg_back->set_standard(ANSWER_UNKNOWN);
                        debug("Bad VLIST");
                    } else {
                        msg_back->set_vlist(my_server->projects(),
                                            msg->project());
                        debug("VLIST");
                    }
                    break;

                case REQ_GET:
                    if (my_server->exist_project(msg->project())) {
                        msg_back->set_xml(my_server->projects(),
                                          msg->project(),
                                          msg->version());
                        debug("GET");
                    } else {
                        msg_back->set_standard(ANSWER_UNKNOWN);
                        debug("Bad GET");
                    }
                    break;

                case REQ_ADD:
                    version = my_server->add_project(msg->project(),
                                                     msg->username(),
                                                     msg->desc(),
                                                     msg->xml());
                    if (version != 0) {
                            msg_back->set_add(version);
                            debug("ADD");
                    } else {
                        msg_back->set_standard(ANSWER_INTERNAL_ERR);
                        debug("Bad ADD");
                    }
                    break;

                case REQ_STEP:
                    if (! msg->project().isEmpty()) {
                        if (! my_server->update_simlog(msg->username(),
                                                       msg->project(),
                                                       msg->version())) {
                            debug("Failed to update SIMLOG");
                        }
                    }
                    sim = new Simulation;

                    if (! sim->prepare(msg->xml())) {
                        msg_back->set_standard(ANSWER_BAD_XML);
                        debug("Bad XML STEP");
                    } else if (sim->step(result)) {
                        msg_back->set_xml(result);
                        debug("STEP");
                    } else {
                        msg_back->set_error(sim->error());
                        debug("Bad STEP");
                    }

                    delete sim;
                    break;

                case REQ_RUN:
                    if (! msg->project().isEmpty()) {
                        if (! my_server->update_simlog(msg->username(),
                                                       msg->project(),
                                                       msg->version())) {
                            debug("E: Failed to update SIMLOG");
                        }
                    }
                    sim = new Simulation;

                    if (! sim->prepare(msg->xml())) {
                        msg_back->set_standard(ANSWER_BAD_XML);
                        debug("Bad XML STEP");
                    } else if (sim->run(result)) {
                        msg_back->set_xml(result);
                        debug("STEP");
                    } else {
                        msg_back->set_error(sim->error());
                        debug("Bad STEP");
                    }

                    delete sim;
                    break;

                case REQ_SIMLOG:
                    if (my_server->exist_project(msg->project(),
                                                 msg->version())) {
                        msg_back->set_simlog(my_server->projects(),
                                             msg->project(),
                                             msg->version());
                    } else {
                        msg_back->set_standard(ANSWER_UNKNOWN);
                        debug("Bad SIMLOG");
                    }
                    break;

                case REQ_NULL:
                    /* WALKTHRU */
                default:
                    msg_back->set_standard(ANSWER_BAD_REQ);
                    debug("E: Unprocessed request");
                    break;
            }
        }
    } else {
        debug("Bad request");
        msg_back->set_standard(ANSWER_BAD_REQ);
    }

    // Zasli spravu spat uzivatelovi.
    socket.write(msg_back->text().toAscii());
    socket.flush();
    socket.waitForBytesWritten();

    // Odstran soket.
    socket.deleteLater();

    delete msg;
    delete msg_back;
}

