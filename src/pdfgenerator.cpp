#include "pdfgenerator.h"
#include <QPrinter>
#include <QPainter>
#include <QFile>
#include <QTextDocument>
#include <QDateTime>
#include <QDebug>
#include <QPageSize>

bool PDFGenerator::genererPDFFormateurs(const QList<Formateur>& formateurs, const QString& chemin)
{
    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(chemin);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageMargins(QMarginsF(20, 20, 20, 20));

    QPainter painter(&printer);
    if (!painter.isActive()) {
        qDebug() << "❌ Erreur : Impossible de créer le PDF";
        return false;
    }

    // ========================================================
    // EN-TÊTE
    // ========================================================

    QFont titleFont("Arial", 20, QFont::Bold);
    QFont subTitleFont("Arial", 12);
    QFont tableHeaderFont("Arial", 10, QFont::Bold);
    QFont tableFont("Arial", 9);

    int y = 20;
    int page = 1;

    // Titre
    painter.setFont(titleFont);
    painter.drawText(0, y, 300, 50, Qt::AlignLeft, "📋 Liste des Formateurs");

    // Date
    painter.setFont(subTitleFont);
    y += 30;
    painter.drawText(0, y, 200, 30, Qt::AlignLeft, "Généré le : " + getCurrentDate());

    // Séparateur
    y += 20;
    painter.setPen(QPen(Qt::black, 1));
    painter.drawLine(0, y, printer.width() - 20, y);

    // ========================================================
    // TABLEAU FORMATEURS - COLONNES ÉLARGIES
    // ========================================================

    y += 20;

    painter.setFont(tableHeaderFont);
    painter.setPen(QPen(Qt::black, 1));

    int x0 = 10;
    int col1 = 40;   // ID
    int col2 = 120;  // Nom
    int col3 = 120;  // Prénom
    int col4 = 180;  // Email
    int col5 = 120;  // Spécialité
    int col6 = 130;  // Date

    painter.drawText(x0, y, col1, 25, Qt::AlignLeft, "ID");
    painter.drawText(x0 + col1, y, col2, 25, Qt::AlignLeft, "Nom");
    painter.drawText(x0 + col1 + col2, y, col3, 25, Qt::AlignLeft, "Prénom");
    painter.drawText(x0 + col1 + col2 + col3, y, col4, 25, Qt::AlignLeft, "Email");
    painter.drawText(x0 + col1 + col2 + col3 + col4, y, col5, 25, Qt::AlignLeft, "Spécialité");
    painter.drawText(x0 + col1 + col2 + col3 + col4 + col5, y, col6, 25, Qt::AlignLeft, "Date");

    y += 25;
    painter.drawLine(x0, y, x0 + col1 + col2 + col3 + col4 + col5 + col6, y);

    painter.setFont(tableFont);
    int rowHeight = 22;

    for (const Formateur& f : formateurs) {
        y += rowHeight;

        QString email = f.getEmail();
        if (email.length() > 25) {
            email = email.left(22) + "...";
        }

        QString specialite = f.getSpecialite();
        if (specialite.length() > 15) {
            specialite = specialite.left(12) + "...";
        }

        painter.drawText(x0, y, col1, rowHeight, Qt::AlignLeft, QString::number(f.getId()));
        painter.drawText(x0 + col1, y, col2, rowHeight, Qt::AlignLeft, f.getNom());
        painter.drawText(x0 + col1 + col2, y, col3, rowHeight, Qt::AlignLeft, f.getPrenom());
        painter.drawText(x0 + col1 + col2 + col3, y, col4, rowHeight, Qt::AlignLeft, email);
        painter.drawText(x0 + col1 + col2 + col3 + col4, y, col5, rowHeight, Qt::AlignLeft, specialite);
        painter.drawText(x0 + col1 + col2 + col3 + col4 + col5, y, col6, rowHeight, Qt::AlignLeft,
                         f.getDateEmbauche().toString("dd/MM/yyyy"));

        if (y > printer.height() - 50) {
            painter.setFont(QFont("Arial", 8));
            painter.drawText(0, printer.height() - 20, printer.width(), 20, Qt::AlignCenter,
                             QString("Formation Center - Page %1").arg(page));

            printer.newPage();
            page++;
            y = 20;
            painter.setPen(QPen(Qt::black, 1));
            painter.drawLine(x0, y, x0 + col1 + col2 + col3 + col4 + col5 + col6, y);
        }
    }

    y = printer.height() - 20;
    painter.setFont(QFont("Arial", 8));
    painter.drawText(0, y, printer.width(), 20, Qt::AlignCenter,
                     QString("Formation Center - Page %1").arg(page));

    painter.end();
    return true;
}

