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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class QLabel;
class PerkinsWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

public slots:

protected:

protected slots:

signals:
    
private slots:
    void showIntroduction();
    void about();
    void updateStatusBar(QString message);

private:
    void createActions();
    void createMenus();
    void createStatusBar();
    QString dataLocation();
    QString shortLocale();
    QString docLocation();

    PerkinsWidget *pw;

    QLabel *statusLabel;
    QMenu *fileMenu;
    QMenu *controlMenu;
    QMenu *helpMenu;
    
    QAction *exitAction;
    QAction *introAction;
    QAction *lastInstructionAction;
    QAction *resetAction;
    QAction *aboutAction;
};

#endif
