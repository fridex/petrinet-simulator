/**
 * \file     simulation.h
 * \brief    Simulacie petriho sieti.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     mar 03 2012
 */

#ifndef PN_SERVER_SIMULATION_H_
#define PN_SERVER_SIMULATION_H_

#include <pn/pnlist.h>
#include <QVector>
#include <QString>

// forward
class Place;
class Transition;

/**
 * \brief Trieda pre simulaciu petriho sieti.
 */
class Simulation {
  public:
    Simulation();
    ~Simulation();
    bool prepare(const QString & xml);
    bool run(QString & result);
    bool step(QString & result);
    const QString & error() const;

  private:
    /**
     * \brief Typ simulacie.
     */
    enum SimType {
        RUN,
        STEP
    };

    /**
     * \brief Struktura pre uchovanie informacii o mieste pri simulacii.
     */
    struct SimPart {
        Place * place;
        QString name;
        int index;
    };

    bool simulate(QString & result, enum SimType type);
    bool transition_sim(Transition * t);

    bool transition_sim_step(int & idx, QVector<SimPart*> * places, bool first);
    bool transition_sim_inc(int idx, QVector<SimPart *> * places);

    bool init_places(Transition * trans, QVector<SimPart*> * places_to,
                                 QVector<SimPart*> * places_from);
    void delete_places(QVector<SimPart*> * places);

    QString my_error;
    PNList my_list;

  private:
    /**
     * \brief DISABLE_COPY_AND_ASSIGN
     */
    Simulation(const Simulation &);
    /**
     * \brief DISABLE_COPY_AND_ASSIGN
     */
    void operator=(const Simulation &);
}; // Simulation

#endif // PN_SERVER_SIMULATION_H_

