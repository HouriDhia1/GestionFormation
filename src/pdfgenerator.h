#ifndef PDFGENERATOR_H
#define PDFGENERATOR_H

#include <QString>
#include <QList>
#include "formateur/formateur.h"
#include "cours/cours.h"

class PDFGenerator
{
public:
    static bool genererPDFFormateurs(const QList<Formateur>& formateurs, const QString& chemin);
    static bool genererPDFCours(const QList<Cours>& cours, const QString& chemin);

private:
    static QString getCurrentDate();
};

#endif // PDFGENERATOR_H