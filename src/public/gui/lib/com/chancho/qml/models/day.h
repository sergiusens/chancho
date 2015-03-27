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

#pragma once

#include <QAbstractListModel>
#include <QModelIndex>

#include <com/chancho/book.h>

namespace com {

namespace chancho {

namespace qml {

class Book;

namespace models {

class Month;

class Day : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int day READ getDay WRITE setDay NOTIFY dayChanged)
    Q_PROPERTY(QString dayName READ getDayName NOTIFY dayNameChanged)
    Q_PROPERTY(int month READ getMonth WRITE setMonth NOTIFY monthChanged)
    Q_PROPERTY(int year READ getYear WRITE setYear NOTIFY yearChanged)
    Q_PROPERTY(QDate date READ getDate WRITE setDate NOTIFY dateChanged)
    Q_PROPERTY(double expenseSum READ getExpenseSum NOTIFY expenseSumChanged)
    Q_PROPERTY(double incomeSum READ getIncomeSum NOTIFY incomeSumChanged)

    friend class com::chancho::qml::Book;
    friend class com::chancho::qml::models::Month;

 public:
    explicit Day(QObject* parent = 0);
    Day(int day, int month, int year, QObject* parent = 0);
    Day(QDate date, QObject* parent = 0);
    virtual ~Day();

    // methods to override to allow the model to be used from qml
    Q_INVOKABLE int numberOfTransactions() const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    QVariant data(int row, int role) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    int getDay() const;
    void setDay(int day);

    QString getDayName() const;

    int getMonth() const;
    void setMonth(int month);

    int getYear() const;
    void setYear(int year);

    QDate getDate() const;
    void setDate(QDate date);

    double getExpenseSum() const;
    double getIncomeSum() const;

 protected:
    Day(BookPtr book, QObject* parent = 0);
    Day(int day, int month, int year, BookPtr book, QObject* parent = 0);
    Day(QDate date, BookPtr book, QObject* parent = 0);

 signals:
    void dayChanged(int day);
    void dayNameChanged(QString name);
    void monthChanged(int month);
    void yearChanged(int year);
    void dateChanged(QDate date);
    void expenseSumChanged(double expense);
    void incomeSumChanged(double income);

 private:
    QDate _date;
    BookPtr _book;
};

}

}

}

}

