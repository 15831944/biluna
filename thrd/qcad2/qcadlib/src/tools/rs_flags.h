/****************************************************************************
** $Id: rs_flags.h 4861 2007-03-07 15:29:43Z andrew $
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


#ifndef RS_FLAGS_H
#define RS_FLAGS_H

#include "rs.h"
#include "rs_debug.h"

/**
 * Base class for objects which have flags.
 *
 * @author Andrew Mustun
 */
class CAD_EXPORT RS_Flags {
public:
    /** Default constructor. Resets all flags to 0. */
    RS_Flags() {
        flags = 0;
    }

    /** Constructor with initialisation to the given flags. */
    RS_Flags(unsigned int f) {
        flags = f;
    }

    virtual ~RS_Flags() {}

    inline unsigned int getFlags() const {
        return flags;
    }

    inline void resetFlags() {
        flags=0;
    }

    inline void setFlags(unsigned int f) {
        flags=f;
    }

    inline void setFlag(unsigned int f) {
        flags=flags|f;
    }

    inline void delFlag(unsigned int f) {
        flags=flags&(~f);
    }

    inline void toggleFlag(unsigned int f) {
        flags=flags^f;
    }

    inline bool getFlag(unsigned int f) const {
        if(flags&f) {
            return true;
        } else {
            return false;
        }
    }

private:
    unsigned int flags;
};

#endif
