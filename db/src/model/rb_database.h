/*****************************************************************
 * $Id: rb_database.h 2248 2015-06-21 09:13:00Z rutger $
 * Created: Nov 15, 2009 1:00:42 PM - rutger
 *
 * Copyright (C) 2009 Red-Bag. All rights reserved.
 * This file is part of the Biluna DB project.
 *
 * See http://www.red-bag.com for further details.
 *****************************************************************/

#ifndef RB_DATABASE_H
#define RB_DATABASE_H

#include <QtSql>
#include "db_global.h"
#include "rb_string.h"

class RB_TcpSocket;

#define RB_DATABASE RB_Database::getInstance()

/**
 * Database connection and data object.
 * NOTE: QSqlDatabase is publicly inherited
 * Before using the connection, it must be initialized. e.g.,
 * call some or all of setDatabaseName(), setUserName(), setPassword(),
 * setHostName(), setPort(), and setConnectOptions(), and, finally, open().
 */
class DB_EXPORT RB_Database : public QSqlDatabase {

public:
    virtual ~RB_Database();
    static RB_Database* getInstance();

    void keepConnectionAlive(const QSqlDatabase& db, int mSec = 60000);

    virtual RB_String localDbName() const;
    virtual RB_String userAuthName() const;
    virtual RB_String userAuthPassword() const;

    virtual void setLocalDbName(const RB_String& name);
    virtual void setUserAuthName(const RB_String& name);
    virtual void setUserAuthPassword(const RB_String& pwd);

    virtual void dummyQuery();

private:
    RB_Database();
    // RB_Database(const QSqlDatabase& other);

    RB_String mLocalDbName;
    RB_String mUserAuthName;
    RB_String mUserAuthPassword;

    RB_TcpSocket* mTcpSocket;

    //! Unique instance
    static RB_Database* mActiveDatabase;

    // For servers that have connections closed
    QSqlDatabase mServerDatabase;
};

#endif // RB_DATABASE_H