/**
 * \file     settings.cpp
 * \brief    Trieda pre nastavenia programu.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     apr 09 2012
 */

#include <QFont>
#include <QFile>
#include <QMap>
#include <QString>
#include <QMessageBox>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include <pn/pnobject.h>
#include <pn/arrow.h>
#include <pn/place.h>
#include <pn/transition.h>
#include <pn/client/settings.h>

/**
 * Predvoleny subor s konfiguraciou.
 */
const char *    SETTINGS_DEFAULT_CONFIG_PATH   = "_pn2012.conf";

/**
 * Predvolene nastavenia pripojenia.
 */
const char *    SETTINGS_DEFAULT_HOST          = "localhost";
const char *    SETTINGS_DEFAULT_USER          = "user";
const char *    SETTINGS_DEFAULT_PASSWORD      = "user";
const unsigned  SETTINGS_DEFAULT_PORT          = 44444;

/**
 * Nazvy tagov v konfiguracnom subore.
 */
const char *    CONFIG_START                   = "pn-config";
const char *    CONFIG_HOST                    = "host";
const char *    CONFIG_USER                    = "user";
const char *    CONFIG_PASSWORD                = "password";
const char *    CONFIG_PORT                    = "port";
const char *    CONFIG_FONT                    = "font";
const char *    CONFIG_ARROW_STYLE             = "arrow-style";
const char *    CONFIG_PLACE_STYLE             = "place-style";
const char *    CONFIG_TRANSITION_STYLE        = "transition-style";


/**
 * Predvolene nastavenia stylu.
 */
const
Arrow::ArrowStyle SETTINGS_DEFAULT_ARROW_STYLE = Arrow::Classic;

const
Place::PlaceStyle SETTINGS_DEFAULT_PLACE_STYLE = Place::Classic;

const
Transition::TransitionStyle SETTINGS_DEFAULT_TRANSITION_STYLE = Transition::Classic;

/**
 * Inicializacia singletonu.
 */
Settings * Settings::my_instance = 0;


/**
 * \brief Konstruktor.
 */
Settings::Settings() {
}

/**
 * \brief Destruktor.
 */
Settings::~Settings() {
}

/**
 * \brief Spristupnenie instancie singletonu.
 * \return instancia singletonu
 */
Settings * Settings::instance() {
    if (! my_instance)
        my_instance = new Settings;

    return my_instance;
}

/**
 * \brief Spracovanie konfiguracneho suboru.
 * \return false v pripade chyby
 */
bool Settings::parse() {
    QFile config(my_path);
    int style;

    if (! config.open(QIODevice::ReadOnly)) {
        QMessageBox msgBox(QMessageBox::Critical,
                           QObject::tr("Failed to open personal settings!"),
                           config.errorString());
        msgBox.exec();
        return false;
    }

    QXmlStreamReader * xml_reader = new QXmlStreamReader(&config);

    while (! xml_reader->atEnd() && ! xml_reader->hasError()) {
        xml_reader->readNext();

        if (xml_reader->isStartElement()) {
            if (xml_reader->name() == CONFIG_HOST) {
                Settings::instance()->set_host(xml_reader->readElementText());
            } else if (xml_reader->name() == CONFIG_USER) {
                Settings::instance()->set_username(xml_reader->readElementText());
            } else if (xml_reader->name() == CONFIG_PASSWORD) {
                Settings::instance()->set_password(xml_reader->readElementText());
            } else if (xml_reader->name() == CONFIG_PORT) {
                Settings::instance()->set_port(xml_reader->readElementText().toUInt());
            } else if (xml_reader->name() == CONFIG_FONT) {
                Settings::instance()->set_font(xml_reader->readElementText());
            } else if ((xml_reader->name() == CONFIG_ARROW_STYLE)) {
                style = xml_reader->readElementText().toInt();
                Settings::instance()->set_arrow_style(style);
            } else if ((xml_reader->name() == CONFIG_TRANSITION_STYLE)) {
                style = xml_reader->readElementText().toInt();
                Settings::instance()->set_transition_style(style);
            } else if ((xml_reader->name() == CONFIG_PLACE_STYLE)) {
                style = xml_reader->readElementText().toInt();
                Settings::instance()->set_place_style(style);
            }
        }
        else if (xml_reader->hasError()) {
            QMessageBox msgBox(QMessageBox::Critical,
                               QObject::tr("Failed to parse personal settings!"),
                               xml_reader->errorString());
            msgBox.exec();
            delete xml_reader;
            return false;
        }
    }

    delete xml_reader;

    return true;
}

