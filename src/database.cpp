#include "database.h"

DatabaseManager::DatabaseManager() {}

DatabaseManager::~DatabaseManager()
{
    disconnect();
}

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::connect()
{
    m_db = QSqlDatabase::addDatabase("QODBC");
    m_db.setDatabaseName("OracleXE");
    m_db.setUserName("system");
    m_db.setPassword("0000");  // ⚠️ Remplace par ton mot de passe

    if (m_db.open()) {
        qDebug() << "✅ Connexion à Oracle réussie !";
        return true;
    } else {
        qDebug() << "❌ Erreur :" << m_db.lastError().text();
        return false;
    }
}

void DatabaseManager::disconnect()
{
    if (m_db.isOpen()) {
        m_db.close();
        qDebug() << "🔌 Déconnexion de la base de données.";
    }
}

bool DatabaseManager::isOpen() const
{
    return m_db.isOpen();
}