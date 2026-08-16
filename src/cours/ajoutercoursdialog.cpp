#include "ajoutercoursdialog.h"
#include "../formateur/formateurdao.h"
#include <QDebug>

AjouterCoursDialog::AjouterCoursDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("➕ Ajouter un cours");
    setModal(true);
    resize(450, 350);

    // ========================================
    // CHAMPS
    // ========================================

    editTitre = new QLineEdit();
    editTitre->setPlaceholderText("Ex: Programmation C++");

    editDescription = new QLineEdit();
    editDescription->setPlaceholderText("Ex: Initiation à C++");

    spinDuree = new QSpinBox();
    spinDuree->setRange(1, 200);
    spinDuree->setSuffix(" h");
    spinDuree->setValue(20);

    comboFormateur = new QComboBox();
    chargerFormateurs();

    // ========================================
    // BOUTONS
    // ========================================

    btnOk = new QPushButton("✅ Ajouter");
    btnOk->setStyleSheet("background:#315BCE; color:white; padding:10px; border-radius:6px; font-weight:bold;");

    btnAnnuler = new QPushButton("❌ Annuler");
    btnAnnuler->setStyleSheet("background:#EF4444; color:white; padding:10px; border-radius:6px; font-weight:bold;");

    connect(btnOk, &QPushButton::clicked, this, &AjouterCoursDialog::valider);
    connect(btnAnnuler, &QPushButton::clicked, this, &QDialog::reject);

    // ========================================
    // LAYOUT
    // ========================================

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    formLayout->addRow("📘 Titre :", editTitre);
    formLayout->addRow("📝 Description :", editDescription);
    formLayout->addRow("⏱️ Durée (heures) :", spinDuree);
    formLayout->addRow("👨‍🏫 Formateur :", comboFormateur);

    QHBoxLayout *layoutBoutons = new QHBoxLayout();
    layoutBoutons->addStretch();
    layoutBoutons->addWidget(btnOk);
    layoutBoutons->addWidget(btnAnnuler);

    QVBoxLayout *layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->addLayout(formLayout);
    layoutPrincipal->addSpacing(20);
    layoutPrincipal->addLayout(layoutBoutons);
}

// ========================================
// CHARGER LA LISTE DES FORMATEURS
// ========================================

void AjouterCoursDialog::chargerFormateurs()
{
    QList<Formateur> formateurs = FormateurDAO::readAll();
    comboFormateur->clear();

    for (const Formateur& f : formateurs) {
        comboFormateur->addItem(f.getNom() + " " + f.getPrenom(), f.getId());
    }
}

// ========================================
// VALIDATION DES CHAMPS
// ========================================

bool AjouterCoursDialog::champsValides()
{
    QString titre = editTitre->text().trimmed();
    QString description = editDescription->text().trimmed();

    // 1. Titre non vide
    if (titre.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Le champ 'Titre' est obligatoire.");
        editTitre->setFocus();
        return false;
    }

    // 2. Description non vide
    if (description.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Le champ 'Description' est obligatoire.");
        editDescription->setFocus();
        return false;
    }

    // 3. Durée > 0
    if (spinDuree->value() <= 0) {
        QMessageBox::warning(this, "Erreur", "La durée doit être supérieure à 0.");
        spinDuree->setFocus();
        return false;
    }

    // 4. Formateur sélectionné
    if (comboFormateur->currentIndex() == -1) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un formateur.");
        comboFormateur->setFocus();
        return false;
    }

    return true;
}

// ========================================
// VALIDER ET FERMER
// ========================================

void AjouterCoursDialog::valider()
{
    if (champsValides()) {
        accept();
    }
}

// ========================================
// RÉCUPÉRER LE COURS
// ========================================

Cours AjouterCoursDialog::getCours() const
{
    return Cours(
        0,
        editTitre->text().trimmed(),
        editDescription->text().trimmed(),
        spinDuree->value(),
        comboFormateur->currentData().toInt()
        );
}