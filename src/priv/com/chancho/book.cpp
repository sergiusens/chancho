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

#include <com/chancho/system/database_lock.h>
#include <com/chancho/system/database_factory.h>

#include "stats.h"
#include "book.h"

namespace com {

namespace chancho {

namespace {
    const QString DATABASE_NAME = "chancho.db";
    const QString ACCOUNTS_TABLE = "CREATE TABLE IF NOT EXISTS Accounts("\
        "uuid VARCHAR(40) PRIMARY KEY, "\
        "name TEXT NOT NULL,"\
        "memo TEXT,"\
        "color VARCHAR(7),"\
        "initialAmount TEXT,"\
        "amount TEXT)";  // amounts are stored in text so that we can use the most precise number
    const QString CATEGORIES_TABLE = "CREATE TABLE IF NOT EXISTS Categories("\
        "uuid VARCHAR(40) PRIMARY KEY, "\
        "parent VARCHAR(40), "\
        "name TEXT NOT NULL,"\
        "type INT,"\
        "color VARCHAR(7),"\
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
    const QString RECURRENT_TRANSACTION_TABLE = "CREATE TABLE IF NOT EXISTS RecurrentTransactions("\
        "uuid VARCHAR(40) PRIMARY KEY, "\
        "amount TEXT,"\
        "account VARCHAR(40) NOT NULL, "\
        "category VARCHAR(40) NOT NULL, "\
        "contents TEXT, "\
        "memo TEXT, "\
        "startDay INT, "\
        "startMonth INT, "\
        "startYear INT, "\
        "lastDay INT, "\
        "lastMonth INT, "\
        "lastYear INT, "\
        "endDay INT, "\
        "endMonth INT, "\
        "endYear INT, "\
        "defaultType INT, "\
        "numberDays INT, "\
        "occurrences INT, "\
        "FOREIGN KEY(account) REFERENCES Accounts(uuid), "\
        "FOREIGN KEY(category) REFERENCES Categories(uuid))";  // amounts are stored in text so that we can used the most precise number
    const QString TRANSACTION_INSERT_TRIGGER = "CREATE TRIGGER UpdateAccountAmountOnTransactionInsert AFTER INSERT ON Transactions "\
        "BEGIN "\
        "UPDATE Accounts SET amount=AddStringNumbers(amount, new.amount) WHERE uuid=new.account; "\
        "END";  // AddStringNumbers is an extension added by the application to the db
    const QString TRANSACTION_UPDATE_SAME_ACCOUNT_TRIGGER = "CREATE TRIGGER UpdateAccountAmountOnTransactionUpdate AFTER UPDATE ON Transactions "\
        "WHEN old.account = new.account BEGIN "\
        "UPDATE Accounts SET amount=AddStringNumbers(SubtractStringNumbers(amount, old.amount), new.amount) WHERE uuid=new.account; "\
        "END";
    const QString TRANSACTION_UPDATE_DIFF_ACCOUNT_TRIGGER = "CREATE TRIGGER UpdateMoveAccountAmountOnTransactionUpdate AFTER UPDATE ON Transactions "\
        "WHEN old.account != new.account BEGIN "\
        "UPDATE Accounts SET amount=SubtractStringNumbers(amount, old.amount) WHERE uuid=old.account; "\
        "UPDATE Accounts SET amount=AddStringNumbers(amount, new.amount) WHERE uuid=new.account; "\
        "END";
    const QString TRANSACTION_DELETE_TRIGGER = "CREATE TRIGGER UpdateAccountAmountOnTransactionDelete AFTER DELETE ON Transactions "\
        "BEGIN "\
        "UPDATE Accounts SET amount=SubtractStringNumbers(amount, old.amount) WHERE uuid=old.account; "\
        "END";
    const QString ACCOUNT_DELETE_TRIGGER = "CREATE TRIGGER DeleteTransactionsOnAccountDelete BEFORE DELETE ON Accounts "\
        "BEGIN "\
        "DELETE FROM Transactions WHERE account=old.uuid; "\
        "END";
    const QString CATEGORY_DELETE_TRIGGER = "CREATE TRIGGER DeleteTransactionsOnCategoryDelete BEFORE DELETE ON Categories "\
        "BEGIN "\
        "DELETE FROM Transactions WHERE category=old.uuid; "\
        "END";
    const QString CATEGORY_UPDATE_DIFF_TYPE_TRIGGER = "CREATE TRIGGER UpdateTransactionsOnCategoryTypeUpdate AFTER UPDATE ON Categories "\
        "WHEN old.type != new.type BEGIN "\
        "UPDATE Transactions SET amount=NegateStringNumber(amount) WHERE category=new.uuid;"
        "END";
    const QString TRANSACTION_MONTH_INDEX = "CREATE INDEX transaction_month_index ON Transactions(year, month);";
    const QString TRANSACTION_DAY_INDEX = "CREATE INDEX transaction_day_index ON Transactions(day, year, month);";
    const QString TRANSACTION_CATEGORY_INDEX = "CREATE INDEX transaction_category_index ON Transactions(category);";
    const QString TRANSACTION_CATEGORY_MONTH_INDEX = "CREATE INDEX transaction_category_month_index ON Transactions(category, year, month);";
    const QString TRANSACTION_ACCOUNT_INDEX = "CREATE INDEX transaction_account_index ON Transactions(account);";
    const QString INSERT_UPDATE_ACCOUNT = "INSERT OR REPLACE INTO Accounts(uuid, name, memo, color, initialAmount, amount) " \
        "VALUES (:uuid, :name, :memo, :color, :initialAmount, :amount)";
    const QString INSERT_CATEGORY = "INSERT INTO Categories(uuid, parent, name, type, color) " \
        "VALUES (:uuid, :parent, :name, :type, :color)";
    const QString UPDATE_CATEGORY = "UPDATE Categories SET parent=:parent, name=:name, type=:type, color=:color " \
        "WHERE uuid=:uuid";
    const QString INSERT_TRANSACTION = "INSERT INTO Transactions(uuid, amount, account, category, "\
        "day, month, year, contents, memo) VALUES (:uuid, :amount, :account, :category, :day, :month, :year, :contents, :memo)";
    const QString UPDATE_TRANSACTION = "UPDATE Transactions SET amount=:amount, account=:account, category=:category, "\
        "day=:day, month=:month, year=:year, contents=:contents, memo=:memo WHERE uuid=:uuid";
    const QString INSERT_UPDATE_RECURRENT_TRANSACTION = "INSERT OR REPLACE INTO RecurrentTransactions("
        "uuid, amount, account, category, contents, memo, startDay, startMonth, startYear, lastDay, lastMonth, lastYear, "\
        "endDay, endMonth, endYear, defaultType, numberDays, occurrences) "\
        "VALUES(:uuid, :amount, :account, :category, :contents, :memo, :startDay, :startMonth, :startYear, :lastDay, "\
        ":lastMonth, :lastYear, :endDay, :endMonth, :endYear, :defaultType, :numberDays, :occurrences)";
    const QString DELETE_ACCOUNT = "DELETE FROM Accounts WHERE uuid=:uuid";
    const QString DELETE_CHILD_CATEGORIES = "DELETE FROM Categories WHERE parent=:uuid";
    const QString DELETE_CATEGORY = "DELETE FROM Categories WHERE uuid=:uuid";
    const QString DELETE_TRANSACTION = "DELETE FROM Transactions WHERE uuid=:uuid";
    const QString SELECT_ALL_ACCOUNTS = "SELECT uuid, name, memo, color, initialAmount, amount FROM Accounts ORDER BY name ASC";
    const QString SELECT_ALL_ACCOUNTS_LIMIT = "SELECT uuid, name, memo, color, initialAmount, amount FROM Accounts ORDER BY name ASC "\
        "LIMIT :limit OFFSET :offset";
    const QString SELECT_ACCOUNTS_COUNT = "SELECT count(*) FROM Accounts";
    const QString SELECT_ALL_CATEGORIES = "SELECT uuid, parent, name, type, color FROM Categories ORDER BY name ASC";
    const QString SELECT_ALL_CATEGORIES_LIMIT = "SELECT uuid, parent, name, type, color FROM Categories ORDER BY name ASC "\
        "LIMIT :limit OFFSET :offset";
    const QString SELECT_ALL_CATEGORIES_TYPE = "SELECT uuid, parent, name, type, color FROM Categories WHERE type=:type "\
        "ORDER BY name ASC";
    const QString SELECT_ALL_CATEGORIES_TYPE_LIMIT = "SELECT uuid, parent, name, type, color FROM Categories "
        "WHERE type=:type ORDER BY name ASC LIMIT :limit OFFSET :offset";
    const QString SELECT_CATEGORIES_COUNT = "SELECT count(*) FROM Categories";
    const QString SELECT_CATEGORIES_COUNT_TYPE = "SELECT count(*) FROM Categories WHERE type=:type";
    const QString SELECT_TRANSACTIONS_MONTH = "SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month, "\
        "t.year, t.contents, t.memo, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t "\
        "INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid "\
        "WHERE t.month=:month AND t.year=:year ORDER BY t.year, t.month";
    const QString SELECT_TRANSACTIONS_MONTH_LIMIT = "SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month, "\
        "t.year, t.contents, t.memo, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t "\
        "INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid "\
        "WHERE t.month=:month AND t.year=:year ORDER BY t.year, t.month LIMIT :limit OFFSET :offset" ;
    const QString SELECT_TRANSACTIONS_COUNT = "SELECT count(uuid) FROM Transactions";
    const QString SELECT_TRANSACTIONS_MONTH_COUNT = "SELECT count(uuid) FROM Transactions "\
        "WHERE month=:month AND year=:year";
    const QString SELECT_TRANSACTIONS_DAY = "SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month, "\
        "t.year, t.contents, t.memo, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t "\
        "INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid "\
        "WHERE t.day=:day AND t.month=:month AND t.year=:year ORDER BY t.day, t.year, t.month";
    const QString SELECT_TRANSACTIONS_DAY_LIMIT = "SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month, "\
        "t.year, t.contents, t.memo, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t "\
        "INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid "\
        "WHERE t.day=:day AND t.month=:month AND t.year=:year ORDER BY t.year, t.month LIMIT :limit OFFSET :offset" ;
    const QString SELECT_TRANSACTIONS_DAY_COUNT = "SELECT count(uuid) FROM Transactions "\
        "WHERE day=:day AND month=:month AND year=:year";
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
    const QString SELECT_MONTHS_WITH_TRANSACTIONS = "SELECT DISTINCT month FROM Transactions WHERE year=:year "\
        "ORDER BY month DESC";
    const QString SELECT_MONTHS_WITH_TRANSACTIONS_LIMIT = "SELECT DISTINCT month FROM Transactions WHERE year=:year "\
            "ORDER BY month DESC LIMIT :limit OFFSET :offset";
    const QString SELECT_MONTHS_WITH_TRANSACTIONS_COUNT = "SELECT COUNT(DISTINCT month) FROM Transactions WHERE year=:year";
    const QString SELECT_DAYS_WITH_TRANSACTIONS = "SELECT DISTINCT day FROM Transactions "\
        "WHERE year=:year AND month=:month ORDER BY day DESC";
    const QString SELECT_DAYS_WITH_TRANSACTIONS_LIMIT = "SELECT DISTINCT day FROM Transactions "\
        "WHERE year=:year AND month=:month ORDER BY day DESC LIMIT :limit OFFSET :offset";
    const QString SELECT_DAYS_WITH_TRANSACTIONS_COUNT = "SELECT COUNT(DISTINCT day) FROM Transactions "\
        "WHERE year=:year AND month=:month";
    const QString SELECT_DAY_CATEGORY_TYPE_SUM = "SELECT SSUM(t.amount) FROM Transactions AS t "\
        "INNER JOIN Categories AS c ON t.category = c.uuid  WHERE c.type=:type AND t.day=:day AND "\
        "t.month=:month AND t.year=:year";
    // views that are used by the stats, this are present to simplify the select statements
    const QString ACCOUNT_MONTH_TOTAL_VIEW = "CREATE VIEW AccountsMonthTotal AS "\
        "SELECT account, SSUM(amount) AS month_amount, month, year from Transactions GROUP BY account, month, year";
    const QString FOREIGN_KEY_SUPPORT = "PRAGMA foreign_keys = ON";

}

double Book::DB_VERSION = 0.1;
std::set<QString> Book::TABLES {"accounts", "categories", "transactions", "recurrenttransactions"};

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

    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(db);
    if (!dbLock.opened()) {
        LOG(ERROR) << "Could not open database to initialize it " << db->lastError().text().toStdString();
        return;
    }

