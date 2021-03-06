/****************************************************************************
** $Id: rs_scriptlist.cpp 2720 2005-09-24 20:42:06Z andrew $
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

#include "rs_scriptlist.h"

#include "rs_fileinfo.h"
#include "rs_stringlist.h"
#include "rs_system.h"

/*
RS_ScriptList* RS_ScriptList::uniqueInstance = NULL;

/ **
 * Default constructor.
 * /
RS_ScriptList::RS_ScriptList() : iter(scripts) {
    //scripts.setAutoDelete(true);
    //init();
    //scriptListListeners.setAutoDelete(false);
    //activeScript = NULL;
}


/ **
 * Initializes the script list by creating RS_Script 
 * objects, one for each script that could be found.
 * /
void RS_ScriptList::init() {

    RS_DEBUG->print("RS_ScriptList::initScripts");

    scripts.clear();
    RS_StringList list = RS_SYSTEM->getScriptList();
    RS_Script* script;

    for ( RS_StringList::Iterator it = list.begin();
            it != list.end(); ++it ) {
        RS_DEBUG->print("script: %s:", (const char*)(*it).toLatin1());

        RS_FileInfo fi(*it);
        script = new RS_Script(fi.baseName(), fi.absoluteFilePath());
        scripts.append(script);

        RS_DEBUG->print("base: %s", (const char*)fi.baseName().toLatin1());
        RS_DEBUG->print("path: %s", (const char*)fi.absoluteFilePath().toLatin1());
    }

    //RS_Script* f = new RS_Script("normal");
    //scripts.append(f);
}


/ **
 * Removes all scripts in the scriptlist.
 * /
void RS_ScriptList::clearScripts() {
    scripts.clear();
}



/ **
 * Removes a script from the list.
 * Listeners are notified after the script was removed from 
 * the list but before it gets deleted.
 * /
void RS_ScriptList::removeScript(RS_Script* script) {
    RS_DEBUG->print("RS_ScriptList::removeScript()");

    // here the script is removed from the list but not deleted
    scripts.removeAt(scripts.indexOf(script));

    //for (uint i=0; i<scriptListListeners.count(); ++i) {
    //    RS_ScriptListListener* l = scriptListListeners.at(i);
    //    l->scriptRemoved(script);
    //}

    // activate an other script if necessary:
    //if (activeScript==script) {
    //    activateScript(scripts.first());
    //}

    // now it's save to delete the script
    //delete script;
}



/ **
 * @return Pointer to the script with the given name or
 * \p NULL if no such script was found. The script will be loaded into
 * memory if it's not already.
 * /
RS_Script* RS_ScriptList::requestScript(const RS_String& name) {
    RS_DEBUG->print("RS_ScriptList::requestScript %s",  (const char*)name.toLatin1());

    RS_String name2 = name.toLower();
    RS_Script* foundScript = NULL;

    RS_DEBUG->print("name2: %s", (const char*)name2.toLatin1());

    // Search our list of available scripts:
    for (int i=0; i<scripts.size(); ++i) {
        RS_Script* s = scripts[i];
        if (s->getName()==name2) {
            foundScript = s;
            break;
        }
    }

    // Script not found:
    return foundScript;
}



/ **
 * @return Pointer to the script with the given name or
 * \p NULL if no such script was found.
 * /
//RS_Script* RS_ScriptList::loadScript(const RS_String& name) {
//}


/ **
 * Tests the script list and its ability to load scripts.
 * /
bool RS_ScriptList::test() {

    //RS_ScriptList* l = RS_ScriptList::instance();

    //std::cout << "RS_ScriptList: " << *l << std::endl;

    return true;
}


// EOF
*/
