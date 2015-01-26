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

#include <glog/logging.h>

#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStandardPaths>
#include <QVariant>

#include <com/chancho/system/database_factory.h>
#include <QtSql/qsqlquerymodel.h>
#include <QtSql/qsqldriver.h>

#include "book.h"

namespace com {

namespace chancho {

namespace {
    const QString DATABASE_NAME = "chancho.db";
    const QString CATEGORIES_TABLE = "CREATE TABLE IF NOT EXISTS Categories("\
        "uuid VARCHAR(40) PRIMARY KEY, "\
        "parent VARCHAR(40), "\
        "name TEXT NOT NULL,"\
        "type INT,"\
        "FOREIGN KEY(parent) REFERENCES Categories(uuid))";
    const QString INSERT_UPDATE_CATEGORY = "INSERT OR REPLACE INTO Categories(uuid, parent, name, type) " \
            "VALUES (:uuid, :parent, :name, :type)";
    const QString DELETE_CHILD_CATEGORIES = "DELETE FROM Categories WHERE parent=:uuid";
    const QString DELETE_CATEGORY = "DELETE FROM Categories WHERE uuid=:uuid";
    const QString SELECT_ALL_CATEGORIES = "SELECT uuid, parent, name, type FROM Categories";
    const QString FOREIGN_KEY_SUPPORT = "PRAGMA foreign_keys = ON";
}

double Book::DB_VERSION = 0.1;
std::set<QString> Book::TABLES {"categories"};

STATIC_INIT(Book) {
    Book::initDatabse();
}

QString
Book::databasePath() {
    auto dbPath = QStandardPaths::locate(QStandardPaths::DataLocation, DATABASE_NAME);
    if (dbPath.isEmpty()) {
        auto dataLocation = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
        QDir dir(dataLocation);
        if (!dir.exists()) {
            auto wasMade = dir.mkpath(".");
            if (!wasMade)
                LOG(ERROR) << "Could not create application dir in " << dir.absolutePath().toStdString();
        }
        dbPath = dir.absoluteFilePath(DATABASE_NAME);
    }
    DLOG(INFO) << "Database path is " << dbPath.toStdString();
    return dbPath;
}

void
Book::initDatabse() {
    // ensure the version of the database and ensure that all the required tables are present
    std::shared_ptr<system::Database> db;
    auto dbPath = Book::databasePath();
    db = system::DatabaseFactory::instance()->addDatabase("QSQLITE", "BOOKS");
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    if (!opened) {
        LOG(ERROR) << "Could not open database to initialize it " << db->lastError().text().toStdString();
        return;
    }

    // once the db has been created we need to check that it has the correct version and the required tables
    bool hasToBeInit = false;
    auto tables = db->tables();
    if (tables.count() > 0) {
        LOG(INFO) << "Tables are " << tables.join(", ").toStdString();
        foreach(const QString& expectedTable, Book::TABLES) {
            if (!tables.contains(expectedTable, Qt::CaseInsensitive)) {
                LOG(INFO) << "Table '" << expectedTable.toStdString() << "' missing";
                hasToBeInit = true;
                break;
            }
        }
    } else {
        DLOG(INFO) << "No tables present in the database";
        hasToBeInit = true;
    }

    if (hasToBeInit) {
        LOG(INFO) << "The database needs to be init";
        db->transaction();

        // create the required tables and indexes
        bool success = true;
        auto query = db->createQuery();
        success &= query->exec(CATEGORIES_TABLE);
        success &= query->exec(FOREIGN_KEY_SUPPORT);

        if (success)
            db->commit();
        else
            db->rollback();

        if (!success)
            LOG(ERROR) << "Could not create the chancho db " << db->lastError().text().toStdString();
    }
    db->close();
}


Book::Book() {
    auto dbPath = Book::databasePath();
    _db = system::DatabaseFactory::instance()->addDatabase("QSQLITE", "BOOKS");
    _db->setDatabaseName(dbPath);
}

Book::~Book() {

}

void
Book::store(CategoryPtr cat) {
    // if the cate has a parent and was not stored, we store it
    if (cat->parent && !cat->parent->wasStoredInDb()) {
        LOG(INFO) << "Storing parent that was not present already";
        store(cat->parent);
    }

    bool opened = _db->open();

    if (!opened) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return;
    }

