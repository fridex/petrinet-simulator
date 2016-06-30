/**
 * \file     projectdb.cpp
 * \brief    Implementacia databazy projektov.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     feb 26 2012
 */

#include <QString>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDateTime>

#include <pn/server/projectdb.h>

const char * PROJECTDB_ERR_UNVER      = "Unknown version";
const char * PROJECTDB_ERR_UNPROJ     = "Unknown project";
const char * PROJECTDB_ERR_ROPEN      = "Unable to open file for reading";
const char * PROJECTDB_ERR_WOPEN      = "Unable to open file for writing";
const char * PROJECTDB_ERR_MKDIR_PROJ = "Failed to create directory for project";
const char * PROJECTDB_ERR_MKDIR_VER  = "Failed to create directory for project";
const char * PROJECTDB_XML_FILE       = "project.xml";
const char * PROJECTDB_DESC_FILE      = "desc.txt";
const char * PROJECTDB_USER_FILE      = "author.txt";
const char * PROJECTDB_SLOG_FILE      = "simlog.txt";
const char * PROJECTDB_SLOG_SEPARATOR = ":";

/**
 * \brief Konstruktor pre databazu projektov.
 * \param dir Umiestnenie adresara databazy projektov.
 * \throws const char * Pokial zadany adresar nie je platny.
 */
ProjectDB::ProjectDB(const QString & dir) {
    my_pdir = dir;
    my_error = 0;

    if (! my_pdir.exists())
        throw "Bad direcotry for projectdb";
}

/**
 * \brief Destruktor pre databazu projektov.
 */
ProjectDB::~ProjectDB() {
}

/**
 * \brief Predikat pre zistenie, ci zadany projekt existuje.
 * \param pname Meno projektu pre zistenie existencie
 * \return Informacia o existencii.
 * \retval true v pripade projekt existuje.
 */
bool ProjectDB::exist(const QString & pname) {
    my_error = 0;
    QDir proj(my_pdir.path());

    if (! proj.cd(pname))
        return false;
    else
        return true;
}

/**
 * \brief Predikat pre zistenie, ci zadany projekt existuje v danej verzii.
 * \param pname Meno projektu pre zistenie existencie
 * \param version Cislo verzie projektu.
 * \return Informacia o existencii.
 * \retval true v pripade projekt existuje v danej verzii.
 */
bool ProjectDB::exist(const QString & pname, unsigned version) {
    my_error = 0;
    QDir proj(my_pdir.path());

    if (! proj.cd(pname))
        return false;

    if (! proj.cd(QString::number(version)))
        return false;

    return true;
}

/**
 * \brief Spristupnenie ulozenych XML dat projektu v danej verzii.
 * \param xml Spristupnene XML data.
 * \param pname Nazov projektu pre spristupnenie dat.
 * \param version Cislo verzie projektu.
 * \return Informacia o uspesnosti prevedenia poziadavku.
 * \retval V pripade uspechu true, v opacnom pripade false.
 */
bool ProjectDB::xml_data(QString & xml,
                         const QString & pname,
                         unsigned version) {
    my_error = 0;
            my_error = PROJECTDB_ERR_MKDIR_PROJ;
    xml.clear();
    QDir proj(my_pdir.path());

    if (! proj.cd(pname)) {
        my_error = PROJECTDB_ERR_UNPROJ;
        return false;
    }

    if (! proj.cd(QString::number(version))) {
        my_error = PROJECTDB_ERR_UNVER;
        return false;
    }

    QFile data(proj.absoluteFilePath(PROJECTDB_XML_FILE));

    if (! data.open(QIODevice::ReadOnly)) {
        my_error = PROJECTDB_ERR_ROPEN;
        return false;
    }

    xml = data.readAll();

    data.close();
    return true;
}

/**
 * \brief Spristupnenie popisu danej verzie projektu.
 * \param desc Spristupneny popis projektu.
 * \param pname Nazov projektu pre spristupnenie dat.
 * \param version Cislo verzie projektu.
 * \return Informacia o uspesnosti prevedenia poziadavku.
 * \retval V pripade uspechu true, v opacnom pripade false.
 */
