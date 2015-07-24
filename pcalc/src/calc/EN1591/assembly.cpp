﻿#include "assembly.h"
NAMESPACE_REDBAG_CALC_EN1591


Assembly_IN::Assembly_IN(RB_ObjectContainer* inputOutput)
            : RB_Report(inputOutput) {
    setName("PCALC EN1591 Assembly");

    mBolt = NULL;
    mGasket = NULL;
    mFlange1 = NULL;
    mFlange2 = NULL;
    mLoadCaseList = NULL;
    mNR = 0.0;
}

Assembly_IN::~Assembly_IN() {
    delete mBolt;
    delete mGasket;
    delete mFlange1;
    delete mFlange2;
    delete mLoadCaseList;
}

Assembly_OUT::Assembly_OUT(RB_ObjectContainer* inputOutput)
            : Assembly_IN(inputOutput) {
    mPhiMax = 0.0;
}

Assembly::Assembly(RB_ObjectContainer *inputOutput)
        : Assembly_OUT(inputOutput) {
    // nothing
}

/**
 * @brief Formula 54: Initial gasket force
 * Bolt area * allowable design stress bolt / by scatter factor -
 * net equivalent external force also sets loadcase 0 F_G.
 * Different from EN13445 Appendix GA
 * @param loadCaseNo
 */
void Assembly::Calc_F_GInitial(int loadCaseNo) {
    LoadCase* loadCase = mLoadCaseList->at(loadCaseNo);

    if (loadCase->F_Bspec > 0)     {
        loadCase->F_G = loadCase->F_Bspec
                * (1 - mBolt->etanminus) - loadCase->F_R;
        addDetail("Formula 54", "F_G", "F_Bspec * (1 - etanminus) - F_R",
                  loadCase->F_G, "N");
    } else {
        loadCase->F_G = (mBolt->AB * loadCase->fB / 3 - loadCase->F_R);
        addDetail("Formula 54", "F_G", "AB * fB / 3 - F_R", loadCase->F_G, "N");
    }
}

/**
 * @brief Formula 68 71 73 76 in Table 1: Effective gasket diameter
 * @param loadCaseNo
 * @param gasket
 */
void Assembly::Calc_dGe(int loadCaseNo, Gasket *gasket)
{
    double tmpdGe = 0.0;

    if (gasket->frmType == Gasket::Flat) {
        tmpdGe = gasket->dG2 - mLoadCaseList->at(loadCaseNo)->bGe;
        addDetail("Formula 68", "dGe", "dG2 - bGe", tmpdGe, "mm");
    } else if (gasket->frmType == Gasket::CurvedSimpleContact) {
        tmpdGe = gasket->dG0;
        addDetail("Formula 71", "dGe", "dG0", tmpdGe, "mm");
    } else if (gasket->frmType == Gasket::CurvedDoubleContact) {
        tmpdGe = gasket->dGt;
        addDetail("Formula 76", "dGe", "dGt", tmpdGe, "mm");
    } else if (gasket->frmType == Gasket::OctagonalDoubleContact) {
        tmpdGe = gasket->dGt;
        addDetail("Formula 73", "dGe", "dGt", tmpdGe, "mm");
    }

    // because dGe is not recalculated
    for (std::vector<LoadCase*>::iterator it = mLoadCaseList->begin();
                it != mLoadCaseList->end(); it++) {
        (*it)->dGe = tmpdGe;
    }
}

/**
 * @brief Formula 64 - 76, Table 1: Effective gasket geometry
 * @param loadCaseNo
 * @param isFirstApproximation
 */
void Assembly::Calc_bGi(int loadCaseNo, bool isFirstApproximation) {
    LoadCase* loadCase0 = mLoadCaseList->at(0);
    LoadCase* loadCaseI = mLoadCaseList->at(loadCaseNo);

    if (isFirstApproximation) {
        if (mGasket->frmType == Gasket::Flat) {
            mGasket->bGi = mGasket->bGt;
            addDetail("Formula 64", "bGi", "bGt", mGasket->bGi, "mm");
        } else if (mGasket->frmType == Gasket::CurvedSimpleContact) {
            mGasket->bGi = pow((6 * mGasket->r2 * cos(mGasket->phiG)
                                * mGasket->bGt * loadCaseI->Q_smax
                                / loadCase0->E_G), 0.5);
            addDetail("Formula 69",
                      "bGi", "(6 * r2 * Cos(phiG) * bGt * Q_smax / E_G) ^ 0.5",
                      mGasket->bGi, "mm");
        } else if (mGasket->frmType == Gasket::CurvedDoubleContact) {
            mGasket->bGi = pow((12 * mGasket->r2 * cos(mGasket->phiG)
                                * mGasket->bGt * loadCaseI->Q_smax
                                / loadCase0->E_G), 0.5);
            addDetail("Formula 74",
                      "bGi", "(12 * r2 * Cos(phiG) * bGt * Q_smax / E_G) ^ 0.5",
                      mGasket->bGi, "mm");
        } else if (mGasket->frmType == Gasket::OctagonalDoubleContact) {
            mGasket->bGi = mGasket->bGiOct;
            addDetail("Formula 72", "bGi", "bGiOct", mGasket->bGi, "mm");
        }
    } else {
        double bGp = loadCase0->F_G / (M_PI *loadCaseI->dGe
                                       * loadCaseI->Q_smax);
        if (mGasket->frmType == Gasket::Flat) {
            if (!mGasket->isMetalic()) {
                loadCaseI->EGm = 0.5 * loadCaseI->E_G;
            } else {
                loadCaseI->EGm = loadCaseI->E_G;
            }

            mGasket->bGi =
                    pow((loadCaseI->eG
                          / (M_PI * loadCaseI->dGe * loadCaseI->EGm))
                         / (loadCase0->hG1 * mFlange1->ZF / loadCase0->EF1
                             + loadCase0->hG2 * mFlange2->ZF / loadCase0->EF2)
                            + pow(bGp, 2), 0.5);
            addDetail("Formula 65",
                      "bGi", "((eG / (PI * dGe * EGm)) / (hG1 * Flange1.ZF "
                      "/ EF1 + hG2 * Flange2.ZF / EF2) + (F_G / (PI * dGe "
                      "* Q_smax)) ^ 2) ^ 0.5", mGasket->bGi, "mm",

                      "((" + qn(loadCaseI->eG)
                      + " / (PI * " + qn(loadCaseI->dGe)
                      + " * " + qn(loadCaseI->EGm) + ")) / ("
                      + qn(loadCase0->hG1) + " * " + qn(mFlange1->ZF) + " / " + qn(loadCase0->EF1) + " + "
                      + qn(loadCase0->hG2) + " * " + qn(mFlange2->ZF) + " / " + qn(loadCase0->EF2) + ") + ("
                      + qn(loadCase0->F_G) + " / (PI *" + qn(loadCaseI->dGe) + " * " + qn(loadCaseI->Q_smax) +
                      ")) ^ 2) ^ 0.5");

        } else if (mGasket->frmType == Gasket::CurvedSimpleContact) {
            //loadCase0->E_G = loadCase0->E0 + loadCaseI->K1 * loadCaseI->Qsmax

            mGasket->bGi =
                    pow((6 * mGasket->r2 * cos(mGasket->phiG) * loadCase0->F_G)
                         / (M_PI * loadCaseI->dGe * loadCase0->E_G)
                        + pow(bGp, 2), 0.5);
            addDetail("Formula 70",
                      "bGi", "((6 * r2 * Cos(phiG) * F_G) "
                      "/ (PI * dGe * E_G) + (F_G / (PI * dGe * Q_smax)) "
                      "^ 2) ^ 0.5", mGasket->bGi, "mm");
        } else if (mGasket->frmType == Gasket::CurvedDoubleContact) {
            //loadCase0->E_G = loadCase0->E0 + loadCaseI->K1 * loadCaseI->Qsmax

            mGasket->bGi =
                    pow((12 * mGasket->r2 * cos(mGasket->phiG) * loadCase0->F_G)
                         / (M_PI * loadCaseI->dGe * loadCaseI->E_G)
                         + pow(bGp, 2), 0.5);
            addDetail("Formula 75",
                      "bGi", "((12 * r2 * Cos(phiG) * F_G) / (PI * dGe * E_G) "
                      "+ (F_G / (PI * dGe * Q_smax)) ^ 2) ^ 0.5",
                      mGasket->bGi, "mm");
        } else if (mGasket->frmType == Gasket::OctagonalDoubleContact) {
            mGasket->bGi = mGasket->bGiOct;
            addDetail("Formula 72", "bGi", "bGiOct", mGasket->bGi, "mm");
        }
    }
}

