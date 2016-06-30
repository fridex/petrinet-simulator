/**
 * \file     connection.cpp
 * \brief    Posielanie poziadavkov od klienta na server a spracovanie odpovedi.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     mar 06 2012
 */

#include <QList>
#include <QtNetwork>
#include <QString>
#include <QObject>
#include <QDebug>

#include <pn/proto.h>
#include <pn/client/connection.h>

const unsigned CONNECTION_TIMEOUT  = 5000;  //!< Doba cakania na spojenie. (ms)
const unsigned CONNECTION_BUFSIZE  =   512;   //!< Velkost bufferu pre odpoved.

/**
 * Inicializacia singletonu.
 */
Connection * Connection::my_instance = 0;

/**
 * \brief Konstruktor pre spojenie.
 */
Connection::Connection() {
    my_error = false;
    my_connected = false;
}

/**
 * \brief Destruktor pre spojenie.
 */
Connection::~Connection() {
}

/**
 * \brief dealokacia alokovaneho singletonu.
 */
void Connection::singleton_destroy() {
    if (my_instance) {
        delete my_instance;
        my_instance = NULL;
    }
}

/**
 * Spristupnenie instancie singletonu pre spojenie
 * @return spristupneny singleton
 */
Connection * Connection::instance() {
    if (! my_instance)
        my_instance = new Connection;

    return my_instance;
}

/**
 * \brief Predikat pre zistenie dostupnosti spojenia.
 * \return true ak uzivatel bol pripojeny
 */
bool Connection::connected() const {
    return my_connected;
}

/**
 * \brief Nastavenie parametrov spojenia.
 * \param host Nazov alebo IP adresa pocitaca so serverom.
 * \param port Cislo portu na ktorom odpocuva server.
 * \retval void
 */
void Connection::set_host(const QString & host, unsigned port) {
    my_host = host;
    my_port = port;
}

/**
 * \brief Nastavenie uzivatelskeho mena, ktore je zasielane servru.
 * \param username Uzivatelske meno.
 * \param password Uzivatelske heslo.
 * \retval void
 */
void Connection::set_username(const QString & username,
                              const QString & password) {
    my_username = username;
    my_password = password;
}

/**
 * \brief Spristupnenie host serveru.
 * \return Nazov serveru, ktory bol nastaveny volanim Connection::set_host()
 * \retval Nazov serveru.
 */
const QString & Connection::host() const {
    return my_host;
}

/**
 * \brief Spristupnenie verzie pridavaneho projektu.
 * \return Cislo verzie pridaneho projektu.
 * \retval Cislo verzie.
 */
unsigned Connection::version() const {
    return my_version;
}

/**
 * \brief Metoda pre ziskanie informacii ci doslo k chybe. Informacie o chybe je
 * mozne spristupnit pomocou Connection::msg().
 * \retval true v pripade, ze nastala chyba pri komunikacii so serverom.
 */
bool Connection::error() const {
    return my_error;
}

/**
 * \brief Spristupnenie mena uzivatela, ktore bolo nastavene pomocou volania
 * Connection::set_username().
 * \return Meno pouzivatela.
 * \retval Meno pouzivatela.
 */
const QString & Connection::username() const {
    return my_username;
}

/**
 * \brief Spristupnenie hesla uzivatela, ktore bolo nastavene pomocou volania
 * Connection::set_username().
 * \return Heslo uzivatela.
 * \retval Heslo pouzivatela.
 */
const QString & Connection::password() const {
    return my_password;
}

/**
 * \brief V pripade, ze ide o dotazovanie poziadavku, ktory ma standardnu
 * odpoved, spristupni informaciu od serveru. V pripade chyby spristupnuje
 * hlasku od serveru. (Polozka "MSG:" v protokole)
 * \return Hlaska od serveru.
 * \retval Hlaska od serveru.
 */
const QString & Connection::msg() const {
    return my_msg;
}

/**
 * \brief Spristupnenie cisla portu.
 * \return Cislo portu.
 * \retval Cislo portu.
 */
unsigned Connection::port() const {
    return my_port;
}

/**
 * \brief V pripade, ze bola poziadavka na XML dokument (RUN, STEP, GET,..),
 * vrati XML dokument zaslany od servru.
 * \return XML dokument
 * \retval XML dokument
 */
