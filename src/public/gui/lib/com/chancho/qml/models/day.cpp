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

#include "com/chancho/qml/transaction.h"
#include "day.h"

namespace com {

namespace chancho {

namespace qml {

namespace models {

Day::Day(QObject* parent)
    : QAbstractListModel(parent),
      _day(-1),
      _month(-1),
      _year(-1),
      _book(std::make_shared<com::chancho::Book>()) {
}

Day::Day(int day, int month, int year, QObject* parent)
    : QAbstractListModel(parent),
      _day(day),
      _month(month),
      _year(year),
      _book(std::make_shared<com::chancho::Book>()) {
}

Day::Day(BookPtr book, QObject* parent)
    : QAbstractListModel(parent),
      _book(book) {
}

Day::Day(int day, int month, int year, BookPtr book, QObject* parent)
    : QAbstractListModel(parent),
      _day(day),
      _month(month),
      _year(year),
      _book(book) {
}

Day::~Day() {
}

int
Day::rowCount(const QModelIndex&) const {
    // the parent is not really used
    if (_day == -1 || _month == -1 || _year == -1) {
        return 0;
    }
    // use the book to return the count of transactions for the given month
    auto count = _book->numberOfTransactions(_day, _month, _year);
    if (_book->isError()) {
        return 0;
    }
    return count;
}

QVariant
Day::data(int row, int role) const {
    auto count = _book->numberOfTransactions(_day, _month, _year);
    if (_book->isError()) {
        return QVariant();
    }

   if (row >= count) {
        LOG(INFO) << "Querying data for to large index";
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        LOG(INFO) << "Getting transactions for day" << _day << "/" << _month << "/" << _year << " with offset " << row;
        auto transactions = _book->transactions(_day, _month, _year, 1, row);
        if (_book->isError()) {
            LOG(INFO) << "Error when getting data from the db" << _book->lastError().toStdString();
            return QVariant();
        }
        if (transactions.count() > 0) {
            auto model = new com::chancho::qml::Transaction(transactions.at(0));
            return QVariant::fromValue(model);
        } else {
            LOG(INFO) << "No transaction was found.";
            return QVariant();
        }
    } else {
        return QVariant();
    }
}

QVariant
Day::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) {
        LOG(INFO) << "Querying data for not valid index.";
        return QVariant();
    }
    return data(index.row(), role);
}

QVariant
Day::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QString("Column %1").arg(section);
    else
        return QString("Row %1").arg(section);
}

int
Day::getDay() const {
    return _day;
}

void
Day::setDay(int day) {
    if (day != _day) {
        _day = day;
        emit dayChanged(_day);
    }
}

int
Day::getMonth() const {
    return _month;
}

void
Day::setMonth(int month) {
    if (month != _month) {
        _month = month;
        emit monthChanged(_month);
    }
}

int
Day::getYear() const {
    return _year;
}

void
Day::setYear(int year) {
    if (year != _year) {
        _year = year;
        emit yearChanged(year);
    }
}

}

}

}

}
