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

#include <QDebug>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStandardPaths>
#include <QVariant>

#include <com/chancho/system/database_factory.h>
#include <QtSql/qsqlquerymodel.h>

#include "book.h"

namespace com {

namespace chancho {

namespace {
    const QString DATABASE_NAME = "chancho.db";
    const QString ACCOUNTS_TABLE = "CREATE TABLE IF NOT EXISTS Accounts("\
        "uuid VARCHAR(40) PRIMARY KEY, "\
        "name TEXT NOT NULL,"\
        "memo TEXT,"\
        "amount TEXT)";  // amounts are stored in text so that we can use the most precise number
    const QString CATEGORIES_TABLE = "CREATE TABLE IF NOT EXISTS Categories("\
        "uuid VARCHAR(40) PRIMARY KEY, "\
        "parent VARCHAR(40), "\
        "name TEXT NOT NULL,"\
        "type INT,"\
        "FOREIGN KEY(parent) REFERENCES Categories(uuid))";
    const QString TRANSACTION_TABLE = "CREATE TABLE IF NOT EXISTS Transactions("\
        "uuid VARCHAR(40) PRIMARY KEY, "\
        "amount TEXT,"\
        "account VARCHAR(40) NOT NULL, "\
        "category VARCHAR(40) NOT NULL, "\
        "day INT, "\
        "month INT, "\
        "year INT, "\
        "contents TEXT, "\
        "memo TEXT, "\
        "FOREIGN KEY(account) REFERENCES Accounts(uuid), "\
        "FOREIGN KEY(category) REFERENCES Categories(uuid))";  // amounts are stored in text so that we can used the most precise number
    const QString TRANSACTION_INSERT_TRIGGER = "CREATE TRIGGER UpdateAccountAmountOnTransactionInsert AFTER INSERT ON Transactions "\
        "BEGIN "\
        "UPDATE Accounts SET amount=AddStringNumbers(amount, new.amount) WHERE uuid=new.account; "\
        "END";  // AddStringNumbers is an extension added by the application to the db
    const QString TRANSACTION_UPDATE_TRIGGER = "CREATE TRIGGER UpdateAccountAmountOnTransactionUpdate AFTER UPDATE ON Transactions "\
        "BEGIN "\
        "UPDATE Accounts SET amount=AddStringNumbers(SubtractStringNumbers(amount, old.amount), new.amount) WHERE uuid=new.account; "\
        "END";
    const QString TRANSACTION_DELETE_TRIGGER = "CREATE TRIGGER UpdateAccountAmountOnTransactionDelete AFTER DELETE ON Transactions "\
        "BEGIN "\
        "UPDATE Accounts SET amount=SubtractStringNumbers(amount, old.amount) WHERE uuid=old.account; "\
        "END";
    const QString ACCOUNT_DELETE_TRIGGER = "CREATE TRIGGER DeleteTransactionsOnAccountDelete BEFORE DELETE ON Accounts "\
        "BEGIN "\
        "DELETE FROM Transactions WHERE account=old.uuid; "\
        "END";
    const QString TRANSACTION_MONTH_INDEX = "CREATE INDEX transaction_month_index ON Transactions(year, month);";
    const QString TRANSACTION_CATEGORY_INDEX = "CREATE INDEX transaction_category_index ON Transactions(category);";
    const QString TRANSACTION_CATEGORY_MONTH_INDEX = "CREATE INDEX transaction_category_month_index ON Transactions(category, year, month);";
    const QString TRANSACTION_ACCOUNT_INDEX = "CREATE INDEX transaction_account_index ON Transactions(account);";
    const QString INSERT_UPDATE_ACCOUNT = "INSERT OR REPLACE INTO Accounts(uuid, name, memo, amount) " \
        "VALUES (:uuid, :name, :memo, :amount)";
    const QString INSERT_UPDATE_CATEGORY = "INSERT OR REPLACE INTO Categories(uuid, parent, name, type) " \
        "VALUES (:uuid, :parent, :name, :type)";
    const QString INSERT_TRANSACTION = "INSERT INTO Transactions(uuid, amount, account, category, "\
        "day, month, year, contents, memo) VALUES (:uuid, :amount, :account, :category, :day, :month, :year, :contents, :memo)";
    const QString UPDATE_TRANSACTION = "UPDATE Transactions SET amount=:amount, account=:account, category=:category, "\
        "day=:day, month=:month, year=:year, contents=:contents, memo=:memo WHERE uuid=:uuid";
    const QString DELETE_ACCOUNT = "DELETE FROM Accounts WHERE uuid=:uuid";
    const QString DELETE_CHILD_CATEGORIES = "DELETE FROM Categories WHERE parent=:uuid";
    const QString DELETE_CATEGORY = "DELETE FROM Categories WHERE uuid=:uuid";
    const QString DELETE_TRANSACTION = "DELETE FROM Transactions WHERE uuid=:uuid";
    const QString SELECT_ALL_ACCOUNTS = "SELECT uuid, name, memo, amount FROM Accounts ORDER BY name ASC";
    const QString SELECT_ALL_CATEGORIES = "SELECT uuid, parent, name, type FROM Categories ORDER BY name ASC";
    const QString SELECT_TRANSACTIONS_MONTH = "SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month, "\
        "t.year, t.contents, t.memo, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t "\
        "INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid "\
        "WHERE t.month=:month AND t.year=:year ORDER BY t.year, t.month";
    const QString SELECT_TRANSACTIONS_CATEGORY = "SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month, "\
        "t.year, t.contents, t.memo, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t "\
        "INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid "\
        "WHERE t.category=:category ORDER BY t.year, t.month";
    const QString SELECT_TRANSACTIONS_CATEGORY_MONTH = "SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month, "\
        "t.year, t.contents, t.memo, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t "\
        "INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid "\
        "WHERE t.category=:category AND t.month=:month AND t.year=:year ORDER BY t.year, t.month";
    const QString SELECT_TRANSACTIONS_ACCOUNT = "SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month, "\
        "t.year, t.contents, t.memo, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t "\
        "INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid "\
        "WHERE t.account=:account ORDER BY t.year, t.month";
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
        success &= query->exec(FOREIGN_KEY_SUPPORT);
        success &= query->exec(ACCOUNTS_TABLE);
        success &= query->exec(CATEGORIES_TABLE);
        success &= query->exec(TRANSACTION_TABLE);
        success &= query->exec(TRANSACTION_INSERT_TRIGGER);
        success &= query->exec(TRANSACTION_UPDATE_TRIGGER);
        success &= query->exec(TRANSACTION_DELETE_TRIGGER);
        success &= query->exec(ACCOUNT_DELETE_TRIGGER);
        success &= query->exec(TRANSACTION_MONTH_INDEX);
        success &= query->exec(TRANSACTION_CATEGORY_INDEX);
        success &= query->exec(TRANSACTION_CATEGORY_MONTH_INDEX);
        success &= query->exec(TRANSACTION_ACCOUNT_INDEX);

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
Book::store(AccountPtr acc) {
    bool opened = _db->open();

    if (!opened) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return;
    }

