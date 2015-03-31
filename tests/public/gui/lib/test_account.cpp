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

#include <QSignalSpy>

#include <com/chancho/qml/account.h>
#include "test_account.h"


void
TestAccount::init() {
    BaseTestCase::init();
}

void
TestAccount::cleanup() {
    BaseTestCase::cleanup();
}

void
TestAccount::testGetName() {
    QString name = "Bankia";
    double amount = 3.4;
    QString memo = "Savings";

    auto acc = std::make_shared<com::chancho::qml::Account>(name, amount, memo);
    QCOMPARE(acc->getName(), name);
}

void
TestAccount::testSetNameNoSignal() {
    QString name = "Bankia";
    double amount = 3.4;
    QString memo = "Savings";

    auto acc = std::make_shared<com::chancho::qml::Account>(name, amount, memo);
    QCOMPARE(acc->getName(), name);

    QSignalSpy spy(acc.get(), SIGNAL(nameChanged(QString)));
    acc->setName(name);
    QCOMPARE(spy.count(), 0);
}

void
TestAccount::testSetNameSignal() {
    QString name = "Bankia";
    double amount = 3.4;
    QString memo = "Savings";

    auto acc = std::make_shared<com::chancho::qml::Account>(name, amount, memo);
    QCOMPARE(acc->getName(), name);

    QSignalSpy spy(acc.get(), SIGNAL(nameChanged(QString)));
    acc->setName("New name");
    QCOMPARE(spy.count(), 1);
}

void
TestAccount::testGetAmount() {
    QString name = "Bankia";
    double amount = 3.4;
    QString memo = "Savings";

    auto acc = std::make_shared<com::chancho::qml::Account>(name, amount, memo);
    QCOMPARE(acc->getAmount(), amount);
}

void
TestAccount::testSetAmountNoSignal() {
    QString name = "Bankia";
    double amount = 3.4;
    QString memo = "Savings";

    auto acc = std::make_shared<com::chancho::qml::Account>(name, amount, memo);
    QCOMPARE(acc->getAmount(), amount);

    QSignalSpy spy(acc.get(), SIGNAL(amountChanged(double)));
    acc->setAmount(amount);
    QCOMPARE(spy.count(), 0);
}

void
TestAccount::testSetAmountSignal() {
    QString name = "Bankia";
    double amount = 3.4;
    QString memo = "Savings";

    auto acc = std::make_shared<com::chancho::qml::Account>(name, amount, memo);
    QCOMPARE(acc->getAmount(), amount);

    QSignalSpy spy(acc.get(), SIGNAL(amountChanged(double)));
    acc->setAmount(amount + 1);
    QCOMPARE(spy.count(), 1);
}

void
TestAccount::testGetMemo() {
    QString name = "Bankia";
    double amount = 3.4;
    QString memo = "Savings";

    auto acc = std::make_shared<com::chancho::qml::Account>(name, amount, memo);
    QCOMPARE(acc->getMemo(), memo);
}

void
TestAccount::testSetMemoNoSignal() {
    QString name = "Bankia";
    double amount = 3.4;
    QString memo = "Savings";

    auto acc = std::make_shared<com::chancho::qml::Account>(name, amount, memo);
    QCOMPARE(acc->getMemo(), memo);

    QSignalSpy spy(acc.get(), SIGNAL(memoChanged(QString)));
    acc->setMemo(memo);
    QCOMPARE(spy.count(), 0);
}

void
TestAccount::testSetMemoSignal() {
    QString name = "Bankia";
    double amount = 3.4;
    QString memo = "Savings";

    auto acc = std::make_shared<com::chancho::qml::Account>(name, amount, memo);
    QCOMPARE(acc->getMemo(), memo);

    QSignalSpy spy(acc.get(), SIGNAL(memoChanged(QString)));
    acc->setMemo("New memo");
    QCOMPARE(spy.count(), 1);
}

void
TestAccount::testGetColor() {
    QString name = "Bankia";
    double amount = 3.4;
    QString memo = "Savings";
    QString color = "#fff";

    auto acc = std::make_shared<com::chancho::qml::Account>(name, amount, memo, color);
    QCOMPARE(acc->getColor(), color);
}

void
TestAccount::testSetColorNoSignal() {
    QString name = "Bankia";
    double amount = 3.4;
    QString memo = "Savings";
    QString color = "#fff";

    auto acc = std::make_shared<com::chancho::qml::Account>(name, amount, memo, color);
    QCOMPARE(acc->getColor(), color);

    QSignalSpy spy(acc.get(), SIGNAL(colorChanged(QString)));
    acc->setColor(color);
    QCOMPARE(spy.count(), 0);
}

void
TestAccount::testSetColorSignal() {
    QString name = "Bankia";
    double amount = 3.4;
    QString memo = "Savings";
    QString color = "#fff";

    auto acc = std::make_shared<com::chancho::qml::Account>(name, amount, memo, color);
    QCOMPARE(acc->getColor(), color);

    QSignalSpy spy(acc.get(), SIGNAL(colorChanged(QString)));
    acc->setColor("#000");
    QCOMPARE(spy.count(), 1);
}

QTEST_MAIN(TestAccount)

