#include <QApplication>
#include "database.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Connexion à la base de données
    if (!DatabaseManager::instance().connect()) {
        qDebug() << "❌ Impossible de se connecter à la base de données.";
        return -1;
    }

    MainWindow w;
    w.show();

    return a.exec();
}