#ifndef FORMATEURDAO_H
#define FORMATEURDAO_H

#include <QList>
#include "formateur.h"

class FormateurDAO
{
public:
    static bool create(const Formateur& formateur);
    static QList<Formateur> readAll();
    static bool update(const Formateur& formateur);
    static bool remove(int id);
    static Formateur readById(int id);
    static bool emailExiste(const QString& email);
    static QList<Formateur> search(const QString& nom, const QString& prenom, const QString& specialite);
};

#endif // FORMATEURDAO_H