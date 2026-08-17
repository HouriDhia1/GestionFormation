#include "statistiquesdialog.h"
#include "formateur/formateurdao.h"
#include "cours/coursdao.h"
#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QPushButton>
#include <QHBoxLayout>

StatistiquesDialog::StatistiquesDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("📊 Statistiques et Graphiques");
    setModal(false);
    resize(900, 650);

    setupUI();
    actualiser();
}

void StatistiquesDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    tabWidget = new QTabWidget();
    tabWidget->setStyleSheet(R"(
        QTabWidget::pane {
            border: 1px solid #E5E7EB;
            border-radius: 8px;
            background: white;
        }
        QTabBar::tab {
            padding: 10px 20px;
            font-weight: bold;
        }
        QTabBar::tab:selected {
            background: #315BCE;
            color: white;
        }
    )");

    // ============================================================
    // PAGE 1 : FORMATEURS
    // ============================================================

    pageFormateurs = new QWidget();
    QVBoxLayout *layoutFormateurs = new QVBoxLayout(pageFormateurs);
    layoutFormateurs->setContentsMargins(20, 20, 20, 20);
    layoutFormateurs->setSpacing(15);

    QLabel *titreFormateurs = new QLabel("👨‍🏫 Statistiques des Formateurs");
    titreFormateurs->setStyleSheet("font-size: 20px; font-weight: 700; color: #1A2332;");
    layoutFormateurs->addWidget(titreFormateurs);

    // Cartes résumé
    QHBoxLayout *cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(15);

    QFrame *cardTotal = new QFrame();
    cardTotal->setStyleSheet("background: white; border-radius: 10px; border: 1px solid #E5E7EB;");
    cardTotal->setFixedHeight(80);
    QVBoxLayout *cardTotalLayout = new QVBoxLayout(cardTotal);
    QLabel *cardTotalLabel = new QLabel("Total Formateurs");
    cardTotalLabel->setStyleSheet("color: #6B7280; font-size: 12px;");
    labelTotalFormateurs = new QLabel("0");
    labelTotalFormateurs->setStyleSheet("font-size: 24px; font-weight: 700; color: #1A2332;");
    cardTotalLayout->addWidget(cardTotalLabel);
    cardTotalLayout->addWidget(labelTotalFormateurs);
    cardsLayout->addWidget(cardTotal);

    QFrame *cardSpecialites = new QFrame();
    cardSpecialites->setStyleSheet("background: white; border-radius: 10px; border: 1px solid #E5E7EB;");
    cardSpecialites->setFixedHeight(80);
    QVBoxLayout *cardSpecialitesLayout = new QVBoxLayout(cardSpecialites);
    QLabel *cardSpecialitesLabel = new QLabel("Spécialités");
    cardSpecialitesLabel->setStyleSheet("color: #6B7280; font-size: 12px;");
    labelSpecialites = new QLabel("0");
    labelSpecialites->setStyleSheet("font-size: 24px; font-weight: 700; color: #1A2332;");
    cardSpecialitesLayout->addWidget(cardSpecialitesLabel);
    cardSpecialitesLayout->addWidget(labelSpecialites);
    cardsLayout->addWidget(cardSpecialites);

    cardsLayout->addStretch();
    layoutFormateurs->addLayout(cardsLayout);

    chartViewFormateurs = new QChartView();
    chartViewFormateurs->setRenderHint(QPainter::Antialiasing);
    chartViewFormateurs->setMinimumHeight(350);
    layoutFormateurs->addWidget(chartViewFormateurs);

    tabWidget->addTab(pageFormateurs, "👨‍🏫 Formateurs");

    // ============================================================
    // PAGE 2 : COURS
    // ============================================================

    pageCours = new QWidget();
    QVBoxLayout *layoutCours = new QVBoxLayout(pageCours);
    layoutCours->setContentsMargins(20, 20, 20, 20);
    layoutCours->setSpacing(15);

    QLabel *titreCours = new QLabel("📚 Statistiques des Cours");
    titreCours->setStyleSheet("font-size: 20px; font-weight: 700; color: #1A2332;");
    layoutCours->addWidget(titreCours);

    // Cartes résumé
    QHBoxLayout *cardsCoursLayout = new QHBoxLayout();
    cardsCoursLayout->setSpacing(15);

    QFrame *cardTotalCours = new QFrame();
    cardTotalCours->setStyleSheet("background: white; border-radius: 10px; border: 1px solid #E5E7EB;");
    cardTotalCours->setFixedHeight(80);
    QVBoxLayout *cardTotalCoursLayout = new QVBoxLayout(cardTotalCours);
    QLabel *cardTotalCoursLabel = new QLabel("Total Cours");
    cardTotalCoursLabel->setStyleSheet("color: #6B7280; font-size: 12px;");
    labelTotalCours = new QLabel("0");
    labelTotalCours->setStyleSheet("font-size: 24px; font-weight: 700; color: #1A2332;");
    cardTotalCoursLayout->addWidget(cardTotalCoursLabel);
    cardTotalCoursLayout->addWidget(labelTotalCours);
    cardsCoursLayout->addWidget(cardTotalCours);

    QFrame *cardCoursParFormateur = new QFrame();
    cardCoursParFormateur->setStyleSheet("background: white; border-radius: 10px; border: 1px solid #E5E7EB;");
    cardCoursParFormateur->setFixedHeight(80);
    QVBoxLayout *cardCoursParFormateurLayout = new QVBoxLayout(cardCoursParFormateur);
    QLabel *cardCoursParFormateurLabel = new QLabel("Cours/Formateur");
    cardCoursParFormateurLabel->setStyleSheet("color: #6B7280; font-size: 12px;");
    labelCoursParFormateur = new QLabel("0");
    labelCoursParFormateur->setStyleSheet("font-size: 24px; font-weight: 700; color: #1A2332;");
    cardCoursParFormateurLayout->addWidget(cardCoursParFormateurLabel);
    cardCoursParFormateurLayout->addWidget(labelCoursParFormateur);
    cardsCoursLayout->addWidget(cardCoursParFormateur);

    cardsCoursLayout->addStretch();
    layoutCours->addLayout(cardsCoursLayout);

    chartViewCours = new QChartView();
    chartViewCours->setRenderHint(QPainter::Antialiasing);
    chartViewCours->setMinimumHeight(350);
    layoutCours->addWidget(chartViewCours);

    tabWidget->addTab(pageCours, "📚 Cours");

    // ============================================================
    // BOUTON FERMER
    // ============================================================

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *btnFermer = new QPushButton("❌ Fermer");
    btnFermer->setStyleSheet("background:#EF4444; color:white; padding:10px 20px; border-radius:8px; font-weight:bold;");
    btnFermer->setFixedWidth(120);
    connect(btnFermer, &QPushButton::clicked, this, &QDialog::close);
    buttonLayout->addStretch();
    buttonLayout->addWidget(btnFermer);

    mainLayout->addWidget(tabWidget);
    mainLayout->addLayout(buttonLayout);
}

