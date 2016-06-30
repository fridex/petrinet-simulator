/**
 * \file     answer.cpp
 * \brief    Implementacia standardnych odpovedi serveru.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     mar 03 2012
 */

#include <QString>
#include <QTime>

#include <pn/server/answer.h>

#include <pn/proto.h>
#include <pn/server/projectdb.h>
#include <pn/server/debug.h>

const char * ANSWER_OK_AUTH_MSG      = "Logged in";
const char * ANSWER_BAD_AUTH_MSG     = "Bad authentication";
const char * ANSWER_OK_LOGOUT_MSG    = "Logged out";
const char * ANSWER_BAD_LOGOUT_MSG   = "Cannot logout";
const char * ANSWER_OK_REGISTER_MSG  = "Registration successful";
const char * ANSWER_BAD_REGISTER_MSG = "Registration failed";
const char * ANSWER_OK_ADD_MSG       = "Added";
const char * ANSWER_BAD_ADD_MSG      = "Unable to add";
const char * ANSWER_BAD_XML_MSG      = "Bad project data";
const char * ANSWER_BAD_REQ_MSG      = "Bad request";
const char * ANSWER_BAD_DUPLICIT_MSG = "Duplicit request";
const char * ANSWER_UNKNOWN_MSG      = "Unknown";
const char * ANSWER_INTERNAL_ERR_MSG = "Internal error on server";
const char * ANSWER_LIMIT_ERR_MSG    = "Simulation time exceeded!";

/**
 * \brief - Konstuktor pre standardnu odpoved.
 */
Answer::Answer() {
}

/**
 * \brief Destruktor pre standardnu odpoved serveru.
 */
Answer::~Answer() {
}

/**
 * \brief Metoda zabezpecujuca zostavenie odpovedi.
 * \param msg Sprava pre prijemcu.
 * \retval void
 */
void Answer::set_standard(enum Answer_msg msg) {
    my_header = PROTOH_DO;

    switch (msg) {
        case ANSWER_OK_AUTH:
            my_header.append(PROTOR_OK);
            my_header.append(PROTOH_MSG);
            my_header.append(ANSWER_OK_AUTH_MSG);
            break;
        case ANSWER_BAD_AUTH:
            my_header.append(PROTOR_BAD);
            my_header.append(PROTOH_MSG);
            my_header.append(ANSWER_BAD_AUTH_MSG);
            break;
        case ANSWER_OK_LOGOUT:
            my_header.append(PROTOR_OK);
            my_header.append(PROTOH_MSG);
            my_header.append(ANSWER_OK_LOGOUT_MSG);
            break;
        case ANSWER_BAD_LOGOUT:
            my_header.append(PROTOR_BAD);
            my_header.append(PROTOH_MSG);
            my_header.append(ANSWER_BAD_LOGOUT_MSG);
            break;
        case ANSWER_OK_REGISTER:
            my_header.append(PROTOR_OK);
            my_header.append(PROTOH_MSG);
            my_header.append(ANSWER_OK_REGISTER_MSG);
            break;
        case ANSWER_BAD_REGISTER:
            my_header.append(PROTOR_BAD);
            my_header.append(PROTOH_MSG);
            my_header.append(ANSWER_BAD_REGISTER_MSG);
            break;
        case ANSWER_BAD_REQ:
            my_header.append(PROTOR_BAD);
            my_header.append(PROTOH_MSG);
            my_header.append(ANSWER_BAD_REQ_MSG);
            break;
        case ANSWER_BAD_XML:
            my_header.append(PROTOR_BAD);
            my_header.append(PROTOH_MSG);
            my_header.append(ANSWER_BAD_XML_MSG);
            break;
        case ANSWER_INTERNAL_ERR:
            my_header.append(PROTOR_BAD);
            my_header.append(PROTOH_MSG);
            my_header.append(ANSWER_INTERNAL_ERR_MSG);
        case ANSWER_UNKNOWN:
        default:
            my_header.append(PROTOR_BAD);
            my_header.append(PROTOH_MSG);
            my_header.append(ANSWER_UNKNOWN_MSG);
            break;
    }

    my_header.append(PROTO_EOL).append(PROTO_END);
}

/**
 * \brief Zostavenie chybovej spravy a zaklade chybovej hlasky.
 * \param error chybova hlaska, ktora ma byt zaslana do spravy
 */
void Answer::set_error(const QString & error) {
    my_header = PROTOH_DO;
    my_header.append(PROTOR_BAD);
    my_header.append(PROTOH_MSG);
    my_header.append(error);

    my_header.append(PROTO_EOL).append(PROTO_END);
}

