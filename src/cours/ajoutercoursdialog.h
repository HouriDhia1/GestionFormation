#ifndef AJOUTERCOURSDIALOG_H
#define AJOUTERCOURSDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include "cours.h"

class AjouterCoursDialog : public QDialog
{
    Q_OBJECT

public:
    AjouterCoursDialog(QWidget *parent = nullptr);
    Cours getCours() const;

private slots:
    void valider();

private:
    QLineEdit *editTitre;
    QLineEdit *editDescription;
    QSpinBox *spinDuree;
    QComboBox *comboFormateur;
    QPushButton *btnOk;
    QPushButton *btnAnnuler;

    bool champsValides();
    void chargerFormateurs();
};

#endif // AJOUTERCOURSDIALOG_H