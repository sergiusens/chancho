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

#include "accounts.h"

namespace com {

namespace chancho {

namespace book {

namespace {
    const QString INSERT_UPDATE_ACCOUNT = "INSERT OR REPLACE INTO Accounts(uuid, name, memo, color, initialAmount, amount) " \
        "VALUES (:uuid, :name, :memo, :color, :initialAmount, :amount)";
    const QString SELECT_ALL_ACCOUNTS = "SELECT uuid, name, memo, color, initialAmount, amount FROM Accounts ORDER BY name ASC";
    const QString SELECT_ALL_ACCOUNTS_LIMIT = "SELECT uuid, name, memo, color, initialAmount, amount FROM Accounts ORDER BY name ASC "\
        "LIMIT :limit OFFSET :offset";
    const QString DELETE_ACCOUNT = "DELETE FROM Accounts WHERE uuid=:uuid";
    const QString SELECT_ACCOUNTS_COUNT = "SELECT count(*) FROM Accounts";
}

Accounts::Accounts(system::DatabasePtr db)
    : _db(db) {
}

bool
Accounts::storeSingleAcc(AccountPtr acc) {
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
Accounts::store(AccountPtr acc) {
    storeSingleAcc(acc);
}

void
Accounts::store(QList<AccountPtr> accs) {
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

void
Accounts::remove(AccountPtr acc) {
    if (acc->_dbId.isNull()) {
        LOG(ERROR) << "Cannot delete account '" << acc->name.toStdString()
        << "' with a NULL id";
        _lastError = "Cannot delete Account that was not added to the db";
        return;
    }

    auto query = _db->createQuery();
    query->prepare(DELETE_ACCOUNT);
    query->bindValue(":uuid", acc->_dbId.toString());
    auto success = query->exec();

    if (!success) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        return;
    }

    acc->_dbId = QUuid();
}

QList<AccountPtr>
Accounts::accounts(boost::optional<int> limit, boost::optional<int> offset) {
    QList<AccountPtr> accs;

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
Accounts::numberOfAccounts() {
    int count = -1;
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

bool
Accounts::isError() {
    return _lastError != QString::null;
}

QString
Accounts::lastError() {
    return _lastError;
}

}
}
}
