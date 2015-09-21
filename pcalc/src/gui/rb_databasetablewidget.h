#ifndef RB_DATABASETABLEWIDGET_H
#define RB_DATABASETABLEWIDGET_H

#include "rb_widget.h"

namespace Ui {
class RB_DatabaseTableWidget;
}

class RB_DatabaseTableWidget : public RB_Widget {

    Q_OBJECT

public:
    RB_DatabaseTableWidget(QWidget* parent = 0);
    virtual ~RB_DatabaseTableWidget();

private:
    Ui::RB_DatabaseTableWidget *ui;
};

#endif // RB_DATABASETABLEWIDGET_H