/**
 * \brief Nacitanie konfiguracneho suboru.
 * \param path uplna cesta k suboru spolu s nazvom suboru
 * \return true v pripade spravneho nacitania konfiguracneho suboru
 */
bool Settings::load_file(const char * path) {
    if (path)
        my_path = path;
    else
        my_path = SETTINGS_DEFAULT_CONFIG_PATH;

    QFile config(my_path);

    if (! config.exists()) {
        my_arrow_style      = SETTINGS_DEFAULT_ARROW_STYLE;
        my_transition_style = SETTINGS_DEFAULT_TRANSITION_STYLE;
        my_place_style      = SETTINGS_DEFAULT_PLACE_STYLE;

        my_host             = SETTINGS_DEFAULT_HOST;
        my_username         = SETTINGS_DEFAULT_USER;
        my_password         = SETTINGS_DEFAULT_PASSWORD;
        my_port             = SETTINGS_DEFAULT_PORT;

        // Pouzi systemovy font.
        my_font.setStyleHint(QFont::System);
    } else {
        return this->parse();
    }

    return true;
}

/**
 * \brief Ulozenie nastaveni do konfiguracneho suboru.
 * \return true v pripade spravneho ulozenia konfiguracneho suboru
 */
bool Settings::store_file() {
    QFile config(my_path);
    int style;

    if (! config.open(QIODevice::WriteOnly)) {
        QMessageBox msgBox(QMessageBox::Critical,
                           QObject::tr("Failed to save personal settings!"),
                           config.errorString());
        msgBox.exec();
        return false;
    }

    QXmlStreamWriter * xml_writer = new QXmlStreamWriter();
    xml_writer->setDevice(&config);
    xml_writer->setAutoFormatting(true);

    // Vytvorenie pociatocnych nastaveni.
    xml_writer->writeStartDocument();
    xml_writer->writeStartElement(CONFIG_START);

    xml_writer->writeStartElement(CONFIG_ARROW_STYLE);
    style = static_cast<int>(Settings::instance()->arrow_style());
    xml_writer->writeCharacters(QString::number(style));
    xml_writer->writeEndElement();

    xml_writer->writeStartElement(CONFIG_PLACE_STYLE);
    style = static_cast<int>(Settings::instance()->place_style());
    xml_writer->writeCharacters(QString::number(style));
    xml_writer->writeEndElement();

    xml_writer->writeStartElement(CONFIG_TRANSITION_STYLE);
    style = static_cast<int>(Settings::instance()->transition_style());
    xml_writer->writeCharacters(QString::number(style));
    xml_writer->writeEndElement();

    xml_writer->writeStartElement(CONFIG_HOST);
    xml_writer->writeCharacters(Settings::instance()->host());
    xml_writer->writeEndElement();

    xml_writer->writeStartElement(CONFIG_USER);
    xml_writer->writeCharacters(Settings::instance()->username());
    xml_writer->writeEndElement();

    xml_writer->writeStartElement(CONFIG_PASSWORD);
    xml_writer->writeCharacters(Settings::instance()->password());
    xml_writer->writeEndElement();

    xml_writer->writeStartElement(CONFIG_PORT);
    xml_writer->writeCharacters(QString::number(Settings::instance()->port()));
    xml_writer->writeEndElement();

    xml_writer->writeStartElement(CONFIG_FONT);
    xml_writer->writeCharacters(Settings::instance()->font().toString());
    xml_writer->writeEndElement();

    // Zapis koncoveho tagu a ukoncenie prace so suborom.
    xml_writer->writeEndElement();
    xml_writer->writeEndDocument();

    delete xml_writer;
    config.close();

    return true;
}