/**
 * @brief Formula 57: Initial gasket stress at assembly
 * @param loadCaseNo
 */
void Assembly::Calc_Q_G(int loadCaseNo) {
    // AGe is not recalculated by loadcase
    mLoadCaseList->at(loadCaseNo)->Q_G =
            mLoadCaseList->at(0)->F_G / mLoadCaseList->at(0)->AGe;
    addDetail("Formula 57", "Q_G", "F_G / AGe",
              mLoadCaseList->at(loadCaseNo)->Q_G, "N/mm^2");
}

/**
 * @brief Formula 91: Axial pressure force
 * @param loadCaseNo
 */
void Assembly::Calc_F_Q(int loadCaseNo) {
    mLoadCaseList->at(loadCaseNo)->F_Q = mLoadCaseList->at(loadCaseNo)->P
            * mLoadCaseList->at(loadCaseNo)->AQ;
    addDetail("Formula 91", "F_Q", "P * AQ",
              mLoadCaseList->at(loadCaseNo)->F_Q, "N");
}

/**
 * @brief Formula 96: Equivalent resulting net force.
 * On side where moment induces a tensile load, the load limit of flange,
 * bolt or minimum gasket compression may govern.
 * On side where moment induces a compression load, the load limit
 * of the gasket may govern.
 * \todo: effect of compression or tensile not yet further implemented
 * @param loadCaseNo
 */
void Assembly::Calc_F_R(int loadCaseNo) {
    LoadCase* loadCase = mLoadCaseList->at(loadCaseNo);
    loadCase->F_Rpositive = loadCase->mForce->F_Z + loadCase->mForce->M_AI
            * 4 / mFlange1->d3e;
    loadCase->F_Rnegative = loadCase->mForce->F_Z - loadCase->mForce->M_AI
            * 4 / mFlange1->d3e;

    // Set calculation value
    if (fabs(loadCase->F_Rnegative) > fabs(loadCase->F_Rpositive)) {
        loadCase->F_R = loadCase->F_Rnegative;
        addDetail("Formula 96",
                  "F_Rnegative", "F_Z - M_AI * 4 / d3e",
                  loadCase->F_Rnegative, "N");
    } else {
        loadCase->F_R = loadCase->F_Rpositive;
        addDetail("Formula 96",
                  "F_Rpositive", "F_Z + M_AI * 4 / d3e",
                  loadCase->F_Rpositive, "N");
    }
}

/**
 * @brief Formula 97: Difference in thermal expansion bolt
 * and assembly (washers, gaskets, flanges)
 * @param loadCaseNo
 */
void Assembly::Calc_dUI(int loadCaseNo) {
    double tmp_T0 = mLoadCaseList->at(0)->T0;
    double tmp_eG = mLoadCaseList->at(0)->eG;
    LoadCase* loadCase = mLoadCaseList->at(loadCaseNo);

    loadCase->dUI = mBolt->lB * loadCase->alphaB * (loadCase->TB - tmp_T0)
        - tmp_eG * loadCase->alphaG * (loadCase->TG - tmp_T0)
        - mFlange1->eFt * loadCase->alphaF1 * (loadCase->TF1 - tmp_T0)
        - mFlange1->eL * loadCase->alphaL1 * (loadCase->TL1 - tmp_T0)
        - mFlange1->mWasher->eW * loadCase->alphaW1 * (loadCase->TW1 - tmp_T0)
        - mFlange2->eFt * loadCase->alphaF2 * (loadCase->TF2 - tmp_T0)
        - mFlange2->eL * loadCase->alphaL2 * (loadCase->TL2 - tmp_T0)
        - mFlange2->mWasher->eW * loadCase->alphaW2 * (loadCase->TW2 - tmp_T0);
    addDetail("Formula 97",
              "dUI", "lB * alphaB * (TB - T0) - eG * alphaG * (TG - T0) "
              "- Flange1.eFt * alphaF1 * (TF1 - T0) - Flange1.eL * alphaL1 "
              "* (TL1 - T0) - Flange1.Washer.eW * alphaW1 * (TW1 - T0) "
              "- Flange2.eFt * alphaF2 * (TF2 - T0) - Flange2.eL * alphaL2 "
              "* (TL2 - T0) - Flange2.Washer.eW * alphaW2 * (TW2 - T0)",
              loadCase->dUI, "N");
}

/**
 * @brief Formula 98: Axial bolt length
 */
