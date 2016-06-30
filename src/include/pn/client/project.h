/**
 * \file     project.h
 * \brief    Reprezentacia petriho siete.
 * \author   Fridolin Pokorny  <fridex.devel@gmail.com>
 * \author   Miroslav Lisik    <xlisik00@stud.fit.vutbr.cz>
 * \date     apr 09 2012
 */

#ifndef PN_CLIENT_PROJECT_H_
#define PN_CLIENT_PROJECT_H_

#include <QGraphicsScene>
#include <QWidget>
#include <QFileInfo>
#include <QImage>
#include <pn/pnobject.h>

/**
 * \brief Trieda pre reprezentaciu projektu.
 */
class Project : public QGraphicsScene {
    Q_OBJECT

    public:
        /* Mody editovania */
        enum Mode {
            MNONE,      /* ziadna akcia */
            MMOVE,      /* presuvat objekt */
            MPLACE,     /* kreslit miesta */
            MTRANS,     /* kreslit prechody */
            MARROW,     /* kreslit sipky */
            MEDIT,      /* editovat objekt */
            MDEL        /* zrusit objekt */
        };
        Project(QObject *parent = 0);
        void xml(QString & data);
        void set_filepath(const QString & path);
        QString filename() const;
        QString filedir() const;
        const QFileInfo & fileinfo() const;
        bool was_saved() const;
        void set_version(unsigned new_version);
        void set_servername(const QString & new_name);
        unsigned version() const;
        bool from_server() const;
        const QString & servername() const;
        void remove_item(QGraphicsItem * item);
        bool set_object_name(PNObject * object, const QString name = "");

    public slots:
        void setMode(Mode mode);

    signals:

    protected:
        void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
        void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
        void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

    private slots:

    private:
        Mode my_mode;
        QGraphicsLineItem *my_line;
        QFileInfo my_fileinfo;
        unsigned int my_pcnt;
        unsigned int my_tcnt;
        unsigned int my_acnt;
        unsigned int my_version; //<! Cislo verzie projektu.
        QString my_servername;   //<! Meno projektu zo serveru.
        bool my_saved; //<! Informacia, ci bol projekt ulozeny pre Save.
        bool no_duplicity(QGraphicsItem *start, QGraphicsItem *end);
        /* Funkcie pre nastavovanie nazvu objektu */
        const QString create_name(PNObject * object);
        bool name_in_list(PNObject * object,
                          QList<QGraphicsItem *> & list,
                          const QString name);

}; // Project

#endif // PN_CLIENT_PROJECT_H_
