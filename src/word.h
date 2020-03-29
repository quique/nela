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

#ifndef WORD_H
#define WORD_H

#include <QObject>
#include <QString>
#include <QStringList>

class Word : public QObject
{
    Q_OBJECT

public:
    Word(QString line);
    QString string();
    QString questionString();
    QString asciiString();
    QChar character(int position);
    int length();
    QString instructionPath();
    QString solutionPath();
    QString pixmapPath();

private:
    QString m_string;
    QString m_asciiString;
    QString m_pixmapName;
    QString m_instructionPath;
    QString m_solutionPath;
    QString m_pixmapPath;

    QString dataLocation();
    QString shortLocale();
};

#endif // WORD_H
