/**
 * \file     simulation.cpp
 * \brief    Implementacia simulacie petriho sieti.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     mar 03 2012
 */

#include <QString>
#include <QVector>
#include <QThread>
#include <QDebug>
#include <QScriptEngine>
#include <QScriptValue>

#include <pn/pnlist.h>
#include <pn/place.h>
#include <pn/arrow.h>
#include <pn/transition.h>
#include <pn/server/debug.h>
#include <pn/server/simulation.h>

const char * SIM_TIME_CYCLE = "Simulation time exceeded!";
const char * SIM_SYN_ERROR  = "Error in expression: ";

/**
 * Maximalny pocet krokov simulacie kvoli zabraneniu zacykleniu.
 */
const unsigned SIMULATION_LOOP_COUNT = 250;

/**
 * \brief Konstruktor.
 */
Simulation::Simulation() {
}

/**
 * \brief Destruktor.
 */
Simulation::~Simulation() {
    my_list.clear();
}

/**
 * \brief Priprava na simulaciu, prevod XML suboru na vnutornu reprezentaciu.
 * \param xml XML reprezentacia petriho siete
 * \return true v pripade, ze petriho siet je korektna
 */
bool Simulation::prepare(const QString & xml) {
    bool rv = my_list.from_xml(xml, true);
    return rv;
}

/**
 * \brief Prevedenie uplnej simulacie petriho siete.
 * \param result vysledna simulacia v XML formate
 * \return false pre indikaciu chyby pri simulacii
 */
bool Simulation::run(QString & result) {
    return this->simulate(result, RUN);
}

/**
 * \brief Spristupnenie chybovej hlasky signalizujucej chybu, ktora nastala.
 * \return chybova hlaska
 */
const QString & Simulation::error() const {
    return my_error;
}

/**
 * \brief Prevedenie kroku simulacie petriho siete.
 * \param result vysledna simulacia v XML formate
 * \return false pre indikaciu chyby pri simulacii
 */
bool Simulation::step(QString & result) {
    return this->simulate(result, STEP);
}

/**
 * \brief Inicializuje strukturu pre simulaciu pre dany prechod.
 * \param trans prechod pre ktory maju byt vytvorene struktuty
 * \param places_to miesta do ktorych sa budu tokeny presuvat
 * \param places_from miesta z ktorych sa budu tokeny presuvat
 */
bool Simulation::init_places(Transition * trans, QVector<SimPart*> * places_to,
                             QVector<SimPart*> * places_from) {
    foreach (Arrow * arr, trans->arrows()) {
        SimPart * sp = new SimPart;
        sp->index = 0;
        sp->name = arr->name();

        if (dynamic_cast<Transition *>(arr->start_pnobject()) == trans) {
            sp->place = dynamic_cast<Place*>(arr->end_pnobject());
            places_to->push_back(sp);
        } else if (dynamic_cast<Transition *>(arr->end_pnobject()) == trans) {
            sp->place = dynamic_cast<Place*>(arr->start_pnobject());
            places_from->push_back(sp);
        } else {
            debug("Error in arrow placement");
            return false;
        }
    }

    return true;
}

/**
 * \brief Dealokacia struktury pre simulaciu.
 * \param struktura simulacie
 */
void Simulation::delete_places(QVector<SimPart*> * places) {
    for (QVector<SimPart*>::iterator it = places->begin();
            it != places->end();) {
        delete *it;
        it = places->erase(it);
    }
}

/**
 * \brief V rekurzii inkrementuje index na tokeny miesta danym indexom idx.
 * Pokial index na tokeny prekroci pocet indexovanych tokenov, index sa zmaze a
 * pokusi sa inkrementovat predchadzajuci index, ak sa nepodari, vracia false.
 * \param idx index miesta, ktoreho index sa ma inkrementovat
 * \param places vektor miest, ktorych sa indexy maju inkrementovat
 * \return false v pripade, ze nie je mozne vytvorit novu permutaciu - nie je
 * mozne inkrementovat  indexovane miesto ani miesta pred
 */
