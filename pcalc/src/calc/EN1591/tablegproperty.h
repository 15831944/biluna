﻿#ifndef TABLEGPROPERTY_H
#define TABLEGPROPERTY_H

#include <vector>
#include "gasket.h"
#include "rb_namespace.h"

NAMESPACE_REDBAG_CALC_EN1591

class Q0min_m_Property {

public:
    Q0min_m_Property(Gasket::InsFilLayMatType insFillMat,
                     double Q0min,
                     double m);

    Gasket::InsFilLayMatType mInsertFillMaterial;
    double mQ0min;
    double mM;
};

/**
 * The EN1591 table G.1 from Annex G with sealing parameters when
 * no leage rate is specified.
 * - Q0min [MPa]
 * - m
 */
class TableGProperty {

public:
    TableGProperty();
    virtual ~TableGProperty();

    double getTableG_Q0min(Gasket::InsFilLayMatType insType);
    double getTableG_m(Gasket::InsFilLayMatType insType);
private:
    void CreateList();
    Q0min_m_Property* getPropertyObject(Gasket::InsFilLayMatType insType);

    std::vector<Q0min_m_Property*> mList;
};

END_NAMESPACE_REDBAG_CALC_EN1591
#endif //TABLEGPROPERTY_H
