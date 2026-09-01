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
#include <QComboBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QPrinter>
#include <QPageSize>
#include <QPainter>
#include <QFileDialog>
#include <QFileDialog>
#include <QDesktopServices>
#include <QDir>
#include <QStandardPaths>

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
    sidebar->addItem("📊  Dashboard Formateur");
    sidebar->addItem("📊  Dashboard Cours");
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
    // ========================================================
    // ========================================================
    // BOUTONS COURS
    // ========================================================

    QHBoxLayout *buttonsCours = new QHBoxLayout();
    buttonsCours->setSpacing(10);

    btnAjouterCours = creerBouton("＋  Ajouter", "primary");
    btnModifierCours = creerBouton("✎  Modifier", "warning");
    btnSupprimerCours = creerBouton("⌫  Supprimer", "danger");
    btnActualiserCours = creerBouton("↻  Actualiser", "secondary");

    // ✅ Bouton PDF (UNIQUEMENT UNE FOIS)
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
                QMessageBox::information(this, "Succès", "✅ PDF généré !");
            }
        }
    });

    // ✅ Bouton Attacher un fichier (Métier 4)
    btnAttacherFichier = creerBouton("📎 Attacher un fichier", "primary");
    connect(btnAttacherFichier, &QPushButton::clicked, this, &MainWindow::attacherFichier);

    // ✅ Bouton Ouvrir le document (Métier 4)
    btnOuvrirDocCours = creerBouton("📂 Ouvrir le document", "primary");
    connect(btnOuvrirDocCours, &QPushButton::clicked, this, &MainWindow::ouvrirDocCours);

    buttonsCours->addWidget(btnAjouterCours);
    buttonsCours->addWidget(btnModifierCours);
    buttonsCours->addWidget(btnSupprimerCours);
    buttonsCours->addStretch();
    buttonsCours->addWidget(btnPDFCours);
    buttonsCours->addWidget(btnAttacherFichier);
    buttonsCours->addWidget(btnOuvrirDocCours);
    buttonsCours->addWidget(btnActualiserCours);

    layoutCours->addLayout(buttonsCours);

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
    // PAGE : DASHBOARD FORMATEUR
    // ============================================================

    QWidget *pageDashboardFormateur = new QWidget();
    QVBoxLayout *layoutDashboardFormateur = new QVBoxLayout(pageDashboardFormateur);
    layoutDashboardFormateur->setContentsMargins(30, 30, 30, 30);
    layoutDashboardFormateur->setSpacing(20);

    // Titre
    QLabel *titreDashboardFormateur = new QLabel("📊 Tableau de bord du formateur");
    titreDashboardFormateur->setStyleSheet("font-size: 28px; font-weight: 700; color: #1A2332;");
    layoutDashboardFormateur->addWidget(titreDashboardFormateur);

    // ========================================================
    // SÉLECTION DU FORMATEUR
    // ========================================================

    QHBoxLayout *selectionLayout = new QHBoxLayout();
    selectionLayout->setSpacing(15);

    QLabel *labelSelectFormateur = new QLabel("👤 Sélectionner un formateur :");
    labelSelectFormateur->setStyleSheet("font-size: 14px; font-weight: 600; color: #374151;");

    QComboBox *comboFormateurDashboard = new QComboBox();
    comboFormateurDashboard->setMinimumWidth(300);
    comboFormateurDashboard->setStyleSheet(R"(
    QComboBox {
        padding: 10px 16px;
        border: 2px solid #E5E7EB;
        border-radius: 10px;
        background: white;
        font-size: 13px;
        color: #1A2332;
    }
    QComboBox:hover {
        border-color: #4F46E5;
    }
    QComboBox::drop-down {
        border: none;
    }
)");

    QPushButton *btnAfficherDashboard = creerBouton("📊 Afficher", "primary");
    btnAfficherDashboard->setMinimumHeight(42);

    QPushButton *btnPDFDashboard = creerBouton("📄 Exporter PDF", "primary");
    btnPDFDashboard->setMinimumHeight(42);
    btnPDFDashboard->setStyleSheet(btnPDFDashboard->styleSheet() +
                                   "QPushButton { background-color: #7C3AED; }"
                                   "QPushButton:hover { background-color: #6D28D9; }"
                                   );

    selectionLayout->addWidget(labelSelectFormateur);
    selectionLayout->addWidget(comboFormateurDashboard);
    selectionLayout->addWidget(btnAfficherDashboard);
    selectionLayout->addWidget(btnPDFDashboard);
    selectionLayout->addStretch();

    layoutDashboardFormateur->addLayout(selectionLayout);

    // ========================================================
    // ZONE DE CONTENU (cachée au départ)
    // ========================================================

    QWidget *contenuDashboard = new QWidget();
    QVBoxLayout *contenuLayout = new QVBoxLayout(contenuDashboard);
    contenuLayout->setSpacing(15);
    contenuDashboard->setVisible(false);  // Caché au départ

    // ========================================================
    // CARTES DE STATISTIQUES (4 cartes)
    // ========================================================

    QHBoxLayout *statsGrid = new QHBoxLayout();
    statsGrid->setSpacing(15);

    // Carte 1 : Total Cours
    QFrame *cardTotal = new QFrame();
    cardTotal->setStyleSheet(R"(
    QFrame {
        background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
            stop:0 #4F46E5, stop:1 #818CF8);
        border-radius: 14px;
        color: white;
    }
)");
    cardTotal->setFixedHeight(100);
    cardTotal->setMinimumWidth(180);

    QVBoxLayout *cardTotalLayout = new QVBoxLayout(cardTotal);
    cardTotalLayout->setContentsMargins(20, 12, 20, 12);
    QLabel *cardTotalIcon = new QLabel("📚");
    cardTotalIcon->setStyleSheet("font-size: 22px;");
    QLabel *cardTotalLabel = new QLabel("Total Cours");
    cardTotalLabel->setStyleSheet("font-size: 12px; font-weight: 600; color: rgba(255,255,255,0.8);");
    QLabel *cardTotalValue = new QLabel("0");
    cardTotalValue->setStyleSheet("font-size: 28px; font-weight: 700;");
    cardTotalValue->setObjectName("cardTotalValue");
    cardTotalLayout->addWidget(cardTotalIcon);
    cardTotalLayout->addWidget(cardTotalLabel);
    cardTotalLayout->addWidget(cardTotalValue);
    statsGrid->addWidget(cardTotal);

    // Carte 2 : Total Heures
    QFrame *cardHeures = new QFrame();
    cardHeures->setStyleSheet(R"(
    QFrame {
        background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
            stop:0 #0891B2, stop:1 #22D3EE);
        border-radius: 14px;
        color: white;
    }
)");
    cardHeures->setFixedHeight(100);
    cardHeures->setMinimumWidth(180);

    QVBoxLayout *cardHeuresLayout = new QVBoxLayout(cardHeures);
    cardHeuresLayout->setContentsMargins(20, 12, 20, 12);
    QLabel *cardHeuresIcon = new QLabel("⏱️");
    cardHeuresIcon->setStyleSheet("font-size: 22px;");
    QLabel *cardHeuresLabel = new QLabel("Total Heures");
    cardHeuresLabel->setStyleSheet("font-size: 12px; font-weight: 600; color: rgba(255,255,255,0.8);");
    QLabel *cardHeuresValue = new QLabel("0");
    cardHeuresValue->setStyleSheet("font-size: 28px; font-weight: 700;");
    cardHeuresValue->setObjectName("cardHeuresValue");
    cardHeuresLayout->addWidget(cardHeuresIcon);
    cardHeuresLayout->addWidget(cardHeuresLabel);
    cardHeuresLayout->addWidget(cardHeuresValue);
    statsGrid->addWidget(cardHeures);

    // Carte 3 : Cours Max Durée
    QFrame *cardMax = new QFrame();
    cardMax->setStyleSheet(R"(
    QFrame {
        background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
            stop:0 #059669, stop:1 #34D399);
        border-radius: 14px;
        color: white;
    }
)");
    cardMax->setFixedHeight(100);
    cardMax->setMinimumWidth(180);

    QVBoxLayout *cardMaxLayout = new QVBoxLayout(cardMax);
    cardMaxLayout->setContentsMargins(20, 12, 20, 12);
    QLabel *cardMaxIcon = new QLabel("🏆");
    cardMaxIcon->setStyleSheet("font-size: 22px;");
    QLabel *cardMaxLabel = new QLabel("Cours le plus long");
    cardMaxLabel->setStyleSheet("font-size: 12px; font-weight: 600; color: rgba(255,255,255,0.8);");
    QLabel *cardMaxValue = new QLabel("0h");
    cardMaxValue->setStyleSheet("font-size: 28px; font-weight: 700;");
    cardMaxValue->setObjectName("cardMaxValue");
    cardMaxLayout->addWidget(cardMaxIcon);
    cardMaxLayout->addWidget(cardMaxLabel);
    cardMaxLayout->addWidget(cardMaxValue);
    statsGrid->addWidget(cardMax);

    // Carte 4 : Moyenne
    QFrame *cardMoyenne = new QFrame();
    cardMoyenne->setStyleSheet(R"(
    QFrame {
        background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
            stop:0 #7C3AED, stop:1 #A78BFA);
        border-radius: 14px;
        color: white;
    }
)");
    cardMoyenne->setFixedHeight(100);
    cardMoyenne->setMinimumWidth(180);

    QVBoxLayout *cardMoyenneLayout = new QVBoxLayout(cardMoyenne);
    cardMoyenneLayout->setContentsMargins(20, 12, 20, 12);
    QLabel *cardMoyenneIcon = new QLabel("📊");
    cardMoyenneIcon->setStyleSheet("font-size: 22px;");
    QLabel *cardMoyenneLabel = new QLabel("Durée moyenne");
    cardMoyenneLabel->setStyleSheet("font-size: 12px; font-weight: 600; color: rgba(255,255,255,0.8);");
    QLabel *cardMoyenneValue = new QLabel("0h");
    cardMoyenneValue->setStyleSheet("font-size: 28px; font-weight: 700;");
    cardMoyenneValue->setObjectName("cardMoyenneValue");
    cardMoyenneLayout->addWidget(cardMoyenneIcon);
    cardMoyenneLayout->addWidget(cardMoyenneLabel);
    cardMoyenneLayout->addWidget(cardMoyenneValue);
    statsGrid->addWidget(cardMoyenne);

    statsGrid->addStretch();
    contenuLayout->addLayout(statsGrid);

    // ========================================================
    // INFOS FORMATEUR + GRAPHIQUE
    // ========================================================

    QHBoxLayout *infoGraphLayout = new QHBoxLayout();
    infoGraphLayout->setSpacing(20);

    // Carte Informations
    QFrame *cardInfo = new QFrame();
    cardInfo->setStyleSheet(R"(
    QFrame {
        background-color: white;
        border-radius: 14px;
        border: 1px solid #E5E7EB;
    }
)");
    cardInfo->setMinimumWidth(350);
    cardInfo->setMaximumWidth(400);
    cardInfo->setFixedHeight(200);

    QVBoxLayout *cardInfoLayout = new QVBoxLayout(cardInfo);
    cardInfoLayout->setContentsMargins(20, 18, 20, 18);
    cardInfoLayout->setSpacing(8);

    QLabel *infoNomPrenom = new QLabel("👤 Sélectionnez un formateur");
    infoNomPrenom->setStyleSheet("font-size: 20px; font-weight: 700; color: #1A2332;");

    QLabel *infoEmail = new QLabel("");
    infoEmail->setStyleSheet("font-size: 13px; color: #6B7280;");

    QLabel *infoSpecialite = new QLabel("");
    infoSpecialite->setStyleSheet("font-size: 13px; color: #6B7280;");

    QLabel *infoDateEmbauche = new QLabel("");
    infoDateEmbauche->setStyleSheet("font-size: 13px; color: #6B7280;");

    QLabel *infoTotalCours = new QLabel("");
    infoTotalCours->setStyleSheet("font-size: 13px; font-weight: 600; color: #4F46E5;");

    cardInfoLayout->addWidget(infoNomPrenom);
    cardInfoLayout->addSpacing(5);
    cardInfoLayout->addWidget(infoEmail);
    cardInfoLayout->addWidget(infoSpecialite);
    cardInfoLayout->addWidget(infoDateEmbauche);
    cardInfoLayout->addWidget(infoTotalCours);
    cardInfoLayout->addStretch();

    infoGraphLayout->addWidget(cardInfo);

    // Graphique
    QChartView *dashboardChartViewFormateur = new QChartView();
    dashboardChartViewFormateur->setRenderHint(QPainter::Antialiasing);
    dashboardChartViewFormateur->setMinimumHeight(200);
    dashboardChartViewFormateur->setStyleSheet("background-color: white; border-radius: 14px; border: 1px solid #E5E7EB;");
    infoGraphLayout->addWidget(dashboardChartViewFormateur);

    contenuLayout->addLayout(infoGraphLayout);

    // ========================================================
    // LISTE DES COURS
    // ========================================================

    QLabel *labelListeCours = new QLabel("📋 Liste des cours");
    labelListeCours->setStyleSheet("font-size: 16px; font-weight: 700; color: #1A2332; margin-top: 5px;");
    contenuLayout->addWidget(labelListeCours);

    QTableView *tableCoursFormateur = new QTableView();
    tableCoursFormateur->setStyleSheet(R"(
    QTableView {
        background-color: white;
        border: 1px solid #E5E7EB;
        border-radius: 10px;
        alternate-background-color: #F8FAFD;
    }
    QHeaderView::section {
        background-color: #F7F9FC;
        padding: 12px 10px;
        border: none;
        border-bottom: 1px solid #E5E7EB;
        font-weight: 700;
        color: #374151;
    }
    QTableView::item {
        padding: 8px;
    }
)");
    tableCoursFormateur->setAlternatingRowColors(true);
    tableCoursFormateur->verticalHeader()->setVisible(false);
    tableCoursFormateur->horizontalHeader()->setStretchLastSection(true);

    QStandardItemModel *modelCoursFormateur = new QStandardItemModel(this);
    modelCoursFormateur->setHorizontalHeaderLabels({"ID", "Titre", "Description", "Durée (h)"});
    tableCoursFormateur->setModel(modelCoursFormateur);
    tableCoursFormateur->setColumnWidth(0, 60);
    tableCoursFormateur->setColumnWidth(1, 180);
    tableCoursFormateur->setColumnWidth(2, 300);

    contenuLayout->addWidget(tableCoursFormateur, 1);

    layoutDashboardFormateur->addWidget(contenuDashboard);

    stackedWidget->addWidget(pageDashboardFormateur);

    // ============================================================
    // REMPLIR LA LISTE DES FORMATEURS
    // ============================================================

    QList<Formateur> formateurs = FormateurDAO::readAll();
    comboFormateurDashboard->addItem("-- Sélectionner un formateur --", 0);
    for (const Formateur& f : formateurs) {
        comboFormateurDashboard->addItem(f.getNom() + " " + f.getPrenom(), f.getId());
    }

    // ============================================================
    // FONCTION POUR CHARGER LE DASHBOARD
    // ============================================================

    auto chargerDashboardFormateur = [comboFormateurDashboard, infoNomPrenom, infoEmail, infoSpecialite, infoDateEmbauche, infoTotalCours, cardTotalValue, cardHeuresValue, cardMaxValue, cardMoyenneValue, modelCoursFormateur, dashboardChartViewFormateur, contenuDashboard]() {
        int idFormateur = comboFormateurDashboard->currentData().toInt();

        if (idFormateur == 0) {
            QMessageBox::warning(nullptr, "Erreur", "Veuillez sélectionner un formateur.");
            contenuDashboard->setVisible(false);
            return;
        }

        contenuDashboard->setVisible(true);

        // Récupérer les informations du formateur
        Formateur f = FormateurDAO::readById(idFormateur);

        // Afficher les informations
        infoNomPrenom->setText("👤 " + f.getPrenom() + " " + f.getNom());
        infoEmail->setText("📧 " + f.getEmail());
        infoSpecialite->setText("🎯 Spécialité : " + f.getSpecialite());
        infoDateEmbauche->setText("📅 Date d'embauche : " + f.getDateEmbauche().toString("dd/MM/yyyy"));

        // Récupérer les cours du formateur
        QSqlQuery query;
        query.prepare("SELECT id_cours, titre, description, duree_heures FROM COURS WHERE id_formateur = :id");
        query.bindValue(":id", idFormateur);

        modelCoursFormateur->removeRows(0, modelCoursFormateur->rowCount());

        int totalCours = 0;
        int totalHeures = 0;
        int maxDuree = 0;
        QList<int> durees;

        if (query.exec()) {
            while (query.next()) {
                QList<QStandardItem*> row;
                row.append(new QStandardItem(query.value("id_cours").toString()));
                row.append(new QStandardItem(query.value("titre").toString()));
                row.append(new QStandardItem(query.value("description").toString()));
                row.append(new QStandardItem(query.value("duree_heures").toString()));

                modelCoursFormateur->appendRow(row);
                totalCours++;
                int duree = query.value("duree_heures").toInt();
                totalHeures += duree;
                if (duree > maxDuree) maxDuree = duree;
                durees.append(duree);
            }
        }

        // Mettre à jour les cartes
        cardTotalValue->setText(QString::number(totalCours));
        cardHeuresValue->setText(QString::number(totalHeures) + "h");
        cardMaxValue->setText(QString::number(maxDuree) + "h");
        double moyenne = totalCours > 0 ? (double)totalHeures / totalCours : 0;
        cardMoyenneValue->setText(QString::number(moyenne, 'f', 1) + "h");

        infoTotalCours->setText("📚 " + QString::number(totalCours) + " cours | ⏱️ " + QString::number(totalHeures) + " heures totales");

        // ========================================================
        // GRAPHIQUE CAMEMBERT DES DURÉES
        // ========================================================

        QPieSeries *series = new QPieSeries();

        // Compter les durées
        QMap<int, int> dureeCount;
        for (int d : durees) {
            dureeCount[d]++;
        }

        for (auto it = dureeCount.begin(); it != dureeCount.end(); ++it) {
            QPieSlice *slice = series->append(QString::number(it.key()) + "h (" + QString::number(it.value()) + ")", it.value());
            slice->setLabelVisible(true);
            slice->setLabelColor(Qt::black);
            slice->setLabelPosition(QPieSlice::LabelInsideHorizontal);
        }

        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle("📊 Répartition des durées des cours");
        chart->setAnimationOptions(QChart::SeriesAnimations);
        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignRight);
        chart->setTheme(QChart::ChartThemeLight);
        chart->setBackgroundBrush(QBrush(Qt::white));
        chart->setBackgroundRoundness(10);
        dashboardChartViewFormateur->setChart(chart);
    };

    // ============================================================
    // CONNEXION DU BOUTON AFFICHER
    // ============================================================

    connect(btnAfficherDashboard, &QPushButton::clicked, chargerDashboardFormateur);

    // ============================================================
    // CONNEXION DU BOUTON PDF
    // ============================================================

    connect(btnPDFDashboard, &QPushButton::clicked, [comboFormateurDashboard]() {
        int idFormateur = comboFormateurDashboard->currentData().toInt();

        if (idFormateur == 0) {
            QMessageBox::warning(nullptr, "Erreur", "Veuillez sélectionner un formateur.");
            return;
        }

        Formateur f = FormateurDAO::readById(idFormateur);

        // Créer un PDF du dashboard
        QString chemin = QFileDialog::getSaveFileName(
            nullptr,
            "Enregistrer le PDF du dashboard",
            QDir::homePath() + "/dashboard_" + f.getNom() + "_" + f.getPrenom() + ".pdf",
            "PDF Files (*.pdf)"
            );

        if (!chemin.isEmpty()) {
            QPrinter printer(QPrinter::PrinterResolution);
            printer.setOutputFormat(QPrinter::PdfFormat);
            printer.setOutputFileName(chemin);
            printer.setPageSize(QPageSize(QPageSize::A4));
            printer.setPageMargins(QMarginsF(20, 20, 20, 20));

            QPainter painter(&printer);
            if (painter.isActive()) {
                // Récupérer les données
                QSqlQuery query;
                query.prepare("SELECT id_cours, titre, description, duree_heures FROM COURS WHERE id_formateur = :id");
                query.bindValue(":id", idFormateur);

                int totalCours = 0;
                int totalHeures = 0;
                QList<QStringList> rows;

                if (query.exec()) {
                    while (query.next()) {
                        QStringList row;
                        row << query.value("id_cours").toString()
                            << query.value("titre").toString()
                            << query.value("description").toString()
                            << query.value("duree_heures").toString() + "h";
                        rows.append(row);
                        totalCours++;
                        totalHeures += query.value("duree_heures").toInt();
                    }
                }

                // En-tête
                QFont titleFont("Arial", 18, QFont::Bold);
                QFont subTitleFont("Arial", 12);
                QFont tableHeaderFont("Arial", 10, QFont::Bold);
                QFont tableFont("Arial", 9);

                int y = 20;

                painter.setFont(titleFont);
                painter.drawText(0, y, 300, 40, Qt::AlignLeft, "📊 Dashboard Formateur");

                painter.setFont(subTitleFont);
                y += 30;
                painter.drawText(0, y, 200, 25, Qt::AlignLeft, "Formateur : " + f.getPrenom() + " " + f.getNom());
                y += 22;
                painter.drawText(0, y, 200, 25, Qt::AlignLeft, "Email : " + f.getEmail());
                y += 22;
                painter.drawText(0, y, 200, 25, Qt::AlignLeft, "Spécialité : " + f.getSpecialite());
                y += 22;
                painter.drawText(0, y, 200, 25, Qt::AlignLeft, "Date d'embauche : " + f.getDateEmbauche().toString("dd/MM/yyyy"));

                y += 30;
                painter.setFont(QFont("Arial", 12, QFont::Bold));
                painter.drawText(0, y, 200, 25, Qt::AlignLeft, "📊 Statistiques");
                y += 25;
                painter.setFont(QFont("Arial", 10));
                painter.drawText(0, y, 200, 25, Qt::AlignLeft, "Total cours : " + QString::number(totalCours));
                painter.drawText(200, y, 200, 25, Qt::AlignLeft, "Total heures : " + QString::number(totalHeures) + "h");

                y += 30;
                painter.setFont(tableHeaderFont);
                painter.setPen(QPen(Qt::black, 1));
                painter.drawLine(0, y, printer.width() - 20, y);

                int x0 = 10;
                int col1 = 30;
                int col2 = 120;
                int col3 = 180;
                int col4 = 50;

                y += 25;
                painter.drawText(x0, y, col1, 25, Qt::AlignLeft, "ID");
                painter.drawText(x0 + col1, y, col2, 25, Qt::AlignLeft, "Titre");
                painter.drawText(x0 + col1 + col2, y, col3, 25, Qt::AlignLeft, "Description");
                painter.drawText(x0 + col1 + col2 + col3, y, col4, 25, Qt::AlignLeft, "Durée");

                y += 25;
                painter.drawLine(0, y, printer.width() - 20, y);

                painter.setFont(tableFont);
                int rowHeight = 22;

                for (const QStringList& row : rows) {
                    y += rowHeight;
                    painter.drawText(x0, y, col1, rowHeight, Qt::AlignLeft, row[0]);
                    painter.drawText(x0 + col1, y, col2, rowHeight, Qt::AlignLeft, row[1]);
                    painter.drawText(x0 + col1 + col2, y, col3, rowHeight, Qt::AlignLeft, row[2]);
                    painter.drawText(x0 + col1 + col2 + col3, y, col4, rowHeight, Qt::AlignLeft, row[3]);

                    if (y > printer.height() - 50) {
                        printer.newPage();
                        y = 20;
                        painter.drawLine(0, y, printer.width() - 20, y);
                    }
                }

                painter.end();

                QMessageBox::information(nullptr, "Succès", "✅ PDF du dashboard généré avec succès !\n" + chemin);
            } else {
                QMessageBox::critical(nullptr, "Erreur", "❌ Erreur lors de la génération du PDF.");
            }
        }
    });

    // ============================================================
    // PAGE : DASHBOARD COURS
    // ============================================================

    QWidget *pageDashboardCours = new QWidget();
    QVBoxLayout *layoutDashboardCours = new QVBoxLayout(pageDashboardCours);
    layoutDashboardCours->setContentsMargins(30, 30, 30, 30);
    layoutDashboardCours->setSpacing(20);

    QLabel *titreDashboardCours = new QLabel("📊 Tableau de bord du cours");
    titreDashboardCours->setStyleSheet("font-size: 28px; font-weight: 700; color: #1A2332;");
    layoutDashboardCours->addWidget(titreDashboardCours);

    // ========================================================
    // SÉLECTION DU COURS
    // ========================================================

    QHBoxLayout *selectionCoursLayout = new QHBoxLayout();
    selectionCoursLayout->setSpacing(15);

    QLabel *labelSelectCours = new QLabel("📘 Sélectionner un cours :");
    labelSelectCours->setStyleSheet("font-size: 14px; font-weight: 600; color: #374151;");

    comboDashboardCours = new QComboBox();
    comboDashboardCours->setMinimumWidth(300);
    comboDashboardCours->setStyleSheet(R"(
    QComboBox {
        padding: 10px 16px;
        border: 2px solid #E5E7EB;
        border-radius: 10px;
        background: white;
        font-size: 13px;
        color: #1A2332;
    }
    QComboBox:hover {
        border-color: #4F46E5;
    }
    QComboBox::drop-down {
        border: none;
    }
)");

    btnAfficherDashboardCours = creerBouton("📊 Afficher", "primary");
    btnAfficherDashboardCours->setMinimumHeight(42);

    btnPDFDashboardCours = creerBouton("📄 Exporter PDF", "primary");
    btnPDFDashboardCours->setMinimumHeight(42);
    btnPDFDashboardCours->setStyleSheet(btnPDFDashboardCours->styleSheet() +
                                        "QPushButton { background-color: #7C3AED; }"
                                        "QPushButton:hover { background-color: #6D28D9; }"
                                        );

    selectionCoursLayout->addWidget(labelSelectCours);
    selectionCoursLayout->addWidget(comboDashboardCours);
    selectionCoursLayout->addWidget(btnAfficherDashboardCours);
    selectionCoursLayout->addWidget(btnPDFDashboardCours);
    selectionCoursLayout->addStretch();

    layoutDashboardCours->addLayout(selectionCoursLayout);

    // ========================================================
    // ZONE DE CONTENU (cachée au départ)
    // ========================================================

    contenuDashboardCours = new QWidget();
    QVBoxLayout *contenuCoursLayout = new QVBoxLayout(contenuDashboardCours);
    contenuCoursLayout->setSpacing(15);
    contenuDashboardCours->setVisible(false);

    // ========================================================
    // CARTES DE STATISTIQUES (4 cartes)
    // ========================================================

    QHBoxLayout *statsGridCours = new QHBoxLayout();
    statsGridCours->setSpacing(15);

    // Carte 1 : Total Cours du formateur
    QFrame *cardCoursTotal = new QFrame();
    cardCoursTotal->setStyleSheet(R"(
    QFrame {
        background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
            stop:0 #4F46E5, stop:1 #818CF8);
        border-radius: 14px;
        color: white;
    }
)");
    cardCoursTotal->setFixedHeight(100);
    cardCoursTotal->setMinimumWidth(180);

    QVBoxLayout *cardCoursTotalLayout = new QVBoxLayout(cardCoursTotal);
    cardCoursTotalLayout->setContentsMargins(20, 12, 20, 12);
    QLabel *cardCoursTotalIcon = new QLabel("📚");
    cardCoursTotalIcon->setStyleSheet("font-size: 22px;");
    QLabel *cardCoursTotalLabel = new QLabel("Total Cours");
    cardCoursTotalLabel->setStyleSheet("font-size: 12px; font-weight: 600; color: rgba(255,255,255,0.8);");
    cardCoursTotalValue = new QLabel("0");
    cardCoursTotalValue->setStyleSheet("font-size: 28px; font-weight: 700;");
    cardCoursTotalLayout->addWidget(cardCoursTotalIcon);
    cardCoursTotalLayout->addWidget(cardCoursTotalLabel);
    cardCoursTotalLayout->addWidget(cardCoursTotalValue);
    statsGridCours->addWidget(cardCoursTotal);

    // Carte 2 : Total Heures
    QFrame *cardCoursHeures = new QFrame();
    cardCoursHeures->setStyleSheet(R"(
    QFrame {
        background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
            stop:0 #0891B2, stop:1 #22D3EE);
        border-radius: 14px;
        color: white;
    }
)");
    cardCoursHeures->setFixedHeight(100);
    cardCoursHeures->setMinimumWidth(180);

    QVBoxLayout *cardCoursHeuresLayout = new QVBoxLayout(cardCoursHeures);
    cardCoursHeuresLayout->setContentsMargins(20, 12, 20, 12);
    QLabel *cardCoursHeuresIcon = new QLabel("⏱️");
    cardCoursHeuresIcon->setStyleSheet("font-size: 22px;");
    QLabel *cardCoursHeuresLabel = new QLabel("Total Heures");
    cardCoursHeuresLabel->setStyleSheet("font-size: 12px; font-weight: 600; color: rgba(255,255,255,0.8);");
    cardCoursDureeValue = new QLabel("0h");
    cardCoursDureeValue->setStyleSheet("font-size: 28px; font-weight: 700;");
    cardCoursHeuresLayout->addWidget(cardCoursHeuresIcon);
    cardCoursHeuresLayout->addWidget(cardCoursHeuresLabel);
    cardCoursHeuresLayout->addWidget(cardCoursDureeValue);
    statsGridCours->addWidget(cardCoursHeures);

    // Carte 3 : Cours le plus long
    QFrame *cardCoursMax = new QFrame();
    cardCoursMax->setStyleSheet(R"(
    QFrame {
        background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
            stop:0 #059669, stop:1 #34D399);
        border-radius: 14px;
        color: white;
    }
)");
    cardCoursMax->setFixedHeight(100);
    cardCoursMax->setMinimumWidth(180);

    QVBoxLayout *cardCoursMaxLayout = new QVBoxLayout(cardCoursMax);
    cardCoursMaxLayout->setContentsMargins(20, 12, 20, 12);
    QLabel *cardCoursMaxIcon = new QLabel("🏆");
    cardCoursMaxIcon->setStyleSheet("font-size: 22px;");
    QLabel *cardCoursMaxLabel = new QLabel("Cours le plus long");
    cardCoursMaxLabel->setStyleSheet("font-size: 12px; font-weight: 600; color: rgba(255,255,255,0.8);");
    cardCoursMaxValue = new QLabel("0h");
    cardCoursMaxValue->setStyleSheet("font-size: 28px; font-weight: 700;");
    cardCoursMaxLayout->addWidget(cardCoursMaxIcon);
    cardCoursMaxLayout->addWidget(cardCoursMaxLabel);
    cardCoursMaxLayout->addWidget(cardCoursMaxValue);
    statsGridCours->addWidget(cardCoursMax);

    // Carte 4 : Durée moyenne
    QFrame *cardCoursMoyenne = new QFrame();
    cardCoursMoyenne->setStyleSheet(R"(
    QFrame {
        background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
            stop:0 #7C3AED, stop:1 #A78BFA);
        border-radius: 14px;
        color: white;
    }
)");
    cardCoursMoyenne->setFixedHeight(100);
    cardCoursMoyenne->setMinimumWidth(180);

    QVBoxLayout *cardCoursMoyenneLayout = new QVBoxLayout(cardCoursMoyenne);
    cardCoursMoyenneLayout->setContentsMargins(20, 12, 20, 12);
    QLabel *cardCoursMoyenneIcon = new QLabel("📊");
    cardCoursMoyenneIcon->setStyleSheet("font-size: 22px;");
    QLabel *cardCoursMoyenneLabel = new QLabel("Durée moyenne");
    cardCoursMoyenneLabel->setStyleSheet("font-size: 12px; font-weight: 600; color: rgba(255,255,255,0.8);");
    cardCoursMoyenneValue = new QLabel("0h");
    cardCoursMoyenneValue->setStyleSheet("font-size: 28px; font-weight: 700;");
    cardCoursMoyenneLayout->addWidget(cardCoursMoyenneIcon);
    cardCoursMoyenneLayout->addWidget(cardCoursMoyenneLabel);
    cardCoursMoyenneLayout->addWidget(cardCoursMoyenneValue);
    statsGridCours->addWidget(cardCoursMoyenne);

    statsGridCours->addStretch();
    contenuCoursLayout->addLayout(statsGridCours);

    // ========================================================
    // INFOS COURS + GRAPHIQUE
    // ========================================================

    QHBoxLayout *infoGraphCoursLayout = new QHBoxLayout();
    infoGraphCoursLayout->setSpacing(20);

    // Carte Informations du cours
    QFrame *cardCoursInfo = new QFrame();
    cardCoursInfo->setStyleSheet(R"(
    QFrame {
        background-color: white;
        border-radius: 14px;
        border: 1px solid #E5E7EB;
    }
)");
    cardCoursInfo->setMinimumWidth(350);
    cardCoursInfo->setMaximumWidth(400);
    cardCoursInfo->setFixedHeight(200);

    QVBoxLayout *cardCoursInfoLayout = new QVBoxLayout(cardCoursInfo);
    cardCoursInfoLayout->setContentsMargins(20, 18, 20, 18);
    cardCoursInfoLayout->setSpacing(8);

    labelCoursTitre = new QLabel("📘 Sélectionnez un cours");
    labelCoursTitre->setStyleSheet("font-size: 20px; font-weight: 700; color: #1A2332;");

    labelCoursDescription = new QLabel("");
    labelCoursDescription->setStyleSheet("font-size: 13px; color: #6B7280;");

    labelCoursDuree = new QLabel("");
    labelCoursDuree->setStyleSheet("font-size: 13px; color: #6B7280;");

    labelCoursFormateur = new QLabel("");
    labelCoursFormateur->setStyleSheet("font-size: 13px; color: #6B7280;");

    labelCoursStatistiques = new QLabel("");
    labelCoursStatistiques->setStyleSheet("font-size: 13px; font-weight: 600; color: #4F46E5;");

    cardCoursInfoLayout->addWidget(labelCoursTitre);
    cardCoursInfoLayout->addSpacing(5);
    cardCoursInfoLayout->addWidget(labelCoursDescription);
    cardCoursInfoLayout->addWidget(labelCoursDuree);
    cardCoursInfoLayout->addWidget(labelCoursFormateur);
    cardCoursInfoLayout->addWidget(labelCoursStatistiques);
    cardCoursInfoLayout->addStretch();

    infoGraphCoursLayout->addWidget(cardCoursInfo);

    // Graphique
    chartViewDashboardCours = new QChartView();
    chartViewDashboardCours->setRenderHint(QPainter::Antialiasing);
    chartViewDashboardCours->setMinimumHeight(200);
    chartViewDashboardCours->setStyleSheet("background-color: white; border-radius: 14px; border: 1px solid #E5E7EB;");
    infoGraphCoursLayout->addWidget(chartViewDashboardCours);

    contenuCoursLayout->addLayout(infoGraphCoursLayout);

    // ========================================================
    // LISTE DES AUTRES COURS DU MÊME FORMATEUR
    // ========================================================

    QLabel *labelAutresCours = new QLabel("📋 Autres cours du même formateur");
    labelAutresCours->setStyleSheet("font-size: 16px; font-weight: 700; color: #1A2332; margin-top: 5px;");
    contenuCoursLayout->addWidget(labelAutresCours);

    tableAutresCours = new QTableView();
    tableAutresCours->setStyleSheet(R"(
    QTableView {
        background-color: white;
        border: 1px solid #E5E7EB;
        border-radius: 10px;
        alternate-background-color: #F8FAFD;
    }
    QHeaderView::section {
        background-color: #F7F9FC;
        padding: 12px 10px;
        border: none;
        border-bottom: 1px solid #E5E7EB;
        font-weight: 700;
        color: #374151;
    }
    QTableView::item {
        padding: 8px;
    }
)");
    tableAutresCours->setAlternatingRowColors(true);
    tableAutresCours->verticalHeader()->setVisible(false);
    tableAutresCours->horizontalHeader()->setStretchLastSection(true);

    modelAutresCours = new QStandardItemModel(this);
    modelAutresCours->setHorizontalHeaderLabels({"ID", "Titre", "Description", "Durée (h)"});
    tableAutresCours->setModel(modelAutresCours);
    tableAutresCours->setColumnWidth(0, 60);
    tableAutresCours->setColumnWidth(1, 180);
    tableAutresCours->setColumnWidth(2, 300);

    contenuCoursLayout->addWidget(tableAutresCours, 1);

    layoutDashboardCours->addWidget(contenuDashboardCours);
    stackedWidget->addWidget(pageDashboardCours);
    // ============================================================
    // REMPLIR LA LISTE DES COURS POUR LE DASHBOARD
    // ============================================================

    QList<Cours> allCours = CoursDAO::readAll();
    comboDashboardCours->addItem("-- Sélectionner un cours --", 0);
    for (const Cours& c : allCours) {
        comboDashboardCours->addItem(QString::number(c.getId()) + " - " + c.getTitre(), c.getId());
    }

    // ============================================================
    // CONNEXION DU BOUTON AFFICHER
    // ============================================================

    connect(btnAfficherDashboardCours, &QPushButton::clicked, this, &MainWindow::afficherDashboardCours);

    // ============================================================
    // CONNEXION DU BOUTON PDF
    // ============================================================

    connect(btnPDFDashboardCours, &QPushButton::clicked, this, &MainWindow::exporterPDFDashboardCours);
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

    // ✅ Requête avec JOIN pour récupérer le nom du formateur ET le fichier attaché
    QSqlQuery query;
    query.prepare(R"(
        SELECT c.id_cours, c.titre, c.description, c.duree_heures,
               f.nom || ' ' || f.prenom AS formateur_nom,
               c.fichier_attache
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

        // ✅ Nom + Prénom du formateur
        row.append(new QStandardItem(query.value("formateur_nom").toString()));

        // ✅ Afficher l'icône 📎 si un fichier est attaché
        QString fichier = query.value("fichier_attache").toString();
        QStandardItem *fichierItem = new QStandardItem(fichier.isEmpty() ? "" : "📎");
        fichierItem->setTextAlignment(Qt::AlignCenter);
        fichierItem->setToolTip(fichier.isEmpty() ? "Aucun fichier attaché" : fichier);
        row.append(fichierItem);

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
// ============================================================
// MÉTIER 4 : ATTACHER UN FICHIER À UN COURS
// ============================================================

void MainWindow::attacherFichier()
{
    // 1. Vérifier qu'un cours est sélectionné
    QModelIndexList selection = tableCours->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, "Sélection", "Veuillez sélectionner un cours.");
        return;
    }

    int row = selection.first().row();
    int id = modelCours->item(row, 0)->text().toInt();
    QString titre = modelCours->item(row, 1)->text();

    // 2. Ouvrir la boîte de dialogue pour choisir un fichier
    QString chemin = QFileDialog::getOpenFileName(
        this,
        "Attacher un fichier au cours : " + titre,
        QDir::homePath(),
        "Tous les fichiers (*.*)"
        );

    if (chemin.isEmpty()) {
        return;
    }

    // 3. Vérifier la taille du fichier (max 10 MB)
    QFileInfo fileInfo(chemin);
    if (fileInfo.size() > 10 * 1024 * 1024) {
        QMessageBox::warning(this, "Erreur", "Le fichier est trop volumineux (max 10 MB).");
        return;
    }

    // 4. Créer le dossier "documents" s'il n'existe pas
    QDir dir;
    if (!dir.exists("documents")) {
        dir.mkdir("documents");
    }

    // 5. Copier le fichier dans le dossier "documents"
    QString nouveauNom = QString("cours_%1_%2").arg(id).arg(fileInfo.fileName());
    QString nouveauChemin = QDir::currentPath() + "/documents/" + nouveauNom;

    if (QFile::exists(nouveauChemin)) {
        QFile::remove(nouveauChemin);
    }

    if (!QFile::copy(chemin, nouveauChemin)) {
        QMessageBox::critical(this, "Erreur", "Impossible de copier le fichier.");
        return;
    }

    // 6. Enregistrer le chemin dans la base de données
    if (CoursDAO::attacherFichier(id, nouveauChemin)) {
        QMessageBox::information(this, "Succès", "✅ Fichier attaché avec succès !");
        chargerCours();
    } else {
        QMessageBox::critical(this, "Erreur", "❌ Erreur lors de l'enregistrement en base.");
    }
}

