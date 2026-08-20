#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopServices>
#include <QUrl>
#include "src/formateur/formateurdao.h"
#include "src/cours/coursdao.h"
#include "database.h"
#include "src/formateur/ajouterformateurdialog.h"
#include "src/statistiquesdialog.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>
#include <QHeaderView>
#include <QPalette>
#include <QFont>
#include <QAbstractItemView>
#include <QGraphicsDropShadowEffect>
#include <QSpacerItem>
#include <QModelIndexList>
#include "src/formateur/modifierformateurdialog.h"
#include "src/cours/ajoutercoursdialog.h"
#include "src/cours/modifiercoursdialog.h"
#include <QSqlQuery>
#include <QSqlError>
#include "src/pdfgenerator.h"
#include <QFileDialog>
#include <QMessageBox>
// ============================================================
// CONSTRUCTEUR
// ============================================================

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if (!DatabaseManager::instance().isOpen()) {
        QMessageBox::critical(
            this,
            "Database Error",
            "La base de données n'est pas connectée !"
            );
        return;
    }

    setupUI();

    chargerFormateurs();
    chargerCours();
}

// ============================================================
// DESTRUCTEUR
// ============================================================

MainWindow::~MainWindow()
{
    delete ui;
}

// ============================================================
// SETUP UI
// ============================================================

