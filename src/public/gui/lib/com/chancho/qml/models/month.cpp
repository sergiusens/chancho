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

#include "day.h"
#include "month.h"

namespace com {

namespace chancho {

namespace qml {

namespace models {

Month::Month(QObject* parent)
    : QAbstractListModel(parent),
      _month(-1),
      _year(-1),
      _book(std::make_shared<com::chancho::Book>()) {
}

Month::Month(int month, int year, QObject* parent)
    : QAbstractListModel(parent),
      _month(month),
      _year(year),
      _book(std::make_shared<com::chancho::Book>()) {
}

Month::Month(BookPtr book, QObject* parent)
    : QAbstractListModel(parent),
      _book(book) {
}

Month::Month(int month, int year, BookPtr book, QObject* parent)
    : QAbstractListModel(parent),
      _month(month),
      _year(year),
      _book(book) {
}

Month::~Month() {
}

int
Month::rowCount(const QModelIndex&) const {
    // the parent is not really used
    if (_month == -1 || _year == -1) {
        return 0;
    }
    // use the book to return the count of transactions for the given month
    auto count = _book->numberOfDaysWithTransactions(_month, _year);
    if (_book->isError()) {
        return 0;
    }
    LOG(INFO) << "Number of days with transactions is " << count;
    return count;
}

QVariant
Month::data(int row, int role) const {
    LOG(INFO) << "Return model for day in index " << row << " for " << _month << "/" << _year;
    auto count = _book->numberOfDaysWithTransactions(_month, _year);
    if (_book->isError()) {
        LOG(INFO) << "Error when getting data from the db" << _book->lastError().toStdString();
        return QVariant();
    }
    if (row > count) {
        LOG(INFO) << "Querying data for to large index";
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        LOG(INFO) << "Getting days with transactions for month " << _month << "/" << _year << " with offset " << row;
        auto days = _book->daysWithTransactions(_month, _year, 1, row);
        if (_book->isError()) {
            LOG(INFO) << "Error when getting data from the db" << _book->lastError().toStdString();
            return QVariant();
        }
        if (days.count() > 0) {
            auto model = new Day(days.at(0), _month, _year, _book);
            LOG(INFO) << "Returning day model " << days.at(0);
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
Month::data(const QModelIndex& index, int role) const {
    LOG(INFO) << "Get data (" << index.row() << ", " << index.column() << ")";
    if (!index.isValid()) {
        LOG(INFO) << "Querying data for not valid index.";
        return QVariant();
    }
    return data(index.row(), role);
}

QVariant
Month::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QString("Column %1").arg(section);
    else
        return QString("Row %1").arg(section);
}

int
Month::getMonth() const {
    return _month;
}

void
Month::setMonth(int month) {
    if (month != _month) {
        _month = month;
        emit monthChanged(_month);
    }
}

int
Month::getYear() const {
    return _year;
}

void
Month::setYear(int year) {
    if (year != _year) {
        _year = year;
        emit yearChanged(year);
    }
}

}

}

}

}
