/**
 * \file     answer.h
 * \brief    Implementacia standardnych odpovedi serveru.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     mar 03 2012
 */

#ifndef PN_SERVER_ANSWER_H_
#define PN_SERVER_ANSWER_H_

#include <pn/proto.h>

// forward
class QString;
class ProjectDB;

/**
 * \brief Polozky MSG v standardenj odpovedi.
 */
enum Answer_msg {
    ANSWER_BAD_REQ,

    ANSWER_OK_AUTH,
    ANSWER_BAD_AUTH,
    ANSWER_OK_LOGOUT,
    ANSWER_BAD_LOGOUT,
    ANSWER_OK_REGISTER,
    ANSWER_BAD_REGISTER,
    ANSWER_UNKNOWN,
    ANSWER_BAD_XML,
    ANSWER_INTERNAL_ERR
};

/**
 * \brief Trieda pre zostavenie standardnej odpovedi na poziadavok od klienta.
 */
class Answer {
  private:
      QString my_header;  ///< Zostaveny text odpovede.

  public:
    Answer();
    virtual ~Answer();

    const QString & text() const;
    void set_standard(enum Answer_msg msg);
    void set_xml(const QString & xml);
    void set_error(const QString & error);
    void set_add(unsigned version);
    void set_xml(ProjectDB & projects,
                 const QString & pname,
                 unsigned version);
    void set_vlist(ProjectDB & projects, const QString pname);
    void set_list(ProjectDB & projects);
    void set_simlog(ProjectDB & projects,
                    const QString & pname,
                    unsigned version);

  private:
    /**
     * \brief DISABLE_COPY_AND_ASSIGN
     */
    Answer(const Answer &);
    /**
     * \brief DISABLE_COPY_AND_ASSIGN
     */
    void operator=(const Answer &);
}; // Answer

#endif // PN_SERVER_ANSWER_H_

