/**
 * \file     proto.h
 * \brief    Deklaracia retazcovych literalov a typy poziadavkov protokolu
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     man 03 2012
 */

#ifndef PN_PROTO_H_
#define PN_PROTO_H_

// forward
class QByteArray;

/**
 * \brief Symbolicke nazvy jednotlivych poziadavkov na server.
 */
enum Req_type {
    REQ_NULL, // Zly request, nevyhovuje ziadnej poziadavke.
    REQ_AUTH,
    REQ_LOGOUT,
    REQ_REGISTER,
    REQ_LIST,
    REQ_VLIST,
    REQ_GET,
    REQ_ADD,
    REQ_STEP,
    REQ_RUN,
    REQ_SIMLOG
};

extern const char * PROTOH_PN;
extern const char * PROTOH_DO;
extern const char * PROTOH_PASS;
extern const char * PROTOH_LIST;
extern const char * PROTOH_VLIST;
extern const char * PROTOH_NAME;
extern const char * PROTOH_DESC;
extern const char * PROTOH_HEAD;
extern const char * PROTOH_USER;
extern const char * PROTOH_TIME;
extern const char * PROTOH_VERSION;
extern const char * PROTOH_XML;
extern const char * PROTOH_SIMLOG;
extern const char * PROTOH_MSG;
extern const char * PROTOH_ADD;

extern const char * PROTOR_AUTH;
extern const char * PROTOR_LOGOUT;
extern const char * PROTOR_REGISTER;
extern const char * PROTOR_LIST;
extern const char * PROTOR_VLIST;
extern const char * PROTOR_GET;
extern const char * PROTOR_SIMLOG;
extern const char * PROTOR_ADD;
extern const char * PROTOR_STEP;
extern const char * PROTOR_RUN;
extern const char * PROTOR_BAD;
extern const char * PROTOR_OK;

extern const char * PROTO_EOL;
extern const char * PROTO_END;

enum Req_type proto_byte2reqt(const QByteArray & byte);
const QByteArray proto_reqt2byte(enum Req_type type);

#endif // PN_PROTO_H_

