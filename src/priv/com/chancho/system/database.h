/*
 * Copyright (c) 2015 Manuel de la Peña <mandel@themacaque.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include <memory>

#include <glog/logging.h>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlIndex>
#include <QSqlQuery>
#include <QString>
#include <QStringList>

#include "query.h"

namespace com {

namespace chancho {

namespace system {

class Database {
    friend class DatabaseFactory;

 public:
    Database() = default;
    Database(const QSqlDatabase& db)
            : _db(db){
    }

    virtual ~Database() = default;

    virtual void close() {
        _db.close();
    }

    virtual bool commit() {
        return _db.commit();
    }

    virtual QString connectOptions() const {
        return _db.connectOptions();
    }

    virtual QString connectionName() const {
        return _db.connectionName();
    }
    virtual std::shared_ptr<Query> createQuery() const {
        QSqlQuery q(_db);
        return std::make_shared<Query>(q);
    }

    virtual QString databaseName() const {
        return _db.databaseName();
    }

    virtual QString driverName() const {
        return _db.driverName();
    }

    virtual QSqlQuery exec(const QString& query = QString()) const {
        return _db.exec(query);
    }

    virtual QString hostName() const {
        return _db.hostName();
    }

    virtual bool isOpen() const {
        return _db.isOpen();
    }

    virtual bool isOpenError() const {
        return _db.isOpenError();
    }

    virtual bool isValid() const {
        return _db.isValid();
    }

    virtual QSqlError lastError() const {
        return _db.lastError();
    }

    virtual bool open() {
        return _db.open();
    }

    virtual bool open(const QString& user, const QString& password) {
        return _db.open(user, password);
    }

    virtual QString password() const {
        return _db.password();
    }

    virtual int port () const {
        return _db.port();
    }

    virtual QSqlIndex primaryIndex(const QString& tablename) const {
        return _db.primaryIndex(tablename);
    }

    virtual QSqlRecord record(const QString& tablename) const {
        return _db.record(tablename);
    }

    virtual bool rollback() {
        return _db.rollback();
    }

    virtual void setConnectOptions(const QString& options = QString()) {
        _db.setConnectOptions(options);
    }

    virtual void setDatabaseName(const QString& name) {
        _db.setDatabaseName(name);
    }

    virtual void setHostName(const QString& host) {
        _db.setHostName(host);
    }

    virtual void setNumericalPrecisionPolicy(QSql::NumericalPrecisionPolicy precisionPolicy) {
        _db.setNumericalPrecisionPolicy(precisionPolicy);
    }

    virtual void setPassword(const QString& password) {
        _db.setPassword(password);
    }

    virtual void setPort(int port) {
        _db.setPort(port);
    }

    virtual void setUserName(const QString& name) {
        _db.setUserName(name);
    }

    virtual QStringList tables(QSql::TableType type = QSql::Tables) const {
        return _db.tables(type);
    }

    virtual bool transaction() {
        return _db.transaction();
    }

    virtual QString userName() const {
        return _db.userName();
    }

 protected:
    QSqlDatabase _db;
};

}

}

}
