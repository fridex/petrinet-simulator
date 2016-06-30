/**
 * \file     proto.cpp
 * \brief    Definicia retazcovych literalov a typy poziadavkov protokolu
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     man 03 2012
 */

#include <QByteArray>

#include <pn/proto.h>

// Typy dotazov.
const char * PROTOH_PN        = "PN: ";
const char * PROTOH_DO        = "DO: ";
const char * PROTOH_PASS      = "PASS: ";
const char * PROTOH_NAME      = "NAME: ";
const char * PROTOH_DESC      = "DESC: ";
const char * PROTOH_HEAD      = "HEAD: ";
const char * PROTOH_USER      = "USER: ";
const char * PROTOH_TIME      = "TIME: ";
const char * PROTOH_VERSION   = "VERSION: ";
const char * PROTOH_MSG       = "MSG: ";
// Viacriadkove odpovede.
const char * PROTOH_LIST      = "LIST:\r\n";
const char * PROTOH_VLIST     = "VLIST:\r\n";
const char * PROTOH_XML       = "XML:\r\n";
const char * PROTOH_SIMLOG    = "SIMLOG:\r\n";
const char * PROTOH_ADD       = "ADD: ";
// Atributy odpovedi.
const char * PROTOR_AUTH      = "AUTH\r\n";
const char * PROTOR_LOGOUT    = "LOGOUT\r\n";
const char * PROTOR_REGISTER  = "REG\r\n";
const char * PROTOR_LIST      = "LIST\r\n";
const char * PROTOR_VLIST     = "VLIST\r\n";
const char * PROTOR_GET       = "GET\r\n";
const char * PROTOR_ADD       = "ADD\r\n";
const char * PROTOR_STEP      = "STEP\r\n";
const char * PROTOR_RUN       = "RUN\r\n";
const char * PROTOR_SIMLOG    = "SIMLOG\r\n";

const char * PROTOR_BAD       = "BAD\r\n";
const char * PROTOR_OK        = "OK\r\n";

const char * PROTO_NA         = "N/A\r\n";
const char * PROTO_EOL        = "\r\n";
const char * PROTO_END        = "\r\n";

/**
 * \brief Funkcia pre prevod prikazu poziadavku z textovej formy do jej internej
 * reprezentacie.
 * \param bytea Textova podoba poziadavku.
 * \retval enum req_type Poziadavok v internej reprezentacii.
 */
enum Req_type proto_byte2reqt(const QByteArray & bytea) {
    if (! qstrcmp(bytea.data(), PROTOR_AUTH)) {
        return REQ_AUTH;
    } else if (! qstrcmp(bytea.data(), PROTOR_LOGOUT)) {
        return REQ_LOGOUT;
    } else if (! qstrcmp(bytea.data(), PROTOR_REGISTER)) {
        return REQ_REGISTER;
    } else if (! strcmp(bytea.data(), PROTOR_LIST)) {
        return REQ_LIST;
    } else if (! qstrcmp(bytea.data(), PROTOR_VLIST)) {
        return REQ_VLIST;
    } else if (! qstrcmp(bytea.data(), PROTOR_SIMLOG)) {
        return REQ_SIMLOG;
    } else if (! qstrcmp(bytea.data(), PROTOR_GET)) {
        return REQ_GET;
    } else if (! qstrcmp(bytea.data(), PROTOR_ADD)) {
        return REQ_ADD;
    } else if (! qstrcmp(bytea.data(), PROTOR_STEP)) {
        return REQ_STEP;
    } else if (! qstrcmp(bytea.data(), PROTOR_RUN)) {
        return REQ_RUN;
    } else {
        return REQ_NULL;
    }
}

/**
 * \brief Funkcia pre prikazu poziadavku z internej reprezentacii do jej
 * textovej podoby.
 * \param type Interna reprezentacia poziadavku.
 * \retval const QByteArray & Textova reprezentacia poziadavku.
 */
const QByteArray proto_reqt2byte(enum Req_type type) {
    QByteArray rv;

    switch (type) {
        case REQ_AUTH:
            rv = PROTOR_AUTH;
            break;

        case REQ_LOGOUT:
            rv = PROTOR_LOGOUT;
            break;

        case REQ_REGISTER:
            rv = PROTOR_REGISTER;
            break;

        case REQ_LIST:
            rv = PROTOR_LIST;
            break;

        case REQ_VLIST:
            rv = PROTOR_VLIST;
            break;

        case REQ_GET:
            rv = PROTOR_GET;
            break;

        case REQ_ADD:
            rv = PROTOR_ADD;
            break;

        case REQ_STEP:
            rv = PROTOR_STEP;
            break;

        case REQ_RUN:
            rv = PROTOR_RUN;
            break;

        case REQ_SIMLOG:
            rv = PROTOR_SIMLOG;
            break;

        case REQ_NULL:
            /* WALKTHRU */
        default:
            rv = PROTO_NA;
            break;
    }

    return rv;
}