void MainWindow::setupUI()
{
    // ========================================================
    // FENÊTRE PRINCIPALE
    // ========================================================

    setWindowTitle("🏢 Formation Center");
    resize(1400, 800);
    setMinimumSize(1200, 700);

    // ========================================================
    // STYLE GLOBAL
    // ========================================================

    QString styleGlobal = R"(
        QMainWindow {
            background-color: #F0F2F5;
        }
        QWidget {
            font-family: "Segoe UI";
            color: #1A2332;
        }
    )";
    setStyleSheet(styleGlobal);

    // ========================================================
    // WIDGET CENTRAL : LAYOUT HORIZONTAL
    // ========================================================

    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    setCentralWidget(centralWidget);

    // ========================================================
    // SIDEBAR (GAUCHE)
    // ========================================================

    QFrame *sidebarFrame = new QFrame();
    sidebarFrame->setFixedWidth(240);
    sidebarFrame->setStyleSheet(R"(
        QFrame {
            background-color: #1A2332;
            border: none;
        }
    )");

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebarFrame);
    sidebarLayout->setContentsMargins(20, 30, 20, 30);
    sidebarLayout->setSpacing(20);

    QLabel *logo = new QLabel("🏢 Formation Center");
    logo->setStyleSheet(R"(
        QLabel {
            color: white;
            font-size: 18px;
            font-weight: 700;
            padding-bottom: 20px;
        }
    )");
    sidebarLayout->addWidget(logo);

    sidebar = new QListWidget();
    sidebar->setStyleSheet(R"(
        QListWidget {
            background: transparent;
            border: none;
            outline: none;
            color: #9CA3AF;
            font-size: 14px;
        }
        QListWidget::item {
            padding: 12px 16px;
            border-radius: 8px;
        }
        QListWidget::item:hover {
            background: rgba(255,255,255,0.05);
            color: white;
        }
        QListWidget::item:selected {
            background: #315BCE;
            color: white;
        }
    )");
    sidebar->addItem("📊  Tableau de bord");
    sidebar->addItem("👨‍🏫  Formateurs");
    sidebar->addItem("📚  Cours");
    sidebar->addItem("⚙️  Paramètres");
    sidebar->addItem("🚪  Quitter");

    connect(sidebar, &QListWidget::currentRowChanged, this, &MainWindow::changerPage);

    sidebarLayout->addWidget(sidebar);
    sidebarLayout->addStretch();

    mainLayout->addWidget(sidebarFrame);

    // ========================================================
    // STACKED WIDGET
    // ========================================================

    stackedWidget = new QStackedWidget();
    stackedWidget->setStyleSheet("background-color: #F0F2F5;");

    // ============================================================
    // PAGE 1 : TABLEAU DE BORD
    // ============================================================

    QWidget *pageDashboard = new QWidget();
    QVBoxLayout *dashboardLayout = new QVBoxLayout(pageDashboard);
    dashboardLayout->setContentsMargins(30, 30, 30, 30);

    QLabel *dashboardTitle = new QLabel("📊 Tableau de bord");
    dashboardTitle->setStyleSheet("font-size: 28px; font-weight: 700; color: #1A2332;");
    dashboardLayout->addWidget(dashboardTitle);

    // ========================================================
    // CARTES STATISTIQUES
    // ========================================================

    QHBoxLayout *cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(20);

    // Carte 1 : Total Formateurs
    QFrame *cardTotalFormateurs = new QFrame();
    cardTotalFormateurs->setStyleSheet(R"(
        QFrame {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #315BCE, stop:1 #4B7BEA);
            border-radius: 16px;
            color: white;
        }
    )");
    cardTotalFormateurs->setFixedHeight(120);
    cardTotalFormateurs->setMinimumWidth(180);

    QVBoxLayout *cardTotalFormateursLayout = new QVBoxLayout(cardTotalFormateurs);
    cardTotalFormateursLayout->setContentsMargins(20, 15, 20, 15);

    QLabel *cardTotalFormateursIcon = new QLabel("👨‍🏫");
    cardTotalFormateursIcon->setStyleSheet("font-size: 28px;");

    QLabel *cardTotalFormateursTitle = new QLabel("Total Formateurs");
    cardTotalFormateursTitle->setStyleSheet("font-size: 13px; font-weight: 600; color: rgba(255,255,255,0.8);");

    dashboardTotalFormateurs = new QLabel("0");
    dashboardTotalFormateurs->setStyleSheet("font-size: 32px; font-weight: 700;");

    cardTotalFormateursLayout->addWidget(cardTotalFormateursIcon);
    cardTotalFormateursLayout->addWidget(cardTotalFormateursTitle);
    cardTotalFormateursLayout->addWidget(dashboardTotalFormateurs);

    cardsLayout->addWidget(cardTotalFormateurs);

    // Carte 2 : Total Cours
    QFrame *cardTotalCours = new QFrame();
    cardTotalCours->setStyleSheet(R"(
        QFrame {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #0EA5E9, stop:1 #38BDF8);
            border-radius: 16px;
            color: white;
        }
    )");
    cardTotalCours->setFixedHeight(120);
    cardTotalCours->setMinimumWidth(180);

    QVBoxLayout *cardTotalCoursLayout = new QVBoxLayout(cardTotalCours);
    cardTotalCoursLayout->setContentsMargins(20, 15, 20, 15);

    QLabel *cardTotalCoursIcon = new QLabel("📚");
    cardTotalCoursIcon->setStyleSheet("font-size: 28px;");

    QLabel *cardTotalCoursTitle = new QLabel("Total Cours");
    cardTotalCoursTitle->setStyleSheet("font-size: 13px; font-weight: 600; color: rgba(255,255,255,0.8);");

    dashboardTotalCours = new QLabel("0");
    dashboardTotalCours->setStyleSheet("font-size: 32px; font-weight: 700;");

    cardTotalCoursLayout->addWidget(cardTotalCoursIcon);
    cardTotalCoursLayout->addWidget(cardTotalCoursTitle);
    cardTotalCoursLayout->addWidget(dashboardTotalCours);

    cardsLayout->addWidget(cardTotalCours);

    // Carte 3 : État du système
    QFrame *cardStatus = new QFrame();
    cardStatus->setStyleSheet(R"(
        QFrame {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #10B981, stop:1 #34D399);
            border-radius: 16px;
            color: white;
        }
    )");
    cardStatus->setFixedHeight(120);
    cardStatus->setMinimumWidth(180);

    QVBoxLayout *cardStatusLayout = new QVBoxLayout(cardStatus);
    cardStatusLayout->setContentsMargins(20, 15, 20, 15);

    QLabel *cardStatusIcon = new QLabel("🟢");
    cardStatusIcon->setStyleSheet("font-size: 28px;");

    QLabel *cardStatusTitle = new QLabel("État du système");
    cardStatusTitle->setStyleSheet("font-size: 13px; font-weight: 600; color: rgba(255,255,255,0.8);");

    dashboardStatus = new QLabel("Connecté");
    dashboardStatus->setStyleSheet("font-size: 32px; font-weight: 700;");

    cardStatusLayout->addWidget(cardStatusIcon);
    cardStatusLayout->addWidget(cardStatusTitle);
    cardStatusLayout->addWidget(dashboardStatus);

    cardsLayout->addWidget(cardStatus);

    // Carte 4 : Spécialités
    QFrame *cardSpecialites = new QFrame();
    cardSpecialites->setStyleSheet(R"(
        QFrame {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #8B5CF6, stop:1 #A78BFA);
            border-radius: 16px;
            color: white;
        }
    )");
    cardSpecialites->setFixedHeight(120);
    cardSpecialites->setMinimumWidth(180);

    QVBoxLayout *cardSpecialitesLayout = new QVBoxLayout(cardSpecialites);
    cardSpecialitesLayout->setContentsMargins(20, 15, 20, 15);

    QLabel *cardSpecialitesIcon = new QLabel("🎯");
    cardSpecialitesIcon->setStyleSheet("font-size: 28px;");

    QLabel *cardSpecialitesTitle = new QLabel("Spécialités");
    cardSpecialitesTitle->setStyleSheet("font-size: 13px; font-weight: 600; color: rgba(255,255,255,0.8);");

    dashboardSpecialites = new QLabel("0");
    dashboardSpecialites->setStyleSheet("font-size: 32px; font-weight: 700;");

    cardSpecialitesLayout->addWidget(cardSpecialitesIcon);
    cardSpecialitesLayout->addWidget(cardSpecialitesTitle);
    cardSpecialitesLayout->addWidget(dashboardSpecialites);

    cardsLayout->addWidget(cardSpecialites);

    cardsLayout->addStretch();
    dashboardLayout->addLayout(cardsLayout);

    // ========================================================
    // BOUTONS DE NAVIGATION STATISTIQUES
    // ========================================================

    QHBoxLayout *toggleLayout = new QHBoxLayout();
    toggleLayout->setSpacing(15);

    QPushButton *btnStatsFormateurs = creerBouton("👨‍🏫 Statistiques Formateurs", "primary");
    QPushButton *btnStatsCours = creerBouton("📚 Statistiques Cours", "primary");

    btnStatsFormateurs->setCheckable(true);
    btnStatsCours->setCheckable(true);
    btnStatsFormateurs->setChecked(true);

    QWidget *chartContainer = new QWidget();
    QVBoxLayout *chartLayout = new QVBoxLayout(chartContainer);
    chartLayout->setContentsMargins(0, 0, 0, 0);

    QChartView *dashboardChartView = new QChartView();
    dashboardChartView->setRenderHint(QPainter::Antialiasing);
    dashboardChartView->setMinimumHeight(350);
    chartLayout->addWidget(dashboardChartView);

    auto chargerStatsDashboard = [this, dashboardChartView, btnStatsFormateurs, btnStatsCours]() {
        if (btnStatsFormateurs->isChecked()) {
            QSqlQuery query;
            query.exec("SELECT specialite, COUNT(*) AS nb FROM FORMATEUR GROUP BY specialite ORDER BY nb DESC");

            QPieSeries *series = new QPieSeries();
            while (query.next()) {
                QString specialite = query.value("specialite").toString();
                int nb = query.value("nb").toInt();
                if (specialite.isEmpty()) specialite = "Non spécifiée";
                QPieSlice *slice = series->append(specialite + " (" + QString::number(nb) + ")", nb);
                slice->setLabelVisible(true);
                slice->setLabelColor(Qt::black);
                slice->setLabelPosition(QPieSlice::LabelInsideHorizontal);
            }

            QChart *chart = new QChart();
            chart->addSeries(series);
            chart->setTitle("👨‍🏫 Répartition des formateurs par spécialité");
            chart->setAnimationOptions(QChart::SeriesAnimations);
            chart->legend()->setVisible(true);
            chart->legend()->setAlignment(Qt::AlignRight);
            chart->setTheme(QChart::ChartThemeLight);
            chart->setBackgroundBrush(QBrush(Qt::white));
            chart->setBackgroundRoundness(10);
            dashboardChartView->setChart(chart);
        } else {
            QSqlQuery query;
            query.exec(R"(
                SELECT f.nom || ' ' || f.prenom AS formateur, COUNT(c.id_cours) AS nb
                FROM FORMATEUR f
                LEFT JOIN COURS c ON f.id_formateur = c.id_formateur
                GROUP BY f.id_formateur, f.nom, f.prenom
                ORDER BY nb DESC
            )");

            QPieSeries *series = new QPieSeries();
            while (query.next()) {
                QString formateur = query.value("formateur").toString();
                int nb = query.value("nb").toInt();
                if (formateur.isEmpty()) formateur = "Formateur inconnu";
                QPieSlice *slice = series->append(formateur + " (" + QString::number(nb) + ")", nb);
                slice->setLabelVisible(true);
                slice->setLabelColor(Qt::black);
                slice->setLabelPosition(QPieSlice::LabelInsideHorizontal);
            }

            QChart *chart = new QChart();
            chart->addSeries(series);
            chart->setTitle("📚 Nombre de cours par formateur");
            chart->setAnimationOptions(QChart::SeriesAnimations);
            chart->legend()->setVisible(true);
            chart->legend()->setAlignment(Qt::AlignRight);
            chart->setTheme(QChart::ChartThemeLight);
            chart->setBackgroundBrush(QBrush(Qt::white));
            chart->setBackgroundRoundness(10);
            dashboardChartView->setChart(chart);
        }
    };

    connect(btnStatsFormateurs, &QPushButton::clicked, [btnStatsFormateurs, btnStatsCours, chargerStatsDashboard]() {
        btnStatsFormateurs->setChecked(true);
        btnStatsCours->setChecked(false);
        chargerStatsDashboard();
    });

    connect(btnStatsCours, &QPushButton::clicked, [btnStatsFormateurs, btnStatsCours, chargerStatsDashboard]() {
        btnStatsCours->setChecked(true);
        btnStatsFormateurs->setChecked(false);
        chargerStatsDashboard();
    });

    toggleLayout->addStretch();
    toggleLayout->addWidget(btnStatsFormateurs);
    toggleLayout->addWidget(btnStatsCours);
    toggleLayout->addStretch();

    dashboardLayout->addLayout(toggleLayout);
    dashboardLayout->addWidget(chartContainer, 1);

    chargerStatsDashboard();

    stackedWidget->addWidget(pageDashboard);

    // ============================================================
    // PAGE 2 : FORMATEURS
    // ============================================================

    pageFormateurs = new QWidget();
    QVBoxLayout *layoutFormateurs = new QVBoxLayout(pageFormateurs);
    layoutFormateurs->setContentsMargins(30, 30, 30, 30);
    layoutFormateurs->setSpacing(20);

    QLabel *titreFormateurs = new QLabel("👨‍🏫 Gestion des Formateurs");
    titreFormateurs->setStyleSheet("font-size: 24px; font-weight: 700; color: #1A2332;");
    layoutFormateurs->addWidget(titreFormateurs);

    QHBoxLayout *statsFormateurs = new QHBoxLayout();
    statsFormateurs->setSpacing(15);
    statsFormateurs->addWidget(creerCard("👨‍🏫", "Total", "0", "white"));
    statsFormateurs->addWidget(creerCard("📌", "Module", "Formateurs", "white"));
    statsFormateurs->addWidget(creerCard("🟢", "État", "Connecté", "#E8F8F0"));
    statsFormateurs->addStretch();
    layoutFormateurs->addLayout(statsFormateurs);

    // ========================================================
    // BARRE DE RECHERCHE FORMATEURS
    // ========================================================

    QHBoxLayout *rechercheLayout = new QHBoxLayout();
    rechercheLayout->setSpacing(10);

    QLabel *labelNom = new QLabel("Nom :");
    searchNom = new QLineEdit();
    searchNom->setPlaceholderText("Rechercher par nom...");
    searchNom->setMinimumWidth(150);

    QLabel *labelPrenom = new QLabel("Prénom :");
    searchPrenom = new QLineEdit();
    searchPrenom->setPlaceholderText("Rechercher par prénom...");
    searchPrenom->setMinimumWidth(150);

    QLabel *labelSpecialite = new QLabel("Spécialité :");
    searchSpecialite = new QLineEdit();
    searchSpecialite->setPlaceholderText("Rechercher par spécialité...");
    searchSpecialite->setMinimumWidth(150);

    btnRechercher = creerBouton("🔍 Rechercher", "primary");
    btnReinitialiser = creerBouton("🔄 Réinitialiser", "secondary");

    rechercheLayout->addWidget(labelNom);
    rechercheLayout->addWidget(searchNom);
    rechercheLayout->addWidget(labelPrenom);
    rechercheLayout->addWidget(searchPrenom);
    rechercheLayout->addWidget(labelSpecialite);
    rechercheLayout->addWidget(searchSpecialite);
    rechercheLayout->addWidget(btnRechercher);
    rechercheLayout->addWidget(btnReinitialiser);
    rechercheLayout->addStretch();

    layoutFormateurs->addLayout(rechercheLayout);

    connect(btnRechercher, &QPushButton::clicked, this, &MainWindow::rechercherFormateurs);
    connect(btnReinitialiser, &QPushButton::clicked, this, &MainWindow::reinitialiserRecherche);

    // ========================================================
    // TRI MULTICRITÈRES FORMATEURS
    // ========================================================

    QHBoxLayout *triFormateursLayout = new QHBoxLayout();
    triFormateursLayout->setSpacing(10);

    QLabel *labelTriFormateurs = new QLabel("Trier par :");
    comboTriFormateurs = new QComboBox();
    comboTriFormateurs->addItem("ID");
    comboTriFormateurs->addItem("Nom");
    comboTriFormateurs->addItem("Prénom");
    comboTriFormateurs->addItem("Spécialité");

    btnTrierFormateurs = creerBouton("🔽 Trier", "primary");

    triFormateursLayout->addWidget(labelTriFormateurs);
    triFormateursLayout->addWidget(comboTriFormateurs);
    triFormateursLayout->addWidget(btnTrierFormateurs);
    triFormateursLayout->addStretch();

    layoutFormateurs->addLayout(triFormateursLayout);

    connect(btnTrierFormateurs, &QPushButton::clicked, this, &MainWindow::trierFormateurs);

    // ========================================================
    // TABLEAU FORMATEURS
    // ========================================================

    modelFormateurs = new QStandardItemModel(this);
    modelFormateurs->setHorizontalHeaderLabels({"ID", "Nom", "Prénom", "Email", "Spécialité", "Date d'embauche"});

    tableFormateurs = new QTableView();
    tableFormateurs->setModel(modelFormateurs);
    styliserTable(tableFormateurs);
    layoutFormateurs->addWidget(tableFormateurs, 1);

    // ========================================================
    // BOUTONS FORMATEURS (Ajouter, Modifier, Supprimer, PDF, Email, Actualiser)
    // ========================================================

    QHBoxLayout *buttonsFormateurs = new QHBoxLayout();
    buttonsFormateurs->setSpacing(10);

    btnAjouterFormateur = creerBouton("＋  Ajouter", "primary");
    btnModifierFormateur = creerBouton("✎  Modifier", "warning");
    btnSupprimerFormateur = creerBouton("⌫  Supprimer", "danger");
    btnActualiserFormateurs = creerBouton("↻  Actualiser", "secondary");

    // ✅ Bouton PDF Formateurs
    QPushButton *btnPDFFormateurs = creerBouton("📄 Exporter PDF", "primary");
    connect(btnPDFFormateurs, &QPushButton::clicked, this, [this]() {
        QString chemin = QFileDialog::getSaveFileName(
            this,
            "Enregistrer le PDF",
            QDir::homePath() + "/formateurs.pdf",
            "PDF Files (*.pdf)"
            );

        if (!chemin.isEmpty()) {
            QList<Formateur> formateurs = FormateurDAO::readAll();
            if (PDFGenerator::genererPDFFormateurs(formateurs, chemin)) {
                QMessageBox::information(this, "Succès", "✅ PDF généré avec succès !\n" + chemin);
            } else {
                QMessageBox::critical(this, "Erreur", "❌ Erreur lors de la génération du PDF.");
            }
        }
    });

    // ✅ Bouton Email Formateurs
    QPushButton *btnEmailFormateur = creerBouton("📧 Envoyer email", "primary");
    connect(btnEmailFormateur, &QPushButton::clicked, this, &MainWindow::envoyerEmailFormateur);

    buttonsFormateurs->addWidget(btnAjouterFormateur);
    buttonsFormateurs->addWidget(btnModifierFormateur);
    buttonsFormateurs->addWidget(btnSupprimerFormateur);
    buttonsFormateurs->addStretch();
    buttonsFormateurs->addWidget(btnPDFFormateurs);
    buttonsFormateurs->addWidget(btnEmailFormateur);
    buttonsFormateurs->addWidget(btnActualiserFormateurs);

    layoutFormateurs->addLayout(buttonsFormateurs);

    connect(btnAjouterFormateur, &QPushButton::clicked, this, &MainWindow::ajouterFormateur);
    connect(btnModifierFormateur, &QPushButton::clicked, this, &MainWindow::modifierFormateur);
    connect(btnSupprimerFormateur, &QPushButton::clicked, this, &MainWindow::supprimerFormateur);
    connect(btnActualiserFormateurs, &QPushButton::clicked, this, &MainWindow::actualiserFormateurs);

    stackedWidget->addWidget(pageFormateurs);
    // ============================================================
    // PAGE 3 : COURS
    // ============================================================

    pageCours = new QWidget();
    QVBoxLayout *layoutCours = new QVBoxLayout(pageCours);
    layoutCours->setContentsMargins(30, 30, 30, 30);
    layoutCours->setSpacing(20);

    QLabel *titreCours = new QLabel("📚 Gestion des Cours");
    titreCours->setStyleSheet("font-size: 24px; font-weight: 700; color: #1A2332;");
    layoutCours->addWidget(titreCours);

    QHBoxLayout *statsCours = new QHBoxLayout();
    statsCours->setSpacing(15);
    statsCours->addWidget(creerCard("📚", "Total", "0", "white"));
    statsCours->addWidget(creerCard("📌", "Module", "Cours", "white"));
    statsCours->addWidget(creerCard("🟢", "État", "Connecté", "#E8F8F0"));
    statsCours->addStretch();
    layoutCours->addLayout(statsCours);

    // ========================================================
    // BARRE DE RECHERCHE COURS
    // ========================================================

    QHBoxLayout *rechercheCoursLayout = new QHBoxLayout();
    rechercheCoursLayout->setSpacing(10);

    QLabel *labelTitre = new QLabel("Titre :");
    searchTitre = new QLineEdit();
    searchTitre->setPlaceholderText("Rechercher par titre...");
    searchTitre->setMinimumWidth(150);

    QLabel *labelDescription = new QLabel("Description :");
    searchDescription = new QLineEdit();
    searchDescription->setPlaceholderText("Rechercher par description...");
    searchDescription->setMinimumWidth(150);

    QLabel *labelDuree = new QLabel("Durée (h) :");
    spinDuree = new QSpinBox();
    spinDuree->setRange(0, 200);
    spinDuree->setValue(0);
    spinDuree->setFixedWidth(70);
    spinDuree->setToolTip("0 = ignorer ce critère");

    btnRechercherCours = creerBouton("🔍 Rechercher", "primary");
    btnReinitialiserCours = creerBouton("🔄 Réinitialiser", "secondary");

    rechercheCoursLayout->addWidget(labelTitre);
    rechercheCoursLayout->addWidget(searchTitre);
    rechercheCoursLayout->addWidget(labelDescription);
    rechercheCoursLayout->addWidget(searchDescription);
    rechercheCoursLayout->addWidget(labelDuree);
    rechercheCoursLayout->addWidget(spinDuree);
    rechercheCoursLayout->addWidget(btnRechercherCours);
    rechercheCoursLayout->addWidget(btnReinitialiserCours);
    rechercheCoursLayout->addStretch();

    layoutCours->addLayout(rechercheCoursLayout);

    connect(btnRechercherCours, &QPushButton::clicked, this, &MainWindow::rechercherCours);
    connect(btnReinitialiserCours, &QPushButton::clicked, this, &MainWindow::reinitialiserRechercheCours);

    // ========================================================
    // TRI MULTICRITÈRES COURS
    // ========================================================

    QHBoxLayout *triCoursLayout = new QHBoxLayout();
    triCoursLayout->setSpacing(10);

    QLabel *labelTriCours = new QLabel("Trier par :");
    comboTriCours = new QComboBox();
    comboTriCours->addItem("ID");
    comboTriCours->addItem("Titre");
    comboTriCours->addItem("Description");
    comboTriCours->addItem("Durée (h)");
    comboTriCours->addItem("Formateur");

    btnTrierCours = creerBouton("🔽 Trier", "primary");

    triCoursLayout->addWidget(labelTriCours);
    triCoursLayout->addWidget(comboTriCours);
    triCoursLayout->addWidget(btnTrierCours);
    triCoursLayout->addStretch();

    layoutCours->addLayout(triCoursLayout);

    connect(btnTrierCours, &QPushButton::clicked, this, &MainWindow::trierCours);

    // ========================================================
    // TABLEAU COURS
    // ========================================================

    modelCours = new QStandardItemModel(this);
    modelCours->setHorizontalHeaderLabels({"ID", "Titre", "Description", "Durée (h)", "Formateur"});

    tableCours = new QTableView();
    tableCours->setModel(modelCours);
    styliserTable(tableCours);
    layoutCours->addWidget(tableCours, 1);

    // ========================================================
    // BOUTONS COURS (Ajouter, Modifier, Supprimer, PDF, Actualiser)
    // ========================================================

    QHBoxLayout *buttonsCours = new QHBoxLayout();
    buttonsCours->setSpacing(10);

    btnAjouterCours = creerBouton("＋  Ajouter", "primary");
    btnModifierCours = creerBouton("✎  Modifier", "warning");
    btnSupprimerCours = creerBouton("⌫  Supprimer", "danger");
    btnActualiserCours = creerBouton("↻  Actualiser", "secondary");

    // ✅ Bouton PDF Cours
    QPushButton *btnPDFCours = creerBouton("📄 Exporter PDF", "primary");
    connect(btnPDFCours, &QPushButton::clicked, this, [this]() {
        QString chemin = QFileDialog::getSaveFileName(
            this,
            "Enregistrer le PDF",
            QDir::homePath() + "/cours.pdf",
            "PDF Files (*.pdf)"
            );

        if (!chemin.isEmpty()) {
            QList<Cours> cours = CoursDAO::readAll();
            if (PDFGenerator::genererPDFCours(cours, chemin)) {
                QMessageBox::information(this, "Succès", "✅ PDF généré avec succès !\n" + chemin);
            } else {
                QMessageBox::critical(this, "Erreur", "❌ Erreur lors de la génération du PDF.");
            }
        }
    });

    buttonsCours->addWidget(btnAjouterCours);
    buttonsCours->addWidget(btnModifierCours);
    buttonsCours->addWidget(btnSupprimerCours);
    buttonsCours->addStretch();
    buttonsCours->addWidget(btnPDFCours);
    buttonsCours->addWidget(btnActualiserCours);

    layoutCours->addLayout(buttonsCours);

    connect(btnAjouterCours, &QPushButton::clicked, this, &MainWindow::ajouterCours);
    connect(btnModifierCours, &QPushButton::clicked, this, &MainWindow::modifierCours);
    connect(btnSupprimerCours, &QPushButton::clicked, this, &MainWindow::supprimerCours);
    connect(btnActualiserCours, &QPushButton::clicked, this, &MainWindow::actualiserCours);

    stackedWidget->addWidget(pageCours);
    // ============================================================
    // PAGE 4 : PARAMÈTRES
    // ============================================================

    QWidget *pageSettings = new QWidget();
    QVBoxLayout *settingsLayout = new QVBoxLayout(pageSettings);
    settingsLayout->setContentsMargins(30, 30, 30, 30);
    QLabel *settingsTitle = new QLabel("⚙️ Paramètres");
    settingsTitle->setStyleSheet("font-size: 24px; font-weight: 700;");
    settingsLayout->addWidget(settingsTitle);
    settingsLayout->addStretch();
    stackedWidget->addWidget(pageSettings);

    // ============================================================
    // PAGE 5 : QUITTER
    // ============================================================

    QWidget *pageQuit = new QWidget();
    QVBoxLayout *quitLayout = new QVBoxLayout(pageQuit);
    quitLayout->setContentsMargins(30, 30, 30, 30);
    QLabel *quitTitle = new QLabel("🚪 Quitter");
    quitTitle->setStyleSheet("font-size: 24px; font-weight: 700;");
    quitLayout->addWidget(quitTitle);
    QPushButton *btnQuit = new QPushButton("Confirmer la sortie");
    btnQuit->setStyleSheet("background:#EF4444; color:white; padding:12px; border-radius:8px; font-weight:700;");
    connect(btnQuit, &QPushButton::clicked, this, &QMainWindow::close);
    quitLayout->addWidget(btnQuit);
    quitLayout->addStretch();
    stackedWidget->addWidget(pageQuit);

    mainLayout->addWidget(stackedWidget, 1);
}