const QString & Connection::xml() const {
    return my_xml;
}

/**
 * \brief Zmaze stavove informacie ukladane zo zasielania poziadavku a
 * spracovania odpovedi.
 * \retval void
 */
void Connection::req_clear() {
    my_xml.clear();
    my_msg.clear();
    my_request.clear();
    my_list.clear();
    my_vlist.clear();
    my_simlog.clear();
    my_error = false;
}

/**
 * \brief Zmaze vsetky informacia a inicializuje objekt do stavu pred zadanim
 * akejkolvek informacie ci poziadavky.
 * \retval void
 */
void Connection::all_clear() {
    this->req_clear();

    my_socket.deleteLater();
    my_host.clear();
    my_username.clear();
}

/**
 * \brief Metoda pre zaslanie poziadavku o autentifikaciu na server.
 * \return Informacia o spravnom prevedeni poziadavku.
 * \retval false v pripade chyby.
 */
bool Connection::req_auth() {
    bool rv;

    this->req_clear(); // Odstrani pozostatky predchadzajuceho poziadavku.
    my_connected = true;

    my_request = PROTOH_PN;

    my_request.append(my_username).append(PROTO_EOL);
    my_request.append(PROTOH_DO).append(PROTOR_AUTH);
    my_request.append(PROTOH_PASS).append(my_password).append(PROTO_EOL);
    my_request.append(PROTO_END);

    rv = this->send();

    // Podarilo sa autentizovat?
    my_connected = rv;
    return rv;
}

/**
 * \brief Zasle na server poziadavku pre odhlasenie na server.
 * \return Informacia o spravnom prevedeni poziadavku.
 * \retval false v pripade chyby.
 */
bool Connection::req_logout() {
    this->req_clear(); // Odstrani pozostatky predchadzajuceho poziadavku.
    my_request = PROTOH_PN;

    my_request.append(PROTOH_PASS).append(my_password).append(PROTO_EOL);
    my_request.append(my_username).append(PROTO_EOL);
    my_request.append(PROTOH_DO).append(PROTOR_LOGOUT);
    my_request.append(PROTO_END);

    return this->send();
}

/**
 * \brief Zaslanie poziadavku pre registrovanie uzivatela.
 * \return Informacia o spravnom prevedeni poziadavku.
 * \retval false v pripade chyby.
 */
bool Connection::req_register() {
    this->req_clear(); // Odstrani pozostatky predchadzajuceho poziadavku.
    my_request = PROTOH_PN;
    my_connected = true; // nastavuje sa prve spojenie (prevencia pred chybou)

    my_request.append(my_username).append(PROTO_EOL);
    my_request.append(PROTOH_DO).append(PROTOR_REGISTER);
    my_request.append(PROTOH_PASS).append(my_password).append(PROTO_EOL);
    my_request.append(PROTO_END);

    return this->send();
}

/**
 * \brief Zaslanie poziadavku o spristupnenie zoznamu projektov zo serveru.
 * \return Informacia o spravnom prevedeni poziadavku.
 * \retval false v pripade chyby.
 */
bool Connection::req_list() {
    this->req_clear(); // Odstrani pozostatky predchadzajuceho poziadavku.
    my_request = PROTOH_PN;

    my_request.append(my_username).append(PROTO_EOL);
    my_request.append(PROTOH_PASS).append(my_password).append(PROTO_EOL);
    my_request.append(PROTOH_DO).append(PROTOR_LIST);
    my_request.append(PROTO_END);

    return this->send();
}

/**
 * \brief Zaslanie poziadavku o spristupnenie zoznamu verzii zo serveru.
 * \return Informacia o spravnom prevedeni poziadavku.
 * \retval false v pripade chyby.
 */
bool Connection::req_vlist(QString & pname) {
    this->req_clear(); // Odstrani pozostatky predchadzajuceho poziadavku.
    my_request = PROTOH_PN;

    my_request.append(my_username).append(PROTO_EOL);
    my_request.append(PROTOH_PASS).append(my_password).append(PROTO_EOL);
    my_request.append(PROTOH_DO).append(PROTOR_VLIST);
    my_request.append(PROTOH_NAME).append(pname).append(PROTO_EOL);
    my_request.append(PROTO_END);

    return this->send();
}

