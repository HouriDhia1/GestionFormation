#include "coursdao.h"
#include "../database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
bool CoursDAO::attacherFichier(int idCours, const QString& cheminFichier) {
    QSqlQuery query;
    query.prepare("UPDATE COURS SET fichier_attache = :chemin WHERE id_cours = :id");
    query.bindValue(":chemin", cheminFichier);
    query.bindValue(":id", idCours);
    return query.exec();
}

bool CoursDAO::titreExiste(const QString& titre, int idCoursExclu)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM COURS WHERE titre = :titre AND id_cours != :id");
    query.bindValue(":titre", titre);
    query.bindValue(":id", idCoursExclu);

    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}
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

QList<Cours> CoursDAO::search(const QString& titre, const QString& description, int duree)
{
    QList<Cours> resultats;

    QString queryStr = R"(
        SELECT c.id_cours, c.titre, c.description, c.duree_heures, c.id_formateur,
               f.nom || ' ' || f.prenom AS formateur_nom
        FROM COURS c
        JOIN FORMATEUR f ON c.id_formateur = f.id_formateur
        WHERE 1=1
    )";

    if (!titre.isEmpty()) {
        queryStr += " AND UPPER(c.titre) LIKE UPPER(:titre)";
    }
    if (!description.isEmpty()) {
        queryStr += " AND UPPER(c.description) LIKE UPPER(:description)";
    }
    if (duree > 0) {
        queryStr += " AND c.duree_heures = :duree";
    }

    queryStr += " ORDER BY c.id_cours";

    QSqlQuery query;
    query.prepare(queryStr);

    if (!titre.isEmpty()) {
        query.bindValue(":titre", "%" + titre + "%");
    }
    if (!description.isEmpty()) {
        query.bindValue(":description", "%" + description + "%");
    }
    if (duree > 0) {
        query.bindValue(":duree", duree);
    }

    if (!query.exec()) {
        qDebug() << "❌ Erreur recherche cours :" << query.lastError().text();
        return resultats;
    }

    while (query.next()) {
        Cours c(
            query.value("id_cours").toInt(),
            query.value("titre").toString(),
            query.value("description").toString(),
            query.value("duree_heures").toInt(),
            query.value("id_formateur").toInt()
            );
        resultats.append(c);
    }

    return resultats;
}
// ============================================================
// RÉCUPÉRER LE CHEMIN DU FICHIER ATTACHÉ
// ============================================================

QString CoursDAO::getFichierAttache(int idCours)
{
    QSqlQuery query;
    query.prepare("SELECT fichier_attache FROM COURS WHERE id_cours = :id");
    query.bindValue(":id", idCours);

    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }
    return "";
}
// ============================================================
// DASHBOARD COURS
// ============================================================

QMap<QString, QVariant> CoursDAO::getCoursDetails(int idCours)
{
    QMap<QString, QVariant> details;

    QSqlQuery query;
    query.prepare(R"(
        SELECT c.id_cours, c.titre, c.description, c.duree_heures,
               f.id_formateur, f.nom, f.prenom, f.email, f.specialite
        FROM COURS c
        JOIN FORMATEUR f ON c.id_formateur = f.id_formateur
        WHERE c.id_cours = :id
    )");
    query.bindValue(":id", idCours);

    if (query.exec() && query.next()) {
        details["id_cours"] = query.value("id_cours").toInt();
        details["titre"] = query.value("titre").toString();
        details["description"] = query.value("description").toString();
        details["duree_heures"] = query.value("duree_heures").toInt();
        details["id_formateur"] = query.value("id_formateur").toInt();
        details["nom_formateur"] = query.value("nom").toString();
        details["prenom_formateur"] = query.value("prenom").toString();
        details["email_formateur"] = query.value("email").toString();
        details["specialite_formateur"] = query.value("specialite").toString();
    }

    return details;
}

QList<Cours> CoursDAO::getAutresCoursByFormateur(int idFormateur, int idCoursExclu)
{
    QList<Cours> coursList;

    QSqlQuery query;
    query.prepare("SELECT * FROM COURS WHERE id_formateur = :id AND id_cours != :exclu ORDER BY id_cours");
    query.bindValue(":id", idFormateur);
    query.bindValue(":exclu", idCoursExclu);

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

int CoursDAO::getTotalCoursByFormateur(int idFormateur)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM COURS WHERE id_formateur = :id");
    query.bindValue(":id", idFormateur);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

int CoursDAO::getTotalHeuresByFormateur(int idFormateur)
{
    QSqlQuery query;
    query.prepare("SELECT SUM(duree_heures) FROM COURS WHERE id_formateur = :id");
    query.bindValue(":id", idFormateur);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}