    // if a category is already present in the db the uuid will be present else is null
    if (cat->_dbId.isNull()) {
        cat->_dbId = QUuid::createUuid();
    }

    // create the command and execute it
    auto query = _db->createQuery();
    query->prepare(INSERT_UPDATE_CATEGORY);
    query->bindValue(":uuid", cat->_dbId.toString());
    query->bindValue(":name", cat->name);
    query->bindValue(":type", static_cast<int>(cat->type));

    if (cat->parent) {
        query->bindValue(":parent", cat->parent->_dbId.toString());
    } else {
        query->bindValue(":parent", QVariant());
    }

    // no need to use a transaction since is a single insert
    auto success = query->exec();
    if (!success) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
    }

    _db->close();
}

void
Book::remove(CategoryPtr cat) {
    if (cat->_dbId.isNull()) {
        LOG(ERROR) << "Cannot delete category '" << cat->name.toStdString()
                << "' with a NULL id";
        _lastError = "Cannot delete Category that was not added to the db";
        return;
    }

    bool opened = _db->open();

    if (!opened) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return;
    }

    // ensure that we have a transaction so that we do not have the db in a non stable state
    _db->transaction();

    auto deleteChildCats = _db->createQuery();
    deleteChildCats->prepare(DELETE_CHILD_CATEGORIES);
    deleteChildCats->bindValue(":uuid", cat->_dbId.toString());
    auto success = deleteChildCats->exec();

    auto deleteCat = _db->createQuery();
    deleteCat->prepare(DELETE_CATEGORY);
    deleteCat->bindValue(":uuid", cat->_dbId.toString());
    success &= deleteCat->exec();

    if (success) {
        DLOG(INFO) << "Commiting transaction";
        _db->commit();
    } else {
        _lastError = _db->lastError().text();
        LOG(ERROR) << "Rolliing back transaction after error: '" << _lastError.toStdString() << "'";
        _db->rollback();
    }

    _db->close();

    // set the _dbId to be a null uuid
    cat->_dbId = QUuid();
}

QList<CategoryPtr>
Book::categories() {
    QMap<QUuid, QList<QUuid>> parentChildMap;
    QMap<QUuid, CategoryPtr> catsMap;

    bool opened = _db->open();

    if (!opened) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        QList<CategoryPtr> cats;
        return cats;
    }

    auto query = _db->createQuery();
    query->exec(SELECT_ALL_CATEGORIES);

    while (query->next()) {
        auto cat = std::make_shared<Category>(query->value("name").toString(),
                static_cast<Category::Type>(query->value("type").toInt()));
        cat->_dbId = QUuid(query->value("uuid").toString());
        LOG(INFO) << "Category found with id " << cat->_dbId.toString().toStdString();
        catsMap[cat->_dbId] = cat;

        if(!query->value("parent").isNull()) {
            auto parent = QUuid(query->value("parent").toString());
            DLOG(INFO) << "Parent relationship foudn with " << parent.toString().toStdString();

            if(parentChildMap.contains(parent)) {
                parentChildMap[parent].append(cat->_dbId);
            } else {
                QList<QUuid> childList;
                childList.append(cat->_dbId);
                parentChildMap[parent] = childList;
            }
        } else {
            LOG(INFO) << "Parent not found";
        }
    }

    // set the parent child relationship
    foreach(const QUuid& parentId, parentChildMap.keys()) {
        if (!catsMap.contains(parentId)) {
            LOG(ERROR) << "DB consintency error categori with id '" << parentId.toString().toStdString()
                    << "' not found.";
            continue;
        }
        foreach(const QUuid& childId, parentChildMap[parentId]) {
            if (!catsMap.contains(childId)) {
                LOG(ERROR) << "DB consintency error categori with id '" << parentId.toString().toStdString()
                        << "' not found.";
                continue;
            }
            // set the parent, do not worry about points since we are using std::shared_ptr
            catsMap[childId]->parent = catsMap[parentId];
        }
    }

    return catsMap.values();
}

bool
Book::isError() {
    return _lastError != QString::null;
}

QString
Book::lastError() {
    return _lastError;
}

}

}