// ============================================================
// TABLE STYLE
// ============================================================

void MainWindow::styliserTable(QTableView *table)
{
    table->setAlternatingRowColors(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setShowGrid(false);
    table->setFocusPolicy(Qt::NoFocus);
    table->verticalHeader()->setVisible(false);
    table->verticalHeader()->setDefaultSectionSize(48);
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    table->horizontalHeader()->setDefaultSectionSize(130);
table->setSortingEnabled(true);
    table->setStyleSheet(R"(
        QTableView {
            background-color: white;
            border: none;
            border-radius: 12px;
            gridline-color: transparent;
            selection-background-color: #E8F0FF;
            selection-color: #1D4ED8;
            font-size: 13px;
            alternate-background-color: #F8FAFD;
        }
        QTableView::item {
            border: none;
            padding: 10px;
        }
        QTableView::item:hover {
            background-color: #F1F5FF;
        }
        QTableView::item:selected {
            background-color: #E7EEFF;
            color: #1D4ED8;
        }
        QHeaderView::section {
            background-color: #F7F9FC;
            color: #64748B;
            padding: 13px 10px;
            border: none;
            border-bottom: 1px solid #E7ECF3;
            font-size: 12px;
            font-weight: 700;
        }
        QTableCornerButton::section {
            background-color: #F7F9FC;
            border: none;
        }
    )");

    // ========================================
    // LARGEURS DES COLONNES (ajustées)
    // ========================================

    // ID
    table->setColumnWidth(0, 60);

    // Pour FORMATEURS
    if (table == tableFormateurs) {
        table->setColumnWidth(1, 150);  // Nom
        table->setColumnWidth(2, 150);  // Prénom
        table->setColumnWidth(3, 250);  // Email (plus large)
        table->setColumnWidth(4, 150);  // Spécialité
        table->setColumnWidth(5, 150);  // Date d'embauche
    }

    // Pour COURS
    if (table == tableCours) {
        table->setColumnWidth(1, 200);  // Titre
        table->setColumnWidth(2, 300);  // Description
        table->setColumnWidth(3, 100);  // Durée
        table->setColumnWidth(4, 130);  // Formateur
    }
}
// ============================================================
// BOUTON CREATOR
// ============================================================

QPushButton* MainWindow::creerBouton(const QString& texte, const QString& styleClass)
{
    QPushButton *btn = new QPushButton(texte);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setMinimumHeight(40);
    btn->setMinimumWidth(120);

    QString style;
    if (styleClass == "primary") {
        style = R"(
            QPushButton {
                background-color: #315BCE;
                color: white;
                border: none;
                border-radius: 8px;
                padding: 10px 20px;
                font-size: 13px;
                font-weight: 700;
            }
            QPushButton:hover { background-color: #2649AD; }
            QPushButton:pressed { background-color: #1E3A8A; }
        )";
    } else if (styleClass == "warning") {
        style = R"(
            QPushButton {
                background-color: #FFF4E5;
                color: #D97706;
                border: 1px solid #FDE3B3;
                border-radius: 8px;
                padding: 10px 20px;
                font-size: 13px;
                font-weight: 700;
            }
            QPushButton:hover { background-color: #FFE8C2; }
            QPushButton:pressed { background-color: #FED7AA; }
        )";
    } else if (styleClass == "danger") {
        style = R"(
            QPushButton {
                background-color: #FFF0F0;
                color: #DC2626;
                border: 1px solid #FFD4D4;
                border-radius: 8px;
                padding: 10px 20px;
                font-size: 13px;
                font-weight: 700;
            }
            QPushButton:hover { background-color: #FFE0E0; }
            QPushButton:pressed { background-color: #FECACA; }
        )";
    } else {
        style = R"(
            QPushButton {
                background-color: #EEF2F7;
                color: #475569;
                border: 1px solid #E2E8F0;
                border-radius: 8px;
                padding: 10px 20px;
                font-size: 13px;
                font-weight: 700;
            }
            QPushButton:hover { background-color: #E2E8F0; }
            QPushButton:pressed { background-color: #CBD5E1; }
        )";
    }
    btn->setStyleSheet(style);
    return btn;
}

// ============================================================
// CHARGEMENT FORMATEURS
// ============================================================

void MainWindow::chargerFormateurs()
{
    if (!modelFormateurs) return;

    modelFormateurs->removeRows(0, modelFormateurs->rowCount());

    QList<Formateur> formateurs = FormateurDAO::readAll();

    for (const Formateur& f : formateurs) {
        QList<QStandardItem*> row;
        row.append(new QStandardItem(QString::number(f.getId())));

        // Centrer l'ID
        row[0]->setTextAlignment(Qt::AlignCenter);

        row.append(new QStandardItem(f.getNom()));
        row.append(new QStandardItem(f.getPrenom()));

        // Email (affiché en entier)
        QStandardItem *emailItem = new QStandardItem(f.getEmail());
        emailItem->setToolTip(f.getEmail());
        row.append(emailItem);

        row.append(new QStandardItem(f.getSpecialite()));

        // ✅ Date avec tri correct
        QDate date = f.getDateEmbauche();
        QString dateStr = date.toString("dd/MM/yyyy");
        QStandardItem *dateItem = new QStandardItem(dateStr);
        // Stocker la date réelle pour le tri
        dateItem->setData(date, Qt::UserRole);
        dateItem->setTextAlignment(Qt::AlignCenter);
        row.append(dateItem);

        modelFormateurs->appendRow(row);
    }

    // ✅ NE PAS réinitialiser le tri ici ! On laisse le tri actif.
    // modelFormateurs->sort(-1); // ❌ SUPPRIME CETTE LIGNE

    // ✅ Mettre à jour le Dashboard
    if (dashboardTotalFormateurs) {
        dashboardTotalFormateurs->setText(QString::number(formateurs.size()));
    }

    // Nombre de spécialités
    QStringList specialites;
    for (const Formateur& f : formateurs) {
        if (!f.getSpecialite().isEmpty() && !specialites.contains(f.getSpecialite())) {
            specialites.append(f.getSpecialite());
        }
    }
    if (dashboardSpecialites) {
        dashboardSpecialites->setText(QString::number(specialites.size()));
    }
}
// ============================================================
// CHARGEMENT COURS
// ============================================================

// ============================================================
// CHARGEMENT COURS (avec nom du formateur)
// ============================================================

void MainWindow::chargerCours()
{
    if (!modelCours) return;

    modelCours->removeRows(0, modelCours->rowCount());

    // ✅ Requête avec JOIN pour récupérer le nom du formateur
    QSqlQuery query;
    query.prepare(R"(
        SELECT c.id_cours, c.titre, c.description, c.duree_heures,
               f.nom || ' ' || f.prenom AS formateur_nom
        FROM COURS c
        JOIN FORMATEUR f ON c.id_formateur = f.id_formateur
        ORDER BY c.id_cours
    )");

    if (!query.exec()) {
        qDebug() << "❌ Erreur chargement cours :" << query.lastError().text();
        return;
    }

    int totalCours = 0;

    while (query.next()) {
        QList<QStandardItem*> row;
        row.append(new QStandardItem(query.value("id_cours").toString()));
        row[0]->setTextAlignment(Qt::AlignCenter);

        row.append(new QStandardItem(query.value("titre").toString()));
        row.append(new QStandardItem(query.value("description").toString()));

        QStandardItem *dureeItem = new QStandardItem(query.value("duree_heures").toString());
        dureeItem->setTextAlignment(Qt::AlignCenter);
        row.append(dureeItem);

        // ✅ Nom + Prénom du formateur (au lieu de son ID)
        row.append(new QStandardItem(query.value("formateur_nom").toString()));

        modelCours->appendRow(row);
        totalCours++;
    }
    modelCours->sort(-1); // Réinitialiser le tri

    // ========================================================
    // ✅ METTRE À JOUR LE DASHBOARD
    // ========================================================

    if (dashboardTotalCours) {
        dashboardTotalCours->setText(QString::number(totalCours));
    }
}
// ============================================================
// SLOTS FORMATEURS (COMPLETS)
// ============================================================

void MainWindow::ajouterFormateur()
{
    AjouterFormateurDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        Formateur f = dialog.getFormateur();

        // ✅ Vérifier si l'email existe déjà
        if (FormateurDAO::emailExiste(f.getEmail())) {
            QMessageBox::warning(this, "Doublon",
                                 "❌ Un formateur avec cet email existe déjà !\n"
                                 "Veuillez utiliser un autre email.");
            return;
        }

        if (FormateurDAO::create(f)) {
            QMessageBox::information(this, "Succès", "✅ Formateur ajouté avec succès !");
            chargerFormateurs();
        } else {
            QMessageBox::critical(this, "Erreur", "❌ Impossible d'ajouter le formateur.");
        }
    }
}
void MainWindow::modifierFormateur()
{
    QModelIndexList selection = tableFormateurs->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, "Sélection", "Veuillez sélectionner un formateur à modifier.");
        return;
    }

    int row = selection.first().row();
    int id = modelFormateurs->item(row, 0)->text().toInt();
    Formateur f = FormateurDAO::readById(id);

    ModifierFormateurDialog dialog(f, this);

    if (dialog.exec() == QDialog::Accepted) {
        Formateur fModifie = dialog.getFormateur();

        // ✅ Vérifier si l'email existe déjà (et que ce n'est pas le même formateur)
        if (fModifie.getEmail() != f.getEmail() && FormateurDAO::emailExiste(fModifie.getEmail())) {
            QMessageBox::warning(this, "Doublon",
                                 "❌ Un autre formateur avec cet email existe déjà !\n"
                                 "Veuillez utiliser un autre email.");
            return;
        }

        if (FormateurDAO::update(fModifie)) {
            QMessageBox::information(this, "Succès", "✅ Formateur modifié avec succès !");
            chargerFormateurs();
        } else {
            QMessageBox::critical(this, "Erreur", "❌ Impossible de modifier le formateur.");
        }
    }
}
void MainWindow::supprimerFormateur()
{
    QModelIndexList selection = tableFormateurs->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, "Sélection", "Veuillez sélectionner un formateur à supprimer.");
        return;
    }

    int row = selection.first().row();
    int id = modelFormateurs->item(row, 0)->text().toInt();
    QString nom = modelFormateurs->item(row, 1)->text();
    QString prenom = modelFormateurs->item(row, 2)->text();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmation",
                                  QString("Voulez-vous vraiment supprimer le formateur %1 %2 ?")
                                      .arg(nom).arg(prenom),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (FormateurDAO::remove(id)) {
            QMessageBox::information(this, "Succès", "✅ Formateur supprimé avec succès !");
            chargerFormateurs();
        } else {
            QMessageBox::critical(this, "Erreur", "❌ Impossible de supprimer le formateur.");
        }
    }
}