/**
 * \brief Metoda pre spristupnenie zoznamu projektov.
 * \return Zoznam projektov na serveri.
 * \retval Zoznam projektov dostupnych na serveri.
 */
ProjectRecord_iter Connection::req_list_begin() {
    return my_list.begin();
}

/**
 * \brief Metoda pre spristupnenie konca zoznamu projektov.
 * \retval Koncovy iterator pre zoznam projektov.
 */
ProjectRecord_iter Connection::req_list_end() {
    return my_list.end();
}

/**
 * \brief Metoda pre spristupnenie poctu prebranych projektov.
 * \return pocet zaznamov v simlogu
 */
unsigned Connection::req_list_size() {
    return my_list.size();
}

/**
 * \brief Metoda pre spristupnenie zoznamu verzii.
 * \return Zoznam verzii na serveri.
 * \retval Zoznam verzii dostupnych na serveri.
 */
VersionRecord_iter Connection::req_vlist_begin() {
    return my_vlist.begin();
}

/**
 * \brief Metoda pre spristupnenie konca zoznamu verzii.
 * \retval Koncovy iterator pre zoznam verzii.
 */
VersionRecord_iter Connection::req_vlist_end() {
    return my_vlist.end();
}

/**
 * \brief Metoda pre spristupnenie poctu prebranych verzii.
 * \return pocet zaznamov v simlogu
 */
unsigned Connection::req_vlist_size() {
    return my_vlist.size();
}

/**
 * \brief Metoda pre spristupnenie poctu zaznamov v simlogu.
 * \return pocet zaznamov v simlogu
 */
unsigned Connection::req_simlog_size() {
    return my_simlog.size();
}

/**
 * \brief Metoda pre spristupnenie informacii z logov.
 * \retval Iterator pre zaznamy z logov.
 */
SimlogRecord_iter Connection::req_simlog_begin() {
    return my_simlog.begin();
}

/**
 * \brief Metoda pre spristupnenie konca informacii z logov.
 * \retval Koncovy iterator pre zoznam verzii.
 */
SimlogRecord_iter Connection::req_simlog_end() {
    return my_simlog.end();
}

/**
 * \brief Zasle poziadavok pre spristupnenie konkretnej verzie daneho projektu.
 * \param name Nazov projektu, ktory ma byt spristupneny.
 * \param version Pozadovana verzia projektu.
 * \return Informacia o spravnom prevedeni poziadavku.
 * \retval false v pripade chyby.
 */
bool Connection::req_get(const QString & name, unsigned version) {
    this->req_clear(); // Odstrani pozostatky predchadzajuceho poziadavku.
    my_request = PROTOH_PN;

    my_request.append(my_username).append(PROTO_EOL);
    my_request.append(PROTOH_PASS).append(my_password).append(PROTO_EOL);
    my_request.append(PROTOH_DO).append(PROTOR_GET);
    my_request.append(PROTOH_NAME).append(name).append(PROTO_EOL);
    my_request.append(PROTOH_VERSION).append(QString::number(version)).append(PROTO_EOL);
    my_request.append(PROTO_END);

    return this->send();
}

/**
 * \brief Zasle poziadavku pre pridanie projektu na server.
 * \param name Nazov pridavaneho projektu.
 * \param desc Popis pridavaneho projektu.
 * \param xml XML format pridavaneho projektu.
 * \return Informacia o spravnom prevedeni poziadavku.
 * \retval false v pripade chyby.
 */
bool Connection::req_add(const QString & name,
                         const QString & desc,
                         const QString & xml) {
    this->req_clear(); // Odstrani pozostatky predchadzajuceho poziadavku.
    my_request = PROTOH_PN;

    my_request.append(my_username).append(PROTO_EOL);
    my_request.append(PROTOH_PASS).append(my_password).append(PROTO_EOL);
    my_request.append(PROTOH_DO).append(PROTOR_ADD);
    my_request.append(PROTOH_NAME).append(name).append(PROTO_EOL);
    my_request.append(PROTOH_DESC).append(desc).append(PROTO_EOL);
    my_request.append(PROTOH_XML).append(xml).append(PROTO_EOL);
    my_request.append(PROTO_END);

    return this->send();
}