    // once the db has been created we need to check that it has the correct version and the required tables
    auto tables = db->tables();

    // only has to be init those dbs with less than 3 tables, the old versions
    bool hasToBeInit = tables.count() < 3;

    // all versions of the application do not have the recurrent transactions table
    bool needsRecurrenceUpdate = !tables.contains("recurrenttransactions", Qt::CaseInsensitive);

    if (!hasToBeInit && needsRecurrenceUpdate) {
        db->transaction();

        bool success = true;
        auto query = db->createQuery();
        success &= query->exec(RECURRENT_TRANSACTION_TABLE);

        if (success)
            db->commit();
        else
            db->rollback();

        if (!success)
            LOG(ERROR) << "Could not update the chancho db " << db->lastError().text().toStdString();

    } else if (hasToBeInit) {
        LOG(INFO) << "The database needs to be init";
        db->transaction();

        // create the required tables and indexes
        bool success = true;
        auto query = db->createQuery();
        success &= query->exec(FOREIGN_KEY_SUPPORT);
        success &= query->exec(ACCOUNTS_TABLE);
        success &= query->exec(CATEGORIES_TABLE);
        success &= query->exec(TRANSACTION_TABLE);
        success &= query->exec(RECURRENT_TRANSACTION_TABLE);
        success &= query->exec(TRANSACTION_INSERT_TRIGGER);
        success &= query->exec(TRANSACTION_UPDATE_SAME_ACCOUNT_TRIGGER);
        success &= query->exec(TRANSACTION_UPDATE_DIFF_ACCOUNT_TRIGGER);
        success &= query->exec(TRANSACTION_DELETE_TRIGGER);
        success &= query->exec(CATEGORY_DELETE_TRIGGER);
        success &= query->exec(ACCOUNT_DELETE_TRIGGER);
        success &= query->exec(CATEGORY_UPDATE_DIFF_TYPE_TRIGGER);
        success &= query->exec(TRANSACTION_MONTH_INDEX);
        success &= query->exec(TRANSACTION_DAY_INDEX);
        success &= query->exec(TRANSACTION_CATEGORY_INDEX);
        success &= query->exec(TRANSACTION_CATEGORY_MONTH_INDEX);
        success &= query->exec(TRANSACTION_ACCOUNT_INDEX);
        success &= query->exec(ACCOUNT_MONTH_TOTAL_VIEW);

        if (success)
            db->commit();
        else
            db->rollback();

        if (!success)
            LOG(ERROR) << "Could not create the chancho db " << db->lastError().text().toStdString();
    }
}


Book::Book() {
    auto dbPath = Book::databasePath();
    _db = system::DatabaseFactory::instance()->addDatabase("QSQLITE", "BOOKS");
    _db->setDatabaseName(dbPath);
}

Book::~Book() {
}

bool
Book::storeSingleAcc(AccountPtr acc) {
    // if a category is already present in the db the uuid will be present else is null
    if (acc->_dbId.isNull()) {
        acc->_dbId = QUuid::createUuid();
    }

    // INSERT_UPDATE_ACCOUNT = INSERT OR REPLACE INTO Accounts(uuid, name, memo, color, initialAmount, amount)
    //     VALUES (:uuid, :name, :memo, :color, :initialAmount, :amount)
    auto query = _db->createQuery();
    query->prepare(INSERT_UPDATE_ACCOUNT);
    query->bindValue(":uuid", acc->_dbId.toString());
    query->bindValue(":name", acc->name);
    query->bindValue(":memo", acc->memo);
    query->bindValue(":color", acc->color);
    query->bindValue(":initialAmount", QString::number(acc->initialAmount));
    query->bindValue(":amount", QString::number(acc->amount));

    // no need to use a transaction since is a single insert
    auto success = query->exec();
    if (!success) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        acc->_dbId = QUuid();
    }
    return success;
}

