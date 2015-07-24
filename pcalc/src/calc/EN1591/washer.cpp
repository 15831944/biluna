﻿#include "washer.h"
NAMESPACE_REDBAG_CALC_EN1591

Washer_IN::Washer_IN(RB_ObjectContainer* inputOutput) : RB_Report(inputOutput) {
    eW = 0;
    dW1 = 0;
    dW2 = 0;
}

Washer_IN::~Washer_IN() {
    // nothing
}

Washer_OUT::Washer_OUT(RB_ObjectContainer* inputOutput)
            : Washer_IN(inputOutput) {
    bW = 0.0;
    dW = 0.0;
    XW = 0.0;
    dK1 = 0.0;
    dK2 = 0.0;
    bKB = 0.0;
}

bool Washer::isPresent() {
    return eW > 0 && dW1 > 0 && dW2 > dW1;
}

/**
 * @brief Formula 44: Effective width of a washer
 */
void Washer::Calc_bW() {
    if (isPresent())     {
        bW = 0.5 * (dW2 - dW1);
        addDetail("Formula 44", "bW", "0.5 * (dW2 - dW1)", bW, "mm");
    }
}

/**
 * @brief Formula 45: Average diameter of washer
 */
void Washer::Calc_dW() {
    if (isPresent())     {
        dW = 0.5 * (dW2 + dW1);
        addDetail("Formula 45", "dW", "0.5 * (dW2 + dW1)", dW, "mm");
    }
}

/**
 * @brief Formula 48: Washer contact width bolt side
 */
void Washer::Calc_bKB() {
    if (isPresent())     {
        bKB = (dK2 - dW1) / 2;
        addDetail("Formula 48", "bKB", "(dK2 - dW1) / 2", bKB, "mm");
    }
}

END_NAMESPACE_REDBAG_CALC_EN1591
