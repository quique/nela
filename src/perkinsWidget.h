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

#ifndef PERKINSWIDGET_H
#define PERKINSWIDGET_H

#include <Phonon/MediaObject>
#include <Phonon/AudioOutput>
#include <QLabel>

class PerkinsModel;
class Word;
class QKeyEvent;

class PerkinsWidget : public QWidget
{
    Q_OBJECT

public:
    PerkinsWidget(QWidget *parent = 0);
    void playSound(QString soundPath);
    void updatePicture(QString svgPath);

public slots:

protected:
    void keyReleaseEvent(QKeyEvent *);

protected slots:

signals:
    void guiReady();
    void processInput(QString& strPressedPoints);
    void clearSettingsSignal();

private slots:
    void emitGuiReady();
    void soundFinished();
    void repeatLastInstruction();
    void criticalMessage(QString title, QString text);
    void warningMessage(QString title, QString text);
    void correctCharacter(QString& partialAnswer, bool expectMore);
    void incorrectCharacter(QString& correctAnswer, Word* askedWord);
    void parsePoints();
    void rightWord();
    void wordChanged(Word* askedWord, bool firstTimeForThisWord);
    void clearSettings();

private:
    QString dataLocation();
    QString shortLocale();
    void waitMs(int amount);

    PerkinsModel* pm;
    bool expectingInput; // Whether the program is expecting keystrokes from the user.
    bool playingSound;
    QStringList pressedPoints;

    QLabel* question;
    QLabel* answer;
    QLabel* picture;

    Phonon::MediaObject* mediaObject;
    Phonon::AudioOutput* audioOutput;
    Phonon::Path path;

};

#endif