void
Book::store(AccountPtr acc) {
    std::lock_guard<std::mutex> lock(_accountsMutex);
    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);
    if (!dbLock.opened()) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return;
    }
    storeSingleAcc(acc);
}

void
Book::store(QList<AccountPtr> accs) {
    std::lock_guard<std::mutex> lock(_accountsMutex);
    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);

    if (!dbLock.opened()) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return;
    }

    bool transaction = _db->transaction();
    if (!transaction) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << "Error creating the transaction " << _lastError.toStdString();
        return;
    }

    foreach(const AccountPtr& acc, accs) {
        bool success = storeSingleAcc(acc);
        if (!success) {
            _db->rollback();
            return;
        }
    }
    _db->commit();
}

bool
Book::storeSingleCat(CategoryPtr cat) {
    auto isPresent = cat->wasStoredInDb();
    if (!isPresent) {
        cat->_dbId = QUuid::createUuid();
    }

    // create the command and execute it
    auto query = _db->createQuery();
    if (!isPresent) {
        query->prepare(INSERT_CATEGORY);
    } else {
        query->prepare(UPDATE_CATEGORY);
    }
    query->bindValue(":uuid", cat->_dbId.toString());
    query->bindValue(":name", cat->name);
    query->bindValue(":type", static_cast<int>(cat->type));
    query->bindValue(":color", cat->color);

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
    return success;
}

