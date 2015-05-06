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

namespace workers {

namespace accounts {
class WorkerFactory;
}

namespace categories {
class WorkerFactory;
}

namespace transactions {
class WorkerFactory;
}

}

class Book : public QObject {
    Q_OBJECT

    friend class workers::accounts::WorkerFactory;
    friend class workers::categories::WorkerFactory;
    friend class workers::transactions::WorkerFactory;

 public:
    enum TransactionType {
        INCOME,
        EXPENSE
    };

    Q_ENUMS(TransactionType)

    explicit Book(QObject* parent=0);
    Book(BookPtr book, QObject* parent=0);

    Q_INVOKABLE QObject* accountsModel();
    Q_INVOKABLE QVariantList accounts();
    Q_INVOKABLE QVariantList monthsTotalForAccount(QObject* account, int year);
    Q_INVOKABLE bool storeAccount(QString name, QString memo, QString color, double initialAmount);
    Q_INVOKABLE bool storeAccounts(QVariantList accounts);
    Q_INVOKABLE bool removeAccount(QObject* account);
    Q_INVOKABLE bool updateAccount(QObject* account, QString name, QString memo, QString color);
    Q_INVOKABLE bool storeTransaction(QObject* account, QObject* category, QDate date, double amount,
            QString contents, QString memo);
    Q_INVOKABLE bool removeTransaction(QObject* transaction);

    Q_INVOKABLE bool updateTransaction(QObject* transaction, QObject* accModel, QObject* catModel, QDate date,
                                       QString contents, QString memo, double amount);
    Q_INVOKABLE int numberOfCategories(TransactionType type);
    Q_INVOKABLE QObject* categoriesModel();
    Q_INVOKABLE QObject* categoriesModelForType(TransactionType type);
    Q_INVOKABLE QVariantList categoryPercentagesForMonth(int month, int year);
    Q_INVOKABLE QVariantList monthsTotalForCategory(QObject* category, int year);
    Q_INVOKABLE bool storeCategory(QString name, QString color, Book::TransactionType type);
    Q_INVOKABLE bool storeCategories(QVariantList categories);
    Q_INVOKABLE bool updateCategory(QObject* category, QString name, QString color, Book::TransactionType type);
    Q_INVOKABLE bool removeCategory(QObject* category);
    Q_INVOKABLE QObject* dayModel(int day, int month, int year);
    Q_INVOKABLE QObject* monthModel(QDate date);

 signals:
    void accountStored();
    void accountRemoved();
    void accountUpdated();
    void categoryStored(Book::TransactionType type);
    void categoryUpdated(Book::TransactionType type);
    void categoryRemoved(Book::TransactionType type);
    void categoryTypeUpdated();
    void transactionStored(QDate date);
    void transactionRemoved(QDate date);
    void transactionUpdated(QDate oldDate, QDate newDate);

 protected:
    // protected for testing purposes
    Book(BookPtr book, std::shared_ptr<workers::accounts::WorkerFactory> accounts,
         std::shared_ptr<workers::categories::WorkerFactory> categories,
         std::shared_ptr<workers::transactions::WorkerFactory> transactions,
         QObject* parent=0);

    std::shared_ptr<workers::accounts::WorkerFactory> _accountWorkersFactory;
    std::shared_ptr<workers::categories::WorkerFactory> _categoryWorkersFactory;
    std::shared_ptr<workers::transactions::WorkerFactory> _transactionWorkersFactory;


 private:
    BookPtr _book;
};

}

}

}

Q_DECLARE_METATYPE(com::chancho::qml::Book::TransactionType)
