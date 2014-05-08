/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainview.h"

#include <QtCore>
#include <QtWidgets>
#include <QtSql>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow mainWin;
    mainWin.setWindowTitle(QObject::tr("ETV Planer"));
    mainWin.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    mainView view (&mainWin);
    mainWin.setCentralWidget(&view);
    mainWin.setFixedSize(view.size());

    QMenu *fileMenu = mainWin.menuBar()->addMenu(QObject::tr("&File"));
    fileMenu->addSeparator();
    fileMenu->addAction(QObject::tr("&Quit"), &app, SLOT(quit()));

    QMenu *propertyMenu = mainWin.menuBar()->addMenu(QObject::tr("&Liegenschaften verwalten"));
    propertyMenu->addAction (QObject::tr("Liegenschaft hinzufügen"), &view, SLOT(addProperty()));

    QMenu *templateMenu = mainWin.menuBar()->addMenu(QObject::tr("&Vorlagen verwalten"));
    templateMenu->addAction (QObject::tr("Tagesordnungspunktvorlagen"), &view, SLOT(patternSettings()));
    templateMenu->addAction (QObject::tr("Protokoll Deckblattvorlagen"), &view, SLOT(reportSettings()));

    QMenu *optionsMenu = mainWin.menuBar()->addMenu(QObject::tr("&Optionen"));
    optionsMenu->addAction (QObject::tr("Systemeinstellungen"), &view, SLOT(systemSettings ()));

    QMenu *helpMenu = mainWin.menuBar()->addMenu(QObject::tr("&Help"));
    helpMenu->addAction(QObject::tr("About Qt"), qApp, SLOT(aboutQt()));

    mainWin.show();

    QString path = QApplication::applicationDirPath() + "/cfg/config.ini";
    //QString path = "cfg/config.ini";

    QSettings settings (path, QSettings::IniFormat);

    QSqlError err = view.addConnection(settings.value("database/type", "").toString(),
                       settings.value("database/name", "").toString(),
                       settings.value("database/host", "").toString(),
                       settings.value("database/user", "").toString(),
                       settings.value("database/pw", "").toString(),
                       settings.value("database/port", "").toInt());

    if (err.type() != QSqlError::NoError)
    {
        qDebug() << "Unable to open predefined connection:" << err;
        qDebug()    << "### VALUES ###" << endl
                    << "database/type: " << settings.value("database/type", "").toString() << endl
                    << "database/name: " << settings.value("database/name", "").toString() << endl
                    << "database/host: " << settings.value("database/host", "").toString() << endl
                    << "database/user: " << settings.value("database/user", "").toString() << endl
                    << "database/pw: " << settings.value("database/pw", "").toString() << endl
                    << "database/port: " << settings.value("database/port", "").toInt() << endl
                    << "##############";

    }
    else
    {
        qDebug() << "Opened predefined database connection";
        qDebug()    << "### VALUES ###" << endl
            << "database/type: " << settings.value("database/type", "").toString() << endl
            << "database/name: " << settings.value("database/name", "").toString() << endl
            << "database/host: " << settings.value("database/host", "").toString() << endl
            << "database/user: " << settings.value("database/user", "").toString() << endl
            << "database/pw: " << settings.value("database/pw", "").toString() << endl
            << "database/port: " << settings.value("database/port", "").toInt() << endl
            << "##############";
    }


    if (QSqlDatabase::connectionNames().isEmpty())
        QMetaObject::invokeMethod(&view, "addConnection", Qt::QueuedConnection);

    QMetaObject::invokeMethod(&view, "refreshView", Qt::QueuedConnection);

    return app.exec();
}
