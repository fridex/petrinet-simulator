/**
 * \file     settings.h
 * \brief    Trieda pre nastavenia programu.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     apr 09 2012
 */

#ifndef PN_CLIENT_SETTINGS_H_
#define PN_CLIENT_SETTINGS_H_

#include <pn/arrow.h>
#include <pn/place.h>
#include <pn/transition.h>

// forward
class QString;
class QFont;

/**
 * \brief Singleton pre nastavenia programu.
 */
class Settings {
  public:
    static Settings * instance();

    bool load_file(const char * path = 0);
    bool store_file();

    void singleton_destroy();

    Arrow::ArrowStyle arrow_style() const;
    Transition::TransitionStyle transition_style() const;
    Place::PlaceStyle place_style() const;

    const QFont & font() const;

    const QString & host() const;
    const QString & username() const;
    const QString & password() const;
    unsigned port() const;

    void set_arrow_style(Arrow::ArrowStyle style);
    void set_arrow_style(int style);
    void set_transition_style(Transition::TransitionStyle style);
    void set_transition_style(int style);
    void set_place_style(Place::PlaceStyle style);
    void set_place_style(int style);
    void set_port(unsigned new_port);

    void set_font(const QString & new_font);
    void set_font(const QFont & new_font);

    void set_host(const QString & new_host);
    void set_username(const QString & new_username);
    void set_password(const QString & new_password);

  private:
    Settings();
    ~Settings();

    static Settings * my_instance;

    Arrow::ArrowStyle my_arrow_style;
    Transition::TransitionStyle my_transition_style;
    Place::PlaceStyle my_place_style;

    QFont my_font;

    QString my_host;
    QString my_username;
    QString my_password;
    QString my_path;

    unsigned my_port;

    bool parse();
}; // Settings

#endif // PN_CLIENT_SETTINGS_H_