bool ProjectDB::desc(QString & desc, const QString & pname, unsigned version) {
    my_error = 0;
    desc.clear();
    QDir proj(my_pdir.path());

    if (! proj.cd(pname)) {
        my_error = PROJECTDB_ERR_UNPROJ;
        return false;
    }

    if (! proj.cd(QString::number(version))) {
        my_error = PROJECTDB_ERR_UNVER;
        return false;
    }

    QFile data(proj.absoluteFilePath(PROJECTDB_DESC_FILE));

    if (! data.open(QIODevice::ReadOnly)) {
        my_error = PROJECTDB_ERR_ROPEN;
        return false;
    }

    char buf;
    while (data.getChar(&buf) && buf != '\n')
        desc.append(buf);

    data.close();
    return true;
}

/**
 * \brief Spristupnenie mena uzivatela, ktory pridal danu verziu projektu.
 * \param username Spristupnene meno pouzivatela.
 * \param time Cas vytvorenia danej verzie (vystupny parameter).
 * \param pname Nazov projektu pre spristupnenie dat.
 * \param version Cislo verzie projektu.
 * \return Informacia o uspesnosti prevedenia poziadavku.
 * \retval V pripade uspechu true, v opacnom pripade false.
 */
bool ProjectDB::user(QString & username, unsigned & time,
                     const QString & pname, unsigned version) {
    my_error = 0;
    username.clear();
    QDir proj(my_pdir.path());

    if (! proj.cd(pname)) {
        my_error = PROJECTDB_ERR_UNPROJ;
        return false;
    }

    if (! proj.cd(QString::number(version))) {
        my_error = PROJECTDB_ERR_UNVER;
        return false;
    }

    QFile data(proj.absoluteFilePath(PROJECTDB_USER_FILE));

    if (! data.open(QIODevice::ReadOnly)) {
        my_error = PROJECTDB_ERR_ROPEN;
        return false;
    }

    char buf;
    while (data.getChar(&buf) && buf != '\n')
        username.append(buf);

    time = username.mid(username.indexOf(PROJECTDB_SLOG_SEPARATOR) + 1).toUInt();
    username = username.mid(0, username.indexOf(PROJECTDB_SLOG_SEPARATOR));

    data.close();
    return true;
}

/**
 * \brief Vytvorenie novej verzie projektu. Pokial zadany projekt este
 * neexistuje, je vytvoreny aj zaznam o projekte.
 * \param pname Nazov projektu, pre ktory ma byt vytvorena nova verzia.
 * \param user  Nazov uzivatela, ktory zadanu verziu vytvoril.
 * \param desc  Popis novej verzie.
 * \param xml   XML data danej verzie.
 * \return Informacia o uspesnosti prevedenia poziadavku.
 * \retval V pripade uspechu cislo novej verzie, v opacnom pripade 0.
 */
unsigned ProjectDB::add_project(const QString & pname,
                                const QString & user,
                                const QString & desc,
                                const QString & xml) {
    my_error = 0;
    QDir proj(my_pdir.path());
    unsigned version = this->version_count(pname) + 1;
    QString version_file = QString::number(version);

    if (! proj.cd(pname)) {
        if (! proj.mkdir(pname)) {
            my_error = PROJECTDB_ERR_MKDIR_PROJ;
            return 0;
        }
        if (! proj.cd(pname)) {
            my_error = PROJECTDB_ERR_UNPROJ;
            return 0;
        }
    }

    if (! proj.mkdir(version_file)) {
        my_error = PROJECTDB_ERR_MKDIR_VER;
        return 0;
    }


    if (! proj.cd(version_file)) {
        my_error = PROJECTDB_ERR_UNVER;
        return 0;
    }

    QFile f_desc(proj.absoluteFilePath(PROJECTDB_DESC_FILE));
    QFile f_user(proj.absoluteFilePath(PROJECTDB_USER_FILE));
    QFile f_xml(proj.absoluteFilePath(PROJECTDB_XML_FILE));
    QFile f_slog(proj.absoluteFilePath(PROJECTDB_SLOG_FILE));

    if (! f_desc.open(QIODevice::WriteOnly)
        || ! f_user.open(QIODevice::WriteOnly)
        || ! f_slog.open(QIODevice::WriteOnly)
        || ! f_xml.open(QIODevice::WriteOnly)) {
        my_error = PROJECTDB_ERR_WOPEN;
        return 0;
    }

    QString time = QString::number(QDateTime::currentDateTime().toTime_t());

    f_desc.write(desc.toAscii().data(), desc.length());
    f_desc.putChar('\n');
    f_user.write(user.toAscii().data(), user.length());
    f_user.write(PROJECTDB_SLOG_SEPARATOR, qstrlen(PROJECTDB_SLOG_SEPARATOR));
    f_user.write(time.toAscii().data(), time.length());
    f_user.putChar('\n');
    f_xml.write(xml.toAscii().data(), xml.length());
    f_xml.putChar('\n');

    f_desc.close(); f_user.close(); f_xml.close(); f_slog.close();

    return version;
}

