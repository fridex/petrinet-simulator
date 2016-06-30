/**
 * \file     src/pn2012/main.cpp
 * \brief    hlavny subor pre pn2012 s main.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     feb 26 2012
 */

#include <iostream>
#include <cstdlib>
#include <cstring>

#include <pn/client/pn2012.h>

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
         << "Usage: " << pname << " [-h]\n"
         << "\t-h\t\t- print this simple help\n";
}

/**
 * \brief main() pre pn2012.
 * \param argc Pocet argumentov z prikazoveho riadku.
 * \param argv Vektor argumentov z priklazoveho riadku.
 * \return Navratovy kod pre operacny system.
 * \retval EXIT_SUCCESS pri bezchybnom chode, v opacnom pripade EXIT_FAILURE.
 */
int main(int argc, char * argv[]) {
    if (argc == 2 && ! strcmp(argv[1], "-h")) {
        print_help(argv[0]);
        return EXIT_SUCCESS;
    }

    try {

        return pn2012(argc, argv);

    }
    catch (const char * what) {
        std::cout << "> ERROR: " << what << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

