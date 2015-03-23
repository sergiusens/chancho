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

namespace com {

namespace chancho {

class TransactionModel : public QObject {
    Q_OBJECT
    Q_ENUMS(TransactionModel::Type)
    Q_PROPERTY(QString account READ getAccount)
    Q_PROPERTY(double amount READ getAmount WRITE setAmount NOTIFY amountChanged)
    Q_PROPERTY(QString category READ getCategory)
    Q_PROPERTY(QDate date READ getDate WRITE setDate NOTIFY dateChanged)
    Q_PROPERTY(QString contents READ getContents WRITE setContents NOTIFY contentsChanged)
    Q_PROPERTY(QString memo READ getMemo WRITE setMemo NOTIFY memoChanged)
    Q_PROPERTY(TransactionModel::Type type READ getType)

    friend class DayModel;

 public:
    enum Type {
        INCOME,
        EXPENSE
    };

    explicit TransactionModel(QObject* parent =0);
    ~TransactionModel();

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

    TransactionModel::Type getType() const;

 private:
    TransactionModel(TransactionPtr transactionPtr, QObject* parent=0);

 signals:
    void amountChanged(double amount);
    void dateChanged(QDate date);
    void contentsChanged(QString contents);
    void memoChanged(QString memo);

 private:
    TransactionPtr _transaction;

};

}

}
