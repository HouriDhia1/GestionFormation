#ifndef FORMATEUR_H
#define FORMATEUR_H

#include <QString>
#include <QDate>

class Formateur
{
public:
    Formateur();
    Formateur(int id, const QString& nom, const QString& prenom,
              const QString& email, const QString& specialite,
              const QDate& dateEmbauche);

    int getId() const;
    QString getNom() const;
    QString getPrenom() const;
    QString getEmail() const;
    QString getSpecialite() const;
    QDate getDateEmbauche() const;

    void setNom(const QString& nom);
    void setPrenom(const QString& prenom);
    void setEmail(const QString& email);
    void setSpecialite(const QString& specialite);
    void setDateEmbauche(const QDate& dateEmbauche);

private:
    int m_id;
    QString m_nom;
    QString m_prenom;
    QString m_email;
    QString m_specialite;
    QDate m_dateEmbauche;
};

#endif // FORMATEUR_H