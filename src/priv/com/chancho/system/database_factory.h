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

#include <QMutex>
#include <QSqlDatabase>

#include "database.h"

namespace com {

namespace chancho {

namespace system {

class DatabaseFactory {

 public:
    virtual ~DatabaseFactory() = default;

    virtual std::shared_ptr<Database> addDatabase(const QString& type, const QString& connectionName) {
        return std::make_shared<Database>(QSqlDatabase::addDatabase(type, connectionName));
    };

    virtual std::shared_ptr<Database> addDatabase(QSqlDriver* driver, const QString& connectionName) {
        return std::make_shared<Database>(QSqlDatabase::addDatabase(driver, connectionName));
    };

    virtual std::shared_ptr<Database> cloneDatabase(const Database& other, const QString& connectionName) {
        return std::make_shared<Database>(QSqlDatabase::cloneDatabase(other._db, connectionName));
    };

    virtual QStringList connectionNames() {
        return QSqlDatabase::connectionNames();
    }

    virtual bool contains(const QString& connectionName) {
        return QSqlDatabase::contains(connectionName);
    }

    virtual std::shared_ptr<Database> database(const QString& connectionName, bool open=true) {
        return std::make_shared<Database>(QSqlDatabase::database(connectionName, open));
    };

    virtual QStringList drivers() {
        return QSqlDatabase::drivers();
    }

    virtual bool isDriverAvailable(const QString& name) {
        return QSqlDatabase::isDriverAvailable(name);
    }

    virtual void registerSqlDriver(const QString& name, QSqlDriverCreatorBase* creator) {
        QSqlDatabase::registerSqlDriver(name, creator);
    }

    virtual void removeDatabase(const QString& connectionName) {
        QSqlDatabase::removeDatabase(connectionName);
    }

    static DatabaseFactory* instance() {
        if(!_instance) {
            _mutex.lock();
            if(!_instance)
                _instance = new DatabaseFactory();
            _mutex.unlock();
        }
        return _instance;
    }

    // only used for testing purposes
    static void setInstance(DatabaseFactory* instance) {
        _instance = instance;
    }

    static void deleteInstance() {
        delete _instance;
        _instance = nullptr;
    }

 protected:
    DatabaseFactory() = default;

 private:
    static DatabaseFactory* _instance;
    static QMutex _mutex;

};

}

}

}
