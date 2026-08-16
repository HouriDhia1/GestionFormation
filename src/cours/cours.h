#ifndef COURS_H
#define COURS_H

#include <QString>

class Cours
{
public:
    Cours();
    Cours(int id, const QString& titre, const QString& description,
          int dureeHeures, int idFormateur);

    int getId() const;
    QString getTitre() const;
    QString getDescription() const;
    int getDureeHeures() const;
    int getIdFormateur() const;

    void setTitre(const QString& titre);
    void setDescription(const QString& description);
    void setDureeHeures(int dureeHeures);
    void setIdFormateur(int idFormateur);

private:
    int m_id;
    QString m_titre;
    QString m_description;
    int m_dureeHeures;
    int m_idFormateur;
};

#endif // COURS_H