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

#include "perkinsWidget.h"
#include "perkinsmodel.h"
#include "word.h"
#include <QApplication>
#include <QFile>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPainter>
#include <QtSvg/QSvgRenderer>
#include <QTimer>
#include <QVBoxLayout>
#include <QDebug>

PerkinsWidget::PerkinsWidget(QWidget *parent) : QWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);

    question = new QLabel();
    question->setAlignment(Qt::AlignCenter);
    question->setFont(QFont("Tiresias PCfont", 96));

    answer = new QLabel();
    answer->setAlignment(Qt::AlignCenter);
    // DejaVu provides an expanded version of the Vera font family aiming
    // for quality and broader Unicode coverage while retaining the
    // original Vera style.  It includes Braille glyphs.
    answer->setFont(QFont("DejaVu Sans", 96));

    picture = new QLabel();
    picture->setAlignment(Qt::AlignCenter);

    QHBoxLayout *layout = new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addWidget(question);
    vlayout->addWidget(answer);
    layout->addLayout(vlayout);
    layout->addWidget(picture);
    setLayout(layout);

    playingSound = false;
    mediaObject = new Phonon::MediaObject(this);
    audioOutput = new Phonon::AudioOutput(Phonon::AccessibilityCategory, this);
    path = Phonon::createPath(mediaObject, audioOutput);
    mediaObject->setTransitionTime(1); // Workaround KDE bug #213706 - Phonon does not play last and/or first second of an audiofile.
    
    pm = new PerkinsModel(this);
    connect(mediaObject, SIGNAL(finished()), this, SLOT(soundFinished()));
    connect(this, SIGNAL(processInput(QString&)), pm, SLOT(processInput(QString&)));
    connect(pm, SIGNAL(correctCharacter(QString&, bool)), this, SLOT(correctCharacter(QString&, bool)));
    connect(pm, SIGNAL(incorrectCharacter(QString&, Word*)), this, SLOT(incorrectCharacter(QString&, Word*)));
    connect(pm, SIGNAL(rightWord()), this, SLOT(rightWord()));
    connect(pm, SIGNAL(wordChanged(Word*, bool)), this, SLOT(wordChanged(Word*, bool)));
    connect(pm, SIGNAL(criticalMessage(QString, QString)), this, SLOT(criticalMessage(QString, QString)));
    connect(pm, SIGNAL(warningMessage(QString, QString)), this, SLOT(warningMessage(QString, QString)));
    connect(this, SIGNAL(clearSettingsSignal()), pm, SLOT(clearSettings()));
    connect(this, SIGNAL(guiReady()), pm, SLOT(guiReady()));

    playSound(dataLocation() + "/sounds/welcome.mp3");
    while (playingSound) waitMs(50);
    QTimer::singleShot(50, this, SLOT(emitGuiReady()));
}

void PerkinsWidget::waitMs(int amount)
{
    QTimer t;
    t.setSingleShot(true);
    t.start(amount);
    while(t.isActive())
        QApplication::processEvents();
}

// This signal informs the model that the GUI is ready to receive signals.
// Thus, it can select the first word.
void PerkinsWidget::emitGuiReady()
{
    emit guiReady();
}

void PerkinsWidget::keyReleaseEvent(QKeyEvent *keyevent)
{
    if (keyevent->isAutoRepeat() || !expectingInput)
        return;

    int code = keyevent->key();
    QTimer::singleShot(400, this, SLOT(parsePoints()));

    switch (code) {
    // FIXME The keys should not be hardcoded.

    case Qt::Key_F:
        pressedPoints << "1";
        break;

    case Qt::Key_D:
        pressedPoints << "2";
        break;

    case Qt::Key_S:
        pressedPoints << "3";
        break;

    case Qt::Key_J:
        pressedPoints << "4";
        break;

    case Qt::Key_K:
        pressedPoints << "5";
        break;

    case Qt::Key_L:
        pressedPoints << "6";
        break;

    case Qt::Key_A:
        pressedPoints << "7";
        break;

    case Qt::Key_Ntilde:
        pressedPoints << "8";
        break;

    case Qt::Key_Return:
    case Qt::Key_Enter:
        repeatLastInstruction();
        break;

    case Qt::Key_Escape:
        QApplication::quit();
        break;

    }  // switch(code)
}  // keyReleaseEvent(QKeyEvent *keyevent)

