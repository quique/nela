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

#ifndef PERKINSMODEL_H
#define PERKINSMODEL_H

#include "word.h"
#include <QHash>
#include <QQueue>
#include <QVector>

class PerkinsModel : public QObject
{
    Q_OBJECT
public:
    explicit PerkinsModel(QObject *parent = 0);
    QChar pointsToUnicode(QString points);

signals:
    void correctCharacter(QString& partialAnswer, bool expectMore);
    void incorrectCharacter(QString& correctAnswer, Word* askedWord);
    void rightWord();
    void wordChanged(Word* askedWord, bool firstTimeForThisWord);
    void criticalMessage(QString title, QString text);
    void warningMessage(QString title, QString text);
    
public slots:
    void guiReady();
    void processInput(QString& strPressedPoints);
    void clearSettings();

protected:

protected slots:
    
private slots:

private:
    int currentPosition; // Position in the current word, which signals the asked character.
    Word* askedWord;
    bool firstTimeForThisWord;
    QHash<QChar, QString> braillehash; // Table character->Braille in a given language.
    QList<Word*> levels; // Ordered list of words to study.
    int currentLevel; // Position of the last word taken from levels.
    QString partialAnswer;

    /*
     * Nela's Spaced Repetion is inspired in Sebastian Leitner's System.
     * There are 5 boxes (numbered 0 to 4). New words enter in box 0.
     * If the user correctly answers a word, this word is passed to the next box.
     * If he fails, the word goes back right to the beginning (box 0).
     */
    QVector<QQueue<Word*> > boxes;
    int currentBox;

    int getRandomBox();
    bool checkCharacter(QString strPressedPoints, QChar askedChar);

    void useNewWord();
    QString dataLocation();
    QString shortLocale();
    void populateBrailleHash();
    void populateLevels();
    void saveSettings();
};

#endif // PERKINSMODEL_H
