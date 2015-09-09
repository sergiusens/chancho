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

#include <com/chancho/system/database_factory.h>
#include <com/chancho/system/database_lock.h>

#include "book.h"
#include "updater.h"
#include "version.h"

namespace com {

namespace chancho {

namespace {
    const QString SELECT_VERSIONS = "SELECT major, minor, patch FROM Versions ORDER BY major, minor, patch DESC";
    const QString INSERT_CURRENT_VERSION = "INSERT OR REPLACE INTO Versions(major, minor, patch) " \
        "VALUES (:major, :minor, :patch)";
    const QString SELECT_TRIGGERS = "SELECT name FROM sqlite_master WHERE type = 'trigger'";
    const QString ALTER_TRANSACTION_TABLE = "ALTER TABLE Transactions ADD COLUMN is_recurrent int";
}

class UpdaterLock {
 public:

    explicit UpdaterLock(Updater* book)
            : _updater(book) {
        _updater->_dbMutex.lock();
        _opened = _updater->_db->open();
    }

    ~UpdaterLock() {
        if (_opened) {
            _updater->_db->close();
        }
        _updater->_dbMutex.unlock();
    }

    bool opened() const {
        return _opened;
    }

    UpdaterLock(const UpdaterLock&) = delete;
    UpdaterLock& operator=(const UpdaterLock&) = delete;

