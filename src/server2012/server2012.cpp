/**
 * \file     server2012.cpp
 * \brief    subor pre TCP server a spracovanie poziadavkov
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     mar 02 2012
 */

#include <QtGlobal>
#include <QObject>
#include <QTcpServer>
#include <QtNetwork>

#include <pn/server/projectdb.h>
#include <pn/server/user.h>
#include <pn/server/userdb.h>
#include <pn/server/serverthread.h>
#include <pn/server/debug.h>

#include <pn/server/server2012.h>

const qint64 FILE_LINE_SIZE   = 256;
const char * USERDB_SEPARATOR = ":";

/**
 * \brief Konstruktor servru nastavi prislusny port a zaistiti vypis informacii
 *        o spustenom servri
 * \param port Cislo portu na ktorom ma sluzba odpocuvat
 * \param userdb Nazov suboru, ktory sa ma pouzit pre uzivatelsky databazu.
 * \param projectdb Nazov adresara, ktory sa ma pouzit pre uchovavanie projekt.
 * \param parent Ukazatel na predchadzajuci objekt Qt.
 */
Server::Server(unsigned port, const char * userdb,
               const char * projectdb, QObject * parent)
        : QTcpServer(parent), my_projects(projectdb),
        my_sem_projdb(1), my_sem_userdb(1), my_sem_simlog(1) {
    QString ip_addr;

    my_userdb = userdb;
    this->load_userdb();

    this->setMaxPendingConnections(20);     // Maximalny pocet pripojeni

    if (! this->listen(QHostAddress::Any, port)) {
        throw "Unable to listen on port";
    }

    // Najde IP adresu pre vypis
    QList<QHostAddress> ip_addr_list = QNetworkInterface::allAddresses();
    for (int i = 0; i < ip_addr_list.size(); ++i) {
        if (ip_addr_list.at(i) != QHostAddress::LocalHost &&
            ip_addr_list.at(i).toIPv4Address()) {
            ip_addr = ip_addr_list[i].toString();
            break;
        }
    }

    // Pokial nebola najdena ziadna adresa, pouzije sa localhost
    if (ip_addr.isEmpty())
        ip_addr = QHostAddress(QHostAddress::LocalHost).toString();

    qDebug() << "Server is running" << ip_addr
             << "listening on port" << this->serverPort();
    debug("Ready to serve...");
}

/**
 * \brief Destruktor TCP servru pre dealokaciu systemovych prostriedkov.
 */
Server::~Server() {
    debug("Shutting down the server...");
}

/**
 * \brief Predefinovana metoda QTcpServer pre vybavovanie poziadavkov vo
 * vlaknach.
 * \param socket Socket pre ktory sa ma spustit nove vlakno.
 * \retval void
 */
void Server::incomingConnection(int socket) {
    ServerThread * thread = new ServerThread(socket, this);
    debug("Incomming connection");
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}

/**
 * \brief Nacitanie uzivatelskej databazy.
 * \retval void
 */
void Server::load_userdb() {
    QFile file;
    QByteArray line;

    file.setFileName(my_userdb);

    if (! file.open(QIODevice::ReadOnly)) {
        throw "Failed to open user DB file";
    }

    int idx;
    line.resize(FILE_LINE_SIZE);
    while (! file.atEnd()) {
        file.readLine(line.data(), FILE_LINE_SIZE);
        line.replace("\n", "\0");
        idx = line.indexOf(USERDB_SEPARATOR);
        my_users.add_user(line.left(idx),
                          line.mid(idx + 1));
    }

    file.close();
}

/**
 * \brief Pridanie uzivatela do perzistentnej databazy uzivatelov.
 * \param username Meno noveho uzivatela.
 * \param password Heslo pre noveho uzivatela.
 * \return Informacia signalizujuca uspesnost akcie
 * \retval false v pripade chyby pri neuspesnom zapise.
 */
bool Server::update_userdb(const QString & username, const QString & password) {
    QFile file;

    my_sem_userdb.acquire();

    file.setFileName(my_userdb);
    if (! file.open(QIODevice::Append)) {
        return false;
    }

    file.write(username.toAscii());
    file.write(USERDB_SEPARATOR);
    file.write(password.toAscii());
    file.write("\n");

    file.close();

    my_sem_userdb.release();
    return true;
}

