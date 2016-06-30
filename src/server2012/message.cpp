/**
 * \file     message.cpp
 * \brief    Rozparsovanie poziadavku od klienta a spristupnenie jednotlivych
 *           poloziek
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     mar 03 2012
 */

#include <QtGlobal>
#include <QString>
#include <QTcpSocket>
#include <QTime>

#include <pn/server/message.h>
#include <pn/proto.h>

const unsigned MSG_TIMEOUT = 30000;
const unsigned MSG_BUFSIZE = 512;

const char * MSG_ERR_MALFORMED   = "Malformed request!";
const char * MSG_ERR_DUPLICIT    = "Duplicit option!";
const char * MSG_ERR_REQUEST     = "Unknown request!";
const char * MSG_ERR_CHECK       = "Request check failed!";
const char * MSG_ERR_PREMATURE   = "Premature end of message! (Timeout ?!)";

/**
 * \brief Konstruktor rozparsovanej spravy.
 */
Message::Message() {
    my_type = REQ_NULL;
    my_version = 0;
    my_version_stated = false;
}

/**
 * \brief Destruktor rozparsovanej spravy.
 */
Message::~Message() {
}

/**
 * \brief Spristupnenie typu poziadavku od klienta.
 * \return Typ poziadavky od klienta.
 * \retval enum req_type Typ poziadavky od klienta.
 */
enum Req_type Message::type() const {
    return my_type;
}

/**
 * \brief Spristupnenie mena uzivatela, ktory zaslal poziadavku.
 * \return Meno pouzivatela, ktory zaslal poziadavku.
 * \retval const QString & meno pouzivatela, ktory zaslal poziadavku.
 */
const QString & Message::username() const {
    return my_username;
}

/**
 * \brief Spristupnenie hesla uzivatela.
 * \return Heslo pouzivatela.
 * \retval const QString & heslo uzivatela.
 */
const QString & Message::password() const {
    return my_password;
}

/**
 * \brief Spristupnenie nazvu projektu z poziadavku.
 * \return Nazov dotazovaneho projektu.
 * \retval const QString & nazov dotazovaneho projektu.
 */
const QString & Message::project() const {
    return my_project;
}

/**
 * \brief Spristupnenie verzie dotazovaneho projektu.
 * \return Cislo dotazovanej verzie.
 * \retval unsigned cislo dotazovanej verzie daneho projektu.
 */
unsigned Message::version() const {
    return my_version;
}

/**
 * \brief Spristupnenie popisu verzie.
 * \return Popis verzie.
 * \retval QString Popis verzie.
 */
const QString & Message::desc() const {
    return my_desc;
}

/**
 * \brief Zaslane XML data projektu.
 * \return XML projektu.
 * \retval const QString & XML dokumentu.
 */
const QString & Message::xml() const {
    return my_xml;
}

/**
 * \brief Pokial metoda parse() vrati false, metodou error() je mozne
 *        spristupnit popis chyby.
 * \return Informacie spojene s chybou.
 * \retval const QString & popis chyby, ktory nastal pri spracovavani.
 */
const QString & Message::error() const {
    return my_error;
}

/**
 * \brief Spracovanie a rozparsovanie poziadavku.
 * \param socket Socket z ktoreho sa zadana poziadavka bude parsovat.
 * \return Informacia o spravnosti poziadavku.
 * \retval true v pripade spravneho poziadavku, inac false.
 */
bool Message::socket(QTcpSocket * socket) {
    Q_ASSERT(socket);

    bool rv;

    rv = this->parse(socket);
    // Ak funkcia parse_line naplnila chybovu hlasku, doslo k chybe.
    if (! my_error.isEmpty()) {
        return false;
    }

    if (rv != false) { // Je ukoncene nacitavanie zo strany klienta.
        if (this->check()) {
            // Skontroluj, ci su vsetky pozadovane polozky vyplnene (podla
            // typu poziadavky
            return true;
        } else {
            return false;
        }
    } else
        return false;
}

/**
 * \brief Spracovanie a rozparsovanie jedneho riadku poziadavku.
 * \param line Riadok z hlavicky poziadavku, ktory bude spracovany.
 * \return Informacia o ukonceni parsovania celeho poziadavku.
 * \retval true v pripade, ze poziadavka nebola zakoncena koncovym znakom.
 */
