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

#include <QObject>

#include <com/chancho/book.h>

namespace com {

namespace chancho {

namespace qml {

class Book : public QObject {
    Q_OBJECT

 public:
    enum TransactionType {
        INCOME,
        EXPENSE
    };

    Q_ENUMS(TransactionType)

    explicit Book(QObject* parent=0);
    explicit Book(BookPtr book, QObject* parent=0);

    Q_INVOKABLE QObject* accountsModel();
    Q_INVOKABLE bool storeAccount(QString name, QString memo, QString color, double initialAmount);
    Q_INVOKABLE bool removeAccount(QObject* account);
    Q_INVOKABLE bool updateAccount(QObject* account, QString name, QString memo, QString color);
    Q_INVOKABLE bool storeTransaction(QObject* account, QObject* category, QDate date, double amount,
            QString contents, QString memo);
    Q_INVOKABLE bool removeTransaction(QObject* transaction);

    Q_INVOKABLE bool updateTransaction(QObject* transaction, QObject* accModel, QObject* catModel, QDate date,
                                       QString contents, QString memo, double amount);
    Q_INVOKABLE QObject* categoriesModel();
    Q_INVOKABLE QObject* categoriesModeForType(TransactionType type);
    Q_INVOKABLE QObject* dayModel(int day, int month, int year);
    Q_INVOKABLE QObject* monthModel(QDate date);

 signals:
    void accountStored();
    void accountRemoved();
    void accountUpdated();
    void transactionStored(QDate date);
    void transactionRemoved(QDate date);
    void transactionUpdated(QDate oldDate, QDate newDate);

 private:
    BookPtr _book;
};

}

}

}