// ============================================================
// GENERER PDF COURS (CORRIGÉ AVEC LES BONNES VARIABLES)
// ============================================================

bool PDFGenerator::genererPDFCours(const QList<Cours>& cours, const QString& chemin)
{
    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(chemin);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageMargins(QMarginsF(20, 20, 20, 20));

    QPainter painter(&printer);
    if (!painter.isActive()) {
        qDebug() << "❌ Erreur : Impossible de créer le PDF";
        return false;
    }

    // ========================================================
    // EN-TÊTE
    // ========================================================

    QFont titleFont("Arial", 20, QFont::Bold);
    QFont subTitleFont("Arial", 12);
    QFont tableHeaderFont("Arial", 10, QFont::Bold);
    QFont tableFont("Arial", 9);

    int y = 20;
    int page = 1;

    painter.setFont(titleFont);
    painter.drawText(0, y, 200, 50, Qt::AlignLeft, "📋 Liste des Cours");

    painter.setFont(subTitleFont);
    y += 30;
    painter.drawText(0, y, 200, 30, Qt::AlignLeft, "Généré le : " + getCurrentDate());

    y += 20;
    painter.setPen(QPen(Qt::black, 1));
    painter.drawLine(0, y, printer.width() - 20, y);

    // ========================================================
    // TABLEAU COURS - COLONNES ÉLARGIES
    // ========================================================

    y += 20;

    painter.setFont(tableHeaderFont);
    painter.setPen(QPen(Qt::black, 1));

    int x0 = 10;
    int col1 = 40;   // ID
    int col2 = 180;  // Titre
    int col3 = 130;  // Description (élargi)
    int col4 = 70;  // Durée

    painter.drawText(x0, y, col1, 25, Qt::AlignLeft, "ID");
    painter.drawText(x0 + col1, y, col2, 25, Qt::AlignLeft, "Titre");
    painter.drawText(x0 + col1 + col2, y, col3, 25, Qt::AlignLeft, "Description");
    painter.drawText(x0 + col1 + col2 + col3, y, col4, 25, Qt::AlignLeft, "Durée (h)");

    y += 25;
    painter.drawLine(x0, y, x0 + col1 + col2 + col3 + col4, y);

    painter.setFont(tableFont);
    int rowHeight = 22;

    for (const Cours& c : cours) {
        y += rowHeight;

        QString description = c.getDescription();
        if (description.length() > 35) {
            description = description.left(32) + "...";
        }

        painter.drawText(x0, y, col1, rowHeight, Qt::AlignLeft, QString::number(c.getId()));
        painter.drawText(x0 + col1, y, col2, rowHeight, Qt::AlignLeft, c.getTitre());
        painter.drawText(x0 + col1 + col2, y, col3, rowHeight, Qt::AlignLeft, description);
        painter.drawText(x0 + col1 + col2 + col3, y, col4, rowHeight, Qt::AlignLeft,
                         QString::number(c.getDureeHeures()));

        if (y > printer.height() - 50) {
            painter.setFont(QFont("Arial", 8));
            painter.drawText(0, printer.height() - 20, printer.width(), 20, Qt::AlignCenter,
                             QString("Formation Center - Page %1").arg(page));

            printer.newPage();
            page++;
            y = 20;
            painter.setPen(QPen(Qt::black, 1));
            painter.drawLine(x0, y, x0 + col1 + col2 + col3 + col4, y);
        }
    }

    y = printer.height() - 20;
    painter.setFont(QFont("Arial", 8));
    painter.drawText(0, y, printer.width(), 20, Qt::AlignCenter,
                     QString("Formation Center - Page %1").arg(page));

    painter.end();
    return true;
}

QString PDFGenerator::getCurrentDate()
{
    return QDateTime::currentDateTime().toString("dd/MM/yyyy à hh:mm");
}