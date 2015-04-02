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

#include <com/chancho/account.h>

namespace com {

namespace chancho {

namespace qml {

class Book;
class Transaction;

namespace models {

class Accounts;

}

class Account : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString color READ getColor WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(double amount READ getAmount WRITE setAmount NOTIFY amountChanged)
    Q_PROPERTY(QString memo READ getMemo WRITE setMemo NOTIFY memoChanged)

    friend class models::Accounts;
    friend class qml::Book;
    friend class qml::Transaction;

 public:
    explicit Account(QObject* parent=0);
    Account(QString name, double amount, QString memo, QObject* parent = 0);
    Account(QString name, double amount, QString memo, QString color, QObject* parent = 0);

    QString getColor() const;
    void setColor(QString color);

    QString getName() const;
    void setName(QString name);

    double getAmount() const;
    void setAmount(double amount);

    QString getMemo() const;
    void setMemo(QString memo);

 signals:
    void colorChanged(QString);
    void nameChanged(QString);
    void amountChanged(double);
    void memoChanged(QString);

 protected:
    Account(AccountPtr ptr, QObject* parent=0);

    AccountPtr getAccount() const;

 private:
    AccountPtr _acc;
};

}

}

}