/**
 * \brief Zasle poziadavku na server pre simulaciu - prevedeni jedneho kroku.
 * \param xml XML format projektu pre odsimulovanie.
 * \return Informacia o spravnom prevedeni poziadavku.
 * \retval false v pripade chyby.
 */
bool Connection::req_step(const QString & xml) {
    this->req_clear(); // Odstrani pozostatky predchadzajuceho poziadavku.
    my_request = PROTOH_PN;

    my_request.append(my_username).append(PROTO_EOL);
    my_request.append(PROTOH_PASS).append(my_password).append(PROTO_EOL);
    my_request.append(PROTOH_DO).append(PROTOR_STEP);
    my_request.append(PROTOH_XML).append(xml).append(PROTO_EOL);
    my_request.append(PROTO_END);

    return this->send();
}

/**
 * \brief Zaslanie poziadavku na server pre simulaciu (krok) konkretnej verzie.
 * \param xml XML format poziadavku pre simulovanie
 * \param name name Meno projektu pre simulovanie
 * \param version Cislo verzie projektu.
 * \return Informacia o spravnom prevedeni poziadavku.
 * \retval false v pripade chyby.
 */
bool Connection::req_step(const QString & xml,
                          const QString & name,
                          unsigned version) {
    this->req_clear(); // Odstrani pozostatky predchadzajuceho poziadavku.
    my_request = PROTOH_PN;

    my_request.append(my_username).append(PROTO_EOL);
    my_request.append(PROTOH_PASS).append(my_password).append(PROTO_EOL);
    my_request.append(PROTOH_DO).append(PROTOR_STEP);

    // Pokial je projekt zo serveru.
    if (! name.isEmpty() && version != 0) {
        my_request.append(PROTOH_NAME).append(name).append(PROTO_EOL);
        my_request.append(PROTOH_VERSION).append(QString::number(version))
                  .append(PROTO_EOL);
    }

    my_request.append(PROTOH_XML).append(xml).append(PROTO_EOL);
    my_request.append(PROTO_END);

    return this->send();
}

/**
 * \brief Zasle poziadavku na server pre simulaciu - uplnu odsimulaciu projektu.
 * \param xml XML format projektu pre odsimulovanie.
 * \return Informacia o spravnom prevedeni poziadavku.
 * \retval false v pripade chyby.
 */
bool Connection::req_run(const QString & xml) {
    this->req_clear(); // Odstrani pozostatky predchadzajuceho poziadavku.
    my_request = PROTOH_PN;

    my_request.append(my_username).append(PROTO_EOL);
    my_request.append(PROTOH_PASS).append(my_password).append(PROTO_EOL);
    my_request.append(PROTOH_DO).append(PROTOR_RUN);
    my_request.append(PROTOH_XML).append(xml).append(PROTO_EOL);
    my_request.append(PROTO_END);

    return this->send();
}

/**
 * \brief Zaslanie poziadavku na server pre odsimulovanie konkretnej verzie.
 * \param xml XML format poziadavku pre simulovanie
 * \param name name Meno projektu pre simulovanie
 * \param version Cislo verzie projektu.
 * \return Informacia o spravnom prevedeni poziadavku.
 * \retval false v pripade chyby.
 */
bool Connection::req_run(const QString & xml,
                         const QString & name,
                         unsigned version) {
    this->req_clear(); // Odstrani pozostatky predchadzajuceho poziadavku.
    my_request = PROTOH_PN;

    my_request.append(my_username).append(PROTO_EOL);
    my_request.append(PROTOH_PASS).append(my_password).append(PROTO_EOL);
    my_request.append(PROTOH_DO).append(PROTOR_RUN);

    // Pokial je projekt zo serveru.
    if (! name.isEmpty() && version != 0) {
        my_request.append(PROTOH_NAME).append(name).append(PROTO_EOL);
        my_request.append(PROTOH_VERSION).append(QString::number(version))
                  .append(PROTO_EOL);
    }

    my_request.append(PROTOH_XML).append(xml).append(PROTO_EOL);
    my_request.append(PROTO_END);

    return this->send();
}