    // if a category is already present in the db the uuid will be present else is null
    if (acc->_dbId.isNull()) {
        acc->_dbId = QUuid::createUuid();
    }

    auto query = _db->createQuery();
    query->prepare(INSERT_UPDATE_ACCOUNT);
    query->bindValue(":uuid", acc->_dbId.toString());
    query->bindValue(":name", acc->name);
    query->bindValue(":memo", acc->memo);

    query->bindValue(":amount", QString::number(acc->amount));

    // no need to use a transaction since is a single insert
    auto success = query->exec();
    if (!success) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        acc->_dbId = QUuid();
    }

    _db->close();
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
        cat->_dbId = QUuid();
    }

    _db->close();
}

void
Book::store(TransactionPtr tran) {
    // usually accounts and categories must be stored before storing a transactions
    if (tran->account && !tran->account->wasStoredInDb()) {
        _lastError = "An account must be stored before adding a transaction to it.";
        LOG(ERROR) << _lastError.toStdString();
        return;
    }

    if (tran->category && !tran->category->wasStoredInDb()) {
        _lastError = "A category must be stored before adding a transaction to it.";
        LOG(ERROR) << _lastError.toStdString();
        return;
    }


    bool opened = _db->open();

    if (!opened) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return;
    }

    auto isPresent = tran->wasStoredInDb();
    if (!isPresent) {
        tran->_dbId = QUuid::createUuid();
    }


    // with transactions we have to make a diff, since we are using two triggers to update the accounts
    // table correctly, we cannot use an INSERT OR REPLACE because it will not trigger and update.
    auto query = _db->createQuery();
    if (!isPresent) {
        DLOG(INFO) << "Using insert statement for transaction";
        query->prepare(INSERT_TRANSACTION);
    } else {
        DLOG(INFO) << "Using update statement for transaction";
        query->prepare(UPDATE_TRANSACTION);
    }
    query->bindValue(":uuid", tran->_dbId.toString());

    // amounts are positive yet if it is an expense we must multiple by -1 to update the account accordingly
    if (tran->type() == Category::Type::EXPENSE && tran->amount > 0) {
        query->bindValue(":amount", QString::number(-1 * tran->amount));
    } else {
        query->bindValue(":amount", QString::number(tran->amount));
    }

    query->bindValue(":account", tran->account->_dbId.toString());
    query->bindValue(":category", tran->category->_dbId.toString());
    query->bindValue(":day", tran->date.day());
    query->bindValue(":month", tran->date.month());
    query->bindValue(":year", tran->date.year());
    query->bindValue(":contents", tran->contents);
    query->bindValue(":memo", tran->memo);

    // no need to use a transaction since is a single insert
    auto success = query->exec();
    if (!success) {
        _lastError = _db->lastError().text();
        LOG(INFO) << _lastError.toStdString();
        tran->_dbId = QUuid();
    }

    _db->close();
}