void
Book::store(CategoryPtr cat) {
    // if the cate has a parent and was not stored, we store it
    if (cat->parent && !cat->parent->wasStoredInDb()) {
        LOG(INFO) << "Storing parent that was not present already";
        store(cat->parent);
    }

    std::lock_guard<std::mutex> lock(_categoriesMutex);
    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);

    if (!dbLock.opened()) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return;
    }
    storeSingleCat(cat);
}

void
Book::store(QList<CategoryPtr> cats) {
    // grab all parents, add the to the list of cats to store
    QList<CategoryPtr> parents;
    foreach(const CategoryPtr& cat, cats) {
        if (cat->parent && !cat->parent->wasStoredInDb()) {
            parents.append(cat->parent);
        }
    }

    std::lock_guard<std::mutex> lock(_categoriesMutex);
    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);

    if (!dbLock.opened()) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return;
    }

    bool transaction = _db->transaction();
    if (!transaction) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << "Error creating the transaction " << _lastError.toStdString();
        return;
    }

    foreach(const CategoryPtr& cat, parents) {
        bool success = storeSingleCat(cat);
        if (!success) {
            _db->rollback();
            return;;
        }
    }

    foreach(const CategoryPtr& cat, cats) {
        bool success = storeSingleCat(cat);
        if (!success) {
            _db->rollback();
            return;;
        }
    }
    _db->commit();
}

bool
Book::storeSingleTransactions(TransactionPtr tran) {
    // usually accounts and categories must be stored before storing a transactions
    if (tran->account && !tran->account->wasStoredInDb()) {
        _lastError = "An account must be stored before adding a transaction to it.";
        LOG(ERROR) << _lastError.toStdString();
        return false;
    }

    if (tran->category && !tran->category->wasStoredInDb()) {
        _lastError = "A category must be stored before adding a transaction to it.";
        LOG(ERROR) << _lastError.toStdString();
        return false;
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
    return success;
}

void
Book::store(TransactionPtr tran) {
    std::lock_guard<std::mutex> lock(_transactionMutex);
    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);

    if (!dbLock.opened()) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return;
    }
    storeSingleTransactions(tran);
}

void
Book::store(QList<TransactionPtr> trans) {
    std::lock_guard<std::mutex> lock(_transactionMutex);
    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);

    if (!dbLock.opened()) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return;
    }

    bool transaction = _db->transaction();
    if (!transaction) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << "Error creating the transaction " << _lastError.toStdString();
        return;
    }

    foreach(const TransactionPtr tran, trans) {
        auto success = storeSingleTransactions(tran);
        if (!success) {
            _db->rollback();
            return;;
        }
    }
    _db->commit();
}

bool
Book::storeSingleRecurrentTransactions(RecurrentTransactionPtr recurrent) {

    // usually accounts and categories must be stored before storing a transactions
    if (recurrent->transaction->account && !recurrent->transaction->account->wasStoredInDb()) {
        _lastError = "An account must be stored before adding a transaction to it.";
        LOG(ERROR) << _lastError.toStdString();
        return false;
    }

    if (recurrent->transaction->category && !recurrent->transaction->category->wasStoredInDb()) {
        _lastError = "A category must be stored before adding a transaction to it.";
        LOG(ERROR) << _lastError.toStdString();
        return false;
    }

    auto isPresent = recurrent->wasStoredInDb();
    if (!isPresent) {
        recurrent->_dbId = QUuid::createUuid();
    }


    // store the data for the specific info

    // INSERT_UPDATE_RECURRENT_TRANSACTION = INSERT OR REPLACE INTO RecurrentTransactions(
    //    uuid, amount, account, category, contents, memo, startDay, startMonth, startYear, lastDay, lastMonth, lastYear,
    //    endDay, endMonth, endYear, defaultType, numberDays, occurrences)
    //    VALUES(:uuid, :amount, :account, :category, :contents, :memo, :startDay, :startMonth, :startYear, :lastDay,
    //    :lastMonth, :lastYear, :endDay, :endMonth, :endYear, :defaultType, :numberDays, :occurrences
    auto query = _db->createQuery();
    query->prepare(INSERT_UPDATE_RECURRENT_TRANSACTION);
    query->bindValue(":uuid", recurrent->_dbId.toString());
    query->bindValue(":amount", QString::number(recurrent->transaction->amount));
    query->bindValue(":account", recurrent->transaction->account->_dbId);
    query->bindValue(":category", recurrent->transaction->category->_dbId);
    query->bindValue(":contents", recurrent->transaction->contents);
    query->bindValue(":memo", recurrent->transaction->memo);
    query->bindValue(":startDay", recurrent->transaction->date.day());
    query->bindValue(":startMonth", recurrent->transaction->date.month());
    query->bindValue(":startYear", recurrent->transaction->date.year());


    if (recurrent->recurrence->lastGenerated.isValid()) {
        query->bindValue(":lastDay", recurrent->recurrence->lastGenerated.day());
        query->bindValue(":lastMonth", recurrent->recurrence->lastGenerated.month());
        query->bindValue(":lastYear", recurrent->recurrence->lastGenerated.year());
    } else {
        query->bindValue(":lastDay", QVariant());
        query->bindValue(":lastMonth", QVariant());
        query->bindValue(":lastYear", QVariant());
    }

    if (recurrent->recurrence->endDate.isValid()) {
        query->bindValue(":endDay", recurrent->recurrence->endDate.day());
        query->bindValue(":endMonth", recurrent->recurrence->endDate.month());
        query->bindValue(":endYear", recurrent->recurrence->endDate.year());
    } else {
        query->bindValue(":endDay", QVariant());
        query->bindValue(":endMonth", QVariant());
        query->bindValue(":endYear", QVariant());
    }

    if (recurrent->recurrence->_defaults) {
        query->bindValue(":defaultType", static_cast<int>(*recurrent->recurrence->_defaults));
    } else {
        query->bindValue(":defaultType", QVariant());
    }

    if (recurrent->recurrence->_numberOfDays) {
        query->bindValue(":numberDays", *recurrent->recurrence->_numberOfDays);
    } else {
        query->bindValue(":numberDays", QVariant());
    }

    if (recurrent->recurrence->occurrences) {
        query->bindValue(":occurrences", *recurrent->recurrence->occurrences);
    } else {
        query->bindValue(":occurrences", QVariant());
    }

    // no need to use a transaction since is a single insert
    auto stored = query->exec();
    if (!stored) {
        _lastError = _db->lastError().text();
        LOG(INFO) << _lastError.toStdString();
        recurrent->_dbId = QUuid();
    }
    return stored;
}

