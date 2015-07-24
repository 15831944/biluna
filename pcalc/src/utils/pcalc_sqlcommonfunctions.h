/*****************************************************************
 * $Id: pcalc_sqlcommonfunctions.h 1836 2012-12-17 16:29:14Z rutger $
 * Created: Mar 19, 2012 8:14:42 PM - rutger
 *
 * Copyright (C) 2012 Red-Bag. All rights reserved.
 * This file is part of the Plant Engineering (PENG) project.
 *
 * See http://www.red-bag.com for further details.
 *****************************************************************/

#ifndef PCALC_SQLCOMMONFUNCTIONS_H
#define PCALC_SQLCOMMONFUNCTIONS_H

#include "rb_sqlcommonfunctions.h"
#include "rb_stringlist.h"
#include "rb_variant.h"

class QSqlQuery;


/**
 * Class with common sql functions for getting and setting frequently used data
 * such as document ID
 */
class PCALC_SqlCommonFunctions : public RB_SqlCommonFunctions {

public:
    PCALC_SqlCommonFunctions();
    virtual ~PCALC_SqlCommonFunctions();

    virtual RB_Variant selectFromWhere(const RB_String& field,
                                       const RB_String& table,
                                       const RB_String& whereStr);


};
#endif // PCALC_SQLCOMMONFUNCTIONS_H