bool Message::parse(QTcpSocket * socket) {
    Q_ASSERT(socket);

    bool xml_line = false;
    my_xml.clear();
    QByteArray line, tmp;

    line.resize(MSG_BUFSIZE);

    while ((socket->canReadLine() || socket->waitForReadyRead())
           && socket->bytesAvailable()) {
        socket->readLine(line.data(), line.size());

        if (! qstrcmp(line.data(), PROTO_END)) {
            return true;
        } else if (xml_line) {
            line.replace("\r\n", "\n");
            my_xml.append(line);
        } else if (! qstrcmp(line.data(), PROTOH_XML)) {
            xml_line = true;
        } else if (! qstrncmp(line.data(), PROTOH_PN, qstrlen(PROTOH_PN))) {
            if (my_username.isEmpty()) {
                line.replace("\r\n", "\0");
                my_username = line.mid(qstrlen(PROTOH_PN));
            } else {
                my_error = MSG_ERR_DUPLICIT;
                return false;
            }
        } else if (! qstrncmp(line.data(), PROTOH_PASS, qstrlen(PROTOH_PASS))) {
            if (my_password.isEmpty()){
                line.replace("\r\n", "\0");
                my_password = line.mid(qstrlen(PROTOH_PASS));
            } else {
                my_error = MSG_ERR_DUPLICIT;
                return false;
            }
        } else if (! qstrncmp(line.data(), PROTOH_NAME, qstrlen(PROTOH_NAME))) {
            if (my_project.isEmpty()) {
                line.replace("\r\n", "\0");
                my_project = line.mid(qstrlen(PROTOH_NAME));
            } else {
                my_error = MSG_ERR_DUPLICIT;
                return false;
            }
        } else if (! qstrncmp(line.data(), PROTOH_DESC, qstrlen(PROTOH_DESC))) {
            if (my_desc.isEmpty()) {
                line.replace("\r\n", "\0");
                my_desc = line.mid(qstrlen(PROTOH_DESC));
            } else {
                my_error = MSG_ERR_DUPLICIT;
                return false;
            }
        } else if (! qstrncmp(line.data(),
                              PROTOH_VERSION, qstrlen(PROTOH_VERSION))) {
            if (my_version_stated) {
                my_error = MSG_ERR_DUPLICIT;
                return false;
            }
            // Rozumnejsie sa to snad v Qt ani neda spravit...
            // Je potrebne vysekat cistu reprezentaciu cisla pre toUInt().
            tmp = line.mid(qstrlen(PROTOH_VERSION),
            qstrlen(line.data()) - qstrlen(PROTOH_VERSION) - qstrlen(PROTO_EOL));
            my_version = tmp.toUInt();
            my_version_stated = true;
        } else if (! qstrncmp(line.data(), PROTOH_DO, qstrlen(PROTOH_DO))) {
            if (my_type == REQ_NULL) {
                QByteArray tmp = line.mid(qstrlen(PROTOH_DO));
                my_type = proto_byte2reqt(tmp);
                if (my_type == REQ_NULL) {
                    my_error = MSG_ERR_REQUEST;
                    return false;
                }
            } else {
                my_error = MSG_ERR_DUPLICIT;
                return false;
            }
        } else {
            my_error = MSG_ERR_MALFORMED;
            return false;
        }
    }

    my_error = MSG_ERR_PREMATURE;
    return false;
}

/**
 * \brief Metoda volana pri rozparsovani poziadavku, skontroluje uplnost
 *        poziadavku.
 * \return Informacia o kontrole, kontroluju sa aj polozky, ktore nie su pri
 * danom poziadavku pozadovane.
 * \retval true v pripade, poziadavok je v poriadku.
 */
bool Message::check() {
    // Pri kazdom dotaze na server sa musi uzivatel autentizovat.
    if (my_username.isEmpty() || my_password.isEmpty()) {
        my_error = MSG_ERR_CHECK;
        return false;
    }

    switch (my_type) {
        case REQ_REGISTER:
            /* WALKTHRU */
        case REQ_AUTH:
            // Poziadavky, ktore musia byt vyplnene su poziadavky pri beznych
            // dotazoch.

            // Poziadavky ktore nesmu byt vyplnene.
            if (! my_project.isEmpty()
                || ! my_desc.isEmpty()
                || my_version_stated
                || ! my_xml.isEmpty()) {
                my_error = MSG_ERR_CHECK;
                return false;
            }
            break;

        case REQ_VLIST:
            // Poziadavky, ktore musia byt vyplnene.
            if (my_project.isEmpty()) {
                my_error = MSG_ERR_CHECK;
                return false;
            }

            // Poziadavky ktore nesmu byt vyplnene.
            if (! my_desc.isEmpty()
                || my_version_stated
                || ! my_xml.isEmpty()) {
                my_error = MSG_ERR_CHECK;
                return false;
            }
            break;

        case REQ_LIST:
            /* WALKTHRU */
        case REQ_LOGOUT:
            // Poziadavky ktore nesmu byt vyplnene (vsetky).
            if (! my_project.isEmpty()
                || ! my_desc.isEmpty()
                || my_version_stated
                || ! my_xml.isEmpty()) {
                my_error = MSG_ERR_CHECK;
                return false;
            }
            break;

        case REQ_GET:
            // Poziadavky, ktore musia byt vyplnene.
            if (my_project.isEmpty() || ! my_version_stated) {
                my_error = MSG_ERR_CHECK;
                return false;
            }

            // Poziadavky ktore nesmu byt vyplnene.
            if (! my_desc.isEmpty()
                || ! my_xml.isEmpty()) {
                my_error = MSG_ERR_CHECK;
                return false;
            }
            break;

        case REQ_ADD:
            // Poziadavky, ktore musia byt vyplnene.
            if (my_project.isEmpty() || my_desc.isEmpty() || my_xml.isEmpty()) {
                my_error = MSG_ERR_CHECK;
                return false;
            }

            // Poziadavky ktore nesmu byt vyplnene.
            if (my_version_stated) {
                my_error = MSG_ERR_CHECK;
                return false;
            }
            break;

        case REQ_SIMLOG:
            // Poziadavky, ktore musia byt vyplnene.
            if (my_project.isEmpty() || ! my_version_stated) {
                my_error = MSG_ERR_CHECK;
                return false;
            }

            // Poziadavky ktore nesmu byt vyplnene.
            if (! my_desc.isEmpty()
                || ! my_xml.isEmpty()) {
                my_error = MSG_ERR_CHECK;
                return false;
            }
            break;

        case REQ_STEP:
            /* WALKTHRU */
        case REQ_RUN:
            // Poziadavky, ktore musia byt vyplnene.
            if (my_xml.isEmpty()) {
                my_error = MSG_ERR_CHECK;
                return false;
            }

            // Poziadavky ktore nesmu byt vyplnene.
            if (! my_desc.isEmpty()) {
                my_error = MSG_ERR_CHECK;
                return false;
            }
            break;

        case REQ_NULL:
            /* WALKTHRU */
        default:
            my_error = MSG_ERR_REQUEST;
            return false;
            break;
    }

    return true;
}

