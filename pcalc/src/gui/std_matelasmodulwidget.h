/*****************************************************************
 * $Id: std_matelasmodulwidget.h 0 2016-06-01 10:23:20Z rutger $
 * Created: June 1, 2016 10:23:42 AM - rutger
 *
 * Copyright (C) 2015 Red-Bag. All rights reserved.
 * This file is part of the Biluna PCALC project.
 *
 * See http://www.red-bag.com for further details.
 *****************************************************************/

#ifndef STD_MATELASMODULWIDGET_H
#define STD_MATELASMODULWIDGET_H

#include "rb_widget.h"

namespace Ui {
class STD_MatElasModulWidget;
}

class STD_MatElasModulWidget : public RB_Widget {

    Q_OBJECT

public:
    explicit STD_MatElasModulWidget(QWidget *parent = 0);
    virtual ~STD_MatElasModulWidget();

    RB_String getName() const;
    RB2::PerspectiveType getPerspectiveType() const;
    void init();

    bool fileSave(bool withSelect);
    void fileRevert();

private:
    Ui::STD_MatElasModulWidget* ui;
    RB_MmProxy* mModel;
    RB_MmProxy* mChildModel;
};

#endif // STD_MATELASMODULWIDGET_H
