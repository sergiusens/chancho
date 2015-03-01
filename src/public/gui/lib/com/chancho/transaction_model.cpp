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

#include "transaction_model.h"

namespace com {

namespace chancho {

TransactionModel::TransactionModel(QObject* parent)
    : QObject(parent),
      _transaction(std::make_shared<Transaction>()){
}

TransactionModel::TransactionModel(TransactionPtr transactionPtr, QObject* parent)
    : QObject(parent),
      _transaction(transactionPtr) {

}

TransactionModel::~TransactionModel() {
}

QString
TransactionModel::getAccount() const {
    return _transaction->account->name;
}

double
TransactionModel::getAmount() const {
    return _transaction->amount;
}

void
TransactionModel::setAmount(double amount) {
    if (amount != _transaction->amount) {
        _transaction->amount = amount;
        emit amountChanged(_transaction->amount);
    }
}

QString
TransactionModel::getCategory() const {
    return _transaction->category->name;
}

QDate
TransactionModel::getDate() const {
    return _transaction->date;
}

void
TransactionModel::setDate(QDate date) {
    if (date != _transaction->date) {
        _transaction->date = date;
        emit dateChanged(_transaction->date);
    }
}

QString
TransactionModel::getContents() const {
    return _transaction->contents;
}

void
TransactionModel::setContents(QString contents) {
    if (contents != _transaction->contents) {
        _transaction->contents = contents;
        emit contentsChanged(_transaction->contents);
    }
}

QString
TransactionModel::getMemo() const {
    return _transaction->memo;
}

void
TransactionModel::setMemo(QString memo) {
    if (memo != _transaction->memo) {
        _transaction->memo = memo;
        emit memoChanged(_transaction->memo);
    }
}

TransactionModel::Type
TransactionModel::getType() const {
    if (_transaction->category->type == Category::Type::EXPENSE) {
        return TransactionModel::Type::EXPENSE;
    } else {
        return TransactionModel::Type::INCOME;
    }
}

}

}
