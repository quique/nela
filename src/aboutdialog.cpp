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

#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include <QFile>
#include <QTextStream>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    ui->textLabel->setText(tr("Nela %1").arg(qApp->applicationVersion()));
    setFont(QFont("Tiresias PCfont", 12));

    // Fill in Authors tab the people who wrote the software
    QString authors;
    authors = format(QString::fromUtf8("Enrique Matías Sánchez"),
                     "quique@unizar.es", tr("Main Developer."));

    // IMHO, the following don't belong here, but to the "Thanks To" tab. Never mind...
    authors += format(QString::fromUtf8("Inmaculada Plaza García"),
                      "iplaza@unizar.es", tr("Project direction."));
    authors += format(QString::fromUtf8("Inés Azucena Benedicto Moya"),
                      "", tr("Initial idea proposal."));
    authors += format(tr("EduQTech Research Group (University of Zaragoza)"),
                      "eduqtech@unizar.es", tr("Study of the state of the art and applicable law."));
    authors += format(QString::fromUtf8("Hermes Ojeda Ruiz"), "hermes.o.r@gmail.com",
                      tr("Patches, and packaging for Chakra GNU/Linux."));
    ui->authorsText->setText(authors);
    ui->authorsText->setReadOnly(true);

    // Fill in Thanks To tab the people who helped
    QString contributors;

    contributors = format(QString::fromUtf8("Setuniman"), "",
                          tr("Author of piano jingle 0L_16po2.WAV, which is used as welcome sound. Released under CC by 3.0."));

    contributors += format(QString::fromUtf8("Stas Zytkiewicz"), "childsplay@users.sf.net",
                           tr("Author of childsplay, from where bummer.mp3 and goal.mp3 were taken."));

    contributors += format(QString::fromUtf8("fastson"), "",
                           tr("Recorder of typewriter_slow.wav, on which keystroke.mp3 is based. Released under CC by 3.0."));

    contributors += format(QString::fromUtf8("Ralph Aichinger"), "",
                           tr("Author of the photograph used as background in the splashscreen. Released under CC by."));

    contributors += format(QString::fromUtf8("Nuno Pinheiro"), "nuno@nuno-icons.com",
                           tr("Made the icon of Akregator, on which Nela's icon is based."));

    contributors += format(QString::fromUtf8("Open Clip Art Library contributors"), "",
                           tr("Dedicated many images to the public domain."));

    contributors += format(QString::fromUtf8("Claus Faerber"), "claus@faerber.name",
                           tr("Author of braille.pl, which was used to create the SVG images for the dots."));

    ui->thanksText->setText(contributors);
    ui->thanksText->setReadOnly(true);

    // Fill in License tab with the GNU GPL v3
    QFile file(":/COPYING");
    QString licenseString;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            licenseString += in.readLine() + '\n';
        }
        file.close();
    }
    ui->licenseText->setText(licenseString);
    ui->licenseText->setReadOnly(true);
}

QString AboutDialog::format(QString credit, QString email, QString role)
{
    if (!email.isEmpty())
        credit += " &lt;<a href=\"mailto:" + email + "\">" + email + "</a>&gt;";
    credit += "<br> &nbsp; " + role + "<br><br>";

    return credit;
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
