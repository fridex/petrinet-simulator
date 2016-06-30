/**
 * \file     projectdb.h
 * \brief    Implementacia databazy projektov.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     feb 26 2012
 */

#ifndef PN_PROJECTDB_H_
#define PN_PROJECTDB_H_

#include <QStringList>
#include <QString>
#include <QDir>

/**
 * \brief Trieda reprezentujuca verzovaciu databazu projektov.
 */
class ProjectDB {
  private:
    QStringList my_projects;
    QDir my_pdir;
    const char * my_error;

  public:
    ProjectDB(const QString & dir);
    virtual ~ProjectDB();
    bool exist(const QString & pname, unsigned version);
    bool exist(const QString & pname);
    bool xml_data(QString & xml, const QString & pname, unsigned version);
    bool desc(QString & desc, const QString & pname, unsigned version);
    bool user(QString & username, unsigned & time,
              const QString & pname, unsigned version);
    bool simlog(QStringList & simlog, const QString & pname, unsigned & version);
    unsigned add_project(const QString & pname,
                         const QString & user,
                         const QString & desc,
                         const QString & xml);
    const QString error() const;
    bool projects(QStringList & projects);
    bool update_simlog(const QString & username,
                       const QString & name,
                       unsigned version);
    unsigned version_count(const QString & pname);

  private:
    /**
     * \brief DISABLE_COPY_AND_ASSIGN
     */
    ProjectDB(const ProjectDB &);
    /**
     * \brief DISABLE_COPY_AND_ASSIGN
     */
    void operator=(const ProjectDB &);
}; // ProjectDB

#endif // PN_PROJECTDB_H_

