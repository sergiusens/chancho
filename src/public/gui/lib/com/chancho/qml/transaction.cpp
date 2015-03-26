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

#include "transaction.h"

namespace com {

namespace chancho {

namespace qml {

Transaction::Transaction(QObject* parent)
    : QObject(parent),
      _transaction(std::make_shared<com::chancho::Transaction>()){
}

Transaction::Transaction(TransactionPtr transactionPtr, QObject* parent)
    : QObject(parent),
      _transaction(transactionPtr) {

}

Transaction::~Transaction() {
}

QString
Transaction::getAccount() const {
    return _transaction->account->name;
}

double
Transaction::getAmount() const {
    return _transaction->amount;
}

void
Transaction::setAmount(double amount) {
    if (amount != _transaction->amount) {
        _transaction->amount = amount;
        emit amountChanged(_transaction->amount);
    }
}

QString
Transaction::getCategory() const {
    return _transaction->category->name;
}

QDate
Transaction::getDate() const {
    return _transaction->date;
}

void
Transaction::setDate(QDate date) {
    if (date != _transaction->date) {
        _transaction->date = date;
        emit dateChanged(_transaction->date);
    }
}

QString
Transaction::getContents() const {
    return _transaction->contents;
}

void
Transaction::setContents(QString contents) {
    if (contents != _transaction->contents) {
        _transaction->contents = contents;
        emit contentsChanged(_transaction->contents);
    }
}

QString
Transaction::getMemo() const {
    return _transaction->memo;
}

void
Transaction::setMemo(QString memo) {
    if (memo != _transaction->memo) {
        _transaction->memo = memo;
        emit memoChanged(_transaction->memo);
    }
}

Transaction::Type
Transaction::getType() const {
    if (_transaction->category->type == Category::Type::EXPENSE) {
        return Transaction::Type::EXPENSE;
    } else {
        return Transaction::Type::INCOME;
    }
}

}

}

}
