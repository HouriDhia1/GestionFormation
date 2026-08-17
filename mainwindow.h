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
#include <QLineEdit>
#include <QStackedWidget>
#include <QSpinBox>
#include <QComboBox>
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
    void trierCours();
    void trierFormateurs();
    // ========================================
    // NAVIGATION
    // ========================================
    void changerPage(int index);

    // ========================================
    // FORMATEURS
    // ========================================
    void ajouterFormateur();
    void modifierFormateur();
    void supprimerFormateur();
    void actualiserFormateurs();

    // ========================================
    // COURS
    // ========================================
    void ajouterCours();
    void modifierCours();
    void supprimerCours();
    void actualiserCours();

    // ========================================
    // RECHERCHE FORMATEURS
    // ========================================
    void rechercherFormateurs();
    void reinitialiserRecherche();

    // ========================================
    // RECHERCHE COURS
    // ========================================
    void rechercherCours();
    void reinitialiserRechercheCours();

private:
    // Dashboard
    QLabel *dashboardTotalFormateurs;
    QLabel *dashboardTotalCours;
    QLabel *dashboardStatus;
    QLabel *dashboardSpecialites;

    Ui::MainWindow *ui;

    // Sidebar
    QListWidget *sidebar;
    QStackedWidget *stackedWidget;
    // Tri Formateurs
    QComboBox *comboTriFormateurs;
    QPushButton *btnTrierFormateurs;
    // Tri Cours
    QComboBox *comboTriCours;
    QPushButton *btnTrierCours;
    // ========================================
    // ONGLET FORMATEURS
    // ========================================
    QWidget *pageFormateurs;
    QTableView *tableFormateurs;
    QStandardItemModel *modelFormateurs;
    QPushButton *btnAjouterFormateur;
    QPushButton *btnModifierFormateur;
    QPushButton *btnSupprimerFormateur;
    QPushButton *btnActualiserFormateurs;
    QLabel *cardTotalFormateurs;

    // Champs de recherche Formateurs
    QLineEdit *searchNom;
    QLineEdit *searchPrenom;
    QLineEdit *searchSpecialite;
    QPushButton *btnRechercher;
    QPushButton *btnReinitialiser;

    // ========================================
    // ONGLET COURS
    // ========================================
    QWidget *pageCours;
    QTableView *tableCours;
    QStandardItemModel *modelCours;
    QPushButton *btnAjouterCours;
    QPushButton *btnModifierCours;
    QPushButton *btnSupprimerCours;
    QPushButton *btnActualiserCours;
    QLabel *cardTotalCours;

    // Champs de recherche Cours
    QLineEdit *searchTitre;
    QLineEdit *searchDescription;
    QSpinBox *spinDuree;
    QPushButton *btnRechercherCours;
    QPushButton *btnReinitialiserCours;

    // ========================================
    // MÉTHODES
    // ========================================
    void setupUI();
    void chargerFormateurs();
    void chargerCours();
    void styliserTable(QTableView *table);
    QPushButton* creerBouton(const QString& texte, const QString& styleClass);
    QFrame* creerCard(const QString& icon, const QString& title, const QString& value, const QString& bgColor = "white");
};

#endif // MAINWINDOW_H