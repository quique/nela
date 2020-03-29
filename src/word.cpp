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

/*!
    \class Word
    The Word class represents a word (or phrase).
 */

/*!
    Constructs a Word using the data in \a line.
 */
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

/*!
    Returns the Word's text in UTF-8.
 */
QString Word::string() {
    return m_string;
}

/*!
    Returns the word's text as it will be shown in the user interface.
    This is needed for Braille points, as they are difficult to distinguish from each other.
 */
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

/*!
    Returns the word's text in US ASCII.
    This is used in filenames to avoid cross-platform issues.
 */
QString Word::asciiString() {
    return m_asciiString;
}

/*!
    Returns the positionth character of this word.
 */
QChar Word::character(int position)
{
    return m_string[position];
}

/*!
    Returns the length of this word.
 */
int Word::length()
{
    return m_string.length();
}

/*!
    Returns the location of the application's data.
    This directory can be located in /usr/share, /usr/local/share, C:/Program Files/Nela, etc.
 */
QString Word::dataLocation() {
#ifdef PKGDATADIR
    QString data_folder = PKGDATADIR;
#else
    QString data_folder = QApplication::applicationDirPath();
#endif
    return data_folder;
}

/*!
    Returns the ISO 639-1 code for this language.
 */
QString Word::shortLocale() {
    QString shortLocale = QLocale::system().name().left(2); // FIXME This should also work with ISO 639-2.
    if (shortLocale == "C") shortLocale = "en";
    return shortLocale;
}

/*!
    Returns the location of this word's instruction recording.
 */
QString Word::instructionPath()
{
    return m_instructionPath;
}

/*!
    Returns the location of this word's solution recording.
 */
QString Word::solutionPath()
{
    return m_solutionPath;
}

/*!
    Returns the location of this word's image (a SVG file).
 */
QString Word::pixmapPath()
{
    return m_pixmapPath;
}
