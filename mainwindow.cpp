#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "src/formateur/formateurdao.h"
#include "src/cours/coursdao.h"
#include "database.h"
#include "src/formateur/ajouterformateurdialog.h"

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

    // Logo / Titre
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

    // Menu sidebar
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
    // STACKED WIDGET (CONTENU PRINCIPAL)
    // ========================================================

    stackedWidget = new QStackedWidget();
    stackedWidget->setStyleSheet("background-color: #F0F2F5;");

    // ----- PAGE 1 : Tableau de bord -----
    QWidget *pageDashboard = new QWidget();
    QVBoxLayout *dashboardLayout = new QVBoxLayout(pageDashboard);
    dashboardLayout->setContentsMargins(30, 30, 30, 30);

    QLabel *dashboardTitle = new QLabel("📊 Tableau de bord");
    dashboardTitle->setStyleSheet("font-size: 28px; font-weight: 700; color: #1A2332;");
    dashboardLayout->addWidget(dashboardTitle);

    // Cartes
    QHBoxLayout *cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(20);

    cardsLayout->addWidget(creerCard("👨‍🏫", "Total Formateurs", "0", "white"));
    cardsLayout->addWidget(creerCard("📚", "Total Cours", "0", "white"));
    cardsLayout->addWidget(creerCard("🟢", "État du système", "Connecté", "#E8F8F0"));

    dashboardLayout->addLayout(cardsLayout);
    dashboardLayout->addStretch();

    stackedWidget->addWidget(pageDashboard);

    // ----- PAGE 2 : Formateurs -----
    pageFormateurs = new QWidget();
    QVBoxLayout *layoutFormateurs = new QVBoxLayout(pageFormateurs);
    layoutFormateurs->setContentsMargins(30, 30, 30, 30);
    layoutFormateurs->setSpacing(20);

    QLabel *titreFormateurs = new QLabel("👨‍🏫 Gestion des Formateurs");
    titreFormateurs->setStyleSheet("font-size: 24px; font-weight: 700; color: #1A2332;");
    layoutFormateurs->addWidget(titreFormateurs);

    // Cartes stats
    QHBoxLayout *statsFormateurs = new QHBoxLayout();
    statsFormateurs->setSpacing(15);
    statsFormateurs->addWidget(creerCard("👨‍🏫", "Total", "0", "white"));
    statsFormateurs->addWidget(creerCard("📌", "Module", "Formateurs", "white"));
    statsFormateurs->addWidget(creerCard("🟢", "État", "Connecté", "#E8F8F0"));
    statsFormateurs->addStretch();
    layoutFormateurs->addLayout(statsFormateurs);

    // Tableau
    modelFormateurs = new QStandardItemModel(this);
    modelFormateurs->setHorizontalHeaderLabels({"ID", "Nom", "Prénom", "Email", "Spécialité", "Date d'embauche"});

    tableFormateurs = new QTableView();
    tableFormateurs->setModel(modelFormateurs);
    styliserTable(tableFormateurs);
    layoutFormateurs->addWidget(tableFormateurs, 1);

    // Boutons
    QHBoxLayout *buttonsFormateurs = new QHBoxLayout();
    buttonsFormateurs->setSpacing(10);

    btnAjouterFormateur = creerBouton("＋  Ajouter", "primary");
    btnModifierFormateur = creerBouton("✎  Modifier", "warning");
    btnSupprimerFormateur = creerBouton("⌫  Supprimer", "danger");
    btnActualiserFormateurs = creerBouton("↻  Actualiser", "secondary");

    buttonsFormateurs->addWidget(btnAjouterFormateur);
    buttonsFormateurs->addWidget(btnModifierFormateur);
    buttonsFormateurs->addWidget(btnSupprimerFormateur);
    buttonsFormateurs->addStretch();
    buttonsFormateurs->addWidget(btnActualiserFormateurs);

    layoutFormateurs->addLayout(buttonsFormateurs);

    connect(btnAjouterFormateur, &QPushButton::clicked, this, &MainWindow::ajouterFormateur);
    connect(btnModifierFormateur, &QPushButton::clicked, this, &MainWindow::modifierFormateur);
    connect(btnSupprimerFormateur, &QPushButton::clicked, this, &MainWindow::supprimerFormateur);
    connect(btnActualiserFormateurs, &QPushButton::clicked, this, &MainWindow::actualiserFormateurs);

    stackedWidget->addWidget(pageFormateurs);

    // ----- PAGE 3 : Cours -----
    pageCours = new QWidget();
    QVBoxLayout *layoutCours = new QVBoxLayout(pageCours);
    layoutCours->setContentsMargins(30, 30, 30, 30);
    layoutCours->setSpacing(20);

    QLabel *titreCours = new QLabel("📚 Gestion des Cours");
    titreCours->setStyleSheet("font-size: 24px; font-weight: 700; color: #1A2332;");
    layoutCours->addWidget(titreCours);

    // Cartes stats
    QHBoxLayout *statsCours = new QHBoxLayout();
    statsCours->setSpacing(15);
    statsCours->addWidget(creerCard("📚", "Total", "0", "white"));
    statsCours->addWidget(creerCard("📌", "Module", "Cours", "white"));
    statsCours->addWidget(creerCard("🟢", "État", "Connecté", "#E8F8F0"));
    statsCours->addStretch();
    layoutCours->addLayout(statsCours);

    // Tableau
    modelCours = new QStandardItemModel(this);
    modelCours->setHorizontalHeaderLabels({"ID", "Titre", "Description", "Durée (h)", "Formateur"});

    tableCours = new QTableView();
    tableCours->setModel(modelCours);
    styliserTable(tableCours);
    layoutCours->addWidget(tableCours, 1);

    // Boutons
    QHBoxLayout *buttonsCours = new QHBoxLayout();
    buttonsCours->setSpacing(10);

    btnAjouterCours = creerBouton("＋  Ajouter", "primary");
    btnModifierCours = creerBouton("✎  Modifier", "warning");
    btnSupprimerCours = creerBouton("⌫  Supprimer", "danger");
    btnActualiserCours = creerBouton("↻  Actualiser", "secondary");

    buttonsCours->addWidget(btnAjouterCours);
    buttonsCours->addWidget(btnModifierCours);
    buttonsCours->addWidget(btnSupprimerCours);
    buttonsCours->addStretch();
    buttonsCours->addWidget(btnActualiserCours);

    layoutCours->addLayout(buttonsCours);

    connect(btnAjouterCours, &QPushButton::clicked, this, &MainWindow::ajouterCours);
    connect(btnModifierCours, &QPushButton::clicked, this, &MainWindow::modifierCours);
    connect(btnSupprimerCours, &QPushButton::clicked, this, &MainWindow::supprimerCours);
    connect(btnActualiserCours, &QPushButton::clicked, this, &MainWindow::actualiserCours);

    stackedWidget->addWidget(pageCours);

    // ----- PAGE 4 : Paramètres -----
    QWidget *pageSettings = new QWidget();
    QVBoxLayout *settingsLayout = new QVBoxLayout(pageSettings);
    settingsLayout->setContentsMargins(30, 30, 30, 30);
    QLabel *settingsTitle = new QLabel("⚙️ Paramètres");
    settingsTitle->setStyleSheet("font-size: 24px; font-weight: 700;");
    settingsLayout->addWidget(settingsTitle);
    settingsLayout->addStretch();
    stackedWidget->addWidget(pageSettings);

    // ----- PAGE 5 : Quitter -----
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
// CHANGER DE PAGE
// ============================================================

