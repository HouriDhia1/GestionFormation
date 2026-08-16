#ifndef AJOUTERFORMATEURDIALOG_H
#define AJOUTERFORMATEURDIALOG_H

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

class AjouterFormateurDialog : public QDialog
{
    Q_OBJECT

public:
    AjouterFormateurDialog(QWidget *parent = nullptr);
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

    bool champsValides();
};

#endif // AJOUTERFORMATEURDIALOG_H