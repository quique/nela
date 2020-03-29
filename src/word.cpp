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

#include "word.h"
#include <QApplication>
#include <QLocale>

Word::Word(QString line)
{
    QStringList data = line.split('\t');
    m_string = data[0];
    m_asciiString = data[1];
    m_pixmapName = data[2];
    m_instructionPath = dataLocation() + "/sounds/instructions/" + shortLocale() + '/' + m_asciiString + ".mp3";
    m_solutionPath = dataLocation() + "/sounds/solutions/" + shortLocale() + '/' + m_asciiString + ".mp3";
    m_pixmapPath = dataLocation() + "/images/" + m_pixmapName;
}

QString Word::string() {
    return m_string;
}

QString Word::questionString() {
    if (m_string.length() > 1 || m_string[0].unicode() < 10240 || m_string[0].unicode() > 10495) {
        return m_string;
    } else {
        int code = m_string[0].unicode();
        if (code == QChar(10241))
            return(tr("(1)"));
        else if (code == QChar(10242))
            return(tr("(2)"));
        else if (code == QChar(10244))
            return(tr("(3)"));
        else if (code == QChar(10248))
            return(tr("(4)"));
        else if (code == QChar(10256))
            return(tr("(5)"));
        else if (code == QChar(10272))
            return(tr("(6)"));
        else if (code == QChar(10304))
            return(tr("(7)"));
        else if (code == QChar(10368))
            return(tr("(8)"));
        else
            return(tr("This should not happen."));
    }
}

QString Word::asciiString() {
    return m_asciiString;
}

QChar Word::character(int position)
{
    return m_string[position];
}

int Word::length()
{
    return m_string.length();
}

QString Word::dataLocation() {
#ifdef PKGDATADIR
    QString data_folder = PKGDATADIR;
#else
    QString data_folder = QApplication::applicationDirPath();
#endif
    return data_folder;
}

QString Word::shortLocale() {
    QString shortLocale = QLocale::system().name().left(2);
    if (shortLocale == "C") shortLocale = "en";
    return shortLocale;
}

QString Word::instructionPath()
{
    return m_instructionPath;
}

QString Word::solutionPath()
{
    return m_solutionPath;
}

QString Word::pixmapPath()
{
    return m_pixmapPath;
}