void Assembly::Calc_lB() {
    mBolt->lB = mGasket->eGt
            + mFlange1->eFt + mFlange2->eFt
            + mFlange1->eRF + mFlange2->eRF
            + mFlange1->eL + mFlange2->eL
            + mFlange1->mWasher->eW + mFlange2->mWasher->eW;
    addDetail("Formula 98",
              "lB", "eGt + Flange1.eFt + Flange2.eFt + Flange1.eRF + Flange2.eRF "
              "+ Flange1->eL + Flange2->eL + Flange1.Washer.eW + Flange2.Washer.eW",
              mBolt->lB, "mm");
}

/**
 * @brief Formula 99: Axial compliances of the joint (mm/N)
 * corresponding to load F_B
 * @param loadCaseNo
 */
void Assembly::Calc_YB(int loadCaseNo) {
    double tmpFlange1Val = 0.0;
    double tmpFlange2Val = 0.0;
    LoadCase* loadCase = mLoadCaseList->at(loadCaseNo);

    if (dynamic_cast<Flange_Loose*>(mFlange1) != NULL) {
        tmpFlange1Val = mFlange1->ZL * (pow(loadCase->hL1, 2))
                / loadCase->EL1;
    }

    if (dynamic_cast<Flange_Loose*>(mFlange2) != NULL) {
        tmpFlange2Val = mFlange2->ZL * (pow(loadCase->hL2, 2))
                / loadCase->EL2;
    }

    loadCase->Y_B = tmpFlange1Val + tmpFlange2Val + mBolt->XB / loadCase->EB;

    if (mFlange1->mWasher->XW > 0 && loadCase->EW1 > 0) {
        loadCase->Y_B += mFlange1->mWasher->XW / loadCase->EW1;
    }

    if (mFlange2->mWasher->XW > 0 && loadCase->EW2 > 0) {
        loadCase->Y_B += mFlange2->mWasher->XW / loadCase->EW2;
    }

    addDetail("Formula 99", "Y_B", "ZL1 * (hL1 ^ 2) "
              "/ EL1 + ZL2 * (hL2 ^ 2) / EL2 "
              "+ XB / EB + XW1 / EW1 + XW2 / EW2",
              loadCase->Y_B, "mm/N",
              qn(mFlange1->ZL) + " * " + qn(loadCase->hL1) + "^2 / " + qn(loadCase->EL1) + " + "
              + qn(mFlange2->ZL) + " * " + qn(loadCase->hL2) + "^2 / " + qn(loadCase->EL2) + " + "
              + qn(mBolt->XB) + " / " + qn(loadCase->EB) + " + "
              + qn(mFlange1->mWasher->XW) + " / " + qn(loadCase->EW1) + " + "
              + qn(mFlange2->mWasher->XW) + " / " + qn(loadCase->EW2));
}

/**
 * @brief Formula 100: Axial compliances of the joint (mm/N)
 * corresponding to load F_G
 * @param loadCaseNo
 */
void Assembly::Calc_YG(int loadCaseNo) {
    LoadCase* loadCase = mLoadCaseList->at(loadCaseNo);
    loadCase->Y_G = mFlange1->ZF * (pow(loadCase->hG1, 2)) / loadCase->EF1
            + mFlange2->ZF * (pow(loadCase->hG2, 2)) / loadCase->EF2
            + loadCase->Y_B + mGasket->XG / loadCase->E_G;
    addDetail("Formula 100", "Y_G", "Flange1.ZF * (hG1 ^ 2) / EF1 "
              "+ Flange2.ZF * (hG2 ^ 2) / EF2 + Y_B + XG / E_G",
              loadCase->Y_G, "mm/N",
              qn(mFlange1->ZF) + " * (" + qn(loadCase->hG1) + "^2) / " + qn(loadCase->EF1) + " + "
              + qn(mFlange2->ZF) + " * (" + qn(loadCase->hG2) + "^2) / " + qn(loadCase->EF2) + " + "
              + qn(loadCase->Y_B) + " + " + qn(mGasket->XG) + " / " + qn(loadCase->E_G));
}

/**
 * @brief Formula 101: Axial compliances of the joint (mm/N)
 * corresponding to load F_Q
 * @param loadCaseNo
 */
void Assembly::Calc_YQ(int loadCaseNo) {
    LoadCase* loadCase = mLoadCaseList->at(loadCaseNo);
    loadCase->Y_Q = mFlange1->ZF * loadCase->hG1
            * (loadCase->hH1 - mFlange1->hP + mFlange1->hQ) / loadCase->EF1
            + mFlange2->ZF * loadCase->hG2
            * (loadCase->hH2 - mFlange2->hP + mFlange2->hQ) / loadCase->EF2
            + loadCase->Y_B;
    addDetail("Formula 101", "Y_Q", "Flange1.ZF * hG1 "
              "* (hH1 - Flange1.hP + Flange1.hQ) / EF1 "
              "+ Flange2.ZF * hG2 "
              "* (hH2 - Flange2.hP + Flange2.hQ) / EF2 + Y_B",
              loadCase->Y_Q, "mm/N",
              qn(mFlange1->ZF) + " * " + qn(loadCase->hG1) + " * ("
              + qn(loadCase->hH1) + " - " + qn(mFlange1->hP) + " + " + qn(mFlange1->hQ) + ") / " + qn(loadCase->EF1) + " + "
              + qn(mFlange2->ZF) + " * " + qn(loadCase->hG2) + " * ("
              + qn(loadCase->hH2) + " - " + qn(mFlange2->hP) + " + " + qn(mFlange2->hQ) + ") / " + qn(loadCase->EF2) + " + "
              + qn(loadCase->Y_B));
}

/**
 * @brief Formula 102: Axial compliances of the joint (mm/N)
 * corresponding to load F_R
 * @param loadCaseNo
 */
void Assembly::Calc_YR(int loadCaseNo) {
    LoadCase* loadCase = mLoadCaseList->at(loadCaseNo);
    loadCase->Y_R = mFlange1->ZF * loadCase->hG1
            * (loadCase->hH1 + mFlange1->hR) / loadCase->EF1
            + mFlange2->ZF * loadCase->hG2
            * (loadCase->hH2 + mFlange2->hR) / loadCase->EF2 + loadCase->Y_B;
    addDetail("Formula 102", "Y_R", "Flange1.ZF * hG1 * (hH1 + Flange1.hR) / EF1"
              " + Flange2.ZF * hG2 * (hH2 + Flange2.hR) / EF2 + Y_B",
              loadCase->Y_R, "mm/N",
              qn(mFlange1->ZF) + " * " + qn(loadCase->hG1) + " * ("
              + qn(loadCase->hH1) + " + " + qn(mFlange1->hR) + ") / " + qn(loadCase->EF1) + " + "
              + qn(mFlange2->ZF) + " * " + qn(loadCase->hG2) + " * ("
              + qn(loadCase->hH2) + " + " + qn(mFlange2->hR) + ") / " + qn(loadCase->EF2) + " + "
              + qn(loadCase->Y_B));
}

