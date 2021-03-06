/*****************************************************************
 * $Id: acc_assetattribute.h 1473 2011-10-24 17:16:18Z rutger $
 * Created: Oct 24, 2011 1:24:25 PM - rutger
 *
 * Copyright (C) 2011 Red-Bag. All rights reserved.
 * This file is part of the Biluna ACC project.
 *
 * See http://www.red-bag.com for further details.
 *****************************************************************/

#ifndef ACC_ASSETATTRIBUTE_H
#define ACC_ASSETATTRIBUTE_H

#include "rb_objectcontainer.h"

/**
 * Class for asset attribute such as rentable area
 */
class ACC_AssetAttribute : public RB_ObjectContainer {

public:
    ACC_AssetAttribute(const RB_String& id = "", RB_ObjectBase* p = NULL,
                const RB_String& n = "", RB_ObjectFactory* f = NULL);
    ACC_AssetAttribute(ACC_AssetAttribute* branch);
    virtual ~ACC_AssetAttribute();

private:
    void createMembers();

};

#endif // ACC_ASSETATTRIBUTE_H