void
Book::remove(AccountPtr acc) {
    if (acc->_dbId.isNull()) {
        LOG(ERROR) << "Cannot delete account '" << acc->name.toStdString()
                << "' with a NULL id";
        _lastError = "Cannot delete Account that was not added to the db";
        return;
    }

    bool opened = _db->open();

    if (!opened) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return;
    }

    auto query = _db->createQuery();
    query->prepare(DELETE_ACCOUNT);
    query->bindValue(":uuid", acc->_dbId.toString());
    auto success = query->exec();

    if (!success) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
    }

    acc->_dbId = QUuid();

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
        // set the _dbId to be a null uuid
        cat->_dbId = QUuid();
    } else {
        _lastError = _db->lastError().text();
        LOG(ERROR) << "Rolliing back transaction after error: '" << _lastError.toStdString() << "'";
        _db->rollback();
    }

    _db->close();
}

void
Book::remove(TransactionPtr tran) {
    if (tran->_dbId.isNull()) {
        LOG(ERROR) << "Cannot delete transaction with a NULL id";
        _lastError = "Cannot delete Account that was not added to the db";
        return;
    }

    bool opened = _db->open();

    if (!opened) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return;
    }

    auto query = _db->createQuery();
    query->prepare(DELETE_TRANSACTION);
    query->bindValue(":uuid", tran->_dbId.toString());
    auto success = query->exec();

    if (!success) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
    }

    tran->_dbId = QUuid();

    _db->close();
}

QList<AccountPtr>
Book::accounts() {
    QList<AccountPtr> accs;

    bool opened = _db->open();

    if (!opened) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return accs;
    }

    auto query = _db->createQuery();
    auto sucess = query->exec(SELECT_ALL_ACCOUNTS);
    if (!sucess) {
        _lastError = _db->lastError().text();
        LOG(INFO) << "Error retrieving the accounts " << _lastError.toStdString();
        _db->close();
        return accs;
    }

    // SELECT_ALL_ACCOUNTS = "SELECT uuid, name, memo, amount FROM Accounts";
    // therefore:
    // index 0 => uuid
    // index 1 => name
    // index 2 => memo
    // index 3 => amount
    // using indexes is more efficient than strings
    while (query->next()) {
        auto uuid = QUuid(query->value(0).toString());
        auto name = query->value(1).toString();
        auto memo = query->value(2).toString();
        auto amount = query->value(3).toString().toDouble();
        auto current = std::make_shared<Account>(name, amount, memo);
        current->_dbId = uuid;
        accs.append(current);
    }

    _db->close();

    return accs;
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
    auto sucess = query->exec(SELECT_ALL_CATEGORIES);

    if (!sucess) {
        _lastError = _db->lastError().text();
        LOG(INFO) << "Error retrieving the categories " << _lastError.toStdString();
        _db->close();
        QList<CategoryPtr> cats;
        return cats;
    }


    // SELECT_ALL_CATEGORIES = "SELECT uuid, parent, name, type FROM Categories";
    // therefore
    // index 0 => uuid
    // index 1 => parent
    // index 2 => name
    // index 3 => type
    // is more efficient to use indexes that strings
    while (query->next()) {
        auto uuid = QUuid(query->value(0).toString());
        auto parentVar = query->value(1);
        auto name = query->value(2).toString();
        auto type = static_cast<Category::Type>(query->value(3).toInt());
        auto cat = std::make_shared<Category>(name, type);
        cat->_dbId = uuid;
        LOG(INFO) << "Category found with id " << cat->_dbId.toString().toStdString();
        catsMap[cat->_dbId] = cat;

        if(!parentVar.isNull()) {
            auto parent = QUuid(parentVar.toString());
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

    _db->close();

    return catsMap.values();
}

QList<TransactionPtr>
Book::parseTransactions(std::shared_ptr<system::Query> query) {
    QList<TransactionPtr> trans;

    // accounts are categories are usually repeated so we can keep a map to just create them the first time the appear
    // in the inner join
    QMap<QUuid, AccountPtr> accounts;
    QMap<QUuid, CategoryPtr> categories;

    auto success = query->exec();
    if (!success) {
        _lastError = _db->lastError().text();
        LOG(INFO) << "Error retrieving the transactions " << _lastError.toStdString();
        return trans;
    }

    while (query->next()) {
        auto transUuid = QUuid(query->value(0).toString());
        auto transAmount = query->value(1).toString().toDouble();
        auto accUuid = QUuid(query->value(2).toString());
        auto catUuid = QUuid(query->value(3).toString());
        auto transDay = query->value(4).toInt();
        auto transMonth = query->value(5).toInt();
        auto transYear = query->value(6).toInt();
        auto transContents = query->value(7).toString();
        auto transMemo = query->value(8).toString();

        CategoryPtr category;
        if (categories.contains(catUuid)) {
            category = categories[catUuid];
        } else {
            // ignore the parent
            // auto catParent = query->value(9).toString();
            auto catName = query->value(10).toString();
            auto catType = static_cast<Category::Type>(query->value(11).toInt());
            category = std::make_shared<Category>(catName, catType);
            category->_dbId = catUuid;
            // add it to be faster with other transactions with the same category
            categories[catUuid] = category;
        }

        AccountPtr account;
        if (accounts.contains(accUuid)) {
            account = accounts[accUuid];
        } else {
            auto accName = query->value(12).toString();
            auto accMemo = query->value(13).toString();
            auto accAmount = query->value(14).toString().toDouble();
            account = std::make_shared<Account>(accName, accAmount, accMemo);
            account->_dbId = accUuid;
            accounts[accUuid] = account;
        }

        auto transaction = std::make_shared<Transaction>(
                account, transAmount, category, QDate(transYear, transMonth, transDay), transContents, transMemo);
        transaction->_dbId = transUuid;
        trans.append(transaction);
    }
    return trans;
}

QList<TransactionPtr>
Book::transactions(int moth, int year) {
    QList<TransactionPtr> trans;
    bool opened = _db->open();

    if (!opened) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return trans;
    }

    // SELECT_TRANSACTIONS_MONTH = "SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month, t.year, t.contents, t.memo,
    //         c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t
    //         INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid
    //         WHERE t.month=:month AND t.year=:year";
    auto query = _db->createQuery();
    query->prepare(SELECT_TRANSACTIONS_MONTH);
    query->bindValue(":month", moth);
    query->bindValue(":year", year);

    // executes the query and parses the result
    trans = parseTransactions(query);

    _db->close();

    return trans;
}