void MainWindow::changerPage(int index)
{
    stackedWidget->setCurrentIndex(index);
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
        emailItem->setToolTip(f.getEmail()); // Tooltip si trop long
        row.append(emailItem);

        row.append(new QStandardItem(f.getSpecialite()));

        // Date formatée
        QString dateStr = f.getDateEmbauche().toString("dd/MM/yyyy");
        QStandardItem *dateItem = new QStandardItem(dateStr);
        dateItem->setTextAlignment(Qt::AlignCenter); // Centrer la date
        row.append(dateItem);

        modelFormateurs->appendRow(row);
    }
}
// ============================================================
// CHARGEMENT COURS
// ============================================================

void MainWindow::chargerCours()
{
    if (!modelCours) return;

    modelCours->removeRows(0, modelCours->rowCount());

    QList<Cours> cours = CoursDAO::readAll();

    for (const Cours& c : cours) {
        QList<QStandardItem*> row;
        row.append(new QStandardItem(QString::number(c.getId())));
        row[0]->setTextAlignment(Qt::AlignCenter);

        row.append(new QStandardItem(c.getTitre()));
        row.append(new QStandardItem(c.getDescription()));

        QStandardItem *dureeItem = new QStandardItem(QString::number(c.getDureeHeures()));
        dureeItem->setTextAlignment(Qt::AlignCenter);
        row.append(dureeItem);

        QStandardItem *idFormateurItem = new QStandardItem(QString::number(c.getIdFormateur()));
        idFormateurItem->setTextAlignment(Qt::AlignCenter);
        row.append(idFormateurItem);

        modelCours->appendRow(row);
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
    QMessageBox::information(this, "Ajouter", "Fonctionnalité à implémenter");
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

    QMessageBox::information(this, "Modifier", "Fonctionnalité de modification à implémenter.");
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