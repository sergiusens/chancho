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

#include "book.h"
#include "account.h"

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
Book::storeTransaction(QObject* account, QObject* category, QDate date, QString amount, QString contents,
        QString memo) {
    LOG(INFO) << "Store a new transaction from QML";
    auto acc = qobject_cast<qml::Account*>(account);
    auto cat = qobject_cast<qml::Category*>(category);
    auto amountDouble = amount.toDouble();
    // TODO: fire a signal
    auto tran = std::make_shared<chancho::Transaction>(
            acc->getAccount(), amountDouble, cat->getCategory(), date, contents, memo);
    _book->store(tran);
    if (_book->isError()) {
        LOG(INFO) << "Error storing transaction";
        return false;
    } else {
        LOG(INFO) << "New transaction added";
        return true;
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
    LOG(INFO) << "days model for " << day << " " << month << " " << year;
    return new models::Day(day, month, year, _book);
}

QObject*
Book::monthModel(QDate date) {
    LOG(INFO) << "months model for " << date.month() << " " << date.year();
    return new models::Month(date.month(), date.year(), _book);
}

}
}

}