void MainWindow::actualiserFormateurs()
{
    chargerFormateurs();
    QMessageBox::information(this, "Actualisation", "✅ Liste des formateurs actualisée.");
}

// ============================================================
// SLOTS COURS (COMPLETS)
// ============================================================

void MainWindow::ajouterCours()
{
    AjouterCoursDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        Cours c = dialog.getCours();

        // Vérifier doublon
        if (CoursDAO::titreExiste(c.getTitre())) {
            QMessageBox::warning(this, "Doublon",
                                 "❌ Un cours avec ce titre existe déjà !\n"
                                 "Veuillez utiliser un autre titre.");
            return;
        }

        if (CoursDAO::create(c)) {
            QMessageBox::information(this, "Succès", "✅ Cours ajouté avec succès !");
            chargerCours();
        } else {
            QMessageBox::critical(this, "Erreur", "❌ Impossible d'ajouter le cours.");
        }
    }
}

void MainWindow::modifierCours()
{
    QModelIndexList selection = tableCours->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, "Sélection", "Veuillez sélectionner un cours à modifier.");
        return;
    }

    int row = selection.first().row();
    int id = modelCours->item(row, 0)->text().toInt();
    Cours c = CoursDAO::readById(id);

    ModifierCoursDialog dialog(c, this);

    if (dialog.exec() == QDialog::Accepted) {
        Cours cModifie = dialog.getCours();

        // Vérifier doublon (sauf si c'est le même cours)
        if (cModifie.getTitre() != c.getTitre() &&
            CoursDAO::titreExiste(cModifie.getTitre(), cModifie.getId())) {
            QMessageBox::warning(this, "Doublon",
                                 "❌ Un autre cours avec ce titre existe déjà !\n"
                                 "Veuillez utiliser un autre titre.");
            return;
        }

        if (CoursDAO::update(cModifie)) {
            QMessageBox::information(this, "Succès", "✅ Cours modifié avec succès !");
            chargerCours();
        } else {
            QMessageBox::critical(this, "Erreur", "❌ Impossible de modifier le cours.");
        }
    }
}

