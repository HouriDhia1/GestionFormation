#include "formateur.h"

Formateur::Formateur() : m_id(0) {}

Formateur::Formateur(int id, const QString& nom, const QString& prenom,
                     const QString& email, const QString& specialite,
                     const QDate& dateEmbauche)
    : m_id(id), m_nom(nom), m_prenom(prenom), m_email(email),
    m_specialite(specialite), m_dateEmbauche(dateEmbauche) {}

int Formateur::getId() const { return m_id; }
QString Formateur::getNom() const { return m_nom; }
QString Formateur::getPrenom() const { return m_prenom; }
QString Formateur::getEmail() const { return m_email; }
QString Formateur::getSpecialite() const { return m_specialite; }
QDate Formateur::getDateEmbauche() const { return m_dateEmbauche; }

void Formateur::setNom(const QString& nom) { m_nom = nom; }
void Formateur::setPrenom(const QString& prenom) { m_prenom = prenom; }
void Formateur::setEmail(const QString& email) { m_email = email; }
void Formateur::setSpecialite(const QString& specialite) { m_specialite = specialite; }
void Formateur::setDateEmbauche(const QDate& dateEmbauche) { m_dateEmbauche = dateEmbauche; }