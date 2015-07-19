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

#include <memory>
#include <com/chancho/transaction.h>
#include "book.h"

namespace com {

namespace chancho {

namespace qml {

class Account;
class Book;
class Category;

namespace models {

class Day;
class GeneratedTransactions;

}

class Transaction : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString account READ getAccount NOTIFY accountChanged)
    Q_PROPERTY(QObject* accountModel READ getAccountModel WRITE setAccountModel)
    Q_PROPERTY(double amount READ getAmount WRITE setAmount NOTIFY amountChanged)
    Q_PROPERTY(QString category READ getCategory NOTIFY categoryChanged)
    Q_PROPERTY(QObject* categoryModel READ getCategoryModel WRITE setCategoryModel)
    Q_PROPERTY(QDate date READ getDate WRITE setDate NOTIFY dateChanged)
    Q_PROPERTY(QString contents READ getContents WRITE setContents NOTIFY contentsChanged)
    Q_PROPERTY(QString memo READ getMemo WRITE setMemo NOTIFY memoChanged)
    Q_PROPERTY(com::chancho::qml::Book::TransactionType type READ getType)

    friend class models::Day;
    friend class models::GeneratedTransactions;
    friend class qml::Book;

 public:
    explicit Transaction(QObject* parent =0);
    ~Transaction();

    QString getAccount() const;

    double getAmount() const;
    void setAmount(double amount);

    QString getCategory() const;

    QDate getDate() const;
    void setDate(QDate date);

    QString getContents() const;
    void setContents(QString contents);

    QString getMemo() const;
    void setMemo(QString memo);

    qml::Book::TransactionType getType() const;

    QObject* getAccountModel();
    void setAccountModel(QObject* account);

    QObject* getCategoryModel();
    void setCategoryModel(QObject* category);

 signals:
    void accountChanged(QString account);
    void amountChanged(double amount);
    void categoryChanged(QString category);
    void dateChanged(QDate date);
    void contentsChanged(QString contents);
    void memoChanged(QString memo);

 protected:
    Transaction(TransactionPtr transactionPtr, QObject* parent=0);
    TransactionPtr getTransaction() const;

 private:
    TransactionPtr _transaction;

};

}

}

}