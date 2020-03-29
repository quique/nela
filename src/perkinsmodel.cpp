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

#include "perkinsmodel.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <QApplication>
#include <QFile>
#include <QSettings>
#include <QTextCodec>
#include <QTextStream>
#include <QDebug>

PerkinsModel::PerkinsModel(QObject *parent) :
    QObject(parent)
{
    populateBrailleHash();
    populateLevels();

    srand(time(0));

    QSettings settings;
    currentLevel = settings.value("currentLevel", -1).toInt(); // Original default value is -1 because it will incremented to 0 in useNewWord().
    qDebug() << "Current level: " << currentLevel;

    boxes.resize(5);
    boxes.fill(QQueue<Word*>());

    QVariantList savedWords = settings.value("savedWords").toList();
    int boxNr = 0;
    qDebug() << "Box" << boxNr;
    foreach (const QVariant &wordNumber, savedWords) {
        if (wordNumber != -1) {
            boxes[boxNr].enqueue(levels.at(wordNumber.toInt()));
            qDebug() << wordNumber.toInt();
        } else {
            ++boxNr;
            qDebug() << "Box" << boxNr;
        }
    }
}

/*
 * This slot is run once the GUI is ready to receive signals from the model.
 * Thus, the first word can be selected.
 */
void PerkinsModel::guiReady()
{
    useNewWord();
}

bool PerkinsModel::checkCharacter(QString strPressedPoints, QChar askedChar)
{
    return strPressedPoints == braillehash[askedChar];
}

void PerkinsModel::processInput(QString& strPressedPoints)
{
    qDebug() << "Pos:" << currentPosition << "Ch:" << askedWord->character(currentPosition) << "Dots:" << braillehash[askedWord->character(currentPosition)] << "Got: " << strPressedPoints;

    if (checkCharacter(strPressedPoints, askedWord->character(currentPosition))) {
        qDebug() << "The entered char was right!" << askedWord->character(currentPosition);

        partialAnswer += pointsToUnicode(braillehash[askedWord->character(currentPosition)]);
        ++currentPosition;
        emit correctCharacter(partialAnswer, currentPosition < askedWord->length());

        qDebug() << "Let's go for the next character (if any)";
    } else {
        qDebug() << "The entered character was not right.";
        qDebug() << "The word goes back from box" << currentBox << "to box 0  :-(";
        boxes[currentBox].dequeue();
        boxes[0].enqueue(askedWord);

        // Show the solution
        QString askedBraille("");
        foreach (QChar ch, askedWord->string()) {
            askedBraille.append(pointsToUnicode(braillehash[ch]));
        }
        QString correctAnswer = askedBraille;

        emit incorrectCharacter(correctAnswer, askedWord);

        qDebug() << "Let's ask another word\n";
        useNewWord();
    }

    if (currentPosition == askedWord->length()) {
        // The whole word was answered correctly.
        // The word passes to the next box, unless it already was at the last one.
        qDebug() << "The word was correctly finished!";

        boxes[currentBox].dequeue();
        if (currentBox != boxes.size()-1) {
            qDebug() << "Let's move" << askedWord->string() << "from box" << currentBox << "to box" << currentBox+1;
            boxes[currentBox+1].enqueue(askedWord);
        } else {
            qDebug() << "The word was already at the last level.";
            boxes[currentBox].enqueue(askedWord);
        }

        emit rightWord();

        // The user will be asked a new word.
        qDebug() << "Let's ask another word\n";
        useNewWord();
    }
}

void PerkinsModel::useNewWord()
{
    // Select a new word
    if (currentLevel == -1
            || (boxes[0].isEmpty() && boxes[1].length() <= 1 && boxes[2].length() <= 2
                && !boxes[1].contains(levels.at(currentLevel))
                && !boxes[2].contains(levels.at(currentLevel)))) {
        // If box 0 is empty, there is at most 1 word in box 1,
        // 2 words in box 2, and the word in the current level
        // is not one of them, let's go for a new word.
        qDebug() << "Let's go to the next level!" << currentLevel+1;
        firstTimeForThisWord = true;
        ++currentLevel;

        currentBox = 0;
        askedWord = levels.at(currentLevel);
        boxes[0].enqueue(askedWord); // It will be the only element in this box.

        qDebug() << "A completely new word has been selected.\n";
    } else {
        // Let's select one of the previously introduced words.
        firstTimeForThisWord = false;
        do {
            currentBox = getRandomBox();
            qDebug() << "Current box:" << currentBox;
        } while (boxes[currentBox].isEmpty());
        askedWord = boxes[currentBox].head();
        qDebug() << "Word selected:" << askedWord->string();
    }

    currentPosition = 0;
    partialAnswer = "";
    saveSettings();

    emit wordChanged(askedWord, firstTimeForThisWord);
}

