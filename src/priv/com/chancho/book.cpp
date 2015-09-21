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

#include <com/chancho/book/accounts.h>
#include <com/chancho/book/categories.h>
#include <com/chancho/book/transactions.h>
#include <com/chancho/system/database_lock.h>
#include <com/chancho/system/database_factory.h>

#include "stats.h"
#include "book.h"
#include "updater.h"

namespace com {

namespace chancho {

const QString Book::SELECT_TRIGGERS = "SELECT name FROM sqlite_master WHERE type = 'trigger'";
const QString Book::VERSION_TABLE = "CREATE TABLE IF NOT EXISTS Versions("\
    "major INT, "\
    "minor INT, "\
    "patch INT, "\
    "PRIMARY KEY (major, minor, patch))";
const QString Book::ACCOUNTS_TABLE = "CREATE TABLE IF NOT EXISTS Accounts("\
    "uuid VARCHAR(40) PRIMARY KEY, "\
    "name TEXT NOT NULL,"\
    "memo TEXT,"\
    "color VARCHAR(7),"\
    "initialAmount TEXT,"\
    "amount TEXT)";  // amounts are stored in text so that we can use the most precise number
const QString Book::CATEGORIES_TABLE = "CREATE TABLE IF NOT EXISTS Categories("\
    "uuid VARCHAR(40) PRIMARY KEY, "\
    "parent VARCHAR(40), "\
    "name TEXT NOT NULL,"\
    "type INT,"\
    "color VARCHAR(7),"\
    "FOREIGN KEY(parent) REFERENCES Categories(uuid))";
const QString Book::TRANSACTION_TABLE = "CREATE TABLE IF NOT EXISTS Transactions("\
    "uuid VARCHAR(40) PRIMARY KEY, "\
    "amount TEXT,"\
    "account VARCHAR(40) NOT NULL, "\
    "category VARCHAR(40) NOT NULL, "\
    "day INT, "\
    "month INT, "\
    "year INT, "\
    "contents TEXT, "\
    "memo TEXT, "\
    "is_recurrent INT, "\
    "FOREIGN KEY(account) REFERENCES Accounts(uuid), "\
    "FOREIGN KEY(category) REFERENCES Categories(uuid))";  // amounts are stored in text so that we can used the most precise number
const QString Book::RECURRENT_TRANSACTION_TABLE = "CREATE TABLE IF NOT EXISTS RecurrentTransactions("\
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
const QString Book::RECURRENT_TRANSACTIONS_RELATIONS_TABLE = "CREATE TABLE IF NOT EXISTS RecurrentTransactionRelations("\
    "recurrent_transaction VARCHAR(40),"\
    "generated_transaction VARCHAR(40),"\
    "FOREIGN KEY(recurrent_transaction) REFERENCES RecurrentTransactions(uuid),"\
    "FOREIGN KEY(generated_transaction) REFERENCES Transactions(uuid),"\
    "PRIMARY KEY(recurrent_transaction, generated_transaction))";
const QString Book::ATTACHMENT_TABLE = "CREATE TABLE IF NOT EXISTS Attachments("\
    "uuid VARCHAR(40) PRIMARY KEY, "\
    "name TEXT, "\
    "data TEXT)";
const QString Book::ATTACHMENT_RELATION_TABLE = "CREATE TABLE IF NOT EXISTS AttachmentTransactionRelations("\
    "singleTransaction VARCHAR(40),"\
    "attachment VARCHAR(40),"\
    "FOREIGN KEY(singleTransaction) REFERENCES Transactions(uuid),"\
    "FOREIGN KEY(attachment) REFERENCES Attachment(uuid),"\
    "PRIMARY KEY(singleTransaction, attachment))";
const QString Book::TRANSACTION_INSERT_TRIGGER = "CREATE TRIGGER UpdateAccountAmountOnTransactionInsert AFTER INSERT ON Transactions "\
    "BEGIN "\
    "UPDATE Accounts SET amount=AddStringNumbers(amount, new.amount) WHERE uuid=new.account; "\
    "END";  // AddStringNumbers is an extension added by the application to the db
const QString Book::TRANSACTION_UPDATE_SAME_ACCOUNT_TRIGGER = "CREATE TRIGGER UpdateAccountAmountOnTransactionUpdate AFTER UPDATE ON Transactions "\
    "WHEN old.account = new.account BEGIN "\
    "UPDATE Accounts SET amount=AddStringNumbers(SubtractStringNumbers(amount, old.amount), new.amount) WHERE uuid=new.account; "\
    "END";
const QString Book::TRANSACTION_UPDATE_DIFF_ACCOUNT_TRIGGER = "CREATE TRIGGER UpdateMoveAccountAmountOnTransactionUpdate AFTER UPDATE ON Transactions "\
    "WHEN old.account != new.account BEGIN "\
    "UPDATE Accounts SET amount=SubtractStringNumbers(amount, old.amount) WHERE uuid=old.account; "\
    "UPDATE Accounts SET amount=AddStringNumbers(amount, new.amount) WHERE uuid=new.account; "\
    "END";
const QString Book::TRANSACTION_DELETE_TRIGGER = "CREATE TRIGGER UpdateAccountAmountOnTransactionDelete AFTER DELETE ON Transactions "\
    "BEGIN "\
    "UPDATE Accounts SET amount=SubtractStringNumbers(amount, old.amount) WHERE uuid=old.account; "\
    "DELETE FROM RecurrentTransactionRelations WHERE generated_transaction=old.uuid; "\
    "DELETE FROM Attachments WHERE uuid IN (SELECT attachment FROM AttachmentTransactionRelations WHERE singleTransaction=old.uuid);"\
    "DELETE FROM AttachmentTransactionRelations WHERE singleTransaction=old.uuid; "\
    "END";
const QString Book::ACCOUNT_DELETE_TRIGGER = "CREATE TRIGGER DeleteTransactionsOnAccountDelete BEFORE DELETE ON Accounts "\
    "BEGIN "\
    "DELETE FROM Transactions WHERE account=old.uuid; "\
    "END";
const QString Book::CATEGORY_DELETE_TRIGGER = "CREATE TRIGGER DeleteTransactionsOnCategoryDelete BEFORE DELETE ON Categories "\
    "BEGIN "\
    "DELETE FROM Transactions WHERE category=old.uuid; "\
    "END";
const QString Book::CATEGORY_UPDATE_DIFF_TYPE_TRIGGER = "CREATE TRIGGER UpdateTransactionsOnCategoryTypeUpdate AFTER UPDATE ON Categories "\
    "WHEN old.type != new.type BEGIN "\
    "UPDATE Transactions SET amount=NegateStringNumber(amount) WHERE category=new.uuid;"
    "END";
const QString Book::RECURRENT_RELATIONS_DELETE_TRIGGER = "CREATE TRIGGER DeleteRecurrentRelationsOnDelete AFTER DELETE ON RecurrentTransactions "\
    "BEGIN "\
    "DELETE FROM RecurrentTransactionRelations WHERE recurrent_transaction=old.uuid; "\
    "END";
const QString Book::RECURRENT_RELATIONS_INSERT_TRIGGER = "CREATE TRIGGER UpdateRecurrentRelationsOnInsert AFTER INSERT ON RecurrentTransactionRelations "\
    "BEGIN "\
    "UPDATE Transactions SET is_recurrent=1 WHERE uuid=new.generated_transaction; "\
    "END";
const QString Book::RECURRENT_RELATIONS_UPDATE_TRIGGER = "CREATE TRIGGER UpdateGeneratedRelationsOnUpdate AFTER UPDATE ON RecurrentTransactions "\
    "BEGIN "\
    "UPDATE Transactions SET amount=new.amount, account=new.account, category=new.category, contents=new.contents, memo=new.memo "\
    "WHERE uuid IN (SELECT generated_transaction FROM RecurrentTransactionRelations WHERE recurrent_transaction=new.uuid);"\
    "END";
const QString Book::ATTACHMENT_DELETE_TRIGGER = "CREATE TRIGGER DeleteAttachmentRelatioOnAttachmentDelete AFTER DELETE ON Attachments "\
    "BEGIN "\
    "DELETE FROM AttachmentTransactionRelations WHERE singleTransaction=old.uuid; "\
    "END";

namespace {
    const QString DATABASE_NAME = "chancho.db";
    const QString ALTER_TRANSACTION_TABLE = "ALTER TABLE Transactions ADD COLUMN is_recurrent int";
    const QString TRANSACTION_MONTH_INDEX = "CREATE INDEX transaction_month_index ON Transactions(year, month);";
    const QString TRANSACTION_DAY_INDEX = "CREATE INDEX transaction_day_index ON Transactions(day, year, month);";
    const QString TRANSACTION_CATEGORY_INDEX = "CREATE INDEX transaction_category_index ON Transactions(category);";
    const QString TRANSACTION_CATEGORY_MONTH_INDEX = "CREATE INDEX transaction_category_month_index ON Transactions(category, year, month);";
    const QString TRANSACTION_ACCOUNT_INDEX = "CREATE INDEX transaction_account_index ON Transactions(account);";
    const QString SELECT_DAY_CATEGORY_TYPE_SUM = "SELECT SSUM(t.amount) FROM Transactions AS t "\
        "INNER JOIN Categories AS c ON t.category = c.uuid  WHERE c.type=:type AND t.day=:day AND "\
        "t.month=:month AND t.year=:year";
    // views that are used by the stats, this are present to simplify the select statements
    const QString ACCOUNT_MONTH_TOTAL_VIEW = "CREATE VIEW AccountsMonthTotal AS "\
        "SELECT account, SSUM(amount) AS month_amount, month, year from Transactions GROUP BY account, month, year";
    const QString FOREIGN_KEY_SUPPORT = "PRAGMA foreign_keys = ON";

}

class BookLock {
 public:

