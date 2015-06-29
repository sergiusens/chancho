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
#include <com/chancho/recurrent_transaction.h>
#include "book.h"

namespace com {

namespace chancho {

namespace qml {

class Account;
class Book;
class Category;

class RecurrentTransaction : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString account READ getAccount NOTIFY accountChanged)
    Q_PROPERTY(QObject* accountModel READ getAccountModel WRITE setAccountModel)
    Q_PROPERTY(double amount READ getAmount WRITE setAmount NOTIFY amountChanged)
    Q_PROPERTY(QString category READ getCategory NOTIFY categoryChanged)
    Q_PROPERTY(QObject* categoryModel READ getCategoryModel WRITE setCategoryModel)
    Q_PROPERTY(QDate startDate READ getStartDate NOTIFY startDateChanged)
    Q_PROPERTY(QDate endDate READ getEndDate WRITE setEndDate NOTIFY endDateChanged)
    Q_PROPERTY(int occurrences READ getOccurrences WRITE setOccurrences NOTIFY occurrencesChanged)
    Q_PROPERTY(int numberOfDays READ getNumberOfDays WRITE setNumberOfDays NOTIFY numberOfDaysChanged)
    Q_PROPERTY(QString contents READ getContents WRITE setContents NOTIFY contentsChanged)
    Q_PROPERTY(QString memo READ getMemo WRITE setMemo NOTIFY memoChanged)
    Q_PROPERTY(com::chancho::qml::Book::TransactionType type READ getType)
    Q_PROPERTY(com::chancho::qml::Book::RecurrenceType recurrence READ getRecurrenceType)

 public:
    explicit RecurrentTransaction(QObject* parent =0);
    ~RecurrentTransaction();

    QString getAccount() const;

    double getAmount() const;
    void setAmount(double amount);

    QString getCategory() const;

    QDate getStartDate() const;

    QDate getEndDate() const;
    void setEndDate(QDate date);

    int getOccurrences() const;
    void setOccurrences(int occurrences);

    int getNumberOfDays() const;
    void setNumberOfDays(int days);

    QString getContents() const;
    void setContents(QString contents);

    QString getMemo() const;
    void setMemo(QString memo);

    qml::Book::TransactionType getType() const;

    qml::Book::RecurrenceType getRecurrenceType() const;

    QObject* getAccountModel();
    void setAccountModel(QObject* account);

    QObject* getCategoryModel();
    void setCategoryModel(QObject* category);

 signals:
    void accountChanged(QString account);
    void amountChanged(double amount);
    void categoryChanged(QString category);
    void startDateChanged(QDate date);
    void endDateChanged(QDate date);
    void occurrencesChanged(int occurrences);
    void numberOfDaysChanged(int days);
    void contentsChanged(QString contents);
    void memoChanged(QString memo);

 protected:
    RecurrentTransaction(RecurrentTransactionPtr transactionPtr, QObject* parent=0);
    RecurrentTransactionPtr getTransaction() const;

 private:
    RecurrentTransactionPtr _transaction;

};

}

}

}
