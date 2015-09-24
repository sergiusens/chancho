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

#include "public_attachment.h"
#include "public_transaction.h"
#include "test_transaction.h"

namespace chancho = com::chancho;

void
TestTransaction::init() {
    BaseTestCase::init();

}

void
TestTransaction::cleanup() {
    BaseTestCase::cleanup();
}

void
TestTransaction::testConstructorWithDate_data() {
    QTest::addColumn<chancho::AccountPtr>("account");
    QTest::addColumn<double >("amount");
    QTest::addColumn<QDate>("date");
    QTest::addColumn<chancho::CategoryPtr>("category");
    QTest::addColumn<QString>("content");
    QTest::addColumn<QString>("memo");

    auto firstAccount = std::make_shared<chancho::Account>("Bankia", 23.4);
    auto secondAccount = std::make_shared<chancho::Account>("BBVA", 9823.22);

    auto firstCategory = std::make_shared<chancho::Category>("Food", chancho::Category::Type::EXPENSE);
    auto secondCategory = std::make_shared<chancho::Category>("Salary", chancho::Category::Type::INCOME);

    QTest::newRow("income-obj") << firstAccount << 9.0 << QDate::fromString("1999-10-15", "yyyy-MM-dd") << firstCategory  << "Test content" << "My memo";
    QTest::newRow("expense-obj") << secondAccount << 2901.2 << QDate::fromString("2012-02-15", "yyyy-MM-dd") << secondCategory << "" << "Hello";
}

void
TestTransaction::testConstructorWithDate() {
    QFETCH(chancho::AccountPtr, account);
    QFETCH(double, amount);
    QFETCH(QDate, date);
    QFETCH(chancho::CategoryPtr, category);
    QFETCH(QString, content);
    QFETCH(QString, memo);

    auto transaction = std::make_shared<PublicTransaction>(account, amount, category, date, content, memo);
    QCOMPARE(transaction->account->name, account->name);
    QCOMPARE(transaction->amount, amount);
    QCOMPARE(transaction->category->name, category->name);
    QCOMPARE(transaction->contents, content);
    QCOMPARE(transaction->memo, memo);
    QCOMPARE(transaction->date, date);
}

void
TestTransaction::testContrutorCurrentDate_data() {
    QTest::addColumn<chancho::AccountPtr>("account");
    QTest::addColumn<double >("amount");
    QTest::addColumn<chancho::CategoryPtr>("category");
    QTest::addColumn<QString>("content");
    QTest::addColumn<QString>("memo");

    auto firstAccount = std::make_shared<chancho::Account>("Bankia", 23.4);
    auto secondAccount = std::make_shared<chancho::Account>("BBVA", 9823.22);

    auto firstCategory = std::make_shared<chancho::Category>("Food", chancho::Category::Type::EXPENSE);
    auto secondCategory = std::make_shared<chancho::Category>("Salary", chancho::Category::Type::INCOME);

    QTest::newRow("income-obj") << firstAccount << 9.0 << firstCategory  << "Test content" << "My memo";
    QTest::newRow("expense-obj") << secondAccount << 2901.2 << secondCategory << "" << "Hello";
}

void
TestTransaction::testContrutorCurrentDate() {
    QFETCH(chancho::AccountPtr, account);
    QFETCH(double, amount);
    QFETCH(chancho::CategoryPtr, category);
    QFETCH(QString, content);
    QFETCH(QString, memo);

    auto transaction = std::make_shared<PublicTransaction>(account, amount, category, content, memo);
    QCOMPARE(transaction->account->name, account->name);
    QCOMPARE(transaction->amount, amount);
    QCOMPARE(transaction->category->name, category->name);
    QCOMPARE(transaction->contents, content);
    QCOMPARE(transaction->memo, memo);
    QCOMPARE(transaction->date, QDate::currentDate());
}

void
TestTransaction::testWasDbStored_data() {
    QTest::addColumn<chancho::AccountPtr>("account");
    QTest::addColumn<double >("amount");
    QTest::addColumn<chancho::CategoryPtr>("category");
    QTest::addColumn<QString>("content");
    QTest::addColumn<QString>("memo");
    QTest::addColumn<QUuid>("uuid");
    QTest::addColumn<bool>("result");

    auto firstAccount = std::make_shared<chancho::Account>("Bankia", 23.4);
    auto secondAccount = std::make_shared<chancho::Account>("BBVA", 9823.22);

    auto firstCategory = std::make_shared<chancho::Category>("Food", chancho::Category::Type::EXPENSE);
    auto secondCategory = std::make_shared<chancho::Category>("Salary", chancho::Category::Type::INCOME);

    QTest::newRow("income-obj") << firstAccount << 9.0 << firstCategory
            << "Test content" << "My memo" << QUuid() << false;
    QTest::newRow("expense-obj") << secondAccount << 2901.2 << secondCategory
            << "" << "Hello" << QUuid::createUuid() << true;
}

void
TestTransaction::testWasDbStored() {
    QFETCH(chancho::AccountPtr, account);
    QFETCH(double, amount);
    QFETCH(chancho::CategoryPtr, category);
    QFETCH(QString, content);
    QFETCH(QString, memo);
    QFETCH(QUuid, uuid);
    QFETCH(bool, result);

    auto transaction = std::make_shared<PublicTransaction>(account, amount, category, content, memo);
    transaction->_dbId = uuid;
    QCOMPARE(transaction->wasStoredInDb(), result);
}

