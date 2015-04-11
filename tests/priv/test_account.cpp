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

#include <QVariant>
#include "test_account.h"

namespace chancho = com::chancho;

void
TestAccount::init() {
    BaseTestCase::init();

}

void
TestAccount::cleanup() {
    BaseTestCase::cleanup();
}

void
TestAccount::testNameConstructor_data() {
    QTest::addColumn<QString>("name");

    QTest::newRow("income-obj") << "Bankia";
    QTest::newRow("expense-obj") << "BBVA";
}

void
TestAccount::testNameConstructor() {
    QFETCH(QString, name);

    auto account = std::make_shared<PublicAccount>(name);
    QCOMPARE(account->name, name);
}

void
TestAccount::testNameAmountConstructor_data() {
    QTest::addColumn<QString>("name");
    QTest::addColumn<double>("amount");

    QTest::newRow("income-obj") << "Bankia" << 98.7;
    QTest::newRow("expense-obj") << "BBVA" << 23.4;
}

void
TestAccount::testNameAmountConstructor() {
    QFETCH(QString, name);
    QFETCH(double, amount);

    auto account = std::make_shared<PublicAccount>(name, amount);
    QCOMPARE(account->name, name);
    QVERIFY(account->amount == amount);
}

void
TestAccount::testNameAmountMemo_data() {
    QTest::addColumn<QString>("name");
    QTest::addColumn<double>("amount");
    QTest::addColumn<QString>("memo");

    QTest::newRow("income-obj") << "Bankia" << 98.7 << "Account used for the general usage.";
    QTest::newRow("expense-obj") << "BBVA" << 23.4 << "Savings";
}

void
TestAccount::testNameAmountMemo() {
    QFETCH(QString, name);
    QFETCH(double, amount);
    QFETCH(QString, memo);

    auto account = std::make_shared<PublicAccount>(name, amount, memo);
    QCOMPARE(account->name, name);
    QVERIFY(account->amount == amount);
    QCOMPARE(account->memo, memo);
}

void
TestAccount::testWasDbStored_data() {
    QTest::addColumn<QString>("name");
    QTest::addColumn<QUuid>("dbId");
    QTest::addColumn<bool>("result");

    QTest::newRow("income-obj") << "Bankia" << QUuid() << false;
    QTest::newRow("expense-obj") << "BBVA" << QUuid::createUuid() << true;
}

void
TestAccount::testWasDbStored() {
    QFETCH(QString, name);
    QFETCH(QUuid, dbId);
    QFETCH(bool, result);

    auto account = std::make_shared<PublicAccount>(name);
    account->_dbId = dbId;
    QCOMPARE(account->wasStoredInDb(), result);
}

void
TestAccount::testAccountEquals() {
    auto uuid = QUuid::createUuid();
    PublicAccount firstAcc;
    firstAcc._dbId = uuid;

    PublicAccount secondAcc;
    secondAcc._dbId = uuid;

    QVERIFY(firstAcc == secondAcc);
}

void
TestAccount::testAccountPtrEquals() {
    auto uuid = QUuid::createUuid();
    com::chancho::AccountPtr firtAcc = std::make_shared<PublicAccount>(uuid);
    com::chancho::AccountPtr secondAcc = std::make_shared<PublicAccount>(uuid);

    QVERIFY(firtAcc == secondAcc);
}

void
TestAccount::testAccountPtrNotEquals() {
    auto firstUuid = QUuid::createUuid();
    auto secondUuid = QUuid::createUuid();
    com::chancho::AccountPtr firtAcc = std::make_shared<PublicAccount>(firstUuid);
    com::chancho::AccountPtr secondAcc = std::make_shared<PublicAccount>(secondUuid);

    QVERIFY(firtAcc != secondAcc);
}

void
TestAccount::testAccountPtrEqualsNull() {
    auto uuid = QUuid::createUuid();
    com::chancho::AccountPtr firtAcc = std::make_shared<PublicAccount>(uuid);
    com::chancho::AccountPtr secondAcc;

    QVERIFY(!(firtAcc == secondAcc));
}

QTEST_MAIN(TestAccount)