/**
 * \brief Metoda pre cyklenie a pasivneho cakania na prichadzajuce poziadavky.
 * \return bool Informacia o dostupnosti spojenia
 * \retval bool true, ak je spojenie dostupne.
 */
bool Server::loop() {
    return this->waitForNewConnection(-1);
}

/**
 * \brief Zapuzdrena metoda pre zistenie existencie uzivatela.
 * \param username Meno uzivatela, ktoreho existencia sa ma zistit.
 * \return Informacia o existencii uzivatela.
 * \retval true v pripade, ze sa uzivatel v databaze nachadza.
 */
bool Server::exist_user(const QString & username) {
    return my_users.exist(username);
}

/**
 * \brief Zapuzdrena metoda pre verifikaciu spravnych udajov uzivatela.
 * \param username Meno uzivatela ktoreho verifikacia sa ma zistit.
 * \param password Heslo uzivatela pre autentizaciu.
 * \retval true v pripade, ze uzivatelske data su spravne.
 * \return Informacia o uspesnosti autentizacie uzivatela.
 */
bool Server::verify_user(const QString & username, const QString & password) {
    return my_users.verify(username, password);
}

/**
 * \brief Zapuzdrena metoda pre pridanie uzivatela.
 * \param username Meno uzivatela.
 * \param password heslo uzivatela.
 * \return Informacia o uspesnosti pridania uzivatela.
 * \retval true v pripade uspesneho pridania uzivatela.
 */
bool Server::add_user(const QString & username, const QString & password) {
    return my_users.add_user(username, password);
}

/**
 * \brief Zapuzdrena metoda pre zistenie existencie projektu.
 * \param pname Nazov projektu ktoreho existencia sa ma zistit.
 * \param version Verzia pre zistenie konkretnej verzie projektu.
 * \return Informacia o existencii projektu.
 * \retval true v pripade, ze projekt existuje.
 */
bool Server::exist_project(const QString & pname, unsigned version) {
    return my_projects.exist(pname, version);
}

/**
 * Spristupnenie databazy projektov.
 * \return Spristupnena databaza projektov.
 */
ProjectDB & Server::projects() {
    return my_projects;
}

/**
 * \brief Zapuzdrena metoda pre pridanie projektu.
 * \param pname Nazov pridavaneho projektu.
 * \param username Meno uzivatela, ktory projekt pridal.
 * \param desc Popis pridavanej verzie projektu.
 * \param xml XML data pridavaneho projektu.
 * \return Informacia o uspesnosti pridania projektu.
 */
bool Server::add_project(const QString & pname,
                         const QString & username,
                         const QString & desc,
                         const QString & xml) {
    bool rv;

    my_sem_projdb.acquire();

    rv = my_projects.add_project(pname, username, desc, xml);

    my_sem_projdb.release();

    return rv;
}

/**
 * \brief Zapuzdrena metoda pre aktualizaciu logu projektu.
 * \param username Nazov uzivatela, ktory simuloval projekt.
 * \param pname Nazov projektu pre aktualizaciu.
 * \param version Verzia projektu, ktora bola simulovana
 * \return Informacia o uspesnosti aktualizacie projektu
 * \retval true v pripade uspesnej aktualizacie projektu
 */
bool Server::update_simlog(const QString & username,
                 const QString & pname,
                 unsigned version) {
    bool rv;

    my_sem_simlog.acquire();

    rv = my_projects.update_simlog(username, pname, version);

    my_sem_simlog.release();

    return rv;
}

/******************************************************************************/

/**
 * \brief Hlavna funkcia servru zabezbecujuca neustale vybavovanie poziadavkov.
 * \param port Port na ktorom ma server poziadavky vybavovat.
 * \param userdb Nazov suboru, ktory sa ma pouzit pre uzivatelsky databazu.
 * \param projectdb Nazov adresara, ktory sa ma pouzit pre uchovavanie projekt.
 * \retval void
 */
void server2012(unsigned port, const char * userdb, const char * projectdb) {
    Q_ASSERT(userdb);
    Server server(port, userdb, projectdb);

    forever {
        server.loop();
    }
}

