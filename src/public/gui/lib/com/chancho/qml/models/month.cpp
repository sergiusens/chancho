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
    : Month(QDate(), std::make_shared<com::chancho::Book>(), parent) {
}

Month::Month(int month, int year, QObject* parent)
    : Month(QDate(year, month, 1), std::make_shared<com::chancho::Book>(), parent) {
}

Month::Month(QDate date, QObject* parent)
    : Month(date, std::make_shared<com::chancho::Book>(), parent) {
}

Month::Month(BookPtr book, QObject* parent)
    : Month(QDate(), book, parent) {
}

Month::Month(int month, int year, BookPtr book, QObject* parent)
    : Month(QDate(year, month, 1), book, parent) {
}

Month::Month(QDate date, BookPtr book, QObject* parent)
    : QAbstractListModel(parent),
      _date(date),
      _book(book) {
}

Month::~Month() {
}

int
Month::rowCount(const QModelIndex&) const {
    // the parent is not really used
    if (!_date.isValid()){
        return 0;
    }
    // use the book to return the count of transactions for the given month
    auto count = _book->numberOfDaysWithTransactions(_date.month(), _date.year());
    if (_book->isError()) {
        return 0;
    }
    DLOG(INFO) << "Number of days with transactions is " << count;
    return count;
}

QVariant
Month::data(int row, int role) const {
    DLOG(INFO) << "Return model for day in index " << _date.toString("mm/yyyy").toStdString();
    auto count = _book->numberOfDaysWithTransactions(_date.month(), _date.year());
    if (_book->isError()) {
        DLOG(INFO) << "Error when getting data from the db" << _book->lastError().toStdString();
        return QVariant();
    }
    if (row > count) {
        DLOG(INFO) << "Querying data for to large index";
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        DLOG(INFO) << "Getting days with transactions for month " << _date.toString("mm/yyyy").toStdString()
                << " with offset " << row;
        auto days = _book->daysWithTransactions(_date.month(), _date.year(), 1, row);
        if (_book->isError()) {
            LOG(INFO) << "Error when getting data from the db" << _book->lastError().toStdString();
            return QVariant();
        }
        if (days.count() > 0) {
            auto model = new Day(days.at(0), _date.month(), _date.year(), _book);
            DLOG(INFO) << "Returning day model " << days.at(0);
            return QVariant::fromValue(model);
        } else {
            DLOG(INFO) << "No transaction was found.";
            return QVariant();
        }
    } else {
        return QVariant();
    }
}

QVariant
Month::data(const QModelIndex& index, int role) const {
    DLOG(INFO) << "Get data (" << index.row() << ", " << index.column() << ")";
    if (!index.isValid()) {
        DLOG(INFO) << "Querying data for not valid index.";
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
    return _date.month();
}

void
Month::setMonth(int month) {
    if (month != _date.month()) {
        beginResetModel();

        _date.setDate(_date.year(), month, _date.day());
        emit monthChanged(month);
        emit dateChanged(_date);

        endResetModel();
    }
}

int
Month::getYear() const {
    return _date.year();
}

void
Month::setYear(int year) {
    if (year != _date.year()) {
        beginResetModel();

        _date.setDate(year, _date.month(), _date.day());
        emit yearChanged(year);
        emit dateChanged(_date);

        endResetModel();
    }
}

QDate
Month::getDate() const {
    return _date;
}

void
Month::setDate(QDate date) {
    DLOG(INFO) << "Setting new date";
    if (date != _date) {
        beginResetModel();
        auto oldDate = _date;
        _date = date;

        emit dateChanged(_date);

        if (_date.month() != oldDate.month()) {
            emit monthChanged(_date.month());
        }

        if (_date.year() != oldDate.year()) {
            emit yearChanged(_date.year());
        }

        endResetModel();
    }
}

void
Month::onTransactionStored(QDate date) {
    // if the transaction was stored in the month of this model, trigger a redraw
    if (_date.month() == date.month() && _date.year() == date.year()) {
        beginResetModel();
        endResetModel();
    }
}

void
Month::onTransactionRemoved(QDate date) {
    if (_date.month() == date.month() && _date.year() == date.year()) {
        beginResetModel();
        endResetModel();
    }
}

void
Month::onTransactionUpdated(QDate oldDate, QDate newDate) {
    // TODO: be smarter, in some cases we just need to update a specific model or even item
    if ((_date.month() == oldDate.month() && _date.year() == oldDate.year())
            || (_date.month() == newDate.month() && _date.year() == newDate.year())) {
        beginResetModel();
        endResetModel();
    }
}

}

}

}

}