void StatistiquesDialog::actualiser()
{
    chargerStatsFormateurs();
    chargerStatsCours();
}

void StatistiquesDialog::chargerStatsFormateurs()
{
    QSqlQuery query;
    query.exec("SELECT specialite, COUNT(*) AS nb FROM FORMATEUR GROUP BY specialite ORDER BY nb DESC");

    QPieSeries *series = new QPieSeries();

    int total = 0;
    int nbSpecialites = 0;

    while (query.next()) {
        QString specialite = query.value("specialite").toString();
        int nb = query.value("nb").toInt();

        if (specialite.isEmpty()) {
            specialite = "Non spécifiée";
        }

        QPieSlice *slice = series->append(specialite + " (" + QString::number(nb) + ")", nb);
        slice->setLabelVisible(true);
        slice->setLabelColor(Qt::black);
        slice->setLabelPosition(QPieSlice::LabelInsideHorizontal);

        total += nb;
        nbSpecialites++;
    }

    labelTotalFormateurs->setText(QString::number(total));
    labelSpecialites->setText(QString::number(nbSpecialites));

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("👨‍🏫 Répartition des formateurs par spécialité");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->setTheme(QChart::ChartThemeLight);

    styliserChart(chart);
    chartViewFormateurs->setChart(chart);
}

void StatistiquesDialog::chargerStatsCours()
{
    QSqlQuery query;
    query.exec(R"(
        SELECT f.nom || ' ' || f.prenom AS formateur, COUNT(c.id_cours) AS nb
        FROM FORMATEUR f
        LEFT JOIN COURS c ON f.id_formateur = c.id_formateur
        GROUP BY f.id_formateur, f.nom, f.prenom
        ORDER BY nb DESC
    )");

    QPieSeries *series = new QPieSeries();

    int totalCours = 0;

    while (query.next()) {
        QString formateur = query.value("formateur").toString();
        int nb = query.value("nb").toInt();

        if (formateur.isEmpty()) {
            formateur = "Formateur inconnu";
        }

        QPieSlice *slice = series->append(formateur + " (" + QString::number(nb) + ")", nb);
        slice->setLabelVisible(true);
        slice->setLabelColor(Qt::black);
        slice->setLabelPosition(QPieSlice::LabelInsideHorizontal);

        totalCours += nb;
    }

    labelTotalCours->setText(QString::number(totalCours));

    QSqlQuery countQuery;
    countQuery.exec("SELECT COUNT(*) FROM FORMATEUR");
    if (countQuery.next()) {
        int totalFormateurs = countQuery.value(0).toInt();
        if (totalFormateurs > 0) {
            labelCoursParFormateur->setText(QString::number((double)totalCours / totalFormateurs, 'f', 1));
        }
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("📚 Nombre de cours par formateur");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->setTheme(QChart::ChartThemeLight);

    styliserChart(chart);
    chartViewCours->setChart(chart);
}

void StatistiquesDialog::styliserChart(QChart *chart)
{
    chart->setBackgroundBrush(QBrush(Qt::white));
    chart->setBackgroundRoundness(10);
    chart->setMargins(QMargins(10, 10, 10, 10));
}