void PerkinsWidget::playSound(QString soundPath)
{
    while (playingSound) {
        waitMs(50);
    }

    qDebug() << "soundPath:" << soundPath;
    QFile file(soundPath);
    if (file.exists()) {
        playingSound = true;
        mediaObject->setCurrentSource(Phonon::MediaSource(soundPath));
        mediaObject->play();
    }
}

void PerkinsWidget::soundFinished()
{
    playingSound = false;
}

void PerkinsWidget::repeatLastInstruction()
{
    mediaObject->setCurrentSource(mediaObject->currentSource().fileName()); // For some reason it has to be reset.
    mediaObject->play();
}

void PerkinsWidget::updatePicture(QString svgPath)
{
    QPixmap image(QPixmap(svgPath).scaled(picture->size(), Qt::KeepAspectRatio, Qt::FastTransformation));

    image.fill(Qt::transparent);
    QPainter painter(&image);
    QSvgRenderer renderer;
    renderer.load(svgPath);
    renderer.render(&painter);

    picture->setPixmap(image);
}

void PerkinsWidget::parsePoints()
{
    // Every keyReleaseEvent generates a QTimer, which launches this slot.
    // The first time this slot is run for a letter, pressedPoints is cleared.
    // The remaining times, there is nothing left to do.
    if (pressedPoints.isEmpty()) {
        return;
    }

    expectingInput = false;

    pressedPoints.sort();
    // A user could manage to press the same key twice in the allotted time.
    pressedPoints.removeDuplicates();
    QString strPressedPoints = pressedPoints.join("");
    pressedPoints.clear();

    emit processInput(strPressedPoints);
}

void PerkinsWidget::wordChanged(Word* askedWord, bool firstTimeForThisWord)
{
    question->setText(askedWord->questionString());
    updatePicture(askedWord->pixmapPath());
    answer->clear();

    if (firstTimeForThisWord) {
        playSound(askedWord->solutionPath());
    }
    playSound(askedWord->instructionPath());
    // while (playingSound) waitMs(30);
    expectingInput = true;
}

void PerkinsWidget::correctCharacter(QString& partialAnswer, bool expectMore)
{
    answer->setText(partialAnswer);
    playSound(dataLocation() + "/sounds/feedback/keystroke.mp3");
    expectingInput = expectMore;
}

void PerkinsWidget::incorrectCharacter(QString& correctAnswer, Word* askedWord)
{
    answer->setText(correctAnswer);
    updatePicture(dataLocation() + "/images/red-not-ok--failure-symbol-by-antares42.svg");
    playSound(dataLocation() + "/sounds/feedback/bummer.mp3");
    playSound(askedWord->solutionPath());
    while (playingSound) waitMs(50);
}

void PerkinsWidget::rightWord()
{
    updatePicture(dataLocation() + "/images/green-ok--success-symbol-by-antares42.svg");
    playSound(dataLocation() + "/sounds/feedback/goal.mp3");
    while (playingSound) waitMs(50);
}

void PerkinsWidget::criticalMessage(QString title, QString text)
{
    QMessageBox::critical(this, title, text);
}

void PerkinsWidget::warningMessage(QString title, QString text)
{
    QMessageBox::warning(this, title, text);
}


QString PerkinsWidget::dataLocation()
{
#ifdef PKGDATADIR
    QString data_folder = PKGDATADIR;
#else
    QString data_folder = QApplication::applicationDirPath();
#endif
    return data_folder;
}

QString PerkinsWidget::shortLocale()
{
    QString shortLocale = QLocale::system().name().left(2);
    if (shortLocale == "C") shortLocale = "en";
    return shortLocale;
}

void PerkinsWidget::clearSettings()
{
    int ret = QMessageBox::question(this, tr("Clean progress done so far?"),
                                    tr("This will clean all the progress done so far,\n"
                                       "so you will restart from the beginning.\n"
                                       "Are you sure you want to reset the program?"),
                                    QMessageBox::Cancel|QMessageBox::Reset,
                                    QMessageBox::Cancel);
    if (ret == QMessageBox::Reset) {
        emit clearSettingsSignal();
    }
}
