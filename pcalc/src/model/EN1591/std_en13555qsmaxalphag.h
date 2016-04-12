/*****************************************************************
 * $Id: std_en13555pqrdeltaegc.h 0001 2016-04-124T16:53:06 rutger $
 *
 * Copyright (C) 2016 Red-Bag. All rights reserved.
 * This file is part of the Biluna PCALC project.
 *
 * See http://www.biluna.com for further details.
 *****************************************************************/

#ifndef STD_EN13555QSMAXALPHAG_H
#define STD_EN13555QSMAXALPHAG_H

#include "rb_objectatomic.h"

/**
 * EN13555 gasket properties Qsmax and alphaG
 */
class STD_EN13555QsmaxAlphaG : public RB_ObjectAtomic {

public:
    STD_EN13555QsmaxAlphaG(const QString& id = "", RB_ObjectBase* p = NULL,
               const QString& n = "", RB_ObjectFactory* f = NULL);
    STD_EN13555QsmaxAlphaG(STD_EN13555QsmaxAlphaG* obj);
    virtual ~STD_EN13555QsmaxAlphaG();

private:
    void createMembers();

};

#endif /*STD_EN13555QSMAXALPHAG_H*/
