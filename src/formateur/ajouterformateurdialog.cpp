#include "ajouterformateurdialog.h"
#include <QRegularExpression>

AjouterFormateurDialog::AjouterFormateurDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("➕ Ajouter un formateur");
    setModal(true);
    resize(450, 350);

    // ========================================
    // CHAMPS
    // ========================================

    editNom = new QLineEdit();
    editNom->setPlaceholderText("Ex: Dupont");

    editPrenom = new QLineEdit();
    editPrenom->setPlaceholderText("Ex: Jean");

    editEmail = new QLineEdit();
    editEmail->setPlaceholderText("Ex: jean.dupont@esprit.tn");

    editSpecialite = new QLineEdit();
    editSpecialite->setPlaceholderText("Ex: C++");

    editDateEmbauche = new QDateEdit(QDate::currentDate());
    editDateEmbauche->setCalendarPopup(true);
    editDateEmbauche->setDisplayFormat("dd/MM/yyyy");

    // ========================================
    // BOUTONS
    // ========================================

    btnOk = new QPushButton("✅ Ajouter");
    btnOk->setStyleSheet("background:#315BCE; color:white; padding:10px; border-radius:6px; font-weight:bold;");

    btnAnnuler = new QPushButton("❌ Annuler");
    btnAnnuler->setStyleSheet("background:#EF4444; color:white; padding:10px; border-radius:6px; font-weight:bold;");

    connect(btnOk, &QPushButton::clicked, this, &AjouterFormateurDialog::valider);
    connect(btnAnnuler, &QPushButton::clicked, this, &QDialog::reject);

    // ========================================
    // LAYOUT
    // ========================================

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    formLayout->addRow("👤 Nom :", editNom);
    formLayout->addRow("👤 Prénom :", editPrenom);
    formLayout->addRow("📧 Email :", editEmail);
    formLayout->addRow("🎯 Spécialité :", editSpecialite);
    formLayout->addRow("📅 Date d'embauche :", editDateEmbauche);

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
// VALIDATION DES CHAMPS
// ========================================

// ========================================
// VALIDATION DES CHAMPS
// ========================================

bool AjouterFormateurDialog::champsValides()
{
    QString nom = editNom->text().trimmed();
    QString prenom = editPrenom->text().trimmed();
    QString email = editEmail->text().trimmed();
    QString specialite = editSpecialite->text().trimmed();

    // 1. Nom non vide
    if (nom.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Le champ 'Nom' est obligatoire.");
        editNom->setFocus();
        return false;
    }

    // 2. Prénom non vide
    if (prenom.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Le champ 'Prénom' est obligatoire.");
        editPrenom->setFocus();
        return false;
    }

    // 3. Email non vide
    if (email.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Le champ 'Email' est obligatoire.");
        editEmail->setFocus();
        return false;
    }

    // 4. ✅ Email valide (quelquechose@quelquechose.extension)
    // ✅ Email : nom@domaine.extension (domaine sans point)
    QRegularExpression emailRegex(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9-]+\.[A-Za-z]{2,}$)");    if (!emailRegex.match(email).hasMatch()) {
        QMessageBox::warning(this, "Erreur",
                             "Format d'email invalide.\n"
                             "Exemple valide : jean.dupont@esprit.tn");
        editEmail->setFocus();
        return false;
    }

    // 5. Spécialité non vide
    if (specialite.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Le champ 'Spécialité' est obligatoire.");
        editSpecialite->setFocus();
        return false;
    }

    return true;
}
// ========================================
// VALIDER ET FERMER
// ========================================

void AjouterFormateurDialog::valider()
{
    if (champsValides()) {
        accept();
    }
}

// ========================================
// RÉCUPÉRER LE FORMATEUR
// ========================================

Formateur AjouterFormateurDialog::getFormateur() const
{
    return Formateur(
        0,
        editNom->text().trimmed(),
        editPrenom->text().trimmed(),
        editEmail->text().trimmed(),
        editSpecialite->text().trimmed(),
        editDateEmbauche->date()
        );
}