 private:
    bool _opened = false;
    Updater* _updater;
};

Updater::Updater() {
    auto dbPath = Book::databasePath();
    _db = system::DatabaseFactory::instance()->addDatabase("QSQLITE", "BOOKS");
    _db->setDatabaseName(dbPath);
}

Updater::~Updater() {

}

QString
Updater::getDatabaseVersion() {
    return QString();
}

void
Updater::setDatabaseVersion() {
    UpdaterLock dbLock(this);
    auto query = _db->createQuery();
    query->prepare(INSERT_CURRENT_VERSION);
    query->bindValue(0, MAJOR_VERSION);
    query->bindValue(1, MINOR_VERSION);
    query->bindValue(2, PATCH_VERSION);

    auto success = query->exec();
    if (!success) {
        LOG(INFO) << "Error setting the latests version;";
    }
}

bool
Updater::needsUpgrade() {
    auto dbVersion = QString("");
    // grab the latest version and compare it with the current one
    UpdaterLock dbLock(this);
    if (!dbLock.opened()) {
        return true;
    }

    auto query = _db->createQuery();
    auto success = query->exec(SELECT_VERSIONS);

    if (!success) {
        LOG(INFO) << "Error retrieving the database version;";
    } else if (query->next()) {
        // build the current version
        auto major = query->value(0).toInt();
        auto minor = query->value(1).toInt();
        auto patch = query->value(2).toInt();
        dbVersion = QString("%1.%2.%3").arg(major).arg(minor).arg(patch);
    }
    return dbVersion == QString(VERSION);
}

void
Updater::addRecurrenceTables(std::shared_ptr<system::Database> db) {
    db->transaction();

    bool success = true;
    auto query = db->createQuery();
    success &= query->exec(ALTER_TRANSACTION_TABLE);
    if (!success) {
        LOG(ERROR) << "Error when upgrading db " << query->lastError().text().toStdString();
        success = true;
    }

    success &= query->exec(Book::RECURRENT_TRANSACTION_TABLE);
    success &= query->exec(Book::RECURRENT_TRANSACTIONS_RELATIONS_TABLE);
    success &= query->exec(Book::RECURRENT_RELATIONS_DELETE_TRIGGER);
    success &= query->exec(Book::RECURRENT_RELATIONS_INSERT_TRIGGER);
    success &= query->exec(Book::RECURRENT_RELATIONS_UPDATE_TRIGGER);

    if (success)
        db->commit();
    else
        db->rollback();

    if (!success) {
        LOG(ERROR) << "Could not update the chancho db " << db->lastError().text().toStdString();
    }
}

void
Updater::addRecurrenceRelation(std::shared_ptr<system::Database> db) {
    db->transaction();

    bool success = true;
    auto query = db->createQuery();
    success &= query->exec(ALTER_TRANSACTION_TABLE);
    if (!success) {
        LOG(ERROR) << "Error when upgrading db " << query->lastError().text().toStdString();
        success = true;
    }
    success &= query->exec(Book::RECURRENT_TRANSACTIONS_RELATIONS_TABLE);
    success &= query->exec(Book::RECURRENT_RELATIONS_DELETE_TRIGGER);
    success &= query->exec(Book::RECURRENT_RELATIONS_INSERT_TRIGGER);
    success &= query->exec(Book::RECURRENT_RELATIONS_UPDATE_TRIGGER);

    if (success)
        db->commit();
    else
        db->rollback();

    if (!success) {
        LOG(ERROR) << "Could not update the chancho db " << db->lastError().text().toStdString();
    }
}

void
Updater::addRecurrenceTrigger(std::shared_ptr<system::Database> db) {
    auto query = db->createQuery();
    auto success = query->exec(Book::RECURRENT_RELATIONS_UPDATE_TRIGGER);

    if (!success) {
        LOG(ERROR) << "Could not update the chancho db " << db->lastError().text().toStdString();
    }
}

void
Updater::upgrade() {
    // before version 0.2.1 we did not store the version of the database, therefore we need to check for different
    // tables
    auto version = lastVersion();
    LOG(INFO) << "Version is " << version.mayor << "." << version.minor << "." << version.patch;
    if (version.mayor == -1 && version.minor == -1 && version.patch == -1) {
        LOG(INFO) << "Dealing with an old version of the app without version number.";

        std::shared_ptr<system::Database> db;
        auto dbPath = Book::databasePath();
        db = system::DatabaseFactory::instance()->addDatabase("QSQLITE", "BOOKS");
        db->setDatabaseName(dbPath);

        system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(db);
        if (!dbLock.opened()) {
            LOG(ERROR) << "Could not open database to initialize it " << db->lastError().text().toStdString();
            return;
        }

        // once the db has been created we need to check that it has the correct version and the required tables
        auto tables = db->tables();
        auto triggers = getTriggers(db);

        // only has to be init those dbs with less than 3 tables, the old versions
        bool hasToBeInit = tables.count() < 3;

        // all versions of the application do not have the recurrent transactions table
        bool needsRecurrenceUpdate = !tables.contains("recurrenttransactions", Qt::CaseInsensitive);
        DLOG(INFO) << "Needs recurrence update " << needsRecurrenceUpdate;
        bool needsRecurrenceRelations = !tables.contains("recurrentTransactionrelations", Qt::CaseInsensitive);
        DLOG(INFO) << "Needs recurrence relations " << needsRecurrenceRelations;
        bool needsRecurrentUpdateTrigger = !triggers.contains("updateGeneratedRelationsOnUpdate", Qt::CaseInsensitive);
        DLOG(INFO) << "Needs recurrence trigger " << needsRecurrentUpdateTrigger;

        if (!hasToBeInit && needsRecurrenceUpdate) {
            LOG(INFO) << "Needs recurrence update.";
            addRecurrenceTables(db);
        } else if (!hasToBeInit && needsRecurrenceRelations) {
            addRecurrenceRelation(db);
        } else if (!hasToBeInit && !needsRecurrenceRelations && needsRecurrentUpdateTrigger) {
            addRecurrenceTrigger(db);
        }
    }
}


Version
Updater::lastVersion() {
    // grab the latest version and compare it with the current one
    UpdaterLock dbLock(this);
    if (!dbLock.opened()) {
        return Version{-1, -1, -1};
    }

    auto query = _db->createQuery();
    auto success = query->exec(SELECT_VERSIONS);

    if (!success) {
        LOG(INFO) << "Error retrieving the database version;";
        return Version{-1, -1, -1};
    } else if (query->next()) {
        // build the current version
        auto major = query->value(0).toInt();
        auto minor = query->value(1).toInt();
        auto patch = query->value(2).toInt();
        return Version{major, minor, patch};
    }
    return Version{-1, -1, -1};
}

QStringList
Updater::getTriggers(std::shared_ptr<system::Database> db) {
    QStringList triggers;
    auto query = db->createQuery();
    bool success = true;
    success &= query->exec(SELECT_TRIGGERS);
    if (success) {
        while(query->next()){
            auto name = query->value(0).toString();
            triggers.append(name);
        }
    }
    return triggers;
}

}

}