/**
 * \brief Nastavenie polozky XML v odpovedi pre konkretny projekt.
 * \param projects Databaza projektov z ktorej sa ma vybrat zadany projekt.
 * \param pname  Nazov projektu, z ktoreho sa maju XML data zaslat.
 * \param version Cislo verzie projektu, ktory sa ma zaslat.
 * \retval void
 */
void Answer::set_xml(ProjectDB & projects,
                     const QString & pname,
                     unsigned version) {
    QString xml;
    if (! projects.xml_data(xml, pname, version)) {
        this->set_standard(ANSWER_INTERNAL_ERR);
        debug("E: XML: Internal error in XML (set xml)");
        return;
    }

    my_header = PROTOH_XML;
    my_header.append(xml).append(PROTO_EOL).append(PROTO_END);
}

/**
 * \brief Nastavenie polozky XML v odpovedi.
 * \param  xml Vstupny subor v XML formate.
 * \retval void
 */
void Answer::set_xml(const QString & xml) {
    my_header = PROTOH_XML;
    my_header.append(xml).append(PROTO_EOL).append(PROTO_END);
}

/**
 * \brief Pripravenie odpovedi pre pridanie projektu do repozitara.
 * \param version verzia pridaneho projektu do repozitara
 * \retval void
 */
void Answer::set_add(unsigned version) {
    my_header.append(PROTOH_ADD);
    my_header.append(ANSWER_OK_ADD_MSG).append(PROTO_EOL);
    my_header.append(PROTOH_VERSION).append(QString::number(version));
    my_header.append(PROTO_EOL).append(PROTO_END);
}

/**
 * \brief Pripravenie odpovedi pre VLIST - zoznam verzii na servri.
 * \param projects Databaza projektov z ktorej sa ma vybrat zadany projekt.
 * \param pname Meno projektu, pre ktory sa ma zoznam verzii generovat.
 * \retval void
 */
void Answer::set_vlist(ProjectDB & projects, const QString pname) {
    unsigned ver_count = projects.version_count(pname);
    unsigned time;
    my_header = PROTOH_VLIST;
    QString data;

    for (unsigned int i = 1; i <= ver_count; ++i) {
        if (! projects.desc(data, pname, i)) {
            this->set_standard(ANSWER_INTERNAL_ERR);
            debug("E: VLIST: Internal error (desc)");
            return;
        }

        my_header.append(PROTOH_DESC).append(data).append(PROTO_EOL);

        if (! projects.user(data, time, pname, i)) {
            this->set_standard(ANSWER_INTERNAL_ERR);
            debug("E: VLIST: Internal error (user)");
            return;
        }

        my_header.append(PROTOH_USER).append(data).append(PROTO_EOL);
        my_header.append(PROTOH_TIME).append(QString::number(time)).append(PROTO_EOL);
    }
    my_header.append(PROTO_END);
}

/**
 * \brief Pripravenie odpovedi pre LIST - zoznam projektov ulozenych na serveri.
 * \param projects Databaza projektov z ktorej sa ma zoznam generovat.
 * \retval void
 */
void Answer::set_list(ProjectDB & projects) {
    QStringList list;

    projects.projects(list);

    my_header = PROTOH_LIST;
    for (int i = 0; i < list.size(); ++i) {
        my_header.append(PROTOH_NAME).append(list[i]).append(PROTO_EOL);
    }
    my_header.append(PROTO_END);
}

/**
 * \brief Zostavenie odpovedi pre vypis logu zo simulacii.
 * \param projects Databaza projektov z ktorej sa ma vybrat zadany log.
 * \param pname Nazov projektu, z ktoreho sa ma log zostavit.
 * \param version Cislo verzie projektu pre log suboru.
 * \retval void
 */
void Answer::set_simlog(ProjectDB & projects,
                        const QString & pname,
                        unsigned version) {
    QStringList simlog;
    if (! projects.simlog(simlog, pname, version)) {
        this->set_standard(ANSWER_INTERNAL_ERR);
        debug("E: SIMLOG: Internal error");
        return;
    }

    my_header = PROTOH_SIMLOG;
    for (int i = 0; i < simlog.size(); ++i) {
        my_header.append(PROTOH_TIME).append(simlog[i]).append(PROTO_EOL);
        ++i; // mena su na neparnych poziciach
        my_header.append(PROTOH_USER).append(simlog[i]).append(PROTO_EOL);
    }
    my_header.append(PROTO_END);
}

/**
 * \brief Spristupnenie textu zostavenej odpovedi.
 * \return Spristupneny text odpovedi.
 * \retval const QString & spristupneny text odpovedi.
 */
const QString & Answer::text() const {
    return my_header;
}