void
TestTransaction::testType_data() {
    QTest::addColumn<chancho::AccountPtr>("account");
    QTest::addColumn<double >("amount");
    QTest::addColumn<chancho::CategoryPtr>("category");
    QTest::addColumn<QString>("content");
    QTest::addColumn<QString>("memo");

    auto firstAccount = std::make_shared<chancho::Account>("Bankia", 23.4);
    auto secondAccount = std::make_shared<chancho::Account>("BBVA", 9823.22);

    auto firstCategory = std::make_shared<chancho::Category>("Food", chancho::Category::Type::EXPENSE);
    auto secondCategory = std::make_shared<chancho::Category>("Salary", chancho::Category::Type::INCOME);

    QTest::newRow("income-obj") << firstAccount << 9.0 << firstCategory << "Test content" << "My memo";
    QTest::newRow("expense-obj") << secondAccount << 2901.2 << secondCategory << "" << "Hello";
}

void
TestTransaction::testType() {
    QFETCH(chancho::AccountPtr, account);
    QFETCH(double, amount);
    QFETCH(chancho::CategoryPtr, category);
    QFETCH(QString, content);
    QFETCH(QString, memo);

    auto transaction = std::make_shared<PublicTransaction>(account, amount, category, content, memo);
    QCOMPARE(transaction->type(), category->type);
}

void
TestTransaction::testTypeMissingCat_data() {
    QTest::addColumn<chancho::AccountPtr>("account");
    QTest::addColumn<double >("amount");
    QTest::addColumn<chancho::CategoryPtr>("category");
    QTest::addColumn<QString>("content");
    QTest::addColumn<QString>("memo");

    auto firstAccount = std::make_shared<chancho::Account>("Bankia", 23.4);
    auto secondAccount = std::make_shared<chancho::Account>("BBVA", 9823.22);

    chancho::CategoryPtr firstCategory;
    chancho::CategoryPtr secondCategory;

    QTest::newRow("income-obj") << firstAccount << 9.0 << firstCategory << "Test content" << "My memo";
    QTest::newRow("expense-obj") << secondAccount << 2901.2 << secondCategory << "" << "Hello";
}

void
TestTransaction::testTypeMissingCat() {
    QFETCH(chancho::AccountPtr, account);
    QFETCH(double, amount);
    QFETCH(chancho::CategoryPtr, category);
    QFETCH(QString, content);
    QFETCH(QString, memo);

    auto transaction = std::make_shared<PublicTransaction>(account, amount, category, content, memo);
    QCOMPARE(chancho::Category::Type::EXPENSE, transaction->type());
}

void
TestTransaction::testAddAttachment_data() {
    QTest::addColumn<QString>("name");
    QTest::addColumn<QByteArray>("data");

    QTest::newRow("first-obj") << "first attachment" << QByteArray();
    QTest::newRow("second-obj") << "second attachment" << QByteArray(80, 'g');
    QTest::newRow("third-obj") << "third attachment" << QByteArray(90, 'a');
    QTest::newRow("fourth-obj") << "fourth attachment" << QByteArray(12, 'r');
    QTest::newRow("last-obj") << "last attachment" << QByteArray(1, 't');
}

void
TestTransaction::testAddAttachment() {
    QFETCH(QString , name);
    QFETCH(QByteArray, data);

    auto account = std::make_shared<chancho::Account>("Bankia", 23.4);
    auto category = std::make_shared<com::chancho::Category>();
    auto transaction = std::make_shared<PublicTransaction>(account, 2.3, category, "", "");
    auto attachment = std::make_shared<com::chancho::Transaction::Attachment>(name, data);
    transaction->attach(attachment);
    QVERIFY(transaction->attachments().count() == 1);
    auto result = transaction->attachments().at(0);
    QCOMPARE(name, result->name);
    QCOMPARE(data, result->data);
}

void
TestTransaction::testRemoveAttachment() {
    QString name("Foo");
    QByteArray data(23, 't');

    auto account = std::make_shared<chancho::Account>("Bankia", 23.4);
    auto category = std::make_shared<com::chancho::Category>();
    auto transaction = std::make_shared<PublicTransaction>(account, 2.3, category, "", "");
    auto attachment = std::make_shared<com::chancho::Transaction::Attachment>(name, data);
    transaction->attach(attachment);

    QVERIFY(transaction->attachments().count() == 1);
    auto result = transaction->attachments().at(0);

    QCOMPARE(name, result->name);
    QCOMPARE(data, result->data);

    transaction->detach(attachment);
    QVERIFY(transaction->attachments().count() == 0);

    transaction->detach(attachment);
    QVERIFY(transaction->attachments().count() == 0);
}

void
TestTransaction::testRemoveAttachmentName() {
    QString name("Foo");
    QByteArray data(23, 't');

    auto account = std::make_shared<chancho::Account>("Bankia", 23.4);
    auto category = std::make_shared<com::chancho::Category>();
    auto transaction = std::make_shared<PublicTransaction>(account, 2.3, category, "", "");
    auto attachment = std::make_shared<PublicAttachment>(name, data);
    transaction->attach(attachment);

    QVERIFY(transaction->attachments().count() == 1);
    auto result = transaction->attachments().at(0);

    QCOMPARE(name, result->name);
    QCOMPARE(data, result->data);

    transaction->detach(attachment->_id.toString());
    QVERIFY(transaction->attachments().count() == 0);

    transaction->detach(attachment->_id.toString());
    QVERIFY(transaction->attachments().count() == 0);
}

QTEST_MAIN(TestTransaction)
