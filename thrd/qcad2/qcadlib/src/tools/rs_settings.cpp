/****************************************************************************
** $Id: rs_settings.cpp 5739 2007-07-02 14:22:01Z andrew $
**
** Copyright (C) 2001-2003 RibbonSoft. All rights reserved.
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


#include "rs_settings.h"
#include <iostream>

RS_Settings* RS_Settings::uniqueInstance = NULL;


RS_Settings::RS_Settings() {
    initialized = false;
    companyKey = "";
    appKey = "";
    group = "";
}

/**
 * Initialisation.
 *
 * @param companyKey String that identifies the company. Must start
 *        with a "/". E.g. "/RibbonSoft"
 * @param appKey String that identifies the application. Must start
 *        with a "/". E.g. "/QCad2"
 */
void RS_Settings::init(const RS_String& companyKey,
                       const RS_String& appKey) {

    group = "";
    this->appKey = appKey;
    this->companyKey = companyKey;

    initialized = true;
}


/**
 * Destructor
 */
RS_Settings::~RS_Settings() {}



void RS_Settings::beginGroup(const RS_String& group) {
    this->group = group;
}

void RS_Settings::endGroup() {
    this->group = "";
}

bool RS_Settings::writeEntry(const RS_String& key, int value) {
    RS_String s = RS_String("%1").arg(value);
    return writeEntry(key, s);
}

bool RS_Settings::writeEntry(const RS_String& key, double value) {
    RS_String s = RS_String("%1").arg(value);
    return writeEntry(key, s);
}

bool RS_Settings::writeEntry(const RS_String& key, const RS_String& value) {
    QSettings s(QSettings::IniFormat, QSettings::UserScope, companyKey, appKey);

    s.beginGroup(group);
    s.setValue(key, value);
    s.endGroup();

    addToCache(key, value);
    return true;

}

RS_Variant RS_Settings::value(const RS_String& key, 
    const RS_Variant& defaultValue) {

    QSettings s(QSettings::IniFormat, QSettings::UserScope, companyKey, appKey);
    s.beginGroup(group);
    RS_Variant ret = s.value(key, defaultValue);
    s.endGroup();
    return ret;
}
    
void RS_Settings::setValue(const RS_String& key, const RS_Variant& value) {
    QSettings s(QSettings::IniFormat, QSettings::UserScope, companyKey, appKey);
    s.beginGroup(group);
    s.setValue(key, value);
    s.endGroup();
}

RS_String RS_Settings::readEntry(const RS_String& key,
                                 const RS_String& def,
                                 bool* ok) {
    if (ok!=NULL) {
        *ok=true;
    }

    // lookup:
    RS_String ret = readEntryCache(key);
    if (ret==RS_String::null) {
        QSettings s(QSettings::IniFormat, QSettings::UserScope, companyKey, appKey);

        s.beginGroup(group);
        ret = s.value(key, def).toString();
        s.endGroup();
        
        addToCache(key, ret);
    }

    return ret;
}



double RS_Settings::readDoubleEntry(const RS_String& key,
                                 double def,
                                 bool* ok) {
    if (ok!=NULL) {
        *ok=true;
    }

    RS_String strDef;
    strDef = RS_String("%1").arg(def);

    RS_String ret = readEntry(key, strDef, ok);

    if (ok==NULL || *ok==true) {
        return ret.toDouble(ok);
    }
    else {
        return def;
    }
}



int RS_Settings::readNumEntry(const RS_String& key, int def, bool* ok) {
    // lookup:
    RS_String sret = readEntryCache(key);
    if (sret==RS_String::null) {
        int ret;
        QSettings s(QSettings::IniFormat, QSettings::UserScope, companyKey, appKey);
        s.beginGroup(group);
        ret = s.value(key, def).toInt(ok);
        s.endGroup();
        addToCache(key, RS_String("%1").arg(ret));
        return ret;
    }
    else {
        if (ok) {
            *ok=true;
        }
        return sret.toInt();
    }
}


RS_String RS_Settings::readEntryCache(const RS_String& key) {
    RS_Hash<QString, QString>::const_iterator i  = cache.find(group + key);
    if (i==cache.end()) {
        return RS_String::null;
    }
    else {
        return i.value();
    }
}


void RS_Settings::addToCache(const RS_String& key, const RS_String& value) {
    cache[group + key] = value;
}