/**
 * @brief Formula 103, 104 and Annex/Table E, G: Minimum gasket force
 * in assemblage
 * NOTE: if no leakage rate is requested use Q0,min from Annex G instead of QA
 * @param loadCaseNo
 */
void Assembly::Calc_F_Gmin(int loadCaseNo) {
    LoadCase* loadCase = mLoadCaseList->at(loadCaseNo);

    if (loadCaseNo == 0)     {
        if (loadCase->Q_A > 0)         {
            loadCase->F_Gmin = loadCase->AGe * loadCase->Q_A;
            addDetail("Formula 103", "F_Gmin", "AGe * Q_A",
                      loadCase->F_Gmin, "N");
        } else {
            TableGProperty* table = new TableGProperty(); // TODO: static class or part of assembly
            double Q0min = table->getTableG_Q0min(mGasket->insType);
            loadCase->F_Gmin = loadCase->AGe * Q0min;
            addDetail("Formula 103", "F_Gmin", "AGe * Q0min",
                      loadCase->F_Gmin, "N");
        }
    } else {
        double tmpVal1 = 0;

        if (loadCase->Q_sminL > 0) {
            tmpVal1 = loadCase->AGe * loadCase->Q_sminL;
        } else {
            TableGProperty *table = new TableGProperty(); // TODO: static class or part of assembly
            double m = table->getTableG_m(mGasket->insType);
            tmpVal1 = loadCase->AGe * m * loadCase->P;
        }

        double tmpVal2 = 0;

        if (mGasket->frmType == Gasket::Flat) {
            tmpVal2 = -2 * loadCase->mForce->M_AI / mGasket->dGt;
        }

        double muG = loadCase->muG;

        if (muG <= 0) {
            TableEProperty* table = new TableEProperty(); // TODO: static class or part of assebly
            muG = table->getTableE_muG(mGasket->insType);
        }

        tmpVal2 += loadCase->mForce->F_LI / muG
                + 2 * loadCase->mForce->M_Z / (muG * mGasket->dGt);

        double tmpVal3 = -(loadCase->F_Q + loadCase->F_R);
        loadCase->F_Gmin = std::max(std::max(tmpVal1, tmpVal2), tmpVal3);
        addDetail("Formula 104",
                  "F_Gmin", "Max(Max(AGe * Q_sminL or AGe * m * P, "
                  "F_LI / muG + 2 * M_Z / (muG * dGt) - 2 * M_AI / dGt), "
                  "F_Q + F_R)",
                  loadCase->F_Gmin, "N");
    }
}

/**
 * @brief Before Formula 105 F.2 and F.3: Creep of gasket
 * under seating pressure and temperature
 * \todo: check whether this approach is correct, formulae are
 * from the code and no additional interpretation is added
 * however it is not sure which data is available from the gasket
 * @param loadCaseNo
 */
void Assembly::Calc_delta_eGc(int loadCaseNo) {
    LoadCase* loadCase = mLoadCaseList->at(loadCaseNo);

    if (mGasket->K > 0 && loadCase->delta_eGc_EN13555 > 0) {
        // F.3
        // TODO annex F and gasketdata.org
        loadCase->delta_eGc = mGasket->K * loadCase->Y_G
                * loadCase->delta_eGc_EN13555;
        addDetail("Before Formula 105 F.3",
                  "delta_eGc", "K * Y_G * delta_eGc_EN13555",
                  loadCase->delta_eGc, "mm");
    } else if (mGasket->dG2_EN13555 > 0
               && mGasket->dG1_EN13555 > 0
               && loadCase->P_QR > 0) {
        // F.2, text and F.3
        loadCase->delta_eGc = loadCase->Y_G * (M_PI / 4)
                * (pow(mGasket->dG2_EN13555, 2) - pow(mGasket->dG1_EN13555, 2))
                * loadCase->Q_A * (1 - loadCase->P_QR);
        addDetail("Before Formula 105 F.2 F.3", "delta_eGc", "Y_G * (PI / 4) "
                  "* (dG2_EN13555 ^ 2 - dG1_EN13555 ^ 2) * Q_A * (1 - P_QR)",
                  loadCase->delta_eGc, "mm");
    } else {
        // no further gasket information available
        loadCase->delta_eGc = 0;
        addDetail("Before Formula 105", "delta_eGc", "0",
                  loadCase->delta_eGc, "mm");
    }
}

/**
 * @brief Formula 105: Required gasket force based on load conditions
 * \todo: delta_eGc is zero and not calculated yet should be based on Pqr?
 * \todo: consider Tuckmantel Formula refer below
 * @param loadCaseNo
 */
void Assembly::Calc_F_Gdelta(int loadCaseNo) {
    if (loadCaseNo == 1) {
        // first time formula 105 is called
        mLoadCaseList->at(0)->F_Gdelta = 0.0;
    }

    double tmpF_Gdelta = 0.0;
    LoadCase* loadCase = mLoadCaseList->at(loadCaseNo);
    tmpF_Gdelta = (loadCase->F_Gmin * loadCase->Y_G
                   + loadCase->F_Q * loadCase->Y_Q
                   + (loadCase->F_R * loadCase->Y_R
                      - mLoadCaseList->at(0)->F_R * mLoadCaseList->at(0)->Y_R)
                   + loadCase->dUI + loadCase->delta_eGc)
            / mLoadCaseList->at(0)->Y_G;
    mLoadCaseList->at(0)->F_Gdelta =
            std::max(mLoadCaseList->at(0)->F_Gdelta, tmpF_Gdelta);

    // Tuckmantel suggestion:
    //tmpF_Gdelta = (.F_Gmin * .Y_G + .F_Q * .Y_Q _
    //               + (.F_R * .Y_R - mLoadCaseList->at(0)->F_R _
    //               * mLoadCaseList->at(0)->Y_R) + .delta_eGc + .dUI) _
    //           / (mLoadCaseList->at(0)->Y_G * .Pqr)

    addDetail("Formula 105",
              "tmpF_Gdelta", "(F_Gmin * Y_G + F_Q * Y_Q "
              "+ (F_R * Y_R - F_R * Y_R) + dUI + delta_eGc) / Y_G",
              tmpF_Gdelta, "N");
    addDetail("Formula 105",
              "F_Gdelta", "Max(F_Gdelta, tmpF_Gdelta)",
              mLoadCaseList->at(0)->F_Gdelta, "N");
}

