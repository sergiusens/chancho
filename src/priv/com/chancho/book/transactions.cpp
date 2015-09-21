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

#include "transactions.h"

namespace com {

namespace chancho {

namespace book {

namespace {
    const QString INSERT_TRANSACTION = "INSERT INTO Transactions(uuid, amount, account, category, "\
        "day, month, year, contents, memo) VALUES (:uuid, :amount, :account, :category, :day, :month, :year, :contents, :memo)";
    const QString UPDATE_TRANSACTION = "UPDATE Transactions SET amount=:amount, account=:account, category=:category, "\
        "day=:day, month=:month, year=:year, contents=:contents, memo=:memo WHERE uuid=:uuid";
    const QString INSERT_UPDATE_RECURRENT_TRANSACTION = "INSERT OR REPLACE INTO RecurrentTransactions("
            "uuid, amount, account, category, contents, memo, startDay, startMonth, startYear, lastDay, lastMonth, lastYear, "\
        "endDay, endMonth, endYear, defaultType, numberDays, occurrences) "\
        "VALUES(:uuid, :amount, :account, :category, :contents, :memo, :startDay, :startMonth, :startYear, :lastDay, "\
        ":lastMonth, :lastYear, :endDay, :endMonth, :endYear, :defaultType, :numberDays, :occurrences)";
    const QString INSERT_UPDATE_RECURRENT_TRANSACTION_RELATION = "INSERT OR REPLACE INTO RecurrentTransactionRelations("\
        "recurrent_transaction, generated_transaction) VALUES(:recurrent_transaction, :generated_transaction)";
    const QString UPDATE_RECURRENT_TRANSACTION = "UPDATE RecurrentTransactions SET "\
        "amount=:amount, account=:account, category=:category, contents=:contents, memo=:memo, "\
        "endDay=:endDay, endMonth=:endMonth, endYear=:endYear WHERE uuid=:uuid";
    const QString INSERT_UPDATE_ATTACHMENT = "INSERT OR REPLACE INTO Attachments(uuid, name, data) "\
        "VALUES (:uuid, :name, :data)";
    const QString INSERT_UPDATE_ATTACHMENT_RELATION = "INSERT OR REPLACE INTO AttachmentTransactionRelations("\
        "singleTransaction, attachment) VALUES (:singleTransaction, :attachment)";
    const QString DELETE_TRANSACTION = "DELETE FROM Transactions WHERE uuid=:uuid";
    const QString DELETE_RECURRENT_TRANSACTION = "DELETE FROM RecurrentTransactions WHERE uuid=:uuid";
    const QString DELETE_RECURRENT_GENERATED = "DELETE FROM Transactions WHERE uuid IN (SELECT generated_transaction FROM "\
        "RecurrentTransactionRelations WHERE recurrent_transaction=:recurrent_Transaction)";
    const QString SELECT_TRANSACTIONS_DAY = "SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month, "\
        "t.year, t.contents, t.memo, t.is_recurrent, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t "\
        "INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid "\
        "WHERE t.day=:day AND t.month=:month AND t.year=:year ORDER BY t.day, t.year, t.month";
    const QString SELECT_TRANSACTIONS_DAY_LIMIT = "SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month, "\
        "t.year, t.contents, t.memo, t.is_recurrent, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t "\
        "INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid "\
        "WHERE t.day=:day AND t.month=:month AND t.year=:year ORDER BY t.year, t.month LIMIT :limit OFFSET :offset" ;
    const QString SELECT_TRANSACTIONS_MONTH = "SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month, "\
        "t.year, t.contents, t.memo, t.is_recurrent, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t "\
        "INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid "\
        "WHERE t.month=:month AND t.year=:year ORDER BY t.year, t.month";
    const QString SELECT_TRANSACTIONS_MONTH_LIMIT = "SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month, "\
        "t.year, t.contents, t.memo, t.is_recurrent, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t "\
        "INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid "\
        "WHERE t.month=:month AND t.year=:year ORDER BY t.year, t.month LIMIT :limit OFFSET :offset" ;
    const QString SELECT_TRANSACTIONS_RECURRENT =  "SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month, "\
        "t.year, t.contents, t.memo, t.is_recurrent, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t "\
        "INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid "\
        "WHERE t.uuid IN (SELECT generated_transaction FROM RecurrentTransactionRelations WHERE "\
        "recurrent_transaction=:recurrent_Transaction) ORDER BY t.year, t.month";
    const QString SELECT_TRANSACTIONS_RECURRENT_LIMIT =  "SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month, "\
        "t.year, t.contents, t.memo, t.is_recurrent, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t "\
        "INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid "\
        "WHERE t.uuid IN (SELECT generated_transaction FROM RecurrentTransactionRelations WHERE "\
        "recurrent_transaction=:recurrent_Transaction) ORDER BY t.year, t.month LIMIT :limit OFFSET :offset";
    const QString SELECT_TRANSACTIONS_COUNT = "SELECT count(uuid) FROM Transactions";
    const QString SELECT_TRANSACTIONS_MONTH_COUNT = "SELECT count(uuid) FROM Transactions "\
        "WHERE month=:month AND year=:year";
    const QString SELECT_TRANSACTIONS_DAY_COUNT = "SELECT count(uuid) FROM Transactions "\
        "WHERE day=:day AND month=:month AND year=:year";
    const QString SELECT_TRANSACTIONS_CATEGORY = "SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month, "\
        "t.year, t.contents, t.memo, t.is_recurrent, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t "\
        "INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid "\
        "WHERE t.category=:category ORDER BY t.year, t.month";
    const QString SELECT_GENERATED_TRANSACTIONS_RECURRENT_COUNT = "SELECT count(*) FROM RecurrentTransactionRelations WHERE "\
        "recurrent_transaction=:recurrent_Transaction";
    const QString SELECT_TRANSACTIONS_CATEGORY_MONTH = "SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month, "\
        "t.year, t.contents, t.memo, t.is_recurrent, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t "\
        "INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid "\
        "WHERE t.category=:category AND t.month=:month AND t.year=:year ORDER BY t.year, t.month";
    const QString SELECT_TRANSACTIONS_ACCOUNT = "SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month, "\
        "t.year, t.contents, t.memo, t.is_recurrent, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t "\
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
    const QString SELECT_RECURRENT_TRANSACTIONS_COUNT = "SELECT count(uuid) FROM RecurrentTransactions";
    const QString SELECT_RECURRENT_TRANSACTIONS_CATEGORY_COUNT = "SELECT count(uuid) FROM RecurrentTransactions "\
        "WHERE category=:category";
    const QString SELECT_RECURRENT_TRANSACTIONS = "SELECT t.uuid, t.amount, t.account, t.category, t.contents, t.memo, "\
        "t.startDay, t.startMonth, t.startYear, t.lastDay, t.lastMonth, t.lastYear, t.endDay, t.endMonth, t.endYear, "\
        "t.defaultType, t.numberDays, t.occurrences, c.parent, c.name, c.type, a.name, a.memo, a.amount "\
        "FROM RecurrentTransactions AS t INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON "\
        "t.account = a.uuid";
    const QString SELECT_RECURRENT_TRANSACTIONS_LIMIT = "SELECT t.uuid, t.amount, t.account, t.category, t.contents, t.memo, "\
        "t.startDay, t.startMonth, t.startYear, t.lastDay, t.lastMonth, t.lastYear, t.endDay, t.endMonth, t.endYear, "\
        "t.defaultType, t.numberDays, t.occurrences, c.parent, c.name, c.type, a.name, a.memo, a.amount "\
        "FROM RecurrentTransactions AS t INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON "\
        "t.account = a.uuid LIMIT :limit OFFSET :offset";
    const QString SELECT_RECURRENT_TRANSACTIONS_CATEGORY = "SELECT t.uuid, t.amount, t.account, t.category, t.contents, t.memo, "\
        "t.startDay, t.startMonth, t.startYear, t.lastDay, t.lastMonth, t.lastYear, t.endDay, t.endMonth, t.endYear, "\
        "t.defaultType, t.numberDays, t.occurrences, c.parent, c.name, c.type, a.name, a.memo, a.amount "\
        "FROM RecurrentTransactions AS t INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON "\
        "t.account = a.uuid WHERE t.category=:category";
    const QString SELECT_RECURRENT_TRANSACTIONS_CATEGORY_LIMIT = "SELECT t.uuid, t.amount, t.account, t.category, t.contents, t.memo, "\
        "t.startDay, t.startMonth, t.startYear, t.lastDay, t.lastMonth, t.lastYear, t.endDay, t.endMonth, t.endYear, "\
        "t.defaultType, t.numberDays, t.occurrences, c.parent, c.name, c.type, a.name, a.memo, a.amount "\
        "FROM RecurrentTransactions AS t INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON "\
        "t.account = a.uuid WHERE t.category=:category LIMIT :limit OFFSET :offset";
}


Transactions::Transactions(system::DatabasePtr db)
    : _db(db){

}

bool
Transactions::storeSingleTransactions(TransactionPtr tran) {
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
        return success;
    }
    if (isPresent) {
        // update the attachments
        // TODO: Update relations etc..
    } else {
        // store the attachments
        foreach(const AttachmentPtr& attachmentPtr, tran->attachments()) {
            success &= storeSingleAttachment(tran, attachmentPtr);
            if (!success) {
                return success;
            }
        }
    }

