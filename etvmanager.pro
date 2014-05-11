TEMPLATE        = app
TARGET          = etvmanager

QT              += sql widgets
QT              += printsupport

HEADERS         = connectionwidget.h qsqlconnectiondialog.h \
    mainview.h \
    propertysettings.h \
    agendatab.h \
    propertytab.h \
    generatortab.h \
    database.h \
    agendasettings.h \
    agendaitemsettings.h \
    qsqlquerymodelrichtext.h \
    agendatablecontextmenu.h \
    ordertab.h \
    patterneditor.h \
    patternitemsettings.h \
    orderitemsettings.h \
    patterneditorreport.h \
    patterneditorreportitemsettings.h \
    systemsettings.h \
    editadvisersdialog.h \
    reportgeneratortab.h \
    wizarddialogbox.h \
    votingdialog.h \
    global.h \
    stringreplacer.h \
    decissionitemsettings.h \
    databasedecissiontableimpl.h \
    decissionlibrarytab.h \
    decissionlibraryeditdialog.h
SOURCES         = main.cpp connectionwidget.cpp qsqlconnectiondialog.cpp \
    mainview.cpp \
    propertysettings.cpp \
    agendatab.cpp \
    propertytab.cpp \
    generatortab.cpp \
    database.cpp \
    agendasettings.cpp \
    agendaitemsettings.cpp \
    qsqlquerymodelrichtext.cpp \
    agendatablecontextmenu.cpp \
    ordertab.cpp \
    patterneditor.cpp \
    patternitemsettings.cpp \
    orderitemsettings.cpp \
    patterneditorreport.cpp \
    patterneditorreportitemsettings.cpp \
    systemsettings.cpp \
    editadvisersdialog.cpp \
    reportgeneratortab.cpp \
    wizarddialogbox.cpp \
    votingdialog.cpp \
    stringreplacer.cpp \
    decissionitemsettings.cpp \
    databasedecissiontableimpl.cpp \
    decissionlibrarytab.cpp \
    decissionlibraryeditdialog.cpp

FORMS           = browserwidget.ui qsqlconnectiondialog.ui \
    mainview.ui \
    propertysettings.ui \
    agendatab.ui \
    propertytab.ui \
    generatortab.ui \
    agendasettings.ui \
    agendaitemsettings.ui \
    agendaitemcontextmenu.ui \
    ordertab.ui \
    patterneditor.ui \
    patternitemsettings.ui \
    orderitemsettings.ui \
    patterneditorreport.ui \
    patterneditorreportitemsettings.ui \
    systemsettings.ui \
    editadvisersdialog.ui \
    reportgeneratortab.ui \
    votingdialog.ui \
    decissionitemsettings.ui \
    decissionlibrarytab.ui \
    decissionlibraryeditdialog.ui
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

wince*: {
    DEPLOYMENT_PLUGIN += qsqlite
}

OTHER_FILES += \
    cfg/config.ini