/**
 * @brief Formula 107: Required gasket force including tightness requirements
 */
void Assembly::Calc_F_G0req() {
    LoadCase* loadCase = mLoadCaseList->at(0);
    loadCase->F_Greq = std::max(loadCase->F_Gmin, loadCase->F_Gdelta);
    addDetail("Formula 107", "F_Greq", "Math.Max(F_Gmin, F_Gdelta))",
              loadCase->F_Greq, "N");
}

/**
 * @brief Formula 108: Required bolt load in assemblage
 */
void Assembly::Calc_F_B0req() {
    LoadCase* loadCase = mLoadCaseList->at(0);
    loadCase->F_Breq = loadCase->F_Greq + loadCase->F_R;
    addDetail("Formula 108", "F_Breq", "F_Greq + F_R", loadCase->F_Breq, "N");
}

/**
 * @brief Formula 109: Assess F_G0actual versus F_G0required
 * @returns true if F_G0 larger F_G0req
 */
bool Assembly::Is_F_G0_larger_F_G0req() {
    bool result = false;
    LoadCase* loadCase = mLoadCaseList->at(0);
    result = loadCase->F_G > loadCase->F_Greq;
    addDetail("Formula 109", "result", "F_G > F_Greq",
              static_cast<int>(result), "-");
    return result;
}

/**
 * @brief Formula 110: F_G0actual larger F_G0required but within 0.1%
 * @returns true if F_G0actual larger F_G0required but within 0.1%
 */
bool Assembly::Is_F_G0act_within_0_1_percent_of_F_G0req() {
    bool result = false;
    LoadCase* loadCase = mLoadCaseList->at(0);
    result = loadCase->F_Greq < loadCase->F_G
            && loadCase->F_G < loadCase->F_Greq * 1.001; // 0.1%
    addDetail("Formula 110",
              "result", "F_Greq < F_G And F_G < F_Greq * 1.001",
              static_cast<int>(result), "-");
    return result;
}

/**
 * @brief Formula B.3: ManualOperatorFeel uncontrolled tightning
 */
void Assembly::Calc_F_B0av() {
    if (! (mBolt->tType == Bolt::ManualStandardRing)) {
        return;
    }

    LoadCase* loadCase = mLoadCaseList->at(0);
    loadCase->F_Bav = std::min(mBolt->AB * loadCase->fB,
                               (double)mFlange1->nB * 200000);
    addDetail("Formula B.3",
              "F_Bav", "Min(AB * fB, Flange1.nB * 200000)",
              loadCase->F_Bav, "N");
}

/**
 * @brief Formula 112 113 117: Required nominal bolt force,
 * F_Bspec is set by the user
 */
void Assembly::Calc_F_B0nom() {
    LoadCase* loadCase = mLoadCaseList->at(0);

    if (loadCase->F_Bspec > 0.0) {
        // User specified nominal bolt load
        loadCase->F_Bnom = loadCase->F_Bspec;
        addDetail("With Formula 111", "F_Bnom", "F_Bspec",
                  loadCase->F_Bnom, "N");
        loadCase->F_Bmin = loadCase->F_Bnom * (1 - mBolt->etanminus);
        addDetail("With Formula 111", "F_Bmin", "F_Bnom * (1 - etanminus)",
                  loadCase->F_Bmin, "N");
        loadCase->F_Bmax = loadCase->F_Bnom * (1 + mBolt->etanplus);
        addDetail("With Formula 111", "F_Bmax", "F_Bnom * (1 + etanplus)",
                  loadCase->F_Bmax, "N");
    } else if (! (mBolt->tType == Bolt::ManualStandardRing)) {
        // Use required bolt as starting point
        loadCase->F_Bnom = loadCase->F_Breq / (1 - mBolt->etanminus);
        addDetail("Formula 115", "F_Bnom", "F_Breq", loadCase->F_Bnom, "N");
        loadCase->F_Bmin = loadCase->F_Breq;
        addDetail("Formula 114", "F_Bmin", "F_Bnom * (1 - etanminus)",
                  loadCase->F_Bmin, "N");
        loadCase->F_Bmax = loadCase->F_Bnom * (1 + mBolt->etanplus);
        addDetail("Formula 117", "F_Bmax", "F_Bnom * (1 + etanplus)",
                  loadCase->F_Bmax, "N");
    } else {
        // Use uncontrolled standard ring as per formula B.3
        double tmp_etanminus = 0.5 *(1 + 3 / (pow(mFlange1->nB, 0.5))) / 4;
        loadCase->F_Bav = loadCase->F_Breq / (1 - tmp_etanminus);
        loadCase->F_Bnom = loadCase->F_Bav;
        addDetail("Formula B.3", "F_Bnom=F_Bav", "F_Breq / (1 - tmp_etanminus)",
                  loadCase->F_Bnom, "N");
        loadCase->F_Bmin = loadCase->F_Bnom * (1 - tmp_etanminus);
        addDetail("Formula 112", "F_Bmin", "F_Bnom * (1 - tmp_etanminus)",
                  loadCase->F_Bmin, "N");
        loadCase->F_Bmax = loadCase->F_Bnom * (1 + mBolt->etanplus);
        addDetail("Formula 113", "F_Bmax", "F_Bnom * (1 + tmp_etanplus)",
                  loadCase->F_Bmax, "N");
    }
}

/**
 * @brief Formula 111 114 115 116: Is F_B0nom greater or equal
 * to F_Bspec, F_Breq and F_Bav
 * @returns true if OK
 */
bool Assembly::Is_F_B0nom_Valid() {
    bool result = false;
    LoadCase* loadCase = mLoadCaseList->at(0);
    result = loadCase->F_Bmin < loadCase->F_Bnom
            && loadCase->F_Bnom < loadCase->F_Bmax; // 111
    addDetail("Formula 111", "result",
              "F_Bmin < F_Bnom And F_Bnom < F_Bmax",
              static_cast<int>(result), "-");

    if (loadCase->F_Bspec > 0.0
            || ! (mBolt->tType == Bolt::ManualStandardRing)) {
        result = result
                && loadCase->F_Bnom >= loadCase->F_Breq
                / (1 - mBolt->etanminus); // 115
        addDetail("Formula 115", "result",
                  "F_Bmin < F_Bnom AND F_Bnom < F_Bmax "
                  "AND F_Bnom >= F_Breq / (1 - etanminus)",
                  static_cast<int>(result), "-");
    } else {
        double tmp_etanminus = 0.5 *(1 + 3 / (pow(mFlange1->nB, 0.5))) / 4;
        result = result && loadCase->F_Bnom >= loadCase->F_Breq
                / (1 - tmp_etanminus); // 116
        addDetail("Formula 116", "result",
                  "F_Bnom >= F_Breq / (1 - tmp_etanminus) "
                  "And .F_Bnom >= F_Breq / (1 - 0.5 * (1 + 3 "
                  "/ (Flange1.nB ^ 0.5)) / 4)",
                  static_cast<int>(result), "-");
    }

    result = result && loadCase->F_Bmin >= loadCase->F_Breq; // 114
    addDetail("Formula 114", "result",
              "111 AND (115 OR 116) And .F_Bmin >= .F_Breq",
              static_cast<int>(result), "-");

    return result;
}