void
Book::store(RecurrentTransactionPtr tran) {
    std::lock_guard<std::mutex> lock(_transactionMutex);
    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);

    if (!dbLock.opened()) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return;
    }

    // a transaction needs to be created because we are storing in two different tables
    bool transaction = _db->transaction();
    if (!transaction) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << "Error creating the transaction " << _lastError.toStdString();
        return;
    }
    auto success = storeSingleRecurrentTransactions(tran);
    if (!success) {
        _db->rollback();
        return;;
    }
    _db->commit();
}

void
Book::store(QList<RecurrentTransactionPtr> trans) {
    std::lock_guard<std::mutex> lock(_transactionMutex);
    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);

    if (!dbLock.opened()) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return;
    }

    bool transaction = _db->transaction();
    if (!transaction) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << "Error creating the transaction " << _lastError.toStdString();
        return;
    }

    foreach(const RecurrentTransactionPtr& tran, trans) {
        auto success = storeSingleRecurrentTransactions(tran);
        if (!success) {
            _db->rollback();
            return;;
        }
    }
    _db->commit();
}

void
Book::remove(AccountPtr acc) {
    std::lock_guard<std::mutex> lock(_accountsMutex);
    if (acc->_dbId.isNull()) {
        LOG(ERROR) << "Cannot delete account '" << acc->name.toStdString()
                << "' with a NULL id";
        _lastError = "Cannot delete Account that was not added to the db";
        return;
    }

    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);

    if (!dbLock.opened()) {
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
}

void
Book::remove(CategoryPtr cat) {
    std::lock_guard<std::mutex> lock(_categoriesMutex);
    if (cat->_dbId.isNull()) {
        LOG(ERROR) << "Cannot delete category '" << cat->name.toStdString()
                << "' with a NULL id";
        _lastError = "Cannot delete Category that was not added to the db";
        return;
    }

    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);

    if (!dbLock.opened()) {
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
}

void
Book::remove(TransactionPtr tran) {
    std::lock_guard<std::mutex> lock(_transactionMutex);
    if (tran->_dbId.isNull()) {
        LOG(ERROR) << "Cannot delete transaction with a NULL id";
        _lastError = "Cannot delete Account that was not added to the db";
        return;
    }

    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);

    if (!dbLock.opened()) {
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
}

QList<AccountPtr>
Book::accounts(boost::optional<int> limit, boost::optional<int> offset) {
    std::lock_guard<std::mutex> lock(_accountsMutex);
    QList<AccountPtr> accs;

    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);

    if (!dbLock.opened()) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return accs;
    }

    auto query = _db->createQuery();
    if (limit) {
        // SELECT_ALL_ACCOUNTS_LIMIT = SELECT uuid, name, memo, color, initialAmount, amount FROM Accounts ORDER BY name ASC
        //     LIMIT :limit OFFSET :offset;
        query->prepare(SELECT_ALL_ACCOUNTS_LIMIT);
        query->bindValue(":limit", *limit);

        if (offset) {
            query->bindValue(":offset", *offset);
        } else {
            query->bindValue(":offset", 0);
        }
    } else {
        // SELECT_ALL_ACCOUNTS = SELECT uuid, name, memo, color, initialAmount, amount FROM Accounts ORDER BY name ASC
        query->prepare(SELECT_ALL_ACCOUNTS);
    }

    auto sucess = query->exec();
    if (!sucess) {
        _lastError = _db->lastError().text();
        LOG(INFO) << "Error retrieving the accounts " << _lastError.toStdString();
        return accs;
    }

    // index 0 => uuid
    // index 1 => name
    // index 2 => memo
    // index 3 => color
    // index 4 => initialAmount
    // index 5 => amount
    // using indexes is more efficient than strings
    while (query->next()) {
        auto uuid = QUuid(query->value(0).toString());
        auto name = query->value(1).toString();
        auto memo = query->value(2).toString();
        auto color = query->value(3).toString();
        auto initialAmount =query->value(4).toString().toDouble();
        auto amount = query->value(5).toString().toDouble();
        auto current = std::make_shared<Account>(name, amount, memo, color);
        current->initialAmount = initialAmount;
        current->_dbId = uuid;
        accs.append(current);
    }

    return accs;
}

