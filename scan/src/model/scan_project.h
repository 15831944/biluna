/*****************************************************************
 * $Id: scan_project.h 1401 2011-04-27 20:58:49Z rutger $
 * Created: 2011-04-17 rutger
 *
 * Copyright (C) 2011 Red-Bag. All rights reserved.
 * This file is part of the Biluna SCAN project.
 *
 * See http://www.red-bag.com for further details.
 *****************************************************************/

#ifndef SCAN_PROJECT
#define SCAN_PROJECT

#include "rb_objectcontainer.h"

/**
 * Root class for the SCAN model
 */
class SCAN_Project : public RB_ObjectContainer {

public:
    SCAN_Project(const RB_String& id = "", RB_ObjectBase* p = NULL,
                const RB_String& n = "", RB_ObjectFactory* f = NULL);
    SCAN_Project(SCAN_Project* project);
    virtual ~SCAN_Project();

private:
    void createMembers();

};

#endif // SCAN_PROJECT