    return success;
}

void
Transactions::store(TransactionPtr tran) {
    bool transaction = _db->transaction();
    if (!transaction) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << "Error creating the transaction " << _lastError.toStdString();
        return;
    }
    auto success = storeSingleTransactions(tran);
    if (success) {
        _db->commit();
    } else {
        _db->rollback();
    }
}

void
Transactions::store(QList<TransactionPtr> trans) {
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
Transactions::storeSingleRecurrentTransactions(RecurrentTransactionPtr recurrent) {
    DLOG(INFO) << __PRETTY_FUNCTION__;
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

    // amounts are positive yet if it is an expense we must multiple by -1 to update the account accordingly
    if (recurrent->transaction->type() == Category::Type::EXPENSE && recurrent->transaction->amount > 0) {
        query->bindValue(":amount", QString::number(-1 * recurrent->transaction->amount));
    } else {
        query->bindValue(":amount", QString::number(recurrent->transaction->amount));
    }

    query->bindValue(":account", recurrent->transaction->account->_dbId.toString());
    query->bindValue(":category", recurrent->transaction->category->_dbId.toString());
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
Transactions::storeRecurrentNoUpdates(RecurrentTransactionPtr tran) {
    LOG(INFO) << __PRETTY_FUNCTION__;

    // a transaction needs to be created because we are storing in two different tables
    bool transaction = _db->transaction();
    if (!transaction) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << "Error creating the transaction " << _lastError.toStdString();
        return;
    }
    auto success = storeSingleRecurrentTransactions(tran);
    LOG(INFO) << "Recurrent transaction stored " << success;
    if (!success) {
        _db->rollback();
        return;
    }

    if (!tran->transaction->wasStoredInDb()) {
        success = storeSingleTransactions(tran->transaction);
        LOG(INFO) << "Single transaction stored " << success;
        if (!success) {
            _db->rollback();
            return;
        }
    }

    // store the relation between the two
    auto query = _db->createQuery();
    query->prepare(INSERT_UPDATE_RECURRENT_TRANSACTION_RELATION);
    query->bindValue(":recurrent_transaction", tran->_dbId.toString());
    query->bindValue(":generated_transaction", tran->transaction->_dbId.toString());

    success = query->exec();
    if (!success) {
        _db->rollback();
        return;
    }

    _db->commit();
}

void
Transactions::storeRecurrentWithUpdate(RecurrentTransactionPtr recurrent) {
    LOG(INFO) << __PRETTY_FUNCTION__;
    // usually accounts and categories must be stored before storing a transactions
    if (recurrent->transaction->account && !recurrent->transaction->account->wasStoredInDb()) {
        _lastError = "An account must be stored before adding a transaction to it.";
        LOG(ERROR) << _lastError.toStdString();
        return;
    }

    if (recurrent->transaction->category && !recurrent->transaction->category->wasStoredInDb()) {
        _lastError = "A category must be stored before adding a transaction to it.";
        LOG(ERROR) << _lastError.toStdString();
        return;
    }

    // store the data for the specific info
    // UPDATE_RECURRENT_TRANSACTION = UPDATE RecurrentTransactions SET
    //     amount=:amount, account=:account, category=:category, contents=:contents, memo=:memo,
    //     endDay=:endDay, endMonth=:endMonth, endYear=:endYear WHERE uuid=:uuid;
    LOG(INFO) << "Updating recurrent transaction.";
    auto query = _db->createQuery();
    query->prepare(UPDATE_RECURRENT_TRANSACTION);
    query->bindValue(":uuid", recurrent->_dbId.toString());

    // amounts are positive yet if it is an expense we must multiple by -1 to update the account accordingly
    if (recurrent->transaction->type() == Category::Type::EXPENSE && recurrent->transaction->amount > 0) {
        query->bindValue(":amount", QString::number(-1 * recurrent->transaction->amount));
    } else {
        query->bindValue(":amount", QString::number(recurrent->transaction->amount));
    }

    query->bindValue(":account", recurrent->transaction->account->_dbId.toString());
    query->bindValue(":category", recurrent->transaction->category->_dbId.toString());
    query->bindValue(":contents", recurrent->transaction->contents);
    query->bindValue(":memo", recurrent->transaction->memo);

    if (recurrent->recurrence->endDate.isValid()) {
        query->bindValue(":endDay", recurrent->recurrence->endDate.day());
        query->bindValue(":endMonth", recurrent->recurrence->endDate.month());
        query->bindValue(":endYear", recurrent->recurrence->endDate.year());
    } else {
        query->bindValue(":endDay", QVariant());
        query->bindValue(":endMonth", QVariant());
        query->bindValue(":endYear", QVariant());
    }

    // no need to use a transaction since is a single insert
    auto stored = query->exec();
    if (!stored) {
        _lastError = query->lastError().text();
        LOG(INFO) << _lastError.toStdString();
    }
}

bool
Transactions::storeSingleAttachment(const TransactionPtr& tran, const AttachmentPtr& attachment) {
    if (!attachment->wasStoredInDb()) {
        attachment->_dbId = attachment->_id;
    }

    // INSERT_UPDATE_ATTACHMENT = INSERT OR REPLACE INTO Attachments(uuid, name, data)
    //     VALUES (:uuid, :name, :data)
    auto query = _db->createQuery();
    query->prepare(INSERT_UPDATE_ATTACHMENT);
    query->bindValue(":uuid", attachment->_dbId.toString());
    query->bindValue(":name", attachment->name);
    query->bindValue("data", QString::fromUtf8(attachment->data.toBase64().toStdString().c_str()));

    auto success = query->exec();
    if (!success) {
        LOG(INFO) << "Error storing attachment " << query->lastError().text().toStdString();
        return false;
    }

    // INSERT_UPDATE_ATTACHMENT_RELATION = "INSERT OR REPLACE INTO AttachmentTransactionRelations("
    //    "singleTransaction, attachment) VALUES (:singleTransaction, :attachment)";
    query->prepare(INSERT_UPDATE_ATTACHMENT_RELATION);
    query->bindValue(":singleTransaction", tran->_dbId);
    query->bindValue(":attachment", attachment->_dbId);

    success = query->exec();
    if (!success) {
        LOG(INFO) << "Error storing attachment " << query->lastError().text().toStdString();
        return false;
    }
    return true;
}

void
Transactions::store(RecurrentTransactionPtr tran, bool updatePast) {
    if (tran->wasStoredInDb() && updatePast) {
        storeRecurrentWithUpdate(tran);
    } else {
        storeRecurrentNoUpdates(tran);
    }
}

void
Transactions::store(QList<RecurrentTransactionPtr> trans) {
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
Transactions::remove(TransactionPtr tran) {
    if (tran->_dbId.isNull()) {
        LOG(ERROR) << "Cannot delete transaction with a NULL id";
        _lastError = "Cannot delete Account that was not added to the db";
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

void
Transactions::remove(RecurrentTransactionPtr tran, bool removeGenerated) {
    if (tran->_dbId.isNull()) {
        LOG(ERROR) << "Cannot delete transaction with a NULL id";
        _lastError = "Cannot delete Account that was not added to the db";
        return;
    }

    bool success = true;
    _db->transaction();

    auto query = _db->createQuery();
    if (removeGenerated) {
        // DELETE_RECURRENT_GENERATED = DELETE FROM Transactions WHERE uuid IN (SELECT generated_transaction FROM
        //     RecurrentTransactionRelations WHERE recurrent_transaction=:recurrent_Transaction)
        query->prepare(DELETE_RECURRENT_GENERATED);
        query->bindValue(":recurrent_Transaction", tran->_dbId.toString());
        success &= query->exec();
    }

    query->prepare(DELETE_RECURRENT_TRANSACTION);
    query->bindValue(":uuid", tran->_dbId.toString());
    success &= query->exec();

    if (!success) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        _db->rollback();
    }

    _db->commit();
    tran->_dbId = QUuid();
}

QList<TransactionPtr>
Transactions::parseTransactions(std::shared_ptr<system::Query> query) {
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
        auto transRecurrent = (query->value(9).toInt() == 0)?false:true;

        CategoryPtr category;
        if (categories.contains(catUuid)) {
            category = categories[catUuid];
        } else {
            // ignore the parent
            // auto catParent = query->value(9).toString();
            auto catName = query->value(11).toString();
            auto catType = static_cast<Category::Type>(query->value(12).toInt());
            category = std::make_shared<Category>(catName, catType);
            category->_dbId = catUuid;
            // add it to be faster with other transactions with the same category
            categories[catUuid] = category;
        }

        AccountPtr account;
        if (accounts.contains(accUuid)) {
            account = accounts[accUuid];
        } else {
            auto accName = query->value(13).toString();
            auto accMemo = query->value(14).toString();
            auto accAmount = query->value(15).toString().toDouble();
            account = std::make_shared<Account>(accName, accAmount, accMemo);
            account->_dbId = accUuid;
            accounts[accUuid] = account;
        }

        auto transaction = std::make_shared<Transaction>(
                account, transAmount, category, QDate(transYear, transMonth, transDay), transContents, transMemo);
        transaction->_dbId = transUuid;
        transaction->is_recurrent = transRecurrent;
        trans.append(transaction);
    }
    return trans;
}

QList<TransactionPtr>
Transactions::transactions(int year, int month, boost::optional<int> day, boost::optional<int> limit,
                           boost::optional<int> offset) {
    QList<TransactionPtr> trans;
    auto query = _db->createQuery();
    if (day) {
        if (limit) {
            // SELECT_TRANSACTIONS_DAY_LIMIT = SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month,
            //     t.year, t.contents, t.memo, t.is_recurrent, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t
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
            //     t.year, t.contents, t.memo, t.is_recurrent, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t
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
            //    t.year, t.contents, t.memo, t.is_recurrent, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t
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
            // SELECT_TRANSACTIONS_MONTH = "SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month, t.year, t.contents, t.memo, t.is_recurrent,
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

QList<TransactionPtr>
Transactions::transactions(RecurrentTransactionPtr recurrent, boost::optional<int> limit, boost::optional<int> offset) {
    QList<TransactionPtr> trans;
    auto query = _db->createQuery();

    if (limit) {
        // SELECT_TRANSACTIONS_RECURRENT_LIMIT =  SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month,
        //     t.year, t.contents, t.memo, t.is_recurrent, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t
        //     INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid
        //     WHERE t.uuid IN (SELECT generated_transaction FROM RecurrentTransactionRelations WHERE
        //     recurrent_transaction=:recurrent_Transaction) ORDER BY t.year, t.month LIMIT :limit OFFSET :offset
        query->prepare(SELECT_TRANSACTIONS_RECURRENT_LIMIT);
        query->bindValue(":limit", *limit);
        if (offset) {
            query->bindValue(":offset", *offset);
        } else {
            query->bindValue(":offset", 0);
        }
    } else {
        // SELECT_TRANSACTIONS_RECURRENT =  SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month,
        //  t.year, t.contents, t.memo, t.is_recurrent, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t
        //  INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid
        //  WHERE t.uuid IN (SELECT generated_transaction FROM RecurrentTransactionRelations WHERE
        //  recurrent_transaction=:recurrent_Transaction) ORDER BY t.year, t.month
        query->prepare(SELECT_TRANSACTIONS_RECURRENT);
    }
    query->bindValue(":recurrent_Transaction", recurrent->_dbId.toString());

    // executes the query and parses the result
    trans = parseTransactions(query);
    return trans;
}

int
Transactions::numberOfTransactions() {
    int count = -1;

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
Transactions::numberOfTransactions(int month, int year) {
    int count = -1;
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
Transactions::numberOfTransactions(int day, int month, int year) {
    int count = -1;
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

int
Transactions::numberOfTransactions(RecurrentTransactionPtr recurrent) {
    int count = -1;
    auto query = _db->createQuery();

    // SELECT_GENERATED_TRANSACTIONS_RECURRENT_COUNT = SELECT count(*) FROM RecurrentTransactionRelations WHERE
    //     recurrent_transaction=:recurrent_Transaction
    query->prepare(SELECT_GENERATED_TRANSACTIONS_RECURRENT_COUNT);
    query->bindValue(":recurrent_Transaction", recurrent->_dbId.toString());
    auto success = query->exec();
    DLOG(INFO) << "Recurrent id is "  << recurrent->_dbId.toString().toStdString();

    if (!success) {
        _lastError = _db->lastError().text();
        LOG(INFO) << "Error retrieving the transactions count" << _lastError.toStdString();
    } else if (query->next()) {
        count = query->value(0).toInt();
    }

    return count;
}

QList<TransactionPtr>
Transactions::transactions(CategoryPtr cat, boost::optional<int> month, boost::optional<int> year) {
    QList<TransactionPtr> trans;

    if (!cat->wasStoredInDb()) {
        LOG(INFO) << "Returning empty list because category was not stored.";
        return  trans;
    }
    auto query = _db->createQuery();

    if (month && year) {
        // SELECT_TRANSACTIONS_CATEGORY_MONTH = "SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month,
        //     t.year, t.contents, t.memo, t.is_recurrent, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t
        //     INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON t.account = a.uuid
        //     WHERE t.category=:category AND t.month=:month AND t.year=:year
        query->prepare(SELECT_TRANSACTIONS_CATEGORY_MONTH);
        query->bindValue(":category", cat->_dbId.toString());
        query->bindValue(":month", *month);
        query->bindValue(":year", *year);
    } else {
        // SELECT_TRANSACTIONS_CATEGORY = SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month,
        //    t.year, t.contents, t.memo, t.is_recurrent, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t
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
Transactions::transactions(AccountPtr acc) {
    QList<TransactionPtr> trans;

    if (!acc->wasStoredInDb()) {
        LOG(INFO) << "Returning empty list because account was not stored.";
        return  trans;
    }

    // SELECT_TRANSACTIONS_ACCOUNT = "SELECT t.uuid, t.amount, t.account, t.category, t.day, t.month,
    //     t.year, t.contents, t.memo, t.is_recurrent, c.parent, c.name, c.type, a.name, a.memo, a.amount FROM Transactions AS t
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
Transactions::monthsWithTransactions(int year, boost::optional<int> limit, boost::optional<int> offset) {
    QList<int> result;

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
Transactions::numberOfMonthsWithTransactions(int year) {
    int count = -1;

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
Transactions::daysWithTransactions(int month, int year, boost::optional<int> limit, boost::optional<int> offset) {
    QList<int> result;
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
Transactions::numberOfDaysWithTransactions(int month, int year) {
    int count = -1;

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

QList<RecurrentTransactionPtr>
Transactions::parseRecurrentTransactions(std::shared_ptr<system::Query> query) {
    QList<RecurrentTransactionPtr> result;
    // accounts are categories are usually repeated so we can keep a map to just create them the first time the appear
    // in the inner join
    QMap<QUuid, AccountPtr> accounts;
    QMap<QUuid, CategoryPtr> categories;

    auto success = query->exec();
    if (!success) {
        _lastError = _db->lastError().text();
        LOG(WARNING) << "Error retrieving the transactions " << _lastError.toStdString();
        return result;
    }

    // indexes are faster than the use of columns names, here are the relations
    // 0 => t.uuid
    // 1 => t.amount
    // 2 => t.account
    // 3 => t.category
    // 4 => t.contents
    // 5 => t.memo
    // 6 => t.startDay
    // 7 => t.startMonth
    // 8 => t.startYear
    // 9 => t.lastDay
    // 10 => t.lastMonth
    // 11 => t.lastYear
    // 12 => t.endDay
    // 13 => t.endMonth
    // 14 => t.endYear
    // 15 => t.defaultType
    // 16 => t.numberDays
    // 17 => t.occurrences
    // 18 => c.parent
    // 19 => c.name
    // 20 => c.type
    // 21 => a.name
    // 22 => a.memo
    // 23 => a.amount
    while (query->next()) {
        auto transUuid = QUuid(query->value(0).toString());
        auto transAmount = query->value(1).toString().toDouble();
        auto accUuid = QUuid(query->value(2).toString());
        auto catUuid = QUuid(query->value(3).toString());
        auto transContents = query->value(4).toString();
        auto transMemo = query->value(5).toString();

        QDate startDate(query->value(8).toInt(), query->value(7).toInt(), query->value(6).toInt());

        QDate lastGeneratedDay;
        if (query->value(9).isNull()) {  // last date is null if any of the members is null
            lastGeneratedDay = QDate();
        } else {
            DLOG(INFO) << "Setting last generated to " << query->value(11).toInt() << "/"
            << query->value(10).toInt() << "/" << query->value(9).toInt();
            lastGeneratedDay = QDate(query->value(11).toInt(), query->value(10).toInt(), query->value(9).toInt());
        }

        QDate endDate;
        if (query->value(12).isNull()) {  // end date is null ig any of the members is null
            endDate = QDate();
        } else {
            endDate = QDate(query->value(14).toInt(), query->value(13).toInt(), query->value(12).toInt());
        }

        boost::optional<RecurrentTransaction::Recurrence::Defaults> defaults = boost::none;
        if (!query->value(15).isNull()) {
            defaults = static_cast<RecurrentTransaction::Recurrence::Defaults>(query->value(15).toInt());
        }

        boost::optional<int> numberOfDays = boost::none;
        if (!query->value(16).isNull()) {
            numberOfDays = query->value(16).toInt();
        }

        CategoryPtr category;
        if (categories.contains(catUuid)) {
            category = categories[catUuid];
        } else {
            // ignore the parent
            // auto catParent = query->value(9).toString();
            auto catName = query->value(19).toString();
            auto catType = static_cast<Category::Type>(query->value(20).toInt());
            category = std::make_shared<Category>(catName, catType);
            category->_dbId = catUuid;
            // add it to be faster with other transactions with the same category
            categories[catUuid] = category;
        }

        AccountPtr account;
        if (accounts.contains(accUuid)) {
            account = accounts[accUuid];
        } else {
            auto accName = query->value(21).toString();
            auto accMemo = query->value(22).toString();
            auto accAmount = query->value(23).toString().toDouble();
            account = std::make_shared<Account>(accName, accAmount, accMemo);
            account->_dbId = accUuid;
            accounts[accUuid] = account;
        }
        RecurrentTransaction::RecurrencePtr recurrence;
        if (defaults) {
            recurrence = std::make_shared<RecurrentTransaction::Recurrence>(*defaults, startDate, endDate);
        }
        if (numberOfDays) {
            recurrence = std::make_shared<RecurrentTransaction::Recurrence>(*numberOfDays, startDate, endDate);
        }
        recurrence->lastGenerated = lastGeneratedDay;
        if (!query->value(17).isNull()) {
            recurrence->occurrences = query->value(17).toInt();
        }

        auto transaction = std::make_shared<Transaction>(
                account, transAmount, category, startDate, transContents, transMemo);
        auto recurrentTrans = std::make_shared<RecurrentTransaction>(transaction, recurrence);
        recurrentTrans->_dbId = transUuid;
        result.append(recurrentTrans);
    }

    return result;
}


QList<RecurrentTransactionPtr>
Transactions::recurrentTransactions(boost::optional<int> limit, boost::optional<int> offset) {
    QList<RecurrentTransactionPtr> result;

    auto query = _db->createQuery();
    if (limit) {

        // SELECT_RECURRENT_TRANSACTIONS = "SELECT t.uuid, t.amount, t.account, t.category, t.contents, t.memo,
        //     t.startDay, t.startMonth, t.startYear, t.lastDay, t.lastMonth, t.lastYear, t.endDay, t.endMonth, t.endYear,
        //     t.defaultType, t.numberDays, t.occurrences c.parent, c.name, c.type, a.name, a.memo, a.amount
        //     FROM RecurrentTransactions AS t INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON
        //     t.account = a.uuid;
        query->prepare(SELECT_RECURRENT_TRANSACTIONS_LIMIT);
        query->bindValue(":limit", *limit);

        if (offset) {
            query->bindValue(":offset", *offset);
        } else {
            query->bindValue(":offset", 0);
        }
    } else {
        // SELECT_RECURRENT_TRANSACTIONS_LIMIT = "SELECT t.uuid, t.amount, t.account, t.category, t.contents, t.memo,
        //     t.startDay, t.startMonth, t.startYear, t.lastDay, t.lastMonth, t.lastYear, t.endDay, t.endMonth, t.endYear,
        //     t.defaultType, t.numberDays, t.occurrences c.parent, c.name, c.type, a.name, a.memo, a.amount
        //     FROM RecurrentTransactions AS t INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON
        //     t.account = a.uuid LIMIT :limit OFFSET :offset;
        query->prepare(SELECT_RECURRENT_TRANSACTIONS);
    }

    return parseRecurrentTransactions(query);
}

QList<RecurrentTransactionPtr>
Transactions::recurrentTransactions(CategoryPtr cat, boost::optional<int> limit, boost::optional<int> offset) {
    QList<RecurrentTransactionPtr> result;

    auto query = _db->createQuery();
    if (limit) {

        // SELECT t.uuid, t.amount, t.account, t.category, t.contents, t.memo,
        //     t.startDay, t.startMonth, t.startYear, t.lastDay, t.lastMonth, t.lastYear, t.endDay, t.endMonth, t.endYear,
        //     t.defaultType, t.numberDays, t.occurrences, c.parent, c.name, c.type, a.name, a.memo, a.amount
        //     FROM RecurrentTransactions AS t INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON
        //     t.account = a.uuid WHERE t.category=:category LIMIT :limit OFFSET :offset
        query->prepare(SELECT_RECURRENT_TRANSACTIONS_CATEGORY_LIMIT);
        query->bindValue(":limit", *limit);

        if (offset) {
            query->bindValue(":offset", *offset);
        } else {
            query->bindValue(":offset", 0);
        }
    } else {

        // SELECT t.uuid, t.amount, t.account, t.category, t.contents, t.memo,
        //     t.startDay, t.startMonth, t.startYear, t.lastDay, t.lastMonth, t.lastYear, t.endDay, t.endMonth, t.endYear,
        //     t.defaultType, t.numberDays, t.occurrences, c.parent, c.name, c.type, a.name, a.memo, a.amount
        //     FROM RecurrentTransactions AS t INNER JOIN Categories AS c ON t.category = c.uuid INNER JOIN Accounts AS a ON
        //     t.account = a.uuid WHERE t.category=:category
        query->prepare(SELECT_RECURRENT_TRANSACTIONS_CATEGORY);
    }
    query->bindValue(":category", cat->_dbId.toString());

    return parseRecurrentTransactions(query);
}

int
Transactions::numberOfRecurrentTransactions() {
    int count = -1;

    auto query = _db->createQuery();
    query->prepare(SELECT_RECURRENT_TRANSACTIONS_COUNT);
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
Transactions::numberOfRecurrentTransactions(CategoryPtr cat) {
    int count = -1;

    auto query = _db->createQuery();
    // SELECT_RECURRENT_TRANSACTIONS_CATEGORY_COUNT = SELECT count(uuid) FROM RecurrentTransactions
    //     WHERE category=:category
    query->prepare(SELECT_RECURRENT_TRANSACTIONS_CATEGORY_COUNT);
    query->bindValue(":category", cat->_dbId.toString());
    auto success = query->exec();

    if (!success) {
        _lastError = _db->lastError().text();
        LOG(INFO) << "Error retrieving the transactions count" << _lastError.toStdString();
    } else if (query->next()) {
        count = query->value(0).toInt();
    }

    return count;
}

void
Transactions::storeGeneratedTransactions(QMap<RecurrentTransactionPtr, QList<TransactionPtr>> transMap) {
    DLOG(INFO) << __PRETTY_FUNCTION__;

    bool transaction = _db->transaction();
    if (!transaction) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << "Error creating the transaction " << _lastError.toStdString();
        return;
    }

    foreach(const RecurrentTransactionPtr& recurrentTransaction, transMap.keys()) {
        auto trans = transMap[recurrentTransaction];
        foreach(const TransactionPtr tran, trans) {
            auto success = storeSingleTransactions(tran);
            if (!success) {
                _db->rollback();
                return;;
            }
        }

        // we need to store the relations between the recurrent transaction and the generated ones
        auto query = _db->createQuery();
        query->prepare(INSERT_UPDATE_RECURRENT_TRANSACTION_RELATION);

        foreach(const TransactionPtr tran, trans) {
            query->bindValue(":recurrent_transaction", recurrentTransaction->_dbId.toString());
            query->bindValue(":generated_transaction", tran->_dbId.toString());
            auto success = query->exec();
            if (!success) {
                _db->rollback();
                return;
            }
        }

        // we need to update the data in which the last generated transactions was added
        DLOG(INFO) << "Updating last generated transaction";
        storeSingleRecurrentTransactions(recurrentTransaction);
    }

    _db->commit();
}


void
Transactions::generateRecurrentTransactions() {
    // get all the recurrent transactions so that we can calculate the missing recurrent transactiosn
    auto recurrent = recurrentTransactions();
    QMap<RecurrentTransactionPtr, QList<TransactionPtr>> transMap;

    foreach(const RecurrentTransactionPtr& recurrentTrans, recurrent) {
        auto missingDates = recurrentTrans->recurrence->generateMissingDates();
        QList<TransactionPtr> trans;
        DLOG(INFO) << "There are " << missingDates.count() << " transactions that have to be generated";
        foreach(const QDate& currentDate, missingDates) {
            auto currentTran = std::make_shared<Transaction>(recurrentTrans->transaction->account,
                    recurrentTrans->transaction->amount, recurrentTrans->transaction->category, currentDate,
                    recurrentTrans->transaction->contents, recurrentTrans->transaction->memo);
            recurrentTrans->recurrence->lastGenerated = currentDate;
                    trans.append(currentTran);
        }
        transMap[recurrentTrans] = trans;
    }

    // store the missing the transactions in the database
    storeGeneratedTransactions(transMap);
    if (isError()) {
        LOG(INFO) << "Error generating recurrent transactions";
    }
}

bool
Transactions::isError() {
    return _lastError != QString::null;
}

QString
Transactions::lastError() {
    return _lastError;
}

}
}
}