/**
 * @brief Formula 118: Maximum gasket load for load limit calculation
 */
void Assembly::Calc_F_G0max() {
    LoadCase* loadCase = mLoadCaseList->at(0);
    loadCase->F_Gmax = loadCase->F_Bmax - loadCase->F_R;
    addDetail("Formula 118", "F_Gmax", "F_Bmax - F_R", loadCase->F_Gmax, "N");
}

/**
 * @brief Formula 119: Increased gasket load due to more
 * than once assemblage (NR)
 */
void Assembly::Calc_F_G0d() {
    LoadCase* loadCase = mLoadCaseList->at(0);
    double tmpF_G = loadCase->F_Gdelta;

    if (loadCase->F_Bspec > 0.0) {
        tmpF_G = loadCase->F_Bmin - loadCase->F_R;
    }

    loadCase->F_Gd = std::max(tmpF_G, (2 / 3.0) * (1 - 10 / mNR)
                              * loadCase->F_Bmax - loadCase->F_R);
    addDetail("Formula 119 or 2",
              "F_Gd", "Max(F_Gdelta, (2 / 3) * (1 - 10 / NR) * F_Bmax - F_R)",
              loadCase->F_Gd, "N");
}

/**
 * @brief Formula 120: Increased gasket force for load limit calculation
 * @param loadCaseNo
 */
void Assembly::Calc_F_G(int loadCaseNo) {
    LoadCase* loadCase0 = mLoadCaseList->at(0);
    LoadCase* loadCaseI = mLoadCaseList->at(loadCaseNo);
    loadCaseI->F_G = (loadCase0->F_Gd * loadCase0->Y_G
                      - (loadCaseI->F_Q * loadCaseI->Y_Q
                         + (loadCaseI->F_R * loadCaseI->Y_R
                            - loadCase0->F_R * loadCase0->Y_R) + loadCaseI->dUI)
                      - loadCaseI->delta_eGc) / loadCaseI->Y_G;
    addDetail("Formula 120", "F_G", "F_Gd * Y_G - (F_Q * Y_Q + (F_R * Y_R "
              "- F_R * Y_R) + dUI) - delta_eGc) / Y_G",
              loadCaseI->F_G, "N");
}

/**
 * @brief Formula 122: Bolt force for load limit calculation
 * @param loadCaseNo
 */
void Assembly::Calc_F_B(int loadCaseNo) {
    LoadCase* loadCase = mLoadCaseList->at(loadCaseNo);
    loadCase->F_B = loadCase->F_G + (loadCase->F_Q + loadCase->F_R);
    addDetail("Formula 122", "F_B", "F_G + (F_Q + F_R)", loadCase->F_B, "N");
}

/**
 * @brief Formula B.9: Torsional moment during assembly in bolts,
 * only for loadcase = 0 with table B EN1591-2001 for ISO bolts/thread for pt
 */
void Assembly::Calc_MtB() {
    LoadCase* loadCase = mLoadCaseList->at(0);
    // from table B EN1591-2001 for ISO bolts/thread
    double pt = (mBolt->dB0 - mBolt->dBe) / 0.9382;
    addDetail("With Formula B.9", "pt", "(Bolt.dB0 - Bolt.dBe) / 0.9382",
              pt, "Nmm");
    loadCase->MtB = (0.159 * pt + 0.577 * mBolt->mut * mBolt->dB2)
            * mLoadCaseList->at(0)->F_Bnom / mFlange1->nB
            * (1 + mBolt->etanplus);
    addDetail("Formula B.9",
              "MtB", "(0.159 * pt + 0.577 * mut * dB2) "
              "* F_Bnom / Flange1.nB * (1 + etanplus)",
              loadCase->MtB, "Nmm");
}

/**
 * @brief Formula 123: Load ratio for bolts
 * @param loadCaseNo
 */
void Assembly::Calc_PhiB(int loadCaseNo) {
    double torsionVal = 0.0;

    if (loadCaseNo == 0) {
        torsionVal = 3 * pow(mLoadCaseList->at(loadCaseNo)->cA
                             * mLoadCaseList->at(0)->MtB / mBolt->IB, 2);
    }

    LoadCase* loadCase = mLoadCaseList->at(loadCaseNo);
    loadCase->PhiB = 1 / (loadCase->fB * loadCase->cB)
            * pow((pow(loadCase->F_B / mBolt->AB, 2) + torsionVal), 0.5);
    addDetail("Formula 123",
              "PhiB", "1 / (fB * cB) * ((F_B / AB) ^ 2 "
              "+ 3 * cA * MtB / IB) ^ 2) ^ 0.5",
              loadCase->PhiB, "-");
}

/**
 * @brief With Formula 123: Load ratio of bolts acceptance
 * @param loadCaseNo
 * @returns true if OK
 */
bool Assembly::Is_PhiB_Valid(int loadCaseNo) {
    bool result = mLoadCaseList->at(loadCaseNo)->PhiB <= 1.0;
    addDetail("With Formula 123", "result",
              "PhiB <= 1.0", static_cast<int>(result), "-");
    return result;
}

/**
 * @brief Formula 124 125 126: Correction factor for bolt tensioning,
 * when calculating load limits
 * @param loadCaseNo
 */
void Assembly::Calc_cA(int loadCaseNo) {
    if (mBolt->tType == Bolt::TorqueWrench && loadCaseNo == 0) {
        if (mBolt->ruptureElongationA >= 0.1)         {
            mLoadCaseList->at(loadCaseNo)->cA = 1.0;
            addDetail("Formula 124", "cA", "1.0",
                      mLoadCaseList->at(loadCaseNo)->cA, "-");
        } else {
            mLoadCaseList->at(loadCaseNo)->cA = 4 / 3.0;
            addDetail("Formula 125", "cA", "4 / 3",
                      mLoadCaseList->at(loadCaseNo)->cA, "-");
        }
    } else {
        mLoadCaseList->at(loadCaseNo)->cA = 0.0;
        addDetail("Formula 126", "cA", "0.0",
                  mLoadCaseList->at(loadCaseNo)->cA, "-");
    }
}