// ============================================================
// MÉTIER 4 : OUVRIR LE FICHIER ATTACHÉ À UN COURS
// ============================================================

void MainWindow::ouvrirDocCours()
{
    // 1. Vérifier qu'un cours est sélectionné
    QModelIndexList selection = tableCours->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, "Sélection", "Veuillez sélectionner un cours.");
        return;
    }

    int row = selection.first().row();
    int id = modelCours->item(row, 0)->text().toInt();

    // 2. Récupérer le chemin du fichier attaché
    QString chemin = CoursDAO::getFichierAttache(id);

    if (chemin.isEmpty()) {
        QMessageBox::information(this, "Information", "Aucun fichier n'est attaché à ce cours.");
        return;
    }

    // 3. Vérifier que le fichier existe
    if (!QFile::exists(chemin)) {
        QMessageBox::warning(this, "Erreur", "Le fichier n'existe plus sur le disque.");
        return;
    }

    // 4. Ouvrir le fichier avec l'application par défaut
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(chemin))) {
        QMessageBox::critical(this, "Erreur", "Impossible d'ouvrir le fichier.");
    }
}
// ============================================================
// AFFICHER LE DASHBOARD COURS
// ============================================================

void MainWindow::afficherDashboardCours()
{
    int idCours = comboDashboardCours->currentData().toInt();

    if (idCours == 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un cours.");
        contenuDashboardCours->setVisible(false);
        return;
    }

    contenuDashboardCours->setVisible(true);

    // Récupérer les détails du cours
    QMap<QString, QVariant> details = CoursDAO::getCoursDetails(idCours);

    if (details.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Cours introuvable.");
        contenuDashboardCours->setVisible(false);
        return;
    }

    QString titre = details["titre"].toString();
    QString description = details["description"].toString();
    int duree = details["duree_heures"].toInt();
    QString nomFormateur = details["prenom_formateur"].toString() + " " + details["nom_formateur"].toString();
    int idFormateur = details["id_formateur"].toInt();

    // Afficher les informations
    labelCoursTitre->setText("📘 " + titre);
    labelCoursDescription->setText("📝 " + description);
    labelCoursDuree->setText("⏱️ Durée : " + QString::number(duree) + " heures");
    labelCoursFormateur->setText("👨‍🏫 Formateur : " + nomFormateur);

    // Récupérer les statistiques
    int totalCours = CoursDAO::getTotalCoursByFormateur(idFormateur);
    int totalHeures = CoursDAO::getTotalHeuresByFormateur(idFormateur);
    int maxDuree = 0;
    double moyenne = 0;

    QList<Cours> autresCours = CoursDAO::getAutresCoursByFormateur(idFormateur, idCours);
    QList<int> durees;
    durees.append(duree);
    for (const Cours& c : autresCours) {
        durees.append(c.getDureeHeures());
        if (c.getDureeHeures() > maxDuree) maxDuree = c.getDureeHeures();
    }
    if (duree > maxDuree) maxDuree = duree;

    if (totalCours > 0) {
        moyenne = (double)totalHeures / totalCours;
    }

    // Mettre à jour les cartes
    cardCoursTotalValue->setText(QString::number(totalCours));
    cardCoursDureeValue->setText(QString::number(totalHeures) + "h");
    cardCoursMaxValue->setText(QString::number(maxDuree) + "h");
    cardCoursMoyenneValue->setText(QString::number(moyenne, 'f', 1) + "h");

    labelCoursStatistiques->setText("📚 " + QString::number(totalCours) + " cours | ⏱️ " + QString::number(totalHeures) + " heures totales");

    // Afficher les autres cours
    modelAutresCours->removeRows(0, modelAutresCours->rowCount());
    for (const Cours& c : autresCours) {
        QList<QStandardItem*> row;
        row.append(new QStandardItem(QString::number(c.getId())));
        row.append(new QStandardItem(c.getTitre()));
        row.append(new QStandardItem(c.getDescription()));
        row.append(new QStandardItem(QString::number(c.getDureeHeures())));
        modelAutresCours->appendRow(row);
    }

    // Graphique
    QPieSeries *series = new QPieSeries();

    // Compter les durées
    QMap<int, int> dureeCount;
    for (int d : durees) {
        dureeCount[d]++;
    }

    for (auto it = dureeCount.begin(); it != dureeCount.end(); ++it) {
        QPieSlice *slice = series->append(QString::number(it.key()) + "h (" + QString::number(it.value()) + ")", it.value());
        slice->setLabelVisible(true);
        slice->setLabelColor(Qt::black);
        slice->setLabelPosition(QPieSlice::LabelInsideHorizontal);
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("📊 Répartition des durées des cours");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->setTheme(QChart::ChartThemeLight);
    chart->setBackgroundBrush(QBrush(Qt::white));
    chart->setBackgroundRoundness(10);
    chartViewDashboardCours->setChart(chart);
}

// ============================================================
// EXPORTER LE DASHBOARD COURS EN PDF
// ============================================================

void MainWindow::exporterPDFDashboardCours()
{
    int idCours = comboDashboardCours->currentData().toInt();

    if (idCours == 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un cours.");
        return;
    }

    QMap<QString, QVariant> details = CoursDAO::getCoursDetails(idCours);

    if (details.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Cours introuvable.");
        return;
    }

    QString titre = details["titre"].toString();
    QString chemin = QFileDialog::getSaveFileName(
        this,
        "Enregistrer le PDF du dashboard cours",
        QDir::homePath() + "/dashboard_cours_" + titre + ".pdf",
        "PDF Files (*.pdf)"
        );

    if (chemin.isEmpty()) return;

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(chemin);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageMargins(QMarginsF(20, 20, 20, 20));

    QPainter painter(&printer);
    if (!painter.isActive()) {
        QMessageBox::critical(this, "Erreur", "Erreur lors de la génération du PDF.");
        return;
    }

    QFont titleFont("Arial", 18, QFont::Bold);
    QFont subTitleFont("Arial", 12);
    QFont tableHeaderFont("Arial", 10, QFont::Bold);
    QFont tableFont("Arial", 9);

    int y = 20;

    painter.setFont(titleFont);
    painter.drawText(0, y, 300, 40, Qt::AlignLeft, "📊 Dashboard Cours");

    painter.setFont(subTitleFont);
    y += 30;
    painter.drawText(0, y, 200, 25, Qt::AlignLeft, "Cours : " + details["titre"].toString());
    y += 22;
    painter.drawText(0, y, 200, 25, Qt::AlignLeft, "Description : " + details["description"].toString());
    y += 22;
    painter.drawText(0, y, 200, 25, Qt::AlignLeft, "Durée : " + QString::number(details["duree_heures"].toInt()) + "h");
    y += 22;
    painter.drawText(0, y, 200, 25, Qt::AlignLeft, "Formateur : " + details["prenom_formateur"].toString() + " " + details["nom_formateur"].toString());

    int idFormateur = details["id_formateur"].toInt();
    int totalCours = CoursDAO::getTotalCoursByFormateur(idFormateur);
    int totalHeures = CoursDAO::getTotalHeuresByFormateur(idFormateur);

    y += 30;
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.drawText(0, y, 200, 25, Qt::AlignLeft, "📊 Statistiques");
    y += 25;
    painter.setFont(QFont("Arial", 10));
    painter.drawText(0, y, 200, 25, Qt::AlignLeft, "Total cours du formateur : " + QString::number(totalCours));
    painter.drawText(200, y, 200, 25, Qt::AlignLeft, "Total heures : " + QString::number(totalHeures) + "h");

    y += 30;
    painter.setFont(tableHeaderFont);
    painter.setPen(QPen(Qt::black, 1));
    painter.drawLine(0, y, printer.width() - 20, y);

    int x0 = 10;
    int col1 = 30;
    int col2 = 120;
    int col3 = 180;
    int col4 = 50;

    y += 25;
    painter.drawText(x0, y, col1, 25, Qt::AlignLeft, "ID");
    painter.drawText(x0 + col1, y, col2, 25, Qt::AlignLeft, "Titre");
    painter.drawText(x0 + col1 + col2, y, col3, 25, Qt::AlignLeft, "Description");
    painter.drawText(x0 + col1 + col2 + col3, y, col4, 25, Qt::AlignLeft, "Durée");

    y += 25;
    painter.drawLine(0, y, printer.width() - 20, y);

    painter.setFont(tableFont);
    int rowHeight = 22;

    QList<Cours> autresCours = CoursDAO::getAutresCoursByFormateur(idFormateur, idCours);

    for (const Cours& c : autresCours) {
        y += rowHeight;
        QString desc = c.getDescription();
        if (desc.length() > 25) desc = desc.left(22) + "...";
        painter.drawText(x0, y, col1, rowHeight, Qt::AlignLeft, QString::number(c.getId()));
        painter.drawText(x0 + col1, y, col2, rowHeight, Qt::AlignLeft, c.getTitre());
        painter.drawText(x0 + col1 + col2, y, col3, rowHeight, Qt::AlignLeft, desc);
        painter.drawText(x0 + col1 + col2 + col3, y, col4, rowHeight, Qt::AlignLeft, QString::number(c.getDureeHeures()) + "h");

        if (y > printer.height() - 50) {
            printer.newPage();
            y = 20;
            painter.drawLine(0, y, printer.width() - 20, y);
        }
    }

    painter.end();
    QMessageBox::information(this, "Succès", "✅ PDF du dashboard cours généré !\n" + chemin);
}