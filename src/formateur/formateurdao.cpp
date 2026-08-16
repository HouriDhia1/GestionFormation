#include "formateurdao.h"
#include "../database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
bool FormateurDAO::emailExiste(const QString& email)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM FORMATEUR WHERE email = :email");
    query.bindValue(":email", email);

    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}
bool FormateurDAO::create(const Formateur& formateur)
{
    QSqlQuery query;
    query.prepare("INSERT INTO FORMATEUR (nom, prenom, email, specialite, date_embauche) "
                  "VALUES (:nom, :prenom, :email, :specialite, :date_embauche)");
    query.bindValue(":nom", formateur.getNom());
    query.bindValue(":prenom", formateur.getPrenom());
    query.bindValue(":email", formateur.getEmail());
    query.bindValue(":specialite", formateur.getSpecialite());
    query.bindValue(":date_embauche", formateur.getDateEmbauche());

    if (!query.exec()) {
        qDebug() << "❌ Erreur insertion formateur :" << query.lastError().text();
        return false;
    }
    return true;
}

QList<Formateur> FormateurDAO::readAll()
{
    QList<Formateur> formateurs;
    QSqlQuery query("SELECT * FROM FORMATEUR ORDER BY id_formateur");

    while (query.next()) {
        Formateur f(
            query.value("id_formateur").toInt(),
            query.value("nom").toString(),
            query.value("prenom").toString(),
            query.value("email").toString(),
            query.value("specialite").toString(),
            query.value("date_embauche").toDate()
            );
        formateurs.append(f);
    }
    return formateurs;
}

bool FormateurDAO::update(const Formateur& formateur)
{
    QSqlQuery query;
    query.prepare("UPDATE FORMATEUR SET nom = :nom, prenom = :prenom, email = :email, "
                  "specialite = :specialite, date_embauche = :date_embauche "
                  "WHERE id_formateur = :id");
    query.bindValue(":nom", formateur.getNom());
    query.bindValue(":prenom", formateur.getPrenom());
    query.bindValue(":email", formateur.getEmail());
    query.bindValue(":specialite", formateur.getSpecialite());
    query.bindValue(":date_embauche", formateur.getDateEmbauche());
    query.bindValue(":id", formateur.getId());

    if (!query.exec()) {
        qDebug() << "❌ Erreur mise à jour formateur :" << query.lastError().text();
        return false;
    }
    return true;
}

bool FormateurDAO::remove(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM FORMATEUR WHERE id_formateur = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "❌ Erreur suppression formateur :" << query.lastError().text();
        return false;
    }
    return true;
}

Formateur FormateurDAO::readById(int id)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM FORMATEUR WHERE id_formateur = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        return Formateur(
            query.value("id_formateur").toInt(),
            query.value("nom").toString(),
            query.value("prenom").toString(),
            query.value("email").toString(),
            query.value("specialite").toString(),
            query.value("date_embauche").toDate()
            );
    }
    return Formateur();
}