int
Book::numberOfAccounts() {
    std::lock_guard<std::mutex> lock(_accountsMutex);
    int count = -1;

    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);

    if (!dbLock.opened()) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return count;
    }

    auto query = _db->createQuery();
    // SELECT_ACCOUNTS_COUNT = "SELECT count(*) FROM Accounts";
    query->prepare(SELECT_ACCOUNTS_COUNT);
    auto success = query->exec();

    if (!success) {
        _lastError = _db->lastError().text();
        LOG(INFO) << "Error retrieving the transactions count" << _lastError.toStdString();
    } else if (query->next()) {
        count = query->value(0).toInt();
    }

    return count;
}


QList<CategoryPtr>
Book::categories(boost::optional<Category::Type> type, boost::optional<int> limit, boost::optional<int> offset) {
    std::lock_guard<std::mutex> lock(_categoriesMutex);
    QMap<QUuid, QList<QUuid>> parentChildMap;
    QMap<QUuid, CategoryPtr> catsMap;
    QList<QUuid> orderedIds;

    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);

    if (!dbLock.opened()) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        QList<CategoryPtr> cats;
        return cats;
    }

    auto query = _db->createQuery();

    if (type) {
        if (limit) {
            // SELECT_ALL_CATEGORIES_TYPE_LIMIT = "SELECT uuid, parent, name, type FROM Categories ORDER BY name ASC
            //     WHERE type=:type LIMIT :limit OFFSET :offset
            query->prepare(SELECT_ALL_CATEGORIES_TYPE_LIMIT);
            query->bindValue(":type", static_cast<int>(*type));
            query->bindValue(":limit", *limit);

            if (offset) {
                query->bindValue(":offset", *offset);
            } else {
                query->bindValue(":offset", 0);
            }
        } else {
            // SELECT_ALL_CATEGORIES_TYPE = SELECT uuid, parent, name, type FROM Categories ORDER BY name ASC
            // WHERE type=:type
            query->prepare(SELECT_ALL_CATEGORIES_TYPE);
            query->bindValue(":type", static_cast<int>(*type));
        }
    } else {
        if (limit) {
            // SELECT_ALL_CATEGORIES_LIMIT = SELECT uuid, parent, name, type FROM Categories ORDER BY name ASC
            //     LIMIT :limit OFFSET :offset;
            query->prepare(SELECT_ALL_CATEGORIES_LIMIT);
            query->bindValue(":limit", *limit);

            if (offset) {
                query->bindValue(":offset", *offset);
            } else {
                query->bindValue(":offset", 0);
            }
        } else {
            query->prepare(SELECT_ALL_CATEGORIES);
        }
    }

    auto success = query->exec();

    if (!success) {
        _lastError = _db->lastError().text();
        LOG(INFO) << "Error retrieving the categories " << _lastError.toStdString();
        QList<CategoryPtr> cats;
        return cats;
    }


    // SELECT_ALL_CATEGORIES = "SELECT uuid, parent, name, type, color FROM Categories ORDER BY name ASC";
    // therefore
    // index 0 => uuid
    // index 1 => parent
    // index 2 => name
    // index 3 => type
    // index 4 => color
    // is more efficient to use indexes that strings
    while (query->next()) {
        auto uuid = QUuid(query->value(0).toString());
        auto parentVar = query->value(1);
        auto name = query->value(2).toString();
        auto type = static_cast<Category::Type>(query->value(3).toInt());
        auto color = query->value(4).toString();
        auto cat = std::make_shared<Category>(name, type, color);
        cat->_dbId = uuid;
        DLOG(INFO) << "Category found with id " << cat->_dbId.toString().toStdString();
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
            DLOG(INFO) << "Parent not found";
        }
        orderedIds.append(uuid);
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

    // QMap and QHash will not return the results in the right order, we have the QList of uuids to get them in
    // the order that the select returned
    QList<CategoryPtr> orderedCats;
    foreach(const QUuid& id, orderedIds) {
            orderedCats.append(catsMap[id]);
    }
    return orderedCats;
}

