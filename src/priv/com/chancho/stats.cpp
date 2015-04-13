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

#include <com/chancho/system/database_lock.h>
#include <com/chancho/system/database_factory.h>

#include "book.h"
#include "stats.h"

namespace {
    const QString SELECT_ACCOUNT_MONTHS_FOR_YEAR = "SELECT month, month_amount FROM AccountsMonthTotal "\
        "WHERE account=:account AND year=:year ORDER BY month ASC";
    const QString SELECT_OCURRENCES_FOR_MONTH = "SELECT c.uuid AS uuid, c.name AS name, c.type AS type, "\
        "c.color AS color, COUNT(*) AS occurrences, SSUM(t.amount) FROM Transactions AS t INNER JOIN Categories AS c WHERE "\
        "t.category=c.uuid AND t.year=:year AND t.month=:month GROUP BY t.category";
    const QString SELECT_OCURRENCES_FOR_CATEGORY = "SELECT month, sum(amount) FROM Transactions "\
        "WHERE year=:year AND category=:category GROUP BY category, month ORDER BY month ASC";
}

namespace com {

namespace chancho {


Stats::Stats() {
    auto dbPath = Book::databasePath();
    _db = system::DatabaseFactory::instance()->addDatabase("QSQLITE", "STATS");
    _db->setDatabaseName(dbPath);
}

Stats::Stats(std::shared_ptr<system::Database> db):
    _db(db) {

}

Stats::~Stats() {
}


QList<double>
Stats::monthsTotalForAccount(AccountPtr acc, int year) {
    QList<double> amounts;

    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);

    if (!dbLock.opened()) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return amounts;
    }

    auto query = _db->createQuery();

    // SELECT_ACCOUNT_MONTHS_FOR_YEAR = SELECT month, month_amount FROM AccountsMonthTotal
    //     WHERE account=:account ORDER BY month ASC;
    query->prepare(SELECT_ACCOUNT_MONTHS_FOR_YEAR);
    DLOG(INFO) << "Query is " << SELECT_ACCOUNT_MONTHS_FOR_YEAR.toStdString();
    query->bindValue(":account", acc->_dbId);
    query->bindValue(":year", year);

    auto sucess = query->exec();
    if (!sucess) {
        _lastError = _db->lastError().text();
        DLOG(INFO) << "Error retrieving the amounts " << _lastError.toStdString();
        return amounts;
    }

    // index 0 => month
    // index 1 => month_amount

    // keep track of the month, we do that by starting an index if the next month is not the expected
    // one, we append as many 0 as needed
    auto index = 0;
    while (query->next()) {
        auto month = query->value(0).toInt();
        auto currentAmount = query->value(1).toString().toDouble();
        DLOG(INFO) << "Month is " << month << " with amount " << currentAmount;
        if (month != (index + 1)) {
            // we need to add 0 until we get there
            while(index != month -1) {
                amounts.append(0.0);
                index++;
            }
        }
        amounts.append(currentAmount);
        index = month;
    }

    if (amounts.count() != 12) {
        DLOG(INFO) << "We just have " << amounts.count() << " instead of 12";
        while(amounts.count() < 12) {
            amounts.append(0);
        }
    }
    return amounts;
}

QPair<Stats::CategoryPercentageTotal, QList<Stats::CategoryPercentage>>
Stats::categoryPercentages(int month, int year) {
    CategoryPercentageTotal total;
    QList<CategoryPercentage> list;
    QPair<CategoryPercentageTotal, QList<CategoryPercentage>> result(total, list);

    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);

    if (!dbLock.opened()) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return result;
    }

    auto query = _db->createQuery();

    // SELECT_OCURRENCES_FOR_MONTH = SELECT c.uuid AS uuid, c.name AS name, c.type AS type,
    //     c.color AS color, COUNT(*) AS occurrences, SSUM(t.amount) FROM Transactions AS t INNER JOIN Categories AS c WHERE
    //     t.category=c.uuid AND t.year=:year AND t.month=:month GROUP BY t.category
    query->prepare(SELECT_OCURRENCES_FOR_MONTH);
    query->bindValue(":month", month);
    query->bindValue(":year", year);

    auto sucess = query->exec();
    if (!sucess) {
        _lastError = _db->lastError().text();
        DLOG(INFO) << "Error retrieving the amounts " << _lastError.toStdString();
        return result;
    }

    // index 0 => uuid
    // index 1 => name
    // index 2 => type
    // index 3 => color
    // index 4 => occurrences
    // index 5 => amount
    while (query->next()) {
        auto uuid = QUuid(query->value(0).toString());
        auto name = query->value(1).toString();
        auto type = static_cast<Category::Type>(query->value(2).toInt());
        auto color = query->value(3).toString();
        auto count = query->value(4).toInt();
        auto amount = query->value(5).toString().toDouble();
        auto cat = std::make_shared<Category>(name, type, color);
        cat->_dbId = uuid;
        CategoryPercentage percentage {cat, count, amount};
        result.first.amount += amount;
        result.second.append(percentage);
    }

    // set the total info
    result.first.count = result.second.count();

    return result;
}

QList<double>
Stats::monthsTotalForCategory(CategoryPtr cat, int year) {
    QList<double> result;

    system::DatabaseLock<std::shared_ptr<system::Database>> dbLock(_db);

    if (!dbLock.opened()) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return result;
    }

    auto query = _db->createQuery();

    // SELECT_OCURRENCES_FOR_CATEGORY = SELECT month, sum(amount) FROM Transactions
    //    WHERE year=:year AND category=:category GROUP BY category, month ORDER BY month ASC"
    query->prepare(SELECT_OCURRENCES_FOR_CATEGORY);
    query->bindValue(":category", cat->_dbId);
    query->bindValue(":year", year);

    auto sucess = query->exec();
    if (!sucess) {
        _lastError = _db->lastError().text();
        DLOG(INFO) << "Error retrieving the amounts " << _lastError.toStdString();
        return result;
    }

    // index 0 => month
    // index 1 => month_amount

    // keep track of the month, we do that by starting an index if the next month is not the expected
    // one, we append as many 0 as needed
    auto index = 0;
    while (query->next()) {
        auto month = query->value(0).toInt();
        auto currentAmount = query->value(1).toString().toDouble();
        if (currentAmount < -1) {
            currentAmount = -1 * currentAmount;
        }
        DLOG(INFO) << "Month is " << month << " with amount " << currentAmount;
        if (month != (index + 1)) {
            // we need to add 0 until we get there
            while(index != month -1) {
                result.append(0.0);
                index++;
            }
        }
        result.append(currentAmount);
        index = month;
    }

    if (result.count() != 12) {
        DLOG(INFO) << "We just have " << result.count() << " instead of 12";
        while(result.count() < 12) {
            result.append(0);
        }
    }

    return result;
}

bool
Stats::isError() {
    return _lastError != QString::null;
}

QString
Stats::lastError() {
    return _lastError;
}

}

}
