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
#include "perkinsWidget.h"
#include "aboutdialog.h"
#include <QAction>
#include <QApplication>
#include <QDesktopServices>
#include <QLocale>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPointer>
#include <QStatusBar>
#include <QUrl>

MainWindow::MainWindow()
{
    pw = new PerkinsWidget();
    setCentralWidget(pw);
    
    createActions();
    createMenus();
    createStatusBar();
}

void MainWindow::showIntroduction() {
    QDesktopServices::openUrl(QUrl("file:///" + docLocation() + '/' + shortLocale() + "/introduction/index.html", QUrl::TolerantMode));
}

void MainWindow::about()
{
    AboutDialog* about = new AboutDialog();
    about->show();
    /*
    QPointer<QMessageBox> aboutBox = new QMessageBox(this);
    aboutBox->setTextFormat(Qt::RichText);

    aboutBox->setText(tr("<center><img src=\":/icons/hicolor/256x256/apps/nela.png\"/></img></center>"
                    "<h2>Nela %1</h2>"
                    "<p>Nela is a Braille code trainer.</p>"
                    "<p>Copyright &copy; 2012 Enrique Mat&iacute;as S&aacute;nchez.<br>Grupo EduQTech.<br>Universidad de Zaragoza.</p>"
                   ).arg(qApp->applicationVersion()));

    aboutBox->setWindowTitle(tr("About %1").arg(qApp->applicationName()));
    aboutBox->setIcon(QMessageBox::NoIcon);
    aboutBox->exec();
    delete aboutBox;
    */
}

void MainWindow::updateStatusBar(QString message)
{
    statusLabel->setText(message);
}

void MainWindow::createActions()
{
    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setIcon(QIcon::fromTheme(QLatin1String("application-exit")));
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(tr("Exit the application"));
    exitAction->setMenuRole(QAction::QuitRole);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    lastInstructionAction = new QAction(tr("&Repeat last instruction"), this);
    // lastInstructionAction->setShortcut(tr("Intro")); // Already in pw
    lastInstructionAction->setStatusTip(tr("Repeats the word to be entered"));
    connect(lastInstructionAction, SIGNAL(triggered()), pw, SLOT(repeatLastInstruction()));

    resetAction = new QAction(tr("&Start from scratch"), this);
    // resetInstructionAction->setShortcut(tr("XXX")); // Unnecessary and a bit dangerous, I think.
    resetAction->setStatusTip(tr("Clears the progress done so far."));
    connect(resetAction, SIGNAL(triggered()), pw, SLOT(clearSettings()));

    introAction = new QAction(tr("&Introduction to %1").arg(QApplication::applicationName()), this);
    introAction->setIcon(QIcon::fromTheme(QLatin1String("help-contents")));
    introAction->setShortcut(QKeySequence::HelpContents);
    introAction->setStatusTip(tr("Read an introduction to the application in your browser"));
    connect(introAction, SIGNAL(triggered()), this, SLOT(showIntroduction()));

    aboutAction = new QAction(tr("&About %1").arg(QApplication::applicationName()), this);
    aboutAction->setIcon(windowIcon());
    aboutAction->setStatusTip(tr("Show the application's About box"));
    aboutAction->setMenuRole(QAction::AboutRole);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(exitAction);

    controlMenu = menuBar()->addMenu(tr("&Control"));
    controlMenu->addAction(lastInstructionAction);
    controlMenu->addAction(resetAction);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(introAction);
    helpMenu->addAction(aboutAction);
}

void MainWindow::createStatusBar()
{
    statusLabel = new QLabel();
    statusBar()->addWidget(statusLabel, 1);

    updateStatusBar(tr("Use keyboard's central row"));
}

QString MainWindow::dataLocation()
{
#ifdef PKGDATADIR
    QString data_folder = PKGDATADIR;
#else
    QString data_folder = QApplication::applicationDirPath();
#endif
    return data_folder;
}

QString MainWindow::shortLocale()
{
    QString shortLocale = QLocale::system().name().left(2);
    if (shortLocale == "C") shortLocale = "en";
    return shortLocale;
}

QString MainWindow::docLocation()
{
#ifdef PKGDOCDIR
    QString doc_folder = PKGDOCDIR;
#else
    QString doc_folder = QApplication::applicationDirPath() + "/doc";
#endif
    return doc_folder;
}