int
Book::numberOfCategories(boost::optional<Category::Type> type) {
    std::lock_guard<std::mutex> lock(_categoriesMutex);
    int count = -1;

    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);

    if (!dbLock.opened()) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return count;
    }

    auto query = _db->createQuery();

    if (type) {
        // SELECT_CATEGORIES_COUNT_TYPE = SELECT count(*) FROM Categories WHERE type=:type;
        query->prepare(SELECT_CATEGORIES_COUNT_TYPE);
        query->bindValue(":type", static_cast<int>(*type));
    } else {
        // SELECT_CATEGORIES_COUNT = "SELECT count(*) FROM Categories";
        query->prepare(SELECT_CATEGORIES_COUNT);
    }

    auto success = query->exec();

    if (!success) {
        _lastError = _db->lastError().text();
        LOG(INFO) << "Error retrieving the transactions count" << _lastError.toStdString();
    } else if (query->next()) {
        count = query->value(0).toInt();
    }

    return count;
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
Book::transactions(int year, int month, boost::optional<int> day, boost::optional<int> limit,
        boost::optional<int> offset) {
    std::lock_guard<std::mutex> lock(_transactionMutex);
    QList<TransactionPtr> trans;
    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);

    if (!dbLock.opened()) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return trans;
    }

    auto query = _db->createQuery();
    if (day) {
        if (limit) {
            // SELECT_TRANSACTIONS_DAY_LIMIT = SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month,
            //     t.year, t.contents, t.memo, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t
            //     INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid
            //     WHERE t.day=:day AND t.month=:month AND t.year=:year ORDER BY t.year, t.month LIMIT :limit OFFSET :offset
            query->prepare(SELECT_TRANSACTIONS_DAY_LIMIT);
            query->bindValue(":month", month);
            query->bindValue(":year", year);
            query->bindValue(":day", *day);
            query->bindValue(":limit", *limit);
            if (offset) {
                query->bindValue(":offset", *offset);
            } else {
                query->bindValue(":offset", 0);
            }
        } else {
            // SELECT_TRANSACTIONS_DAY = SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month,
            //     t.year, t.contents, t.memo, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t
            //     INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid
            //     WHERE t.day=:day AND t.month=:month AND t.year=:year ORDER BY t.day, t.year, t.month
            query->prepare(SELECT_TRANSACTIONS_DAY);
            query->bindValue(":month", month);
            query->bindValue(":year", year);
            query->bindValue(":day", *day);
        }
    } else {
        if (limit) {
            // SELECT_TRANSACTIONS_MONTH_LIMIT = "SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month,
            //    t.year, t.contents, t.memo, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t
            //    INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid
            //    WHERE t.month=:month AND t.year=:year ORDER BY t.year, t.month LIMIT :limit OFFSET :offset
            query->prepare(SELECT_TRANSACTIONS_MONTH_LIMIT);
            query->bindValue(":month", month);
            query->bindValue(":year", year);
            query->bindValue(":limit", *limit);
            if (offset) {
                query->bindValue(":offset", *offset);
            } else {
                query->bindValue(":offset", 0);
            }

        } else {
            // SELECT_TRANSACTIONS_MONTH = "SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month, t.year, t.contents, t.memo,
            //         c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t
            //         INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid
            //         WHERE t.month=:month AND t.year=:year";
            query->prepare(SELECT_TRANSACTIONS_MONTH);
            query->bindValue(":month", month);
            query->bindValue(":year", year);
        }
    }
    // executes the query and parses the result
    trans = parseTransactions(query);
    return trans;

}

int
Book::numberOfTransactions() {
    std::lock_guard<std::mutex> lock(_transactionMutex);
    int count = -1;

    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);

    if (!dbLock.opened()) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return count;
    }

    auto query = _db->createQuery();
    query->prepare(SELECT_TRANSACTIONS_COUNT);
    auto success = query->exec();

    if (!success) {
        _lastError = _db->lastError().text();
        LOG(INFO) << "Error retrieving the transactions count" << _lastError.toStdString();
    } else if (query->next()) {
        count = query->value(0).toInt();
    }

    return count;
}

int
Book::numberOfTransactions(int month, int year) {
    std::lock_guard<std::mutex> lock(_transactionMutex);
    int count = -1;

    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);

    if (!dbLock.opened()) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return count;
    }

    auto query = _db->createQuery();
    query->prepare(SELECT_TRANSACTIONS_MONTH_COUNT);
    query->bindValue(":month", month);
    query->bindValue(":year", year);
    auto success = query->exec();

    if (!success) {
        _lastError = _db->lastError().text();
        LOG(INFO) << "Error retrieving the transactions count" << _lastError.toStdString();
    } else if (query->next()) {
        count = query->value(0).toInt();
    }

    return count;
}

int
Book::numberOfTransactions(int day, int month, int year) {
    std::lock_guard<std::mutex> lock(_transactionMutex);
    int count = -1;

    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);

    if (!dbLock.opened()) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << "Error opening database " << _lastError.toStdString();
        LOG(ERROR) << QSqlDatabase::drivers().join(" ").toStdString();
        return count;
    }

    auto query = _db->createQuery();
    query->prepare(SELECT_TRANSACTIONS_DAY_COUNT);
    query->bindValue(":day", day);
    query->bindValue(":month", month);
    query->bindValue(":year", year);
    auto success = query->exec();

    if (!success) {
        _lastError = _db->lastError().text();
        LOG(INFO) << "Error retrieving the transactions count" << _lastError.toStdString();
    } else if (query->next()) {
        count = query->value(0).toInt();
    }

    return count;
}

QList<TransactionPtr>
Book::transactions(CategoryPtr cat, boost::optional<int> month, boost::optional<int> year) {
    std::lock_guard<std::mutex> lock(_transactionMutex);
    QList<TransactionPtr> trans;

    if (!cat->wasStoredInDb()) {
        LOG(INFO) << "Returning empty list because category was not stored.";
        return  trans;
    }
    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);

    if (!dbLock.opened()) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return trans;
    }

    auto query = _db->createQuery();

    if (month && year) {
        // SELECT_TRANSACTIONS_CATEGORY_MONTH = "SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month,
        //     t.year, t.contents, t.memo, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t
        //     INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid
        //     WHERE t.category=:category AND t.month=:month AND t.year=:year
        query->prepare(SELECT_TRANSACTIONS_CATEGORY_MONTH);
        query->bindValue(":category", cat->_dbId.toString());
        query->bindValue(":month", *month);
        query->bindValue(":year", *year);
    } else {
        // SELECT_TRANSACTIONS_CATEGORY = SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month,
        //    t.year, t.contents, t.memo, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t
        //    INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid
        //    WHERE t.category=:category;
        query->prepare(SELECT_TRANSACTIONS_CATEGORY);
        query->bindValue(":category", cat->_dbId.toString());
    }

    // executes the query and parses the result
    trans = parseTransactions(query);

    return trans;
}

QList<TransactionPtr>
Book::transactions(AccountPtr acc) {
    std::lock_guard<std::mutex> lock(_transactionMutex);
    QList<TransactionPtr> trans;

    if (!acc->wasStoredInDb()) {
        LOG(INFO) << "Returning empty list because account was not stored.";
        return  trans;
    }
    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);

    if (!dbLock.opened()) {
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

    return trans;
}