bool Simulation::transition_sim_inc(int idx, QVector<SimPart *> * places) {
    Q_ASSERT(places);

    if (idx < 0) {
        // Podtiekol index v rekurzii.
        return false;
    } else if (places->at(idx)->index
            >= places->at(idx)->place->active_tokens()->size()) {
        // Pretiekol index, nastav aktualny na nula a skus inkrementovat
        // predchadzajuci.
        places->at(idx)->index = 0;
        return transition_sim_inc(idx - 1, places);
    } else {
        // Aktualny index je v poriadku, zvys ho pre zistenie dalsej permutacie.
        places->at(idx)->index++;
        return true;
    }
}

/**
 * \brief Vykona jednu simulaciu nad jednym prechodom s ohladom na permutacie
 * indexov na miesta a ich tokeny.
 * \param idx index na miesto, ktoreho index na tokeny sa bude zvysovat
 * \param places miesta pre simulaciu
 * \param first_run true v pripade, ze ide o prvu permutaciu (index sa nezvysi)
 */
bool Simulation::transition_sim_step(int & idx, QVector<SimPart*> * places,
                                    bool first_run) {
    Q_ASSERT(places);

    if (idx >= places->size()) {
        // Nie je dostatok miest pre uskutocnenie prechodu.
        return false;
    }

    if (first_run
            && places->at(idx)->index
                <  places->at(idx)->place->active_tokens()->size()) {
        return true;
    }

    if (places->at(idx)->index + 1
            >= places->at(idx)->place->active_tokens()->size()) {
        // Je prekroceny index pre indexovanie tokenov, je nutne inkrementovat
        // predchadzajuci pre pokracovanie.
        bool rv = transition_sim_inc(idx - 1, places);
        if (! rv && idx == places->size() - 1) {
            // Uz nie je mozne inkrementovat ziadne predchadzajuce indexy a nie
            // je mozne pokracovat dalsim indexom na miesto -> koniec simulacie
            // prechodu.
            return false;
        }

        // Boli inkrementovane / znulovane predchadzajuce, nastav aktualny na
        // nula a pokracuj dalsim indexom.
        places->at(idx)->index = 0;
        if (! rv)
            ++idx;
        return true;
    } else {
        // Inkrementuj index, pokial nejde o prvu permutaciu. (chybali by tie,
        // ktore zacinaju 0).
        places->at(idx)->index++;
        return true;
    }
}

/**
 * \brief Prevedenie simulacie nad jednym prechodom.
 * \param trans prechod nad ktorym sa ma simulacia previest
 */
bool Simulation::transition_sim(Transition * trans) {
    Q_ASSERT(trans);

    QScriptEngine * engine = new QScriptEngine;
    bool eval_rv;

    QVector<SimPart*> * places_to = new QVector<SimPart*>;
    QVector<SimPart*> * places_from = new QVector<SimPart*>;

    init_places(trans, places_to, places_from);

    int idx = 0;
    bool first_run = true;
    bool fired = false; // Informacia o tom, ci boli rokeny presunute z miesta.
    while (transition_sim_step(idx, places_from, first_run)
            && my_error.isEmpty() && ! fired) {
        fired = false; // Neboli presunute ziadne tokeny v aktualnom cykle.
        first_run = false;

        for (int i = places_from->size() - 1; i >= 0; --i) {
            // Nastav premenne, ktore reprezentuju jednotlive miesta, aktualne
            // hodnoty ber podla indexov vypocitanych v transition_sim_step().
            SimPart * sp = places_from->at(i);
            if (sp->place->active_tokens()->size() == 0) {
                engine->globalObject().setProperty(sp->name, 0);
            } else {
                engine->globalObject().setProperty(sp->name,
                        sp->place->active_tokens()->at(sp->index));
            }
        }

        eval_rv = engine->evaluate(trans->condition()).toBool();
        // Osetrenie chyby.
        if (engine->hasUncaughtException()) {
            my_error = SIM_SYN_ERROR + trans->name();
            return false;
        }

        // Vykonaj podmienku prechodu, ak je true, tak je mozne tokeny presunut.
        if (eval_rv) {
            // Nastav premenne na undefined, aby bolo mozne otestovat, ci sa vo
            // vyraze dane miesto vobec nachadza.
            for (int i = places_to->size() - 1; i >= 0; --i) {
                engine->globalObject().setProperty(places_to->at(i)->name,
                                               engine->undefinedValue());
            }

            // Vykonaj mod prechodu.
            engine->evaluate(trans->mode());

            // Osetrenie chyby.
            if (engine->hasUncaughtException()) {
                my_error = SIM_SYN_ERROR + trans->name();
                return false;
            }

            // Pridaj pasivne tokeny do zadaneho miesta, ak sa vyskytuje hodnota
            // pasivneho tokenu.
            for (int i = places_to->size() - 1; i >= 0; --i) {
                QScriptValue val = engine->globalObject()
                                    .property(places_to->at(i)->name);
                if (val.isUndefined())
                    continue;

                places_to->at(i)->place->add_passive_token(val.toInteger());

                // Splnil sa aspon jeden prechod s tokenmi, takze sa musia
                // odobrat z miesta from.
                fired = true;
            }
        }

        if (fired) {
            // Odober tokeny, ktore boli pouzite.
            for(int i = places_from->size() - 1; i >=0; --i) {
                if (places_from->at(i)->index
                        >= places_from->at(i)->place->active_tokens()->size())
                    continue;
                places_from->at(i)->place->active_tokens()
                    ->remove(places_from->at(i)->index);
            }
        }
    }


    this->delete_places(places_to);
    this->delete_places(places_from);

    delete places_to;
    delete places_from;

    engine->collectGarbage();
    delete engine;

    return fired;
}

