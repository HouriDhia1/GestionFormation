QT += core widgets sql charts printsupport
CONFIG += c++17

# Dossier des headers
INCLUDEPATH += $$PWD/src

# ========== SOURCES ==========
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    src/cours/ajoutercoursdialog.cpp \
    src/cours/modifiercoursdialog.cpp \
    src/database.cpp \
    src/formateur/ajouterformateurdialog.cpp \
    src/formateur/formateur.cpp \
    src/formateur/formateurdao.cpp \
    src/cours/cours.cpp \
    src/cours/coursdao.cpp \
    src/formateur/modifierformateurdialog.cpp \
    src/pdfgenerator.cpp \
    src/statistiquesdialog.cpp

# ========== HEADERS ==========
HEADERS += \
    mainwindow.h \
    src/cours/ajoutercoursdialog.h \
    src/cours/modifiercoursdialog.h \
    src/database.h \
    src/formateur/ajouterformateurdialog.h \
    src/formateur/formateur.h \
    src/formateur/formateurdao.h \
    src/cours/cours.h \
    src/cours/coursdao.h \
    src/formateur/modifierformateurdialog.h \
    src/pdfgenerator.h \
    src/statistiquesdialog.h

# ========== FORMULAIRES ==========
FORMS += \
    mainwindow.ui

# ========== TRADUCTIONS ==========
TRANSLATIONS += \
    GestionFormation_fr_FR.ts
CONFIG += lrelease
CONFIG += embed_translations

# ========== DÉPLOIEMENT ==========
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target