/**
 * \brief Dealokacia singletonu.
 */
void Settings::singleton_destroy() {
    if (my_instance) {
        delete my_instance;
        my_instance = NULL;
    }
}

/**
 * \brief Spristupnenie nastaveneho stylu sipky.
 * \return nastaveny styl sipky
 */
Arrow::ArrowStyle Settings::arrow_style() const {
    return my_arrow_style;
}

/**
 * \brief Spristupnenie nastaveneho stylu prechodu.
 * \return nastaveny styl prechodu
 */
Transition::TransitionStyle Settings::transition_style() const {
    return my_transition_style;
}

/**
 * \brief Spristupnenie nastaveneho stylu miesta.
 * \return nastaveny styl prechodu
 */
Place::PlaceStyle Settings::place_style() const {
    return my_place_style;
}


/**
 * \brief Spristupnenie nastaveneho stylu pisma.
 * \return nastaveny styl pisma
 */
const QFont & Settings::font() const {
    return my_font;
}

/**
 * \brief Spristupnenie nastaveneho hostu.
 * \return nastaveny host
 */
const QString & Settings::host() const {
    return my_host;
}

/**
 * \brief Spristupnenie nastaveneho mena uzivatela.
 * \return nastavene meno uzivatela
 */
const QString & Settings::username() const {
    return my_username;
}

/**
 * \brief Spristupnenie nastaveneho hesla uzivatela.
 * \return nastavene heslo uzivatela
 */
const QString & Settings::password() const {
    return my_password;
}

/**
 * \brief Nastavenie portu.
 * \param new_port novo nastavene cislo portu
 */
void Settings::set_port(unsigned new_port) {
    my_port = new_port;
}

/**
 * \brief Spristupnenie nastaveneho portu.
 * \return nastavene cislo portu
 */
unsigned Settings::port() const {
    return my_port;
}

/**
 * \brief Nastavenie noveho stylu sipky.
 * \param style novy styl
 */
void Settings::set_arrow_style(Arrow::ArrowStyle style) {
    my_arrow_style = style;
}

/**
 * \brief Nastavenie noveho stylu sipky.
 * \param style novy styl
 */
void Settings::set_arrow_style(int style) {
    my_arrow_style = static_cast<Arrow::ArrowStyle>(style);
}

/**
 * \brief Nastavenie noveho stylu prechodu.
 * \param style novy styl
 */
void Settings::set_transition_style(Transition::TransitionStyle style) {
    my_transition_style = style;
}

/**
 * \brief Nastavenie noveho stylu prechodu.
 * \param style novy styl
 */
void Settings::set_transition_style(int style) {
    my_transition_style = static_cast<Transition::TransitionStyle>(style);
}

/**
 * \brief Nastavenie noveho stylu miesta.
 * \param style novy styl
 */
void Settings::set_place_style(Place::PlaceStyle style) {
    my_place_style = style;
}

/**
 * \brief Nastavenie noveho stylu miesta.
 * \param style novy styl
 */
void Settings::set_place_style(int style) {
    my_place_style = static_cast<Place::PlaceStyle>(style);
}

/**
 * \brief Nastavenie noveho stylu sipky.
 * \param new_font novy styl pisma
 */
void Settings::set_font(const QString & new_font) {
    my_font.fromString(new_font);
}

/**
 * \brief Nastavenie noveho stylu sipky.
 * \param new_font novy styl pisma
 */
void Settings::set_font(const QFont & new_font) {
    my_font = new_font;
}

/**
 * \brief Nastavenie noveho hostu.
 * \param new_host novy host servru
 */
void Settings::set_host(const QString & new_host) {
    my_host = new_host;
}

/**
 * \brief Nastavenie noveho mena pouzivatela.
 * \param new_username nove meno pouzivatela
 */
void Settings::set_username(const QString & new_username) {
    my_username = new_username;
}

/**
 * \brief Nastavenie noveho hesla pouzivatela.
 * \param new_password nove heslo pouzivatela
 */
void Settings::set_password(const QString & new_password) {
    my_password = new_password;
}

