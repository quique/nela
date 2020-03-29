/*
 * nela - a Braille code trainer
 * Copyright (C) 2012  Enrique Matias-Sanchez <quique at unizar.es>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mainwindow.h"
#include <QApplication>
#include <QIcon>
#include <QLibraryInfo>
#include <QLocale>
#include <QSplashScreen>
#include <QTranslator>


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Nela");
    app.setApplicationVersion("0.5.0");
    app.setOrganizationDomain("cronopios.net");
    app.setOrganizationName("Enrique Matias-Sanchez");
    app.setWindowIcon(QIcon(":/icons/hicolor/32x32/apps/nela.png"));
    // Tiresias is a family of realist sans-serif typefaces that were designed
    // for best legibility by people with impaired vision at the Scientific
    // Research Unit of Royal National Institute of the Blind in London.
    app.setFont(QFont("Tiresias PCfont", 24));

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);


    #ifdef LOCALEDIR
        QString locale_folder = LOCALEDIR;
    #else
        QString locale_folder = app.applicationDirPath() + "/locale";
    #endif

    QTranslator myappTranslator;
    myappTranslator.load(QLocale::system().name(), locale_folder);
    app.installTranslator(&myappTranslator);

    QPixmap pixmap(":/images/splashscreen.jpg");
    QSplashScreen splash(pixmap);
    splash.show();
    // splash.showMessage("The friendly Braille Trainer");
    app.processEvents(); // This is used to accept a click on the screen so that user can cancel the screen

    MainWindow mainWin;
    mainWin.setWindowState(mainWin.windowState() | Qt::WindowMaximized);
    mainWin.show();

    splash.finish(&mainWin);

    return app.exec();
}
