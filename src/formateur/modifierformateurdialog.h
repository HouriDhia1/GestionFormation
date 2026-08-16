#ifndef MODIFIERFORMATEURDIALOG_H
#define MODIFIERFORMATEURDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include "formateur.h"

class ModifierFormateurDialog : public QDialog
{
    Q_OBJECT

public:
    ModifierFormateurDialog(const Formateur& formateur, QWidget *parent = nullptr);
    Formateur getFormateur() const;

private slots:
    void valider();

private:
    QLineEdit *editNom;
    QLineEdit *editPrenom;
    QLineEdit *editEmail;
    QLineEdit *editSpecialite;
    QDateEdit *editDateEmbauche;
    QPushButton *btnOk;
    QPushButton *btnAnnuler;
    int m_id;

    bool champsValides();
};

#endif // MODIFIERFORMATEURDIALOG_H