QList<TransactionPtr>
Book::transactions(CategoryPtr cat) {
    QList<TransactionPtr> trans;

    if (!cat->wasStoredInDb()) {
        LOG(INFO) << "Returning empty list because category was not stored.";
        return  trans;
    }
    bool opened = _db->open();

    if (!opened) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return trans;
    }


    // SELECT_TRANSACTIONS_CATEGORY = SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month,
    //    t.year, t.contents, t.memo, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t
    //    INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid
    //    WHERE t.category=:category;
    auto query = _db->createQuery();
    query->prepare(SELECT_TRANSACTIONS_CATEGORY);
    query->bindValue(":category", cat->_dbId.toString());

    // executes the query and parses the result
    trans = parseTransactions(query);

    _db->close();

    return trans;
}

QList<TransactionPtr>
Book::transactions(CategoryPtr cat, int month, int year) {
    QList<TransactionPtr> trans;

    if (!cat->wasStoredInDb()) {
        LOG(INFO) << "Returning empty list because category was not stored.";
        return  trans;
    }
    bool opened = _db->open();

    if (!opened) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return trans;
    }


    // SELECT_TRANSACTIONS_CATEGORY_MONTH = "SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month,
    //     t.year, t.contents, t.memo, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t
    //     INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid
    //     WHERE t.category=:category AND t.month=:month AND t.year=:year
    auto query = _db->createQuery();
    query->prepare(SELECT_TRANSACTIONS_CATEGORY_MONTH);
    query->bindValue(":category", cat->_dbId.toString());
    query->bindValue(":month", month);
    query->bindValue(":year", year);

    // executes the query and parses the result
    trans = parseTransactions(query);

    _db->close();

    return trans;
}

QList<TransactionPtr>
Book::transactions(AccountPtr acc) {
    QList<TransactionPtr> trans;

    if (!acc->wasStoredInDb()) {
        LOG(INFO) << "Returning empty list because account was not stored.";
        return  trans;
    }
    bool opened = _db->open();

    if (!opened) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return trans;
    }


    // SELECT_TRANSACTIONS_ACCOUNT = "SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month,
    //     t.year, t.contents, t.memo, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t
    //     INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid
    //     WHERE t.account=:account;
    auto query = _db->createQuery();
    query->prepare(SELECT_TRANSACTIONS_ACCOUNT);
    query->bindValue(":account", acc->_dbId.toString());

    // executes the query and parses the result
    trans = parseTransactions(query);

    _db->close();

    return trans;
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
