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

#include "account.h"

namespace com {

namespace chancho {

namespace qml {


Account::Account(QObject* parent)
    : QObject(parent),
      _acc(std::make_shared<com::chancho::Account>()) {
}

Account::Account(QString name, double amount, QString memo, QObject* parent)
    : QObject(parent),
      _acc(std::make_shared<com::chancho::Account>(name, amount, memo, QString::null)) {

}

Account::Account(QString name, double amount, QString memo, QString color, QObject* parent)
    : QObject(parent),
      _acc(std::make_shared<com::chancho::Account>(name, amount, memo, color)) {

}

Account::Account(AccountPtr ptr, QObject* parent)
    : QObject(parent),
      _acc(ptr) {
}

QString
Account::getColor() const {
    return _acc->color;
}

void
Account::setColor(QString color) {
    if (_acc->color != color) {
        _acc->color = color;
        emit colorChanged(color);
    }
}

QString
Account::getName() const {
    return _acc->name;
}

void
Account::setName(QString name) {
    if (name != _acc->name) {
        _acc->name = name;
        emit nameChanged(_acc->name);
    }
}

double
Account::getAmount() const {
    return _acc->amount;
}

void
Account::setAmount(double amount) {
    if (amount != _acc->amount) {
        _acc->amount = amount;
        emit amountChanged(_acc->amount);
    }
}

QString
Account::getMemo() const {
    return _acc->memo;
}

void
Account::setMemo(QString memo) {
    if (memo != _acc->memo) {
        _acc->memo = memo;
        emit memoChanged(_acc->memo);
    }
}

AccountPtr
Account::getAccount() const {
    return _acc;
}

}

}

}
