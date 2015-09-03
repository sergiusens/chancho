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

#include <com/chancho/qml/recurrent_transaction.h>
#include "public_account.h"
#include "public_qml_transaction.h"
#include "test_transaction.h"

void
TestTransaction::init() {
    BaseTestCase::init();
}

void
TestTransaction::cleanup() {
    BaseTestCase::cleanup();
}

void
TestTransaction::testGetAccount_data() {
    QTest::addColumn<QString>("name");
    QTest::addColumn<double>("initialAmount");
    QTest::addColumn<double>("amount");
    QTest::addColumn<QString>("memo");
    QTest::addColumn<QString>("color");

    QTest::newRow("bankia-obj") << "Bankia" << 50.0 << 3004.89 << "Savings account" << "#ff";
    QTest::newRow("bbva-obj") << "BBVA" << 10.9 << 23.9 << "Student loan" << "#234523";
}

void
TestTransaction::testGetAccount() {
    QFETCH(QString, name);
    QFETCH(double, initialAmount);
    QFETCH(double, amount);
    QFETCH(QString, memo);
    QFETCH(QString, color);

    // create the category and store it
    auto account = std::make_shared<PublicAccount>(name, amount, memo, color);
    account->initialAmount = initialAmount;
    auto category = std::make_shared<com::chancho::Category>("Test category", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, .50, category);

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicTransaction>(transactionPtr);
    QCOMPARE(account->name, qmlTransaction->getAccount());
}

void
TestTransaction::testGetAmount_data() {
    QTest::addColumn<double>("amount");

    QTest::newRow("first-amount") << 0.5;
    QTest::newRow("second-amount") << 130.45;
}

void
TestTransaction::testGetAmount() {
    QFETCH(double, amount);

    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Test category", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicTransaction>(transactionPtr);
    QCOMPARE(amount, qmlTransaction->getAmount());
}

void
TestTransaction::testSetAmountNoSignal() {
    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Test category", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicTransaction>(transactionPtr);

    QSignalSpy spy(qmlTransaction.get(), SIGNAL(amountChanged(double)));
    qmlTransaction->setAmount(amount);
    QCOMPARE(spy.count(), 0);
}

void
TestTransaction::testSetAmount() {
    auto amount = .45;
    auto newAmount = 2.45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Test category", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicTransaction>(transactionPtr);

    QSignalSpy spy(qmlTransaction.get(), SIGNAL(amountChanged(double)));
    qmlTransaction->setAmount(newAmount);
    QCOMPARE(spy.count(), 1);
}

void
TestTransaction::testGetCategory_data() {
    QTest::addColumn<QString>("categoryName");

    QTest::newRow("first-category") << "My testing cat";
    QTest::newRow("second-category") << "Sushi: Food";
}

void
TestTransaction::testGetCategory() {
    QFETCH(QString, categoryName);

    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>(categoryName, com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicTransaction>(transactionPtr);
    QCOMPARE(categoryName, qmlTransaction->getCategory());
}

void
TestTransaction::testGetContents() {
    auto contents = QString("Dinner with friends");
    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Sushi", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    transactionPtr->contents = contents;

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicTransaction>(transactionPtr);
    QCOMPARE(qmlTransaction->getContents(), contents);
}

void
TestTransaction::testSetContentsNoSignal() {
    auto contents = QString("Dinner with friends");
    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Sushi", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    transactionPtr->contents = contents;

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicTransaction>(transactionPtr);
    QSignalSpy spy(qmlTransaction.get(), SIGNAL(contentsChanged(QString)));
    qmlTransaction->setContents(contents);
    QCOMPARE(spy.count(), 0);
}

void
TestTransaction::testSetContents() {
    auto contents = QString("Dinner with friends");
    auto newContents = contents + "23";
    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Sushi", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    transactionPtr->contents = contents;

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicTransaction>(transactionPtr);
    QSignalSpy spy(qmlTransaction.get(), SIGNAL(contentsChanged(QString)));
    qmlTransaction->setContents(newContents);
    QCOMPARE(spy.count(), 1);
}

void
TestTransaction::testGetMemo() {
    auto memo = QString("Dinner with friends");
    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Sushi", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    transactionPtr->memo = memo;

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicTransaction>(transactionPtr);
    QCOMPARE(qmlTransaction->getMemo(), memo);
}

void
TestTransaction::testSetMemoNoSignal() {
    auto memo = QString("Dinner with friends");
    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Sushi", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    transactionPtr->memo = memo;

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicTransaction>(transactionPtr);
    QSignalSpy spy(qmlTransaction.get(), SIGNAL(memoChanged(QString)));
    qmlTransaction->setMemo(memo);
    QCOMPARE(spy.count(), 0);
}

void
TestTransaction::testSetMemo() {
    auto memo = QString("Dinner with friends");
    auto newMemo = memo + "23";
    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Sushi", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    transactionPtr->memo = memo;

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicTransaction>(transactionPtr);
    QSignalSpy spy(qmlTransaction.get(), SIGNAL(memoChanged(QString)));
    qmlTransaction->setMemo(newMemo);
    QCOMPARE(spy.count(), 1);
}

void
TestTransaction::testGetType_data() {
    QTest::addColumn<com::chancho::Category::Type>("transactionType");
    QTest::addColumn<com::chancho::qml::Book::TransactionType>("qmlType");

    QTest::newRow("income") << com::chancho::Category::Type::INCOME << com::chancho::qml::Book::INCOME;
    QTest::newRow("expense") << com::chancho::Category::Type::EXPENSE << com::chancho::qml::Book::EXPENSE;
}

void
TestTransaction::testGetType() {
    QFETCH(com::chancho::Category::Type, transactionType);
    QFETCH(com::chancho::qml::Book::TransactionType, qmlType);

    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Sushi", transactionType);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicTransaction>(transactionPtr);
    QCOMPARE(qmlTransaction->getType(), qmlType);
}

void
TestTransaction::testSetIsRecurrentNoSignal() {
    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Sushi", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    transactionPtr->is_recurrent = true;


    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicTransaction>(transactionPtr);
    QSignalSpy spy(qmlTransaction.get(), SIGNAL(isRecurrentChanged(bool)));
    qmlTransaction->setIsRecurrent(true);
    QCOMPARE(spy.count(), 0);
}

void
TestTransaction::testSetIsRecurrent() {
    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Sushi", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    transactionPtr->is_recurrent = false;


    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicTransaction>(transactionPtr);
    QSignalSpy spy(qmlTransaction.get(), SIGNAL(isRecurrentChanged(bool)));
    qmlTransaction->setIsRecurrent(true);
    QCOMPARE(spy.count(), 1);
}

void
TestTransaction::testGetIsRecurrent_data() {
    QTest::addColumn<bool>("isRecurrent");

    QTest::newRow("true") << true;
    QTest::newRow("false") << false;
}

void
TestTransaction::testGetIsRecurrent() {
    QFETCH(bool, isRecurrent);

    auto amount = .45;
    auto account = std::make_shared<PublicAccount>("Test account", .0, "");
    auto category = std::make_shared<com::chancho::Category>("Sushi", com::chancho::Category::Type::EXPENSE);
    auto transactionPtr = std::make_shared<com::chancho::Transaction>(account, amount, category);
    transactionPtr->is_recurrent = isRecurrent;

    auto qmlTransaction = std::make_shared<com::chancho::tests::PublicTransaction>(transactionPtr);
    QCOMPARE(qmlTransaction->getIsRecurrent(), isRecurrent);
}

QTEST_MAIN(TestTransaction)