    explicit BookLock(Book* book)
            : _book(book) {
        _book->_dbMutex.lock();
        _opened = _book->_db->open();
    }

    ~BookLock() {
        if (_opened) {
            _book->_db->close();
        }
        _book->_dbMutex.unlock();
    }

    bool opened() const {
        return _opened;
    }

    BookLock(const BookLock&) = delete;
    BookLock& operator=(const BookLock&) = delete;

 private:
    bool _opened = false;
    Book* _book;
};

double Book::DB_VERSION = 0.1;
std::set<QString> Book::TABLES {"accounts", "categories", "transactions", "recurrenttransactions"};

STATIC_INIT(Book) {
    Book::initDatabse();
    Updater updater;
    if (updater.needsUpgrade()) {
        LOG(INFO) << "Database needs to be updated";
        updater.upgrade();
    }
    updater.setDatabaseVersion();
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

QStringList
Book::getTriggers(std::shared_ptr<system::Database> db) {
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

    if (hasToBeInit) {
        LOG(INFO) << "The database needs to be init";
        db->transaction();

        // create the required tables and indexes
        bool success = true;
        auto query = db->createQuery();
        // tables
        success &= query->exec(VERSION_TABLE);
        success &= query->exec(FOREIGN_KEY_SUPPORT);
        success &= query->exec(ACCOUNTS_TABLE);
        success &= query->exec(CATEGORIES_TABLE);
        success &= query->exec(TRANSACTION_TABLE);
        success &= query->exec(RECURRENT_TRANSACTION_TABLE);
        success &= query->exec(RECURRENT_TRANSACTIONS_RELATIONS_TABLE);
        success &= query->exec(ATTACHMENT_TABLE);
        success &= query->exec(ATTACHMENT_RELATION_TABLE);

        // triggers
        success &= query->exec(TRANSACTION_INSERT_TRIGGER);
        success &= query->exec(TRANSACTION_UPDATE_SAME_ACCOUNT_TRIGGER);
        success &= query->exec(TRANSACTION_UPDATE_DIFF_ACCOUNT_TRIGGER);
        success &= query->exec(RECURRENT_RELATIONS_DELETE_TRIGGER);
        success &= query->exec(RECURRENT_RELATIONS_INSERT_TRIGGER);
        success &= query->exec(CATEGORY_DELETE_TRIGGER);
        success &= query->exec(ACCOUNT_DELETE_TRIGGER);
        success &= query->exec(CATEGORY_UPDATE_DIFF_TYPE_TRIGGER);
        success &= query->exec(TRANSACTION_MONTH_INDEX);
        success &= query->exec(TRANSACTION_DAY_INDEX);
        success &= query->exec(TRANSACTION_CATEGORY_INDEX);
        success &= query->exec(TRANSACTION_CATEGORY_MONTH_INDEX);
        success &= query->exec(TRANSACTION_ACCOUNT_INDEX);
        success &= query->exec(ACCOUNT_MONTH_TOTAL_VIEW);
        success &= query->exec(RECURRENT_RELATIONS_UPDATE_TRIGGER);
        success &= query->exec(ATTACHMENT_DELETE_TRIGGER);
        success &= query->exec(TRANSACTION_DELETE_TRIGGER);

        if (success)
            db->commit();
        else
            db->rollback();

        if (!success)
            LOG(ERROR) << "Could not create the chancho db " << db->lastError().text().toStdString();
    }
}

QStringList
Book::tables() {
    static QStringList expected {
            "Versions",
            "Accounts",
            "Categories",
            "Transactions",
            "RecurrentTransactions",
            "RecurrentTransactionRelations",
            "Attachments",
            "AttachmentTransactionRelations"
    };
    return expected;
}

QStringList
Book::triggers() {
    static QStringList expected {
            "UpdateAccountAmountOnTransactionInsert",
            "UpdateAccountAmountOnTransactionUpdate",
            "UpdateMoveAccountAmountOnTransactionUpdate",
            "UpdateAccountAmountOnTransactionDelete",
            "DeleteTransactionsOnAccountDelete",
            "DeleteTransactionsOnCategoryDelete",
            "UpdateTransactionsOnCategoryTypeUpdate",
            "DeleteRecurrentRelationsOnDelete",
            "UpdateRecurrentRelationsOnInsert",
            "UpdateGeneratedRelationsOnUpdate",
            "DeleteAttachmentRelatioOnAttachmentDelete"
    };
    return expected;
}

Book::Book() {
    auto dbPath = Book::databasePath();
    _db = system::DatabaseFactory::instance()->addDatabase("QSQLITE", "BOOKS");
    _db->setDatabaseName(dbPath);
    _accs = std::make_shared<book::Accounts>(_db);
    _cats = std::make_shared<book::Categories>(_db);
    _trans = std::make_shared<book::Transactions>(_db);
}

Book::~Book() {
}

void
Book::store(AccountPtr acc) {
    relay([&]() {
        _accs->store(acc);
        if (_accs->isError()) {
            _lastError = _accs->lastError();
        }
    });
}

void
Book::store(QList<AccountPtr> accs) {
    relay([&]() {
        _accs->store(accs);
        if (_accs->isError()) {
            _lastError = _accs->lastError();
        }
    });
}

void
Book::store(CategoryPtr cat) {
    relay([&]() {
        _cats->store(cat);

        if (_cats->isError()) {
            _lastError = _cats->lastError();
            LOG(ERROR) << _lastError.toStdString();
        }
    });
}

void
Book::store(QList<CategoryPtr> cats) {
    relay([&, this]() {
        _cats->store(cats);
        if (_cats->isError()) {
            _lastError = _cats->lastError();
            LOG(ERROR) << _lastError.toStdString();
        }
    });
}

void
Book::store(TransactionPtr tran) {
    relay([&, this]() {
        _trans->store(tran);
        if (_trans->isError()) {
            _lastError = _trans->lastError();
        }
    });
}

void
Book::store(QList<TransactionPtr> trans) {
    relay([&, this]() {
        _trans->store(trans);
        if (_trans->isError()) {
            _lastError = _trans->lastError();
        }
    });
}

void
Book::store(RecurrentTransactionPtr tran, bool updatePast) {
    relay([&, this]() {
        _trans->store(tran, updatePast);
        if (_trans->isError()) {
            _lastError = _trans->lastError();
        }
    });
}

void
Book::store(QList<RecurrentTransactionPtr> trans) {
    relay([&, this]() {
        _trans->store(trans);
        if (_trans->isError()) {
            _lastError = _trans->lastError();
        }
    });
}

void
Book::remove(AccountPtr acc) {
    relay([&, this]() {
        _accs->remove(acc);
        if (_accs->isError()) {
            _lastError = _accs->lastError();
        }
    });
}

void
Book::remove(CategoryPtr cat) {
    relay([&, this]() {
        _cats->remove(cat);
        if (_cats->isError()) {
            _lastError = _cats->lastError();
        }
    });
}

void
Book::remove(TransactionPtr tran) {
    relay([&, this]() {
        _trans->remove(tran);
        if (_trans->isError()) {
            _lastError = _trans->lastError();
        }
    });
}

void
Book::remove(RecurrentTransactionPtr tran, bool removeGenerated) {
    relay([&, this]() {
        _trans->remove(tran, removeGenerated);
        if (_trans->isError()) {
            _lastError = _trans->lastError();
        }
    });
}

QList<AccountPtr>
Book::accounts(boost::optional<int> limit, boost::optional<int> offset) {
    QList<AccountPtr> accs;
    return relay<QList<AccountPtr>>(accs, [&, this]() {
        auto result = _accs->accounts(limit, offset);

        if (_accs->isError()) {
            _lastError = _accs->lastError();
            return QList<AccountPtr>();
        }
        return result;
    });
}

int
Book::numberOfAccounts() {
    auto defaultResult = -1;
    return relay<int>(defaultResult, [&, this]() {
        auto result = _accs->numberOfAccounts();

        if (_accs->isError()) {
            _lastError = _accs->lastError();
            return -1;
        }
        return result;
    });
}

QList<CategoryPtr>
Book::categories(boost::optional<Category::Type> type, boost::optional<int> limit, boost::optional<int> offset) {
    QList<CategoryPtr> cats;
    return relay<QList<CategoryPtr>>(cats, [&, this]() {
        QList<CategoryPtr> result = _cats->categories(type, limit, offset);
        if (_cats->isError()) {
            _lastError = _cats->lastError();
        }
        return result;
    });
}

int
Book::numberOfCategories(boost::optional<Category::Type> type) {
    int defaultResult = -1;
    return relay<int>(defaultResult, [&, this]() {
        auto count = _cats->numberOfCategories(type);
        if (_cats->isError()) {
            _lastError = _cats->lastError();
        }
        return count;
    });
}

QList<TransactionPtr>
Book::transactions(int year, int month, boost::optional<int> day, boost::optional<int> limit,
        boost::optional<int> offset) {
    QList<TransactionPtr> defaultResult;
    return relay<QList<TransactionPtr>>(defaultResult, [&, this]() {
        auto trans = _trans->transactions(year, month, day, limit, offset);
        if (_trans->isError()) {
            _lastError = _trans->lastError();
        }
        return trans;
    });
}

QList<TransactionPtr>
Book::transactions(RecurrentTransactionPtr recurrent, boost::optional<int> limit, boost::optional<int> offset) {
    QList<TransactionPtr> defaultResult;
    return relay<QList<TransactionPtr>>(defaultResult, [&, this]() {
        auto trans = _trans->transactions(recurrent, limit, offset);
        if (_trans->isError()) {
            _lastError = _trans->lastError();
        }
        return trans;
    });
}

template <class R>
R Book::relay(R defaultResult, std::function<R()> cb) {
    BookLock dbLock(this);

    if (!dbLock.opened()) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return defaultResult;
    }
    return cb();
}

void
Book::relay(std::function<void()> cb) {
    BookLock dbLock(this);

    if (!dbLock.opened()) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return;
    }
    cb();
}

int
Book::numberOfTransactions() {
    int defaultResult = -1;
    return relay<int>(defaultResult, [&, this]() {
        auto count = _trans->numberOfTransactions();
        if (_trans->isError()) {
            _lastError = _trans->lastError();
        }
        return count;
    });
}

int
Book::numberOfTransactions(int month, int year) {
    int defaultResult = -1;
    return relay<int>(defaultResult, [&, this]() {
        auto count = _trans->numberOfTransactions(month, year);
        if (_trans->isError()) {
            _lastError = _trans->lastError();
        }
        return count;
    });
}

int
Book::numberOfTransactions(int day, int month, int year) {
    int defaultResult = -1;
    return relay<int>(defaultResult, [&, this](){
        auto count = _trans->numberOfTransactions(day, month, year);
        if (_trans->isError()) {
            _lastError = _trans->lastError();
        }
        return count;
    });
}

int
Book::numberOfTransactions(RecurrentTransactionPtr recurrent) {
    int defaultResult = -1;
    return relay<int>(defaultResult, [&, this]() {
        auto count = _trans->numberOfTransactions(recurrent);
        if (_trans->isError()) {
            _lastError = _trans->lastError();
        }
        return count;
    });
}

QList<TransactionPtr>
Book::transactions(CategoryPtr cat, boost::optional<int> month, boost::optional<int> year) {
    QList<TransactionPtr> defaultResult;
    return relay<QList<TransactionPtr>>(defaultResult, [&, this]() {
        auto trans = _trans->transactions(cat, month, year);
        if (_trans->isError()) {
            _lastError = _trans->lastError();
        }
        return trans;
    });
}

QList<TransactionPtr>
Book::transactions(AccountPtr acc) {
    QList<TransactionPtr> defaultResult;
    return relay<QList<TransactionPtr>>(defaultResult, [&, this]() {
        auto trans = _trans->transactions(acc);
        if (_trans->isError()) {
            _lastError = _trans->lastError();
        }
        return trans;
    });
}

QList<int>
Book::monthsWithTransactions(int year, boost::optional<int> limit, boost::optional<int> offset) {
    QList<int> defaultResult;
    return relay<QList<int>>(defaultResult, [&, this]() {
        auto result = _trans->monthsWithTransactions(year, limit, offset);
        if (_trans->isError()) {
            _lastError = _trans->lastError();
        }
        return result;
    });
}

int
Book::numberOfMonthsWithTransactions(int year) {
    int defaultResult = -1;
    return relay<int>(defaultResult, [&, this]() {
        auto count = _trans->numberOfMonthsWithTransactions(year);
        if (_trans->isError()) {
            _lastError = _trans->lastError();
        }
        return count;
    });
}

QList<int>
Book::daysWithTransactions(int month, int year, boost::optional<int> limit, boost::optional<int> offset) {
    QList<int> defaultResult;
    return relay<QList<int>>(defaultResult, [&, this]() {
        auto result = _trans->daysWithTransactions(month, year, limit, offset);
        if (_trans->isError()) {
            _lastError = _trans->lastError();
        }
        return result;
    });
}

int
Book::numberOfDaysWithTransactions(int month, int year) {
    int defaultResult = -1;
    return relay<int>(defaultResult, [&, this]() {
        auto count = _trans->numberOfDaysWithTransactions(month, year);
        if (_trans->isError()) {
            _lastError = _trans->lastError();
        }
        return count;
    });
}

QList<RecurrentTransactionPtr>
Book::recurrentTransactions(boost::optional<int> limit, boost::optional<int> offset) {
    QList<RecurrentTransactionPtr> defaultResult;
    return relay<QList<RecurrentTransactionPtr>>(defaultResult, [&, this]() {
        auto result = _trans->recurrentTransactions(limit, offset);
        if (_trans->isError()) {
            _lastError = _trans->lastError();
        }
        return result;
    });
}

QList<RecurrentTransactionPtr>
Book::recurrentTransactions(CategoryPtr cat, boost::optional<int> limit, boost::optional<int> offset) {
    QList<RecurrentTransactionPtr> defaultResult;
    return relay<QList<RecurrentTransactionPtr>>(defaultResult, [&, this]() {
        auto result = _trans->recurrentTransactions(cat, limit, offset);
        if (_trans->isError()) {
            _lastError = _trans->lastError();
        }
        return result;
    });

}

QList<CategoryPtr>
Book::recurrentCategories(boost::optional<int> limit, boost::optional<int> offset) {
    QList<CategoryPtr> defaultResult;
    return relay<QList<CategoryPtr>>(defaultResult, [&, this]() {
        auto result = _cats->recurrentCategories(limit, offset);
        if (_cats->isError()) {
            _lastError = _cats->lastError();
        }
        return result;
    });
}

int
Book::numberOfRecurrentTransactions() {
    int defaultResult = -1;
    return relay<int>(defaultResult, [&, this]() {
        auto count = _trans->numberOfRecurrentTransactions();
        if (_trans->isError()) {
            _trans->lastError();
        }
        return count;
    });
}

int
Book::numberOfRecurrentTransactions(CategoryPtr cat) {
    int defaultResult = -1;
    return relay<int>(defaultResult, [&, this]() {
        auto count = _trans->numberOfRecurrentTransactions(cat);
        if (_trans->isError()) {
            _lastError = _trans->lastError();
        }
        return count;
    });
}

int
Book::numberOfRecurrentCategories() {
    int defaultResult = -1;
    return relay<int>(defaultResult, [&, this]() {
        auto count = _cats->numberOfRecurrentCategories();
        if (_cats->isError()) {
            _lastError = _cats->lastError();
        }
        return count;
    });
}

void
Book::generateRecurrentTransactions() {
    relay([this]() {
        _trans->generateRecurrentTransactions();
        if (_trans->isError()) {
            _lastError = _trans->lastError();
        }
    });
}

double
Book::amountForTypeInDay(int day, int month, int year, Category::Type type) {
    double result = 0;

    BookLock dbLock(this);

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
