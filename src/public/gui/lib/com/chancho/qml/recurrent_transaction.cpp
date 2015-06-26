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

#include "account.h"
#include "category.h"

#include "recurrent_transaction.h"

namespace com {

namespace chancho {

namespace qml {

RecurrentTransaction::RecurrentTransaction(QObject* parent)
        : QObject(parent),
          _transaction(std::make_shared<com::chancho::RecurrentTransaction>()) {

}

RecurrentTransaction::~RecurrentTransaction() {
}

RecurrentTransaction::RecurrentTransaction(RecurrentTransactionPtr transactionPtr, QObject* parent)
        : QObject(parent),
          _transaction(transactionPtr) {

}

QString
RecurrentTransaction::getAccount() const {
    return _transaction->transaction->account->name;
}

double
RecurrentTransaction::getAmount() const {
    return _transaction->transaction->amount;
}

void
RecurrentTransaction::setAmount(double amount) {
    if (amount != _transaction->transaction->amount) {
        _transaction->transaction->amount = amount;
        emit amountChanged(_transaction->transaction->amount);
    }
}

QString
RecurrentTransaction::getCategory() const {
    return _transaction->transaction->category->name;
}

QDate
RecurrentTransaction::getStartDate() const {
    return _transaction->recurrence->startDate;
}

QDate
RecurrentTransaction::getEndDate() const {
    if (_transaction->recurrence->endDate.isValid()) {
        return _transaction->recurrence->endDate;
    } else {
        return QDate();
    }
}

void
RecurrentTransaction::setEndDate(QDate date) {
    auto endDate = _transaction->recurrence->endDate;
    if (!endDate.isValid() || endDate != date) {
        _transaction->recurrence->endDate = date;
        emit endDateChanged(date);
    }
}

int
RecurrentTransaction::getOccurrences() const {
    if (_transaction->recurrence->occurrences) {
        return *_transaction->recurrence->occurrences;
    } else {
        return -1;
    }
}

void
RecurrentTransaction::setOccurrences(int occurrences) {
    if (!_transaction->recurrence->occurrences || _transaction->recurrence->occurrences != occurrences) {
        _transaction->recurrence->occurrences = occurrences;
        emit occurrencesChanged(occurrences);
    }
}

int
RecurrentTransaction::getNumberOfDays() const {
    if (_transaction->recurrence->numberOfDays()) {
        return *_transaction->recurrence->numberOfDays();
    } else {
        return -1;
    }
}

void
RecurrentTransaction::setNumberOfDays(int days) {
    auto numberOfDays = _transaction->recurrence->numberOfDays();
    if (!numberOfDays || numberOfDays != days) {
        auto oldRecurrence = _transaction->recurrence;
        _transaction->recurrence = std::make_shared<chancho::RecurrentTransaction::Recurrence>(
                days, oldRecurrence->startDate, oldRecurrence->endDate);
        emit numberOfDaysChanged(days);
    }
}

QString
RecurrentTransaction::getContents() const {
    return _transaction->transaction->contents;
}

void
RecurrentTransaction::setContents(QString contents) {
    if (contents != _transaction->transaction->contents) {
        _transaction->transaction->contents = contents;
        emit contentsChanged(_transaction->transaction->contents);
    }
}

QString
RecurrentTransaction::getMemo() const {
    return _transaction->transaction->memo;
}

void
RecurrentTransaction::setMemo(QString memo) {
    if (memo != _transaction->transaction->memo) {
        _transaction->transaction->memo = memo;
        emit memoChanged(_transaction->transaction->memo);
    }
}

qml::Book::TransactionType
RecurrentTransaction::getType() const {
    if (_transaction->transaction->category->type == chancho::Category::Type::EXPENSE) {
        return qml::Book::EXPENSE;
    } else {
        return qml::Book::INCOME;
    }
}

qml::Book::RecurrenceType
RecurrentTransaction::getRecurrenceType() const {
    switch (_transaction->recurrence->defaults().get()){
        case com::chancho::RecurrentTransaction::Recurrence::Defaults::DAILY:
            return qml::Book::DAILY;
        case com::chancho::RecurrentTransaction::Recurrence::Defaults::WEEKLY:
            return qml::Book::WEEKLY;
        default:
            return qml::Book::MONTHLY;
    }
}

QObject*
RecurrentTransaction::getAccountModel() {
    // pass this as the parent to ensure that we clean the memory on destruction
    return new qml::Account(_transaction->transaction->account, this);
}

void
RecurrentTransaction::setAccountModel(QObject* account) {
    auto acc = qobject_cast<qml::Account*>(account);
    if(acc != nullptr) {
        _transaction->transaction->account = acc->getAccount();
        emit accountChanged(_transaction->transaction->account->name);
    }
}

QObject*
RecurrentTransaction::getCategoryModel() {
    // pass this as the parent to ensure that we clean the memory on destruction
    return new qml::Category(_transaction->transaction->category, this);
}

void
RecurrentTransaction::setCategoryModel(QObject* category) {
    auto cat = qobject_cast<qml::Category*>(category);
    if(cat != nullptr) {
        _transaction->transaction->category = cat->getCategory();
    }
}

RecurrentTransactionPtr
RecurrentTransaction::getTransaction() const {
    return _transaction;
}

}
}
}
