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

#include <QDate>

#include "com/chancho/qml/transaction.h"
#include "day.h"

namespace com {

namespace chancho {

namespace qml {

namespace models {

Day::Day(QObject* parent)
    : Day(QDate(), std::make_shared<com::chancho::Book>(), parent) {
}

Day::Day(int day, int month, int year, QObject* parent)
    : Day(QDate(year, month, day), std::make_shared<com::chancho::Book>(), parent) {
}

Day::Day(BookPtr book, QObject* parent)
    : Day(QDate(), book, parent) {
}

Day::Day(int day, int month, int year, BookPtr book, QObject* parent)
    : Day(QDate(year, month, day), book, parent) {
}

Day::Day(QDate date, QObject* parent)
    : Day(date, std::make_shared<com::chancho::Book>(), parent) {
}

Day::Day(QDate date, BookPtr book, QObject* parent)
    : QAbstractListModel(parent),
      _date(date),
      _book(book) {
    // connect the signal of day changed to emit that the day name changed too
    connect(this, &Day::dayChanged, [&](int){
        int day = _date.dayOfWeek();
        emit dayNameChanged(QDate::shortDayName(day));
    });
}

Day::~Day() {
}

int
Day::numberOfTransactions() const {
    // the parent is not really used
    if (!_date.isValid()) {
        return 0;
    }

    // use the book to return the count of transactions for the given month
    auto count = _book->numberOfTransactions(_date.day(), _date.month(), _date.year());
    if (_book->isError()) {
        return 0;
    }
    return count;
}

int
Day::rowCount(const QModelIndex&) const {
    return numberOfTransactions();
}

QVariant
Day::data(int row, int role) const {
    auto count = _book->numberOfTransactions(_date.day(), _date.month(), _date.year());
    if (_book->isError()) {
        return QVariant();
    }

   if (row >= count) {
        DLOG(INFO) << "Querying data for to large index";
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        DLOG(INFO) << "Getting transactions for day " << _date.toString("dd/MM/yyyy").toStdString()
                << " with offset " << row;
        auto transactions = _book->transactions(_date.day(), _date.month(), _date.year(), 1, row);
        if (_book->isError()) {
            LOG(INFO) << "Error when getting data from the db" << _book->lastError().toStdString();
            return QVariant();
        }
        if (transactions.count() > 0) {
            auto model = new com::chancho::qml::Transaction(transactions.at(0));
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
Day::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) {
        DLOG(INFO) << "Querying data for not valid index.";
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
    return _date.day();
}

void
Day::setDay(int day) {
    if (day != _date.day()) {
        beginResetModel();

        _date.setDate(_date.year(), _date.month(), day);
        emit dayChanged(day);
        emit dateChanged(_date);
        emit expenseSumChanged(_book->expenseForDay(_date.day(), _date.month(), _date.year()));
        emit incomeSumChanged(_book->incomeForDay(_date.day(), _date.month(), _date.year()));

        endResetModel();
    }
}

QString
Day::getDayName() const {
    int day = _date.dayOfWeek();
    return  QDate::shortDayName(day);
}

int
Day::getMonth() const {
    return _date.month();
}

void
Day::setMonth(int month) {
    if (month != _date.month()) {
        beginResetModel();

        _date.setDate(_date.year(), month, _date.day());
        emit monthChanged(month);
        emit dateChanged(_date);
        emit expenseSumChanged(_book->expenseForDay(_date.day(), _date.month(), _date.year()));
        emit incomeSumChanged(_book->incomeForDay(_date.day(), _date.month(), _date.year()));

        endResetModel();
    }
}

int
Day::getYear() const {
    return _date.year();
}

void
Day::setYear(int year) {
    if (year != _date.year()) {
        beginResetModel();

        _date.setDate(year, _date.month(), _date.day());
        emit yearChanged(year);
        emit dateChanged(_date);
        emit expenseSumChanged(_book->expenseForDay(_date.day(), _date.month(), _date.year()));
        emit incomeSumChanged(_book->incomeForDay(_date.day(), _date.month(), _date.year()));

        endResetModel();
    }
}

QDate
Day::getDate() const {
    return _date;
}

void
Day::setDate(QDate date) {
    if (date != _date) {
        beginResetModel();
        auto oldDate = _date;
        _date = date;
        emit dateChanged(date);

        if (_date.day() != oldDate.day()) {
            emit dayChanged(_date.day());
        }

        if (_date.month() != oldDate.month()) {
            emit monthChanged(_date.month());
        }

        if (_date.year() != oldDate.year()) {
            emit yearChanged(_date.year());
        }
        emit expenseSumChanged(_book->expenseForDay(_date.day(), _date.month(), _date.year()));
        emit incomeSumChanged(_book->incomeForDay(_date.day(), _date.month(), _date.year()));
        endResetModel();
    }
}

double
Day::getExpenseSum() const {
    return _book->expenseForDay(_date.day(), _date.month(), _date.year());
}

double
Day::getIncomeSum() const {
    return _book->incomeForDay(_date.day(), _date.month(), _date.year());
}

}

}

}

}
