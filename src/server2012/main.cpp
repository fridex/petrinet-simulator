/**
 * \file     src/server2012/main.cpp
 * \brief    hlavny subor pre server2012 s main.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     feb 26 2012
 */

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <QApplication>
#include <signal.h>

#include <pn/server/server2012.h>

/**
 * \brief Struktura pre spracovane odpovede.
 */
struct Param {
    /**
     * \brief true ak je vyzadovana napoved
     */
    bool help;
    /**
     * \brief cislo portu na ktorom ma odpocuvat server
     */
    unsigned port;
    /**
     * \brief cesta k databazy uzivatelov
     */
    const char * userdb;
    /**
     * \brief cesta k databazy projektov
     */
    const char * projectdb;
};

/**
 * \brief Vytlaci napovedu pre uzivatela na standardny vystup.
 * \param pname Nazov programu.
 * \retval void
 */
void print_help(const char * pname) {
    using namespace std;

    cout << "Petri net simulator and editor - server\n"
         << "Fridolin Pokorny\t<fridex.devel@gmail.com>\n"
         << "Miroslav Lisik\t\t<xlisik00@stud.fit.vutbr.cz>\n\n"
         << "Usage: " << pname << " --userdb FILE --projectdb DIR [-h] [-p PORT]\n"
         << "\t-h\t\t- print this simple help\n"
         << "\t-p PORT\t\t- specify port to be used\n"
         << "\t--userdb FILE\t- texfile with registered users\n"
         << "\t--projectdb DIR\t- directory with a project tree\n";
}

void sig_catcher(int sig) {
    std::cout << "\rSignal catched: " << sig << std::endl;
    throw 0;
}

/**
 * \brief Spracuje parametre z prikazoveho riadku pre server.
 * \param p Struktura pre spracovanie zaznamenanie spracovanych parametrov.
 * \param argc Pocet argumentov z prikazoveho riadku.
 * \param argv Vektor argumentov z priklazoveho riadku.
 * \return Informaciu o spravnosti parametroch.
 * \retval true pri spravnych parametroch.
 */
bool parse_param(Param & p, int argc, char * argv[]) {
    char * nptr;
    p.help = false; p.port = 0; p.userdb = 0;

    for (int i = 1; i < argc; ++i) {
        if (! strcmp(argv[i], "-h")) {
            p.help = true;
            if (argc != 2)
                return false;
        } else if (! strcmp(argv[i], "-p")) {
            ++i;
            if (i >= argc) {
                std::cerr << "Option '-p requires an option!\n";
                return false;
            }
            p.port = strtoul(argv[i], &nptr, 10);
            if (! nptr || *nptr != '\0') {
                std::cerr << "Bad port number!\n";
                return false;
            }
        } else if (! strcmp(argv[i], "--userdb")) {
            ++i;
            if (i >= argc) {
                std::cerr << "Option '--userdb' requires an option!\n";
                return false;
            }
            p.userdb = argv[i];
        } else if (! strcmp(argv[i], "--projectdb")) {
            ++i;
            if (i >= argc) {
                std::cerr << "Option '--projectdb' requires an option!\n";
                return false;
            }
            p.projectdb = argv[i];
        } else {
            std::cerr << "Unknown option: " << argv[i] << std::endl;
            return false;
        }
    }

    // --userdb a --projectdb musi byt vzdy zadane
    return p.userdb != 0 && p.projectdb != 0;
}

/**
 * \brief main() pre server2012
 * \param argc Pocet argumentov z prikazoveho riadku.
 * \param argv Vektor argumentov z priklazoveho riadku.
 * \return Navratovy kod pre operacny system.
 * \retval EXIT_SUCCESS pri bezchybnom chode, v opacnom pripade EXIT_FAILURE.
 */
int main(int argc, char * argv[]) {
    Param p;

    if (! parse_param(p, argc, argv)) {
        print_help(argv[0]);
        return EXIT_FAILURE;
    } else if (p.help) {
        print_help(argv[0]);
        return EXIT_SUCCESS;
    }

    signal(SIGTERM, sig_catcher);
    signal(SIGINT, sig_catcher);

    try {

        QApplication a(argc, argv);
        // Hlavna smycka serveru.
        server2012(p.port, p.userdb, p.projectdb);

    }
    catch (const char * what) {
        std::cout << "> FATAL ERROR: " << what << std::endl;
        return EXIT_FAILURE;
    }
    catch (int num) {
        return num;
    }

    return EXIT_SUCCESS;
}

