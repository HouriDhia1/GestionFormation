#ifndef COURSDAO_H
#define COURSDAO_H

#include <QList>
#include "cours.h"

class CoursDAO
{
public:
    static bool create(const Cours& cours);
    static QList<Cours> readAll();
    static bool update(const Cours& cours);
    static bool remove(int id);
    static Cours readById(int id);
    static bool titreExiste(const QString& titre, int idCoursExclu = -1);

    // 📎 GESTION DES FICHIERS ATTACHÉS
    static bool attacherFichier(int idCours, const QString& cheminFichier);
    static QString getFichierAttache(int idCours);

    // 🔍 RECHERCHE MULTICRITÈRES
    static QList<Cours> search(const QString& titre, const QString& description, int duree);

    // 📋 AUTRES MÉTHODES
    static QList<Cours> getCoursByFormateur(int idFormateur);
};

#endif // COURSDAO_H