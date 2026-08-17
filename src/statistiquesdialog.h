#ifndef STATISTIQUESDIALOG_H
#define STATISTIQUESDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QChartView>
#include <QPieSeries>
#include <QPieSlice>
#include <QChart>

class StatistiquesDialog : public QDialog
{
    Q_OBJECT

public:
    StatistiquesDialog(QWidget *parent = nullptr);

public slots:
    void actualiser();

private:
    QTabWidget *tabWidget;

    // Onglet Formateurs
    QWidget *pageFormateurs;
    QChartView *chartViewFormateurs;
    QLabel *labelTotalFormateurs;
    QLabel *labelSpecialites;

    // Onglet Cours
    QWidget *pageCours;
    QChartView *chartViewCours;
    QLabel *labelTotalCours;
    QLabel *labelCoursParFormateur;

    void setupUI();
    void chargerStatsFormateurs();
    void chargerStatsCours();
    void styliserChart(QChart *chart);
};

#endif // STATISTIQUESDIALOG_H