/**
 * @brief Formula 127: Correction factor for unusual cases
 * @param loadCaseNo
 */
void Assembly::Calc_cB(int loadCaseNo) {
    double tmp_cB1 = Calc_cB_helper(mFlange1, loadCaseNo);
    double tmp_cB2 = Calc_cB_helper(mFlange2, loadCaseNo);
    mLoadCaseList->at(loadCaseNo)->cB = std::min(tmp_cB1, tmp_cB2);
    addDetail("Formula 127",
              "cB", "Min{1.0; eN * fN / (0.8 * dB0 * fB); "
              "l5t * fF / (0.8 * dB0 * fB)}",
              mLoadCaseList->at(loadCaseNo)->cB, "-");
}

/**
 * @brief With Formula 127: Helper for correction factor for unusual cases
 * @param flange
 * @param loadCaseNo
 * @return
 */
double Assembly::Calc_cB_helper(Flange* flange, int loadCaseNo) {
    LoadCase* loadCase = mLoadCaseList->at(loadCaseNo);
    double tmp_fF = loadCase->fF1;

    if (flange->getFlangeNumber() == 2) {
        tmp_fF = loadCase->fF2;
    }

    double denom = 0.8 * mBolt->dB0 *loadCase->fB;

    if (dynamic_cast<Flange_Blind*>(flange) != NULL) {
        return std::min(1.0, std::min(mBolt->eN * loadCase->fN / denom,
                                      mBolt->l5t * tmp_fF / denom));
    } else {
        return std::min(1.0, mBolt->eN * loadCase->fN / denom);
    }
}

/**
 * @brief Formula 128: Load ratio for the gasket
 * @param loadCaseNo
 */
void Assembly::Calc_PhiG(int loadCaseNo) {
    LoadCase* loadCase = mLoadCaseList->at(loadCaseNo);
    loadCase->PhiG = loadCase->F_G / (mGasket->AGt * loadCase->Q_smax);
    addDetail("Formula 128", "PhiG", "F_G / (AGt * Q_smax)",
              loadCase->PhiG, "-");
}

/**
 * @brief With Formula 128: Load ratio of gasket acceptance
 * @param loadCaseNo
 * @returns true if OK
 */
bool Assembly::Is_PhiG_Valid(int loadCaseNo) {
    bool result = mLoadCaseList->at(loadCaseNo)->PhiG <= 1.0;
    addDetail("With Formula 128", "result", "PhiG <= 1.0",
              static_cast<int>(result), "-");
    return result;
}

/**
 * @brief Formula 131: Minimum allowable stress of flange and shell
 * @param flange
 * @param loadCaseNo
 */
void Assembly::Calc_fE(Flange *flange, int loadCaseNo) {
    LoadCase* loadCase = mLoadCaseList->at(loadCaseNo);

    if (flange->getFlangeNumber() == 1)     {
        if (flange->mShell != NULL && flange->mShell->eS > 0)         {
            loadCase->fE1 = std::min(loadCase->fF1, loadCase->fS1);
            addDetail("Formula 131", "fE1", "Min(fF1, fS1)",
                      loadCase->fE1, "N/mm2");
        } else {
            loadCase->fE1 = loadCase->fF1;
            addDetail("Formula 131", "fE1", "fF1", loadCase->fE1, "N/mm2");
        }
    } else if (flange->getFlangeNumber() == 2) {
        if (flange->mShell != NULL && flange->mShell->eS > 0) {
            loadCase->fE2 = std::min(loadCase->fF2, loadCase->fS2);
            addDetail("Formula 131", "fE2", "Min(fF2, fS2)",
                      loadCase->fE2, "N/mm2");
        } else {
            loadCase->fE2 = loadCase->fF2;
            addDetail("Formula 131", "fE2", "fF2", loadCase->fE2, "N/mm2");
        }
    }
}

/**
 * @brief Formula C.3 C.4 C.9 C.10: Maximum bolt load for load limit calculation
 * @param loadCaseNo
 */
void Assembly::Calc_F_BImaxmin(int loadCaseNo) {
    LoadCase* loadCase0 = mLoadCaseList->at(0);
    LoadCase* loadCaseI = mLoadCaseList->at(loadCaseNo);

    if (loadCaseNo == 0) {
        loadCaseI->F_BImin = loadCase0->F_Bnom * (1 - mBolt->etanminus);
        loadCaseI->F_BImax = loadCase0->F_Bnom * (1 + mBolt->etanplus);
        addDetail("Formula C.3", "F_BImin", "F_Bnom * (1 - etanminus)",
                  loadCaseI->F_BImin, "N");
        addDetail("Formula C.4", "F_BImax", "F_Bnom * (1 + etanplus)",
                  loadCaseI->F_BImax, "N");
    } else {
        loadCaseI->F_BImin = loadCaseI->F_GImin
                + (loadCaseI->F_Q + loadCaseI->F_R);
        loadCaseI->F_BImax = loadCaseI->F_GImax
                + (loadCaseI->F_Q + loadCaseI->F_R);
        addDetail("Formula C.9", "F_BImin", "F_GImin + (F_Q + F_R)",
                  loadCaseI->F_BImin, "N");
        addDetail("Formula C.10", "F_BImax", "F_GImax + (F_Q + F_R)",
                  loadCaseI->F_BImax, "N");
    }

}

/**
 * @brief Formula C.5 C.6 C.7 C.8: Maximum gasket load
 * for load limit calculation
 * @param loadCaseNo
 */
