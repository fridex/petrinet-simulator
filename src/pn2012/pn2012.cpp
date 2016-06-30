/**
 * \file     pn2012.cpp
 * \brief    hlavny subor pre pn2012
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     feb 29 2012
 */

#include <QApplication>
#include <QtGui>

#include <pn/client/pn2012.h>
#include <pn/client/settings.h>
#include <pn/client/mainwindow.h>
#include <pn/client/connection.h>

/**
 * \brief Hlavna funkcia klienta pre inicializaciu GUI a spojenim so serverom.
 * \param argc Pocet argumentov z prikazoveho riadku
 * \param argv Argument vektor z prikazoveho riadku
 * \retval int Vysledok z QObject exec() hlavneho okna.
 */
int pn2012(int argc, char * argv[]) {
    bool rv;
    QApplication app(argc, argv);

    if (argc > 2 && ! qstrcmp(argv[1], "-c"))
        Settings::instance()->load_file(argv[2]);
    else
        Settings::instance()->load_file();

    // Uzivatelske nastavenia pripojenia.
    Connection::instance()->set_host(Settings::instance()->host(),
                                     Settings::instance()->port());

    Connection::instance()->set_username(Settings::instance()->username(),
                                         Settings::instance()->password());

    // Nastavenie pisma aplikacie.
    app.setFont(Settings::instance()->font());

    // Nastavenie uzivatelskej temy editoru.
    Arrow::set_style(Settings::instance()->arrow_style());
    Place::set_style(Settings::instance()->place_style());
    Transition::set_style(Settings::instance()->transition_style());

    MainWindow win;
    win.show();

    rv = app.exec();

    // Dealokacia alokovanych singletonov.
    Settings::instance()->store_file();
    Settings::instance()->singleton_destroy();
    Connection::instance()->singleton_destroy();

    return rv;
}

