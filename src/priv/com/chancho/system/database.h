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
#include <sqlite3.h>

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlIndex>
#include <QSqlQuery>
#include <QString>
#include <QStringList>

#include "query.h"

namespace {

static void addStringNumbers(sqlite3_context *context, int argc, sqlite3_value **argv) {
    DLOG(INFO) << __PRETTY_FUNCTION__;
    if (argc == 2) {
        auto first = reinterpret_cast<const char*>(sqlite3_value_text(argv[0]));
        auto second = reinterpret_cast<const char*>(sqlite3_value_text(argv[1]));
        if (first && second) {
            auto firstAmount = QString::fromUtf8(first).toDouble();
            auto secondAmount = QString::fromUtf8(second).toDouble();
            auto result = QString::number(firstAmount + secondAmount);
            sqlite3_result_text(context, result.toStdString().c_str(), -1, SQLITE_TRANSIENT);
            return;
        }
    }
}

static void subtractStringNumbers(sqlite3_context *context, int argc, sqlite3_value **argv) {
    DLOG(INFO) << __PRETTY_FUNCTION__;
    if (argc == 2) {
        auto first = reinterpret_cast<const char*>(sqlite3_value_text(argv[0]));
        auto second = reinterpret_cast<const char*>(sqlite3_value_text(argv[1]));
        if (first && second) {
            auto firstAmount = QString::fromUtf8(first).toDouble();
            auto secondAmount = QString::fromUtf8(second).toDouble();
            auto result = QString::number(firstAmount - secondAmount);
            sqlite3_result_text(context, result.toStdString().c_str(), -1, SQLITE_TRANSIENT);
            return;
        }
    }
}

static void stringSumStep(sqlite3_context *context, int, sqlite3_value** argv) {
    double *amount = (double *) sqlite3_aggregate_context(context, sizeof(double));

    if (amount == nullptr) {
        LOG(ERROR) << "Error when calculating SSUM";
        sqlite3_result_error_nomem(context);
        return;
    }

    auto numStr = reinterpret_cast<const char*>(sqlite3_value_text(argv[0]));
    auto num = QString::fromUtf8(numStr).toDouble();
    *amount += num;
}

static void stringSumFinal(sqlite3_context *context) {
    double *amount= (double *) sqlite3_aggregate_context(context, sizeof(double));

    if (amount == nullptr) {
        LOG(ERROR) << "Error when calculating SSUM";
        sqlite3_result_error_nomem(context);
        return;
    }

    auto amountStr = QString::number(*amount).toStdString();
    sqlite3_result_text(context, amountStr.c_str(), -1, SQLITE_TRANSIENT);
}

static void trace(void*, const char* query ) {
    DLOG(INFO) << "SQlite: " << query;
}

}

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
        auto opened = _db.open();
        if (!opened) {
            return opened;
        } else {
            bool extensionsAdded = addSqlite3Extensions();
            if (!extensionsAdded) {
                _db.close();
                return extensionsAdded;
            }
        }
        return opened;
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

    virtual bool addSqlite3Extensions() {
        DLOG(INFO) << __PRETTY_FUNCTION__;
        // Get handle to the driver and check it is both valid and refers to SQLite3.
        auto v = _db.driver()->handle();
        if (!v.isValid() || qstrcmp(v.typeName(), "sqlite3*") != 0) {
            LOG(INFO) << "Cannot get a sqlite3 handle to the driver.";
            return false;
        }

        // Create a handler and attach functions.
        auto handler = *static_cast<sqlite3**>(v.data());
        if (!handler) {
            LOG(INFO) << "Cannot get a sqlite3 handler.";
            return false;
        }

        // Check validity of the state.
        if (!_db.isValid()) {
            LOG(INFO) << "Cannot create SQLite custom functions: db object is not valid.";
            return false;
        }

        if (!_db.isOpen()) {
            LOG(INFO) << "Cannot create SQLite custom functions: db object is not open.";
            return false;
        }

        auto added = sqlite3_create_function(handler, "AddStringNumbers", 2, SQLITE_UTF8, nullptr, &addStringNumbers, nullptr, nullptr);
        if (added == SQLITE_OK) {
            DLOG(INFO) << "AddStringNumbers added";
        } else {
            LOG(WARNING) << "Cannot create SQLite functions: AddStringNumbers";
            return false;
        }
        added = sqlite3_create_function(handler, "SubtractStringNumbers", 2, SQLITE_UTF8, nullptr, &subtractStringNumbers, nullptr, nullptr);
        if (added == SQLITE_OK) {
            DLOG(INFO) << "SubtractStringNumbers added";
        } else {
            LOG(WARNING) << "Cannot create SQLite functions: SubtractStringNumbers";
            return false;
        }

        added = sqlite3_create_function_v2(handler, "SSUM", 1, SQLITE_ANY, NULL, NULL, stringSumStep, stringSumFinal, NULL);
        if (added == SQLITE_OK) {
            DLOG(INFO) << "SSUM added";
        } else {
            LOG(WARNING) << "Cannot create SQLite functions: SSUM";
            return false;
        }

        sqlite3_trace(handler, trace, NULL);

        return true;
    }

 protected:
    QSqlDatabase _db;
};

typedef std::shared_ptr<Database> DatabasePtr;

}

}

}