/**
 * \brief Spristupnie chyby, ktora sa vyskytla v pripade vratenia akejkolvek
 * funkcie false.
 * \return Spristupnena chybova hlaska.
 * \retval const QString Spristupnena chybova hlaska.
 */
const QString ProjectDB::error() const {
    return my_error;
}

/**
 * \brief Spristupnenie zoznamu dostupnych projektov.
 * \param projects Zoznam spristupnenych projektov.
 * \return Informacia o uspesnosti prevedenia poziadavku.
 * \retval V pripade uspechu true, v opacnom pripade false.
 */
bool ProjectDB::projects(QStringList & projects) {
    my_error = 0;
    projects = my_pdir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    return true;
}

/**
 * \brief Zaznamenianie simulacie na strane servru.
 * \param username Meno pouzivatela, ktory odsimuloval projekt.
 * \param pname Nazov projektu, ktory bol odsimulovany.
 * \param version Cislo verzie, ktora bola simulovana.
 * \return Pokial pozadovany projekt v danej verzii neexistuje false ako aj v
 * pripade, ze nebolo mozne pridat zaznam do logu.
 * \retval false v pripade chyby, inac true.
 */
bool ProjectDB::update_simlog(const QString & username,
                              const QString & pname,
                              unsigned version) {
    QDir proj(my_pdir.path());

    if (! proj.cd(pname)) {
        my_error = PROJECTDB_ERR_UNPROJ;
        return false;
    }
    if (! proj.cd(QString::number(version))) {
        my_error = PROJECTDB_ERR_UNVER;
        return false;
    }

    QFile f_slog(proj.absoluteFilePath(PROJECTDB_SLOG_FILE));
    if (! f_slog.open(QIODevice::Append)) {
        my_error = PROJECTDB_ERR_WOPEN;
        return false;
    }

    QTextStream log(&f_slog);
    log << QDateTime::currentDateTime().toTime_t()
        << PROJECTDB_SLOG_SEPARATOR << username << '\n';
    f_slog.close();
    return true;
}

/**
 * \brief Rozparsovanie suboru s informaciami o simulaciach do zoznamu.
 * \param log Vystupny parameter - naplneny zoznam TIME, USER pre kazdy
 * zaznam
 * \param pname Nazov projektu, pre ktory sa ma subor s informaciami o simulacii
 * rozparsovat
 * \param version Verzia projektu projektu.
 * \retval false v pripade, ze projekt v danej verzii neexistuje alebo nie je
 * mozne citat data.
 * \return Informacia o uspesnom otvoreni suboru a rozparsovani.
 */
bool ProjectDB::simlog(QStringList & log,
                       const QString & pname,
                       unsigned & version) {
    QDir proj(my_pdir.path());

    if (! proj.cd(pname)) {
        my_error = PROJECTDB_ERR_UNPROJ;
        return false;
    }
    if (! proj.cd(QString::number(version))) {
        my_error = PROJECTDB_ERR_UNVER;
        return false;
    }

    QFile f_slog(proj.absoluteFilePath(PROJECTDB_SLOG_FILE));
    if (! f_slog.open(QIODevice::ReadOnly)) {
        my_error = PROJECTDB_ERR_WOPEN;
        return false;
    }

    int pos;
    QByteArray line;
    while (!  f_slog.atEnd()) {
        line = f_slog.readLine();
        pos = line.lastIndexOf(PROJECTDB_SLOG_SEPARATOR);
        log << line.mid(0, pos);
        // Musi sa odstranit znak noveho riadku.
        log << line.mid(pos + 1, qstrlen(line.data()) - pos - 2);
    }

    f_slog.close();
    return true;
}

/**
 * \brief Spristupnenie poctu verzii zadaneho projektu.
 * \param pname Projekt pre ktory ma byt zisteny pocet verzii.
 * \return Pocet verzii pre zadany projekt.
 * \retval unsigned Pocet verzii pre zadany projekt.
 */
unsigned ProjectDB::version_count(const QString & pname) {
    my_error = 0;
    QDir ver(my_pdir.path());
    if (! ver.cd(pname))
        return 0;

    return ver.entryList(QDir::Dirs | QDir::NoDotAndDotDot).count();
}

