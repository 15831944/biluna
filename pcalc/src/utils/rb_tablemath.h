﻿/*****************************************************************
 * $Id: rb_tablemath.h 2227 2015-03-09 20:08:03Z rutger $
 * Created: Mar 4, 2015 8:50:42 AM - rutger
 *
 * Copyright (C) 2015 Red-Bag. All rights reserved.
 * This file is part of the Biluna PENG project.
 *
 * See http://www.red-bag.com for further details.
 *****************************************************************/

#ifndef RB_TABLEMATH_H
#define RB_TABLEMATH_H

#include "rb_namespace.h"

NAMESPACE_REDBAG_CALC

/**
 * Base class for table math such as interpolation. To be inherited
 * by calculation or utility.
 */
class RB_TableMath {

public:
    RB_TableMath();
    virtual ~RB_TableMath();

    double getBilinearValue(double valX, double valY,
                            double p11X, double p11Y, double p11val,
                            double p12X, double p12Y, double p12val,
                            double p21X, double p21Y, double p21val,
                            double p22X, double p22Y, double p22val);
};

END_NAMESPACE_REDBAG_CALC

#endif //RB_TABLEMATH_H