/**
 * \brief Zaslanie poziadavku pre ziskanie zaznamu z logovacieho suboru simulacii
 * \param pname Nazov projektu pre spristupnenie logu
 * \param version Konkretna verzia projektu pre spristupnenie logu
 * \return Informacia o spravnom prevedeni poziadavku.
 * \retval false v pripade chyby.
 */
bool Connection::req_simlog(const QString & pname, unsigned version) {
    this->req_clear(); // Odstrani pozostatky predchadzajuceho poziadavku.
    my_request = PROTOH_PN;

    my_request.append(my_username).append(PROTO_EOL);
    my_request.append(PROTOH_PASS).append(my_password).append(PROTO_EOL);
    my_request.append(PROTOH_DO).append(PROTOR_SIMLOG);
    my_request.append(PROTOH_NAME).append(pname).append(PROTO_EOL);
    my_request.append(PROTOH_VERSION).append(QString::number(version)).append(PROTO_EOL);
    my_request.append(PROTO_END);

    return this->send();
}

/**
 * \brief Metoda pre spracovanie odpovedi od servru
 * \return Informacia o spravnom spracovani odpovedi.
 * \retval false v pripade chyby.
 */
bool Connection::parse() {
    QByteArray line;
    bool parsed = false; // informacia o spracovani poziadavku (return)

    for (;;) {
        line = my_socket.readLine();

        if (! qstrcmp(line.data(), PROTO_END)) {
            break;
        } else if (! qstrcmp(line.data(), PROTOH_XML)) {
            line = my_socket.readLine();
            while ((my_xml.mid(my_xml.length() - 2) != "\n\n"
                    && my_xml.mid(my_xml.length() - 4) != "\r\n\r\n")
                    && my_socket.isReadable()) {
                my_xml.append(line);
                line = my_socket.readLine();
            }
            parsed = true;
            break;

        } else if (! qstrcmp(line.data(), PROTOH_LIST)) {
            ProjectRecord proj;

            line = my_socket.readLine();
            while (qstrcmp(line.data(), PROTO_END)) {
                if (! qstrncmp(line.data(), PROTOH_NAME, qstrlen(PROTOH_NAME))) {
                    proj.name = line.mid(qstrlen(PROTOH_NAME));
                    proj.name.resize(qstrlen(proj.name.toAscii()) - 2);
                } else {
                    my_msg = QObject::tr("Malformed answer");
                    my_error = true;
                    return false;
                }
                my_list.push_back(proj);
                line = my_socket.readLine();
            }

            parsed = true;
            break;

        } else if (! qstrcmp(line.data(), PROTOH_VLIST)) {
            VersionRecord ver;

            line = my_socket.readLine();
            while (qstrcmp(line.data(), PROTO_END)) {
                if (! qstrncmp(line.data(), PROTOH_DESC, qstrlen(PROTOH_DESC))) {
                    ver.desc = line.mid(qstrlen(PROTOH_DESC));
                    ver.desc.resize(qstrlen(ver.desc.toAscii()) - 2);
                } else {
                    my_msg = QObject::tr("Malformed answer");
                    my_error = true;
                    return false;
                }

                line = my_socket.readLine();

                if (! qstrncmp(line.data(), PROTOH_USER, qstrlen(PROTOH_USER))) {
                    ver.user =  line.mid(qstrlen(PROTOH_USER));
                    ver.user.resize(qstrlen(ver.user.toAscii()) - 2);
                } else {
                    my_msg = QObject::tr("Malformed answer");
                    my_error = true;
                    return false;
                }

                line = my_socket.readLine();

                if (! qstrncmp(line.data(), PROTOH_TIME, qstrlen(PROTOH_TIME))) {
                    line.chop(2);
                    ver.time =  line.mid(qstrlen(PROTOH_TIME)).toUInt();
                } else {
                    my_msg = QObject::tr("Malformed answer");
                    my_error = true;
                    return false;
                }

                my_vlist.push_front(ver);
                line = my_socket.readLine();
            }

            parsed = true;
            break;

        } else if (! qstrcmp(line.data(), PROTOH_SIMLOG)) {
            SimlogRecord log;

            line = my_socket.readLine();
            while (qstrcmp(line.data(), PROTO_END)) {
                if (! qstrncmp(line.data(), PROTOH_TIME, qstrlen(PROTOH_TIME))) {
                    line = line.mid(qstrlen(PROTOH_TIME));
                    line.resize(qstrlen(line) - 2);
                    log.time = line.toUInt();
                } else {
                    my_msg = QObject::tr("Malformed answer");
                    my_error = true;
                    return false;
                }

                line = my_socket.readLine();

                if (! qstrncmp(line.data(), PROTOH_USER, qstrlen(PROTOH_USER))) {
                    log.user =  line.mid(qstrlen(PROTOH_USER));
                    log.user.resize(qstrlen(log.user.toAscii()) - 2);
                } else {
                    my_msg = QObject::tr("Malformed answer");
                    my_error = true;
                    return false;
                }

                my_simlog.push_back(log);
                line = my_socket.readLine();
            }

            parsed = true;
            break;

        } else if (! qstrncmp(line.data(), PROTOH_DO, qstrlen(PROTOH_DO))) {
            parsed = true;
            if (! qstrcmp(line.mid(qstrlen(PROTOH_DO)).data(), PROTOR_OK)) {
                line = my_socket.readLine();
                if (! qstrncmp(line.data(), PROTOH_MSG, qstrlen(PROTOH_MSG))) {
                    my_msg = line.mid(qstrlen(PROTOH_MSG));
                    my_msg.resize(qstrlen(my_msg.toAscii()) - 2);// Odstrani \r\n
                } else {
                    my_error = true;
                    my_msg = QObject::tr("Malformed answer");
                    return false;
                }
            } else if (! qstrcmp(line.mid(qstrlen(PROTOH_DO)).data(), PROTOR_BAD)) {
                line = my_socket.readLine();
                my_error = true;
                if (! qstrncmp(line.data(), PROTOH_MSG, qstrlen(PROTOH_MSG))) {
                    my_msg = line.mid(qstrlen(PROTOH_MSG));
                    my_msg.resize(qstrlen(my_msg.toAscii()) - 2);// Odstrani \r\n
                } else {
                    my_msg = QObject::tr("Malformed answer");
                    return false;
                }
            } else {
                my_error = true;
                my_msg = QObject::tr("Malformed answer");
                return false;
            }

        } else if (! qstrncmp(line.data(), PROTOH_ADD, qstrlen(PROTOH_ADD))) {
            bool ok;

            parsed = true;
            my_msg = line.mid(qstrlen(PROTOH_ADD));

            line = my_socket.readLine();
            line.chop(2);

            if (qstrncmp(line.data(), PROTOH_VERSION, qstrlen(PROTOH_VERSION))) {
                my_error = true;
                my_msg = QObject::tr("Malformed answer");
                return false;
            }

            my_version = line.mid(qstrlen(PROTOH_VERSION)).toUInt(&ok);

            if (! ok) {
                my_error = true;
                my_msg = QObject::tr("Malformed answer");
                return false;
            }

        }
    }

    if (! parsed) {
        my_error = true;
        my_msg = QObject::tr("Unable to parse answer.");
    }

    return parsed;
}

/**
 * \brief Metoda pre zaslanie zostaveneho poziadavku na server.
 * \return Informacia o spravnom zaslani poziadavku.
 * \retval false v pripade chyby.
 */
bool Connection::send() {
    my_error = false;

    if (! my_connected) {
        my_error = true;
        my_msg = QObject::tr("Not connected.");
        return false;
    }

    my_socket.connectToHost(my_host, my_port);

    if (my_socket.waitForConnected(CONNECTION_TIMEOUT)) {
        qDebug() << "Connected to host:" << my_host << ":" << my_port;

        my_socket.write(my_request.toAscii());
        my_socket.flush();

        if (my_socket.waitForReadyRead(CONNECTION_TIMEOUT)) {
            qDebug() << "Recieving answer";
            this->parse();
        } else {
            my_error = true;
            my_msg = QObject::tr("Unable to receive answer.");
        }
    } else {
        my_error = true;
        my_msg = QObject::tr("Unable to connect to host.");
    }

    my_socket.disconnectFromHost();
    qDebug() << "Disconnected from host.";

    return ! my_error;
}

