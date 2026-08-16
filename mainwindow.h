#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QLabel>
#include <QFrame>
#include <QListWidget>
#include <QStackedWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void changerPage(int index);

    void ajouterFormateur();
    void modifierFormateur();
    void supprimerFormateur();
    void actualiserFormateurs();

    void ajouterCours();
    void modifierCours();
    void supprimerCours();
    void actualiserCours();

private:
    Ui::MainWindow *ui;

    // Sidebar
    QListWidget *sidebar;
    QStackedWidget *stackedWidget;

    // Onglet Formateurs
    QWidget *pageFormateurs;
    QTableView *tableFormateurs;
    QStandardItemModel *modelFormateurs;
    QPushButton *btnAjouterFormateur;
    QPushButton *btnModifierFormateur;
    QPushButton *btnSupprimerFormateur;
    QPushButton *btnActualiserFormateurs;
    QLabel *cardTotalFormateurs;

    // Onglet Cours
    QWidget *pageCours;
    QTableView *tableCours;
    QStandardItemModel *modelCours;
    QPushButton *btnAjouterCours;
    QPushButton *btnModifierCours;
    QPushButton *btnSupprimerCours;
    QPushButton *btnActualiserCours;
    QLabel *cardTotalCours;

    void setupUI();
    void chargerFormateurs();
    void chargerCours();
    void styliserTable(QTableView *table);
    QPushButton* creerBouton(const QString& texte, const QString& styleClass);
    QFrame* creerCard(const QString& icon, const QString& title, const QString& value, const QString& bgColor = "white");
};

#endif // MAINWINDOW_H