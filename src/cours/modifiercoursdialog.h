#ifndef MODIFIERCOURSDIALOG_H
#define MODIFIERCOURSDIALOG_H

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

class ModifierCoursDialog : public QDialog
{
    Q_OBJECT

public:
    ModifierCoursDialog(const Cours& cours, QWidget *parent = nullptr);
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
    int m_id;

    bool champsValides();
    void chargerFormateurs(int idFormateurActuel);
};

#endif // MODIFIERCOURSDIALOG_H