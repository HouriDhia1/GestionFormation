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
    void envoyerEmailFormateur();

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

    // ========================================
    // TRI
    // ========================================
    void trierFormateurs();
    void trierCours();

    // ========================================
    // MÉTIER 4 : ATTACHER UN FICHIER (COURS)
    // ========================================
    void attacherFichier();
    void ouvrirDocCours();  // ⬅️ RENOMMÉ

private:
    Ui::MainWindow *ui;

    // Sidebar
    QListWidget *sidebar;
    QStackedWidget *stackedWidget;

    // Dashboard
    QLabel *dashboardTotalFormateurs;
    QLabel *dashboardTotalCours;
    QLabel *dashboardStatus;
    QLabel *dashboardSpecialites;

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

    // Tri Formateurs
    QComboBox *comboTriFormateurs;
    QPushButton *btnTrierFormateurs;

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

    // Tri Cours
    QComboBox *comboTriCours;
    QPushButton *btnTrierCours;

    // ========================================
    // MÉTIER 4 : ATTACHER UN FICHIER (COURS)
    // ========================================
    QPushButton *btnAttacherFichier;
    QPushButton *btnOuvrirDocCours;  // ⬅️ RENOMMÉ

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