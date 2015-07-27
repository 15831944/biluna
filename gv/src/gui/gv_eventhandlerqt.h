/*****************************************************************
 * $Id: gv_eventhandlerqt.h 1452 2011-09-10 08:19:50Z rutger $
 * Created: Sept 5, 2011 4:59:20 PM - rutger
 *
 * Copyright (C) 2011 Red-Bag. All rights reserved.
 * This file is part of the Biluna gv project.
 *
 * See http://www.red-bag.com for further details.
 *****************************************************************/

#ifndef GV_EVENTHANDLERQT_H
#define GV_EVENTHANDLERQT_H

#include <QObject>
class RB_MdiWindow;
class RDocument;
class RDocumentInterface;
class RGraphicsViewQt;
class RRulerQt;
class QScrollBar;
class QScrollBar;


/**
 * The event handler manages viewport and scrollbar changes
 */
class GV_EventHandlerQt : public QObject {

    Q_OBJECT
	
public:
    GV_EventHandlerQt();
    ~GV_EventHandlerQt();

    virtual void setGraphicsView(RGraphicsViewQt* view);
    virtual void setMdiChild(RB_MdiWindow* mdiW);
    virtual void setDocumentInterface(RDocumentInterface* di);
    virtual void setScrollBar(QScrollBar* sb);
    virtual void setRuler(RRulerQt* ruler);

public slots:
    virtual void horizontalScrolled(int pos);
    virtual void verticalScrolled(int pos);
    virtual void viewportChanged();

protected:
    RB_MdiWindow* mMdiW;
    RGraphicsViewQt* mGraphicsView;
    RDocument* mDocument;
    RDocumentInterface* mDocumentInterface;
    QScrollBar* mHsb;
    QScrollBar* mVsb;
    RRulerQt* mHRuler;
    RRulerQt* mVRuler;

};

#endif
