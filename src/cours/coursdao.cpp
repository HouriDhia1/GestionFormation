#include "coursdao.h"
#include "../database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

bool CoursDAO::create(const Cours& cours)
{
    QSqlQuery query;
    query.prepare("INSERT INTO COURS (titre, description, duree_heures, id_formateur) "
                  "VALUES (:titre, :description, :duree_heures, :id_formateur)");
    query.bindValue(":titre", cours.getTitre());
    query.bindValue(":description", cours.getDescription());
    query.bindValue(":duree_heures", cours.getDureeHeures());
    query.bindValue(":id_formateur", cours.getIdFormateur());

    if (!query.exec()) {
        qDebug() << "❌ Erreur insertion cours :" << query.lastError().text();
        return false;
    }
    return true;
}

QList<Cours> CoursDAO::readAll()
{
    QList<Cours> coursList;
    QSqlQuery query("SELECT * FROM COURS ORDER BY id_cours");

    while (query.next()) {
        Cours c(
            query.value("id_cours").toInt(),
            query.value("titre").toString(),
            query.value("description").toString(),
            query.value("duree_heures").toInt(),
            query.value("id_formateur").toInt()
            );
        coursList.append(c);
    }
    return coursList;
}

bool CoursDAO::update(const Cours& cours)
{
    QSqlQuery query;
    query.prepare("UPDATE COURS SET titre = :titre, description = :description, "
                  "duree_heures = :duree_heures, id_formateur = :id_formateur "
                  "WHERE id_cours = :id");
    query.bindValue(":titre", cours.getTitre());
    query.bindValue(":description", cours.getDescription());
    query.bindValue(":duree_heures", cours.getDureeHeures());
    query.bindValue(":id_formateur", cours.getIdFormateur());
    query.bindValue(":id", cours.getId());

    if (!query.exec()) {
        qDebug() << "❌ Erreur mise à jour cours :" << query.lastError().text();
        return false;
    }
    return true;
}

bool CoursDAO::remove(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM COURS WHERE id_cours = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "❌ Erreur suppression cours :" << query.lastError().text();
        return false;
    }
    return true;
}

Cours CoursDAO::readById(int id)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM COURS WHERE id_cours = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        return Cours(
            query.value("id_cours").toInt(),
            query.value("titre").toString(),
            query.value("description").toString(),
            query.value("duree_heures").toInt(),
            query.value("id_formateur").toInt()
            );
    }
    return Cours();
}

// Métiers avancés
QList<Cours> CoursDAO::search(const QString& titre, const QString& description, int dureeMin, int dureeMax)
{
    QList<Cours> coursList;
    QSqlQuery query;
    query.prepare("SELECT * FROM COURS WHERE titre LIKE :titre "
                  "AND description LIKE :description "
                  "AND duree_heures BETWEEN :dureeMin AND :dureeMax");
    query.bindValue(":titre", "%" + titre + "%");
    query.bindValue(":description", "%" + description + "%");
    query.bindValue(":dureeMin", dureeMin);
    query.bindValue(":dureeMax", dureeMax);

    if (query.exec()) {
        while (query.next()) {
            Cours c(
                query.value("id_cours").toInt(),
                query.value("titre").toString(),
                query.value("description").toString(),
                query.value("duree_heures").toInt(),
                query.value("id_formateur").toInt()
                );
            coursList.append(c);
        }
    } else {
        qDebug() << "❌ Erreur recherche cours :" << query.lastError().text();
    }
    return coursList;
}

QList<Cours> CoursDAO::getCoursByFormateur(int idFormateur)
{
    QList<Cours> coursList;
    QSqlQuery query;
    query.prepare("SELECT * FROM COURS WHERE id_formateur = :id_formateur");
    query.bindValue(":id_formateur", idFormateur);

    if (query.exec()) {
        while (query.next()) {
            Cours c(
                query.value("id_cours").toInt(),
                query.value("titre").toString(),
                query.value("description").toString(),
                query.value("duree_heures").toInt(),
                query.value("id_formateur").toInt()
                );
            coursList.append(c);
        }
    }
    return coursList;
}