QList<int>
Book::monthsWithTransactions(int year, boost::optional<int> limit, boost::optional<int> offset) {
    QList<int> result;
    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);
    if (!dbLock.opened()) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return result;
    }

    // SELECT_MONTHS_WITH_TRANSACTIONS = "SELECT DISTINCT month FROM Transactions WHERE year=:year
    //     ORDER BY month DESC";
    auto query = _db->createQuery();
    // if limit is present, use it, else just get all of the transactions
    if(limit) {
        query->prepare(SELECT_MONTHS_WITH_TRANSACTIONS_LIMIT);
        query->bindValue(":year", year);
        query->bindValue(":limit", *limit);
        if(offset) {
            query->bindValue(":offset", *offset);
        } else {
            query->bindValue(":offset", 0);
        }
    } else {
        query->prepare(SELECT_MONTHS_WITH_TRANSACTIONS);
        query->bindValue(":year", year);
    }
    auto success = query->exec();

    if (!success) {
        _lastError = _db->lastError().text();
        LOG(INFO) << "Error retrieving the montsh with transactions" << _lastError.toStdString();
        return result;
    }
    while (query->next()) {
        result.append(query->value(0).toInt());
    }

    return result;
}

int
Book::numberOfMonthsWithTransactions(int year) {
    std::lock_guard<std::mutex> lock(_transactionMutex);
    int count = -1;

    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);

    if (!dbLock.opened()) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return count;
    }

    // SELECT_MONTHS_WITH_TRANSACTIONS_COUNT = SELECT count(DISTINCT month) FROM Transactions WHERE year=:year
    auto query = _db->createQuery();
    query->prepare(SELECT_MONTHS_WITH_TRANSACTIONS_COUNT);
    query->bindValue(":year", year);
    auto success = query->exec();

    if (!success) {
        _lastError = _db->lastError().text();
        LOG(INFO) << "Error retrieving the months count" << _lastError.toStdString();
    } else if (query->next()) {
        count = query->value(0).toInt();
    }

    return count;
}

QList<int>
Book::daysWithTransactions(int month, int year, boost::optional<int> limit, boost::optional<int> offset) {
    QList<int> result;
    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);
    if (!dbLock.opened()) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return result;
    }

    auto query = _db->createQuery();
    if (limit) {
        // SELECT_DAYS_WITH_TRANSACTIONS_LIMIT = "SELECT DISTINCT day FROM Transactions
        //    WHERE year=:year AND month=:month ORDER BY day DESC LIMIT :limit OFFSET :offset
        query->prepare(SELECT_DAYS_WITH_TRANSACTIONS_LIMIT);
        query->bindValue(":month", month);
        query->bindValue(":year", year);
        query->bindValue(":limit", *limit);

        if (offset) {
            query->bindValue(":offset", *offset);
        } else {
            query->bindValue(":offset", 0);
        }
    } else {
        // SELECT_DAYS_WITH_TRANSACTIONS = "SELECT DISTINCT day FROM Transactions
        //    WHERE year=:year AND month=:month ORDER BY day;
        query->prepare(SELECT_DAYS_WITH_TRANSACTIONS );
        query->bindValue(":month", month);
        query->bindValue(":year", year);
    }

    auto success = query->exec();

    if (!success) {
        _lastError = _db->lastError().text();
        LOG(INFO) << "Error retrieving the montsh with transactions" << _lastError.toStdString();
        return result;
    }
    while (query->next()) {
        result.append(query->value(0).toInt());
    }

    return result;
}

int
Book::numberOfDaysWithTransactions(int month, int year) {
    std::lock_guard<std::mutex> lock(_transactionMutex);
    int count = -1;

    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);

    if (!dbLock.opened()) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return count;
    }

    // SELECT_DAYS_WITH_TRANSACTIONS_COUNT = SELECT COUNT(DISTINCT day) FROM Transactions
    //    WHERE year=:year AND month=:month
    auto query = _db->createQuery();
    query->prepare(SELECT_DAYS_WITH_TRANSACTIONS_COUNT);
    query->bindValue(":month", month);
    query->bindValue(":year", year);
    auto success = query->exec();

    if (!success) {
        _lastError = _db->lastError().text();
        LOG(INFO) << "Error retrieving the months count" << _lastError.toStdString();
    } else if (query->next()) {
        count = query->value(0).toInt();
    }

    return count;
}

double
Book::amountForTypeInDay(int day, int month, int year, Category::Type type) {
    std::lock_guard<std::mutex> lock(_transactionMutex);
    double result = 0;

    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);

    if (!dbLock.opened()) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return result;
    }

    //SELECT_DAY_CATEGORY_TYPE_SUM = SELECT SSUM(t.amount) FROM Transactions AS t
    //    INNER JOIN Categories AS c ON t.category = c.uuid  WHERE c.type=:type AND t.day=:day AND
    //    t.month=:month AND t.year=:year
    // SSUM is a custom function and returns a STRING.
    auto query = _db->createQuery();
    query->prepare(SELECT_DAY_CATEGORY_TYPE_SUM);
    query->bindValue(":day", day);
    query->bindValue(":month", month);
    query->bindValue(":year", year);
    query->bindValue(":type", static_cast<int>(type));
    auto success = query->exec();

    if (!success) {
        _lastError = _db->lastError().text();
        LOG(INFO) << "Error retrieving the amount count" << _lastError.toStdString();
    } else if (query->next()) {
        result = query->value(0).toString().toDouble();
    }

    return result;
}

double
Book::incomeForDay(int day, int month, int year) {
    return amountForTypeInDay(day, month, year, Category::Type::INCOME);
}

double
Book::expenseForDay(int day, int month, int year) {
    return amountForTypeInDay(day, month, year, Category::Type::EXPENSE);
}

std::shared_ptr<Stats>
Book::stats() {
    auto stats = new Stats(_db);
    std::shared_ptr<Stats> result;
    result.reset(stats);
    return result;
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