void PerkinsModel::saveSettings()
{
    QSettings settings;
    settings.setValue("currentLevel", currentLevel);

    QVariantList savedWords;
    for (int i = 0; i < boxes.size(); ++i) {
        qDebug() << "Box" << i;
        QQueue<Word*> box = boxes.at(i);
        foreach (Word* word, box) {
            savedWords.append(levels.indexOf(word));
            qDebug() << levels.indexOf(word);
        }
        savedWords.append(-1);
    }
    settings.setValue("savedWords", savedWords);
}

// Braille characters have Unicode codes 10240 - 10495 (0x2800 - 0x28FF).
QChar PerkinsModel::pointsToUnicode(QString points)
{
    int code = 0;
    foreach (const QString &digit, points) {
        code += pow(2, digit.toInt() - 1);
    }
    code += 10240;
    return QChar(code);
}

int PerkinsModel::getRandomBox()
{
    double n = rand() / (RAND_MAX+1.0);
    if (n < 0.3478)      // 34.78% chance for box 0 (8/23)
        return 0;
    else if (n < 0.6087) // 26.09% chance for box 1 (6/23)
        return 1;
    else if (n < 0.7826) // 17.39% chance for box 2 (4/23)
        return 2;
    else if (n < 0.9130) // 13.04% chance for box 3 (3/23)
        return 3;
    else                 //  8.70% chance for box 4 (2/23)
        return 4;
}

QString PerkinsModel::dataLocation()
{
#ifdef PKGDATADIR
    QString data_folder = PKGDATADIR;
#else
    QString data_folder = QApplication::applicationDirPath();
#endif
    return data_folder;
}

QString PerkinsModel::shortLocale()
{
    QString shortLocale = QLocale::system().name().left(2);
    if (shortLocale == "C") shortLocale = "en";
    return shortLocale;
}

void PerkinsModel::populateLevels()
{
    QFile file(dataLocation() + "/levels/" + shortLocale() + ".csv");
    if (!file.exists()) {
        file.setFileName(dataLocation() + "/levels/es.csv");
        emit warningMessage(tr("Warning"), tr("Unable to find %1 levels, looking for Spanish levels.").arg(QLocale::languageToString(QLocale::system().language())));
    }
    qDebug() << "Levels file:" << file.fileName();

    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        in.setCodec(QTextCodec::codecForName("UTF-8"));
        QString line;
        while (not in.atEnd()) {
            line = in.readLine();
            levels.append(new Word(line));
        }
        file.close();
    } else {
        emit criticalMessage(tr("Critical error"), tr("ERROR: Unable to open file %1").arg(file.fileName()));
        QApplication::quit(); // FIXME It doesn't quit!
    }

    qDebug() << "= LEVELS =";
    foreach (Word* level, levels) {
        qDebug() << level->string();
    }
}

void PerkinsModel::populateBrailleHash()
{
    QFile file(dataLocation() + "/tables/" + shortLocale() + ".txt");
    if (!file.exists()) {
        file.setFileName(dataLocation() + "/tables/es.txt");
        emit warningMessage(tr("Warning"), tr("Unable to find %1 table, looking for Spanish table.").arg(QLocale::languageToString(QLocale::system().language())));
    }
    qDebug() << "Table file:" << file.fileName();

    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        in.setCodec(QTextCodec::codecForName("UTF-8"));
        QString line;
        QChar character;
        QString points;

        while (not in.atEnd()) {
            line = in.readLine();
            QStringList data = line.split(' ');
            character = data[0][0];
            points = data[1];
            braillehash.insert(character, points);
        }
        file.close();
    } else {
        criticalMessage(tr("Critical error"), tr("ERROR: Unable to open the table %1").arg(file.fileName()));
        QApplication::quit(); // FIXME It doesn't quit!
    }

    qDebug() << "= TABLE =";

    QHash<QChar, QString>::const_iterator iter = braillehash.constBegin();
    const QHash<QChar, QString>::const_iterator end = braillehash.constEnd();
    for (; iter != end; ++iter) {
        qDebug() << (*iter);
    }
}

void PerkinsModel::clearSettings()
{
    currentLevel = -1;
    for (int i = 0; i < boxes.size(); ++i) {
        boxes[i].clear();
    }

    saveSettings();
    useNewWord();
}
