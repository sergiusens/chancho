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

#include "models/accounts.h"
#include "models/categories.h"
#include "models/day.h"
#include "models/month.h"

#include "account.h"
#include "transaction.h"

#include "book.h"

namespace com {

namespace chancho {

namespace qml {

Book::Book(QObject* parent)
    : QObject(parent),
      _book(std::make_shared<com::chancho::Book>()) {
    qRegisterMetaType<Book::TransactionType>("Book::TransactionType");
}

Book::Book(BookPtr book, QObject* parent)
    : QObject(parent),
      _book(book) {
    qRegisterMetaType<Book::TransactionType>("Book::TransactionType");
}

QObject*
Book::accountsModel() {
    return new models::Accounts(_book);
}

bool
Book::storeTransaction(QObject* account, QObject* category, QDate date, double amount, QString contents,
        QString memo) {
    auto acc = qobject_cast<qml::Account*>(account);
    if (acc == nullptr) {
        return false;
    }

    auto cat = qobject_cast<qml::Category*>(category);
    if (cat == nullptr) {
        return false;
    }

    auto tran = std::make_shared<chancho::Transaction>(
            acc->getAccount(), amount, cat->getCategory(), date, contents, memo);
    _book->store(tran);

    if (_book->isError()) {
        return false;
    } else {
        emit transactionStored(date);
        return true;
    }
}

bool
Book::removeTransaction(QObject* transaction) {
    auto tran = qobject_cast<qml::Transaction*>(transaction);
    if (tran == nullptr) {
        return false;
    }
    _book->remove(tran->getTransaction());
    if (_book->isError()) {
        return false;
    } else {
        emit transactionRemoved(tran->getTransaction()->date);
        return true;
    }
}

bool
Book::updateTransaction(QObject* tranObj, QObject* accObj, QObject* catObj, QDate date,
                        QString contents, QString memo, double amount) {
    TransactionPtr tran;
    AccountPtr acc;
    CategoryPtr cat;

    auto tranModel = qobject_cast<qml::Transaction*>(tranObj);
    if (tranModel != nullptr) {
        tran = tranModel->getTransaction();
    } else {
        LOG(ERROR) << "Method called with wrong object type as a transaction model";
        return false;
    }

    auto accModel = qobject_cast<qml::Account*>(accObj);
    if (accModel != nullptr) {
        acc = accModel->getAccount();
    } else {
        LOG(ERROR) << "Method called with wrong object type as an account model";
        return false;
    }

    auto catModel = qobject_cast<qml::Category*>(catObj);
    if (catModel != nullptr) {
        cat = catModel->getCategory();
    } else {
        LOG(ERROR) << "Method called with wrong object type as a category model";
        return false;
    }

    // decide if we need to perform the update
    auto requiresUpdate = tran->account != acc || tran->category != cat || tran->date != date
        || tran->contents != contents || tran->memo != memo || tran->amount != amount;

    if (requiresUpdate) {
        auto oldDate = tran->date;
        tran->account = acc;
        tran->category = cat;
        tran->date = date;
        tran->contents = contents;
        tran->memo = memo;
        tran->amount = amount;

        _book->store(tran);
        if (_book->isError()) {
            return false;
        } else {
            emit transactionUpdated(oldDate, date);
            return true;
        }

    } else {
        return false;
    }

}

QObject*
Book::categoriesModel() {
    return new models::Categories(qml::Book::TransactionType::EXPENSE,  _book);
}

QObject*
Book::categoriesModeForType(TransactionType type) {
    return new models::Categories(type,  _book);
}

QObject*
Book::dayModel(int day, int month, int year) {
    return new models::Day(day, month, year, _book);
}

QObject*
Book::monthModel(QDate date) {
    auto model = new models::Month(date.month(), date.year(), _book);
    connect(this, &Book::transactionStored, model, &models::Month::onTransactionStored);
    connect(this, &Book::transactionRemoved, model, &models::Month::onTransactionRemoved);
    connect(this, &Book::transactionUpdated, model, &models::Month::onTransactionUpdated);
    return model;
}

}
}

}