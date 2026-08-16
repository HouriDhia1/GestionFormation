#include "cours.h"

Cours::Cours() : m_id(0), m_dureeHeures(0), m_idFormateur(0) {}

Cours::Cours(int id, const QString& titre, const QString& description,
             int dureeHeures, int idFormateur)
    : m_id(id), m_titre(titre), m_description(description),
    m_dureeHeures(dureeHeures), m_idFormateur(idFormateur) {}

int Cours::getId() const { return m_id; }
QString Cours::getTitre() const { return m_titre; }
QString Cours::getDescription() const { return m_description; }
int Cours::getDureeHeures() const { return m_dureeHeures; }
int Cours::getIdFormateur() const { return m_idFormateur; }

void Cours::setTitre(const QString& titre) { m_titre = titre; }
void Cours::setDescription(const QString& description) { m_description = description; }
void Cours::setDureeHeures(int dureeHeures) { m_dureeHeures = dureeHeures; }
void Cours::setIdFormateur(int idFormateur) { m_idFormateur = idFormateur; }