void Assembly::Calc_F_GImaxmin(int loadCaseNo) {
    LoadCase* loadCase0 = mLoadCaseList->at(0);
    LoadCase* loadCaseI = mLoadCaseList->at(loadCaseNo);

    if (loadCaseNo == 0) {
        loadCaseI->F_GImin = loadCaseI->F_BImin - loadCaseI->F_R;
        loadCaseI->F_GImax = loadCaseI->F_BImax - loadCaseI->F_R;
        addDetail("Formula C.5", "F_GImin", "F_BImin - F_R",
                  loadCaseI->F_GImin, "N");
        addDetail("Formula C.6", "F_GImin", "F_BImax - F_R",
                  loadCaseI->F_GImax, "N");
    } else {
        double tmpF_G = loadCaseI->F_Q * loadCaseI->Y_Q +
                (loadCaseI->F_R *loadCaseI->Y_R - loadCase0->F_R
                 * loadCase0->Y_R) + loadCaseI->dUI;
        loadCaseI->F_GImin = (loadCase0->F_GImin * loadCase0->Y_G - tmpF_G
                              - loadCaseI->delta_eGc) / loadCaseI->Y_G;
        loadCaseI->F_GImax = (loadCase0->F_GImax * loadCase0->Y_G - tmpF_G
                              - loadCaseI->delta_eGc) / loadCaseI->Y_G;
        addDetail("Formula C.7", "F_GImin", "(F_GImin * Y_G - (F_Q * Y_Q "
                  "+ (F_R * Y_R - F_R * Y_R) + dUI) - delta_eGc) / Y_G",
                  loadCaseI->F_GImin, "N");
        addDetail("Formula C.8", "F_GImax", "(F_GImax * Y_G - (F_Q * Y_Q "
                  "+ (F_R * Y_R - F_R * Y_R) + dUI) - delta_eGc) / Y_G",
                  loadCaseI->F_GImax, "N");
    }
}

/**
 * @brief Formula C.1: Rotation flange or stub/collar
 * @param loadCaseNo
 */
void Assembly::Calc_ThetaFmaxmin(int loadCaseNo) {
    double radToDeg = 180 / M_PI;
    LoadCase* loadCase = mLoadCaseList->at(loadCaseNo);

    double tmpFlange1 =
            loadCase->F_Q *(loadCase->hH1 - mFlange1->hP + mFlange1->hQ)
            + loadCase->F_R *(loadCase->hH1 + mFlange1->hR);
    double tmpFlange2 =
            loadCase->F_Q *(loadCase->hH2 - mFlange2->hP + mFlange2->hQ)
            + loadCase->F_R *(loadCase->hH2 + mFlange2->hR);
    loadCase->ThetaF1min = mFlange1->ZF / loadCase->EF1
            * (loadCase->F_GImin * loadCase->hG1 + tmpFlange1);
    loadCase->ThetaF1max = mFlange1->ZF / loadCase->EF1
            * (loadCase->F_GImax * loadCase->hG1 + tmpFlange1);
    loadCase->ThetaF2min = mFlange2->ZF / loadCase->EF2
            * (loadCase->F_GImin * loadCase->hG2 + tmpFlange2);
    loadCase->ThetaF2max = mFlange2->ZF / loadCase->EF2
            * (loadCase->F_GImax * loadCase->hG2 + tmpFlange2);
    addDetail("Formula C.1", "ThetaF1min", "Flange1.ZF / EF1 * (F_GImin * hG1 "
              "+ F_Q * (hH1 - Flange1.hP + Flange1.hQ) "
              "+ F_R * (hH1 + Flange1.hR))",
              radToDeg * loadCase->ThetaF1min, "degree");
    addDetail("Formula C.1", "ThetaF1max", "Flange1.ZF / EF1 * (F_GImax * hG1 "
              "+ F_Q * (hH1 - Flange1hP + Flange1.hQ) "
              "+ F_R * (hH1 + Flange1.hR))",
              radToDeg * loadCase->ThetaF1max, "degree");
    addDetail("Formula C.2", "ThetaF2min", "Flange2.ZF / EF2 * (F_GImin * .hG2 "
              "+ F_Q * (hH2 - Flange2.hP + Flange2.hQ) "
              "+ F_R * (hH2 + Flange2.hR))",
              radToDeg * loadCase->ThetaF2min, "degree");
    addDetail("Formula C.2", "ThetaF2max", "Flange2.ZF / EF2 * (F_GImax * hG2 "
              "+ F_Q * (hH2 - Flange2.hP + Flange2.hQ) "
              "+ F_R * (hH2 + Flange2.hR))",
              radToDeg * loadCase->ThetaF2max, "degree");
}

/**
 * @brief Formula C.2: Rotation loose flange
 * @param loadCaseNo
 */
void Assembly::Calc_ThetaLmaxmin(int loadCaseNo) {
    double radToDeg = 180 / M_PI;
    LoadCase* loadCase = mLoadCaseList->at(loadCaseNo);

    if (dynamic_cast<Flange_Loose*>(mFlange1) != NULL)     {
        loadCase->ThetaL1min = mFlange1->ZL / loadCase->EL1
                * (loadCase->F_BImin * loadCase->hL1);
        loadCase->ThetaL1max = mFlange1->ZL / loadCase->EL1
                * (loadCase->F_BImax * loadCase->hL1);
        addDetail("Formula C.2",
                  "ThetaL1min", "Flange1.ZL / EL1 * (F_BImin * hL1)",
                  radToDeg * loadCase->ThetaL1min, "degree");
        addDetail("Formula C.2",
                  "ThetaL1max", "Flange1.ZL / EL1 * (F_BImax * hL1)",
                  radToDeg * loadCase->ThetaL1max, "degree");
    } else {
        loadCase->ThetaL1min = 0.0;
        loadCase->ThetaL1max = 0.0;
        addDetail("Formula C.2", "ThetaL1min", "0.0",
                  radToDeg * loadCase->ThetaL1min, "degree");
        addDetail("Formula C.2", "ThetaL1max", "0.0",
                  radToDeg * loadCase->ThetaL1max, "degree");
    }

    if (dynamic_cast<Flange_Loose*>(mFlange2) != NULL)     {
        loadCase->ThetaL2min = mFlange2->ZL / loadCase->EL2
                * (loadCase->F_BImin * loadCase->hL2);
        loadCase->ThetaL2max = mFlange2->ZL / loadCase->EL2
                * (loadCase->F_BImax * loadCase->hL2);
        addDetail("Formula C.2",
                  "ThetaL2min", "Flange2.ZL / EL2 * (F_BImin * hL2)",
                  radToDeg * loadCase->ThetaL2min, "degree");
        addDetail("Formula C.2",
                  "ThetaL2max", "Flange2.ZL / EL2 * (F_BImax * hL2)",
                  radToDeg * loadCase->ThetaL2max, "degree");
    } else {
        loadCase->ThetaL2min = 0.0;
        loadCase->ThetaL2max = 0.0;
        addDetail("Formula C.2", "ThetaL2min", "0.0",
                  radToDeg * loadCase->ThetaL2min, "degree");
        addDetail("Formula C.2", "ThetaL2max", "0.0",
                  radToDeg * loadCase->ThetaL2max, "degree");
    }
}

END_NAMESPACE_REDBAG_CALC_EN1591