void MainWindow::supprimerCours()
{
    QModelIndexList selection = tableCours->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, "Sélection", "Veuillez sélectionner un cours à supprimer.");
        return;
    }

    int row = selection.first().row();
    int id = modelCours->item(row, 0)->text().toInt();
    QString titre = modelCours->item(row, 1)->text();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmation",
                                  QString("Voulez-vous vraiment supprimer le cours '%1' ?")
                                      .arg(titre),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (CoursDAO::remove(id)) {
            QMessageBox::information(this, "Succès", "✅ Cours supprimé avec succès !");
            chargerCours();
        } else {
            QMessageBox::critical(this, "Erreur", "❌ Impossible de supprimer le cours.");
        }
    }
}


void MainWindow::actualiserCours()
{
    chargerCours();
    QMessageBox::information(this, "Actualisation", "✅ Liste des cours actualisée.");
}
// ============================================================
// RECHERCHE MULTICRITÈRES
// ============================================================

void MainWindow::rechercherFormateurs()
{
    QString nom = searchNom->text().trimmed();
    QString prenom = searchPrenom->text().trimmed();
    QString specialite = searchSpecialite->text().trimmed();

    // Vérifier qu'au moins un champ est rempli
    if (nom.isEmpty() && prenom.isEmpty() && specialite.isEmpty()) {
        QMessageBox::warning(this, "Recherche",
                             "Veuillez saisir au moins un critère de recherche.");
        return;
    }

    // Effectuer la recherche
    QList<Formateur> resultats = FormateurDAO::search(nom, prenom, specialite);

    // Vider le tableau
    modelFormateurs->removeRows(0, modelFormateurs->rowCount());

    // Afficher les résultats
    if (resultats.isEmpty()) {
        QMessageBox::information(this, "Recherche",
                                 "Aucun formateur ne correspond à vos critères.");
        return;
    }

    for (const Formateur& f : resultats) {
        QList<QStandardItem*> row;
        row.append(new QStandardItem(QString::number(f.getId())));
        row[0]->setTextAlignment(Qt::AlignCenter);
        row.append(new QStandardItem(f.getNom()));
        row.append(new QStandardItem(f.getPrenom()));

        QStandardItem *emailItem = new QStandardItem(f.getEmail());
        emailItem->setToolTip(f.getEmail());
        row.append(emailItem);

        row.append(new QStandardItem(f.getSpecialite()));

        QString dateStr = f.getDateEmbauche().toString("dd/MM/yyyy");
        QStandardItem *dateItem = new QStandardItem(dateStr);
        dateItem->setTextAlignment(Qt::AlignCenter);
        row.append(dateItem);

        modelFormateurs->appendRow(row);
    }

    QMessageBox::information(this, "Recherche",
                             QString("✅ %1 formateur(s) trouvé(s).").arg(resultats.size()));
}

