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
    Q_INVOKABLE bool storeTransaction(QObject* account, QObject* category, QDate date, QString amount,
            QString contents, QString memo);
    Q_INVOKABLE QObject* categoriesModel();
    Q_INVOKABLE QObject* categoriesModeForType(TransactionType type);
    Q_INVOKABLE QObject* dayModel(int day, int month, int year);
    Q_INVOKABLE QObject* monthModel(int month, int year);

 private:
    BookPtr _book;
};

}

}

}
