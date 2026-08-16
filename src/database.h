#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql>
#include <QDebug>

class DatabaseManager
{
public:
    static DatabaseManager& instance();
    bool connect();
    void disconnect();
    bool isOpen() const;

private:
    DatabaseManager();
    ~DatabaseManager();
    QSqlDatabase m_db;
};

#endif // DATABASE_H