void MainWindow::reinitialiserRecherche()
{
    // Vider les champs de recherche
    searchNom->clear();
    searchPrenom->clear();
    searchSpecialite->clear();

    // Recharger tous les formateurs
    chargerFormateurs();

    QMessageBox::information(this, "Réinitialisation",
                             "✅ Liste complète des formateurs affichée.");
}
// ============================================================
// RECHERCHE MULTICRITÈRES COURS
// ============================================================

void MainWindow::rechercherCours()
{
    QString titre = searchTitre->text().trimmed();
    QString description = searchDescription->text().trimmed();
    int duree = spinDuree->value();

    // Vérifier qu'au moins un critère est rempli
    if (titre.isEmpty() && description.isEmpty() && duree == 0) {
        QMessageBox::warning(this, "Recherche",
                             "Veuillez saisir au moins un critère de recherche.");
        return;
    }

    // Appel de la recherche (duree = 0 signifie "ignorer ce critère")
    QList<Cours> resultats = CoursDAO::search(titre, description, duree);

    // Vider le tableau
    modelCours->removeRows(0, modelCours->rowCount());

    // Afficher les résultats
    if (resultats.isEmpty()) {
        QMessageBox::information(this, "Recherche",
                                 "Aucun cours ne correspond à vos critères.");
        return;
    }

    for (const Cours& c : resultats) {
        QList<QStandardItem*> row;
        row.append(new QStandardItem(QString::number(c.getId())));
        row[0]->setTextAlignment(Qt::AlignCenter);

        row.append(new QStandardItem(c.getTitre()));
        row.append(new QStandardItem(c.getDescription()));

        QStandardItem *dureeItem = new QStandardItem(QString::number(c.getDureeHeures()));
        dureeItem->setTextAlignment(Qt::AlignCenter);
        row.append(dureeItem);

        // Récupérer le nom du formateur
        QSqlQuery query;
        query.prepare("SELECT nom || ' ' || prenom AS formateur_nom FROM FORMATEUR WHERE id_formateur = :id");
        query.bindValue(":id", c.getIdFormateur());
        QString formateurNom = "";
        if (query.exec() && query.next()) {
            formateurNom = query.value("formateur_nom").toString();
        }
        row.append(new QStandardItem(formateurNom));

        modelCours->appendRow(row);
    }

    QMessageBox::information(this, "Recherche",
                             QString("✅ %1 cours trouvé(s).").arg(resultats.size()));
}

