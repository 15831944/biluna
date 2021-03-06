/****************************************************************************
**
** Copyright (C) 2001-2008 RibbonSoft. All rights reserved.
**
** This file is part of the qcadlib Library project.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid qcadlib Professional Edition licenses may use 
** this file in accordance with the qcadlib Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.ribbonsoft.com for further details.
**
** Contact info@ribbonsoft.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "qg_dlginitial.h"

#include "rs_line.h"
#include "rs_graphic.h"

/**
 * Constructor
 */
QG_DlgInitial::QG_DlgInitial(QWidget* parent)
        : QDialog(parent) {
    setupUi(this);

    connect(buttonOk, SIGNAL(clicked()),
            this, SLOT(ok()));

    // Fill combobox with languages:
    QStringList languageList = RS_SYSTEM->getLanguageList();
    for (RS_StringList::Iterator it = languageList.begin();
            it!=languageList.end();
            it++) {

        QString l = RS_SYSTEM->symbolToLanguage(*it);
        cbLanguage->addItem(l);
        //cbLanguageCmd->addItem(l);
    }


    // units:
    for (int i=RS2::None; i<RS2::LastUnit; i++) {
        cbUnit->addItem(RS_Units::unitToString((RS2::Unit)i));
    }

    cbUnit->setCurrentIndex(cbUnit->findText("Millimeter"));
    cbLanguage->setCurrentIndex(cbLanguage->findText("English"));
     //cbLanguageCmd->setCurrentIndex(cbLanguageCmd->findText("English"));
	 
    // Biluna alternative to disabled language selection
    cbLanguage->addItem("English");
    cbLanguage->setCurrentIndex(cbLanguage->findText("English"));
    cbLanguage->setEnabled(false);
}


/**
 * Destructor
 */
QG_DlgInitial::~QG_DlgInitial() {}

void QG_DlgInitial::setText(const QString& t) {
    lWelcome->setText(t);
}

void QG_DlgInitial::setPixmap(const QPixmap& p) {
    lImage->setPixmap(p);
}

void QG_DlgInitial::ok() {
    RS_SETTINGS->beginGroup("/Appearance");
    RS_SETTINGS->writeEntry("/Language",
                            RS_SYSTEM->languageToSymbol(cbLanguage->currentText()));
    //RS_SETTINGS->writeEntry("/LanguageCmd",
    //                        RS_SYSTEM->languageToSymbol(cbLanguageCmd->currentText()));
    RS_SETTINGS->endGroup();

    RS_SETTINGS->beginGroup("/Defaults");
    RS_SETTINGS->writeEntry("/Unit", cbUnit->currentText());
    RS_SETTINGS->endGroup();
    accept();
}