/**
 * \brief Implementacia simulacie petriho siete.
 * \param result vysledok simulacie v XML
 * \param type typ simulacie (krok, odsimulovanie)
 * \return false pre indikaciu chyby pri simulacii (prekroceny limit)
 */
bool Simulation::simulate(QString & result, enum SimType type) {
    Transition * t = 0;     // Pomocny ukazatel na prechod.
    Transition * tsim = 0;  // Prechod, ktory bude simulovany.
    Place * p;
    bool rv;
    unsigned count;
    unsigned nop_count;   // Pocet prechodov u ktorych nedoslo k presunu tokenov.

    my_error.clear();

    result.clear();

    count = type == RUN ? SIMULATION_LOOP_COUNT : 1;

    do {
        nop_count = 0;
        for (unsigned i = my_list.transition_count(); i != 0; --i) {
            t = 0;
            tsim = 0;

            // Vyber prechod s najvyssou prioritou.
            for (PNList_iter it = my_list.begin(); it != my_list.end(); ++it) {
                if ((t = dynamic_cast<Transition*>(*it))) {
                    if ((! tsim && t->active())
                            || (tsim && t->active()
                                && tsim->priority() < t->priority())) {
                        tsim = t;
                    }
                }
            }

            // Odsimulovanie a zneaktivnenie prechodu.
            if (tsim) {
                rv = transition_sim(tsim);

                if (! rv && ! my_error.isEmpty()) {
                    return false; // Doslo k chybe pri simulacii.
                } else if (! rv) {
                    nop_count++; // V simulacii nebol token premiestneny.
                }

                tsim->set_active(false);
            }
        }

        // Obnovenie siete a priprava pre dalsi beh simulacie.
        for (PNList_iter it = my_list.begin(); it != my_list.end(); ++it) {
            if ((p = dynamic_cast<Place *>(*it))) {
                p->flush_tokens(); // Vsetky tokeny zarad medzi aktivne.
            } else if ((t = dynamic_cast<Transition*>(*it))) {
                t->set_active(true);
            }
        }

    // Simuluje sa kym sa neprekroci limit vyhradeny pre simulaciu alebo kym uz
    // nie je co simulovat - pocet prechodov u ktorych nebol token presunuty, je
    // rovny poctu celkovych prechodov.
    } while (--count && nop_count != my_list.transition_count());

    // Bolo presiahnute maximalne mnozstvo iteracii pri plnej simulacii.
    if (count == 0 && type == RUN) {
        my_error = SIM_TIME_CYCLE;
        return false;
    }

    my_list.xml(result);
    return true;
}