void MainWindow::reinitialiserRechercheCours()
{
    searchTitre->clear();
    searchDescription->clear();
    spinDuree->setValue(0);

    chargerCours();

    QMessageBox::information(this, "Réinitialisation",
                             "✅ Liste complète des cours affichée.");
}
// ============================================================
// TRI FORMATEURS
// ============================================================
void MainWindow::trierFormateurs()
{
    int colonne = comboTriFormateurs->currentIndex();
    Qt::SortOrder ordre = Qt::AscendingOrder;

    modelFormateurs->sort(colonne, ordre);

    QMessageBox::information(this, "Tri",
                             QString("✅ Tri effectué par : %1")
                                 .arg(comboTriFormateurs->currentText()));
}
// ============================================================
// TRI COURS
// ============================================================

void MainWindow::trierCours()
{
    int colonne = comboTriCours->currentIndex();
    Qt::SortOrder ordre = Qt::AscendingOrder;

    // Appliquer le tri sur le modèle
    modelCours->sort(colonne, ordre);

    QMessageBox::information(this, "Tri",
                             QString("✅ Tri effectué par : %1")
                                 .arg(comboTriCours->currentText()));
}
// ============================================================
// CRÉER UNE CARTE
// ============================================================

QFrame* MainWindow::creerCard(const QString& icon, const QString& title, const QString& value, const QString& bgColor)
{
    QFrame *card = new QFrame();
    card->setFixedHeight(100);
    card->setMinimumWidth(180);
    card->setStyleSheet(QString(R"(
        QFrame {
            background-color: %1;
            border-radius: 12px;
            border: 1px solid #E5E7EB;
        }
    )").arg(bgColor == "white" ? "white" : bgColor));

    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(16, 14, 16, 14);
    layout->setSpacing(4);

    QLabel *iconLabel = new QLabel(icon);
    iconLabel->setStyleSheet("font-size: 22px;");

    QLabel *titleLabel = new QLabel(title);
    titleLabel->setStyleSheet("color: #6B7280; font-size: 12px; font-weight: 600;");

    QLabel *valueLabel = new QLabel(value);
    valueLabel->setStyleSheet("color: #1A2332; font-size: 20px; font-weight: 700;");

    layout->addWidget(iconLabel);
    layout->addWidget(titleLabel);
    layout->addWidget(valueLabel);

    return card;
}
// ============================================================
// CHANGER DE PAGE
// ============================================================

void MainWindow::changerPage(int index)
{
    stackedWidget->setCurrentIndex(index);
}
// ============================================================
// ENVOYER UN EMAIL À UN FORMATEUR
// ============================================================

void MainWindow::envoyerEmailFormateur()
{
    // 1. Vérifier qu'un formateur est sélectionné
    QModelIndexList selection = tableFormateurs->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, "Sélection",
                             "Veuillez sélectionner un formateur dans la liste.");
        return;
    }

    // 2. Récupérer les informations du formateur sélectionné
    int row = selection.first().row();
    QString email = modelFormateurs->item(row, 3)->text();
    QString nom = modelFormateurs->item(row, 1)->text();
    QString prenom = modelFormateurs->item(row, 2)->text();

    // 3. Vérifier que l'email n'est pas vide
    if (email.isEmpty()) {
        QMessageBox::warning(this, "Erreur",
                             "Ce formateur n'a pas d'adresse email enregistrée.");
        return;
    }

    // 4. Construire l'URL mailto
    QString sujet = "Communication Formation Center";
    QString corps = "Bonjour " + prenom + " " + nom + ",\n\n";
    corps += "Nous vous contactons dans le cadre de vos activités au centre de formation.\n\n";
    corps += "Cordialement,\n";
    corps += "L'équipe Formation Center";

    QString mailto = "mailto:" + email +
                     "?subject=" + QUrl::toPercentEncoding(sujet) +
                     "&body=" + QUrl::toPercentEncoding(corps);

    // 5. Ouvrir le client mail par défaut
    QDesktopServices::openUrl(QUrl(mailto));

    QMessageBox::information(this, "Email",
                             "✅ Ouverture du client mail pour :\n" + email);
}