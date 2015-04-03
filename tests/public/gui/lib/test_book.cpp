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

#include <gmock/gmock.h>

#include <com/chancho/qml/account.h>
#include <com/chancho/qml/book.h>
#include <com/chancho/qml/category.h>
#include <com/chancho/qml/transaction.h>

#include "book.h"
#include "matchers.h"
#include "public_account.h"
#include "public_category.h"
#include "public_qml_account.h"
#include "public_qml_category.h"

#include "test_book.h"

using ::testing::_;
using ::testing::Mock;
using ::testing::Matcher;
using ::testing::AnyNumber;
using ::testing::Return;
using ::testing::AnyOf;

void
TestBook::init() {
    BaseTestCase::init();
}

void
TestBook::cleanup() {
    BaseTestCase::cleanup();
}

void
TestBook::testStoreTransaction() {
    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);

    QSignalSpy spy(qmlBook.get(), SIGNAL(transactionStored(QDate)));
    QSignalSpy accSpy(qmlBook.get(), SIGNAL(accountUpdated()));

    auto account = new com::chancho::qml::Account();
    auto category = new com::chancho::qml::Category();
    QDate date(2014, 2, 1);
    double amount = 2.5;
    QString contents = "The contents of the transaction";
    QString memo = "The memo";

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::TransactionPtr>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto stored = qmlBook->storeTransaction(account, category, date, amount, contents, memo);

    QVERIFY(stored);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(accSpy.count(), 1);
}

void
TestBook::testStoreTransactionWrongAccount() {
    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);

    QSignalSpy spy(qmlBook.get(), SIGNAL(transactionStored(QDate)));
    QSignalSpy accSpy(qmlBook.get(), SIGNAL(accountUpdated()));

    auto account = new com::chancho::qml::Category();
    auto category = new com::chancho::qml::Category();
    QDate date(2014, 2, 1);
    double amount = 2.5;
    QString contents = "The contents of the transaction";
    QString memo = "The memo";

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::TransactionPtr>(_)))
            .Times(0);

    auto stored = qmlBook->storeTransaction(account, category, date, amount, contents, memo);

    QVERIFY(!stored);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(accSpy.count(), 0);
}

void
TestBook::testStoreTransactionWrongCategory() {
    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);

    QSignalSpy spy(qmlBook.get(), SIGNAL(transactionStored(QDate)));
    QSignalSpy accSpy(qmlBook.get(), SIGNAL(accountUpdated()));

    auto account = new com::chancho::qml::Account();
    auto category = new com::chancho::qml::Account();
    QDate date(2014, 2, 1);
    double amount = 2.5;
    QString contents = "The contents of the transaction";
    QString memo = "The memo";

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::TransactionPtr>(_)))
            .Times(0);

    auto stored = qmlBook->storeTransaction(account, category, date, amount, contents, memo);

    QVERIFY(!stored);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(accSpy.count(), 0);
}

void
TestBook::testStoreTransactionBookError() {
    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);

    QSignalSpy spy(qmlBook.get(), SIGNAL(transactionStored(QDate)));
    QSignalSpy accSpy(qmlBook.get(), SIGNAL(accountUpdated()));

    auto account = new com::chancho::qml::Account();
    auto category = new com::chancho::qml::Category();
    QDate date(2014, 2, 1);
    double amount = 2.5;
    QString contents = "The contents of the transaction";
    QString memo = "The memo";

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::TransactionPtr>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(true));

    auto stored = qmlBook->storeTransaction(account, category, date, amount, contents, memo);

    QVERIFY(!stored);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(accSpy.count(), 0);
}

void
TestBook::testRemoveTransaction() {
    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);
    auto qmlTransaction = std::make_shared<com::chancho::qml::Transaction>();

    QSignalSpy spy(qmlBook.get(), SIGNAL(transactionRemoved(QDate)));
    QSignalSpy accSpy(qmlBook.get(), SIGNAL(accountUpdated()));

    EXPECT_CALL(*book.get(), remove(Matcher<com::chancho::TransactionPtr>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto removed = qmlBook->removeTransaction(qmlTransaction.get());

    QVERIFY(removed);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(accSpy.count(), 1);
}

void
TestBook::testRemoveTransactionWrongObject() {
    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);
    auto qmlTransaction = std::make_shared<com::chancho::qml::Category>();

    QSignalSpy spy(qmlBook.get(), SIGNAL(transactionRemoved(QDate)));
    QSignalSpy accSpy(qmlBook.get(), SIGNAL(accountUpdated()));

    EXPECT_CALL(*book.get(), remove(Matcher<com::chancho::TransactionPtr>(_)))
            .Times(0);

    auto removed = qmlBook->removeTransaction(qmlTransaction.get());

    QVERIFY(!removed);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(accSpy.count(), 0);
}

void
TestBook::testRemoveTransactionBookError() {
    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);
    auto qmlTransaction = std::make_shared<com::chancho::qml::Transaction>();

    QSignalSpy spy(qmlBook.get(), SIGNAL(transactionRemoved(QDate)));
    QSignalSpy accSpy(qmlBook.get(), SIGNAL(accountUpdated()));

    EXPECT_CALL(*book.get(), remove(Matcher<com::chancho::TransactionPtr>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(true));

    auto removed = qmlBook->removeTransaction(qmlTransaction.get());

    QVERIFY(!removed);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(accSpy.count(), 0);
}

void
TestBook::testUpdateTransactionWrongAccount() {
    auto tran = std::make_shared<com::chancho::qml::Transaction>();
    auto account = std::make_shared<com::chancho::qml::Transaction>();
    auto category = std::make_shared<com::chancho::qml::Category>();
    QDate date(2014, 2, 1);
    QString conents = "My new contents";
    QString memo = "My new memo";
    double amount = 23.4;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);
    auto qmlTransaction = std::make_shared<com::chancho::qml::Transaction>();

    QSignalSpy spy(qmlBook.get(), SIGNAL(transactionUpdated(QDate, QDate)));
    QSignalSpy accSpy(qmlBook.get(), SIGNAL(accountUpdated()));

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::TransactionPtr>(_)))
            .Times(0);

    auto updated = qmlBook->updateTransaction(tran.get(), account.get(), category.get(), date, conents, memo, amount);

    QVERIFY(!updated);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(accSpy.count(), 0);
}

void
TestBook::testUpdateTransactionWrongCategory() {
    auto tran = std::make_shared<com::chancho::qml::Transaction>();
    auto account = std::make_shared<com::chancho::qml::Account>();
    auto category = std::make_shared<com::chancho::qml::Account>();
    QDate date(2014, 2, 1);
    QString conents = "My new contents";
    QString memo = "My new memo";
    double amount = 23.4;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);
    auto qmlTransaction = std::make_shared<com::chancho::qml::Transaction>();

    QSignalSpy spy(qmlBook.get(), SIGNAL(transactionUpdated(QDate, QDate)));
    QSignalSpy accSpy(qmlBook.get(), SIGNAL(accountUpdated()));

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::TransactionPtr>(_)))
            .Times(0);

    auto updated = qmlBook->updateTransaction(tran.get(), account.get(), category.get(), date, conents, memo, amount);

    QVERIFY(!updated);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(accSpy.count(), 0);
}

void
TestBook::testUpdateTransactionWrongObject() {
    auto tran = std::make_shared<com::chancho::qml::Account>();
    auto account = std::make_shared<com::chancho::qml::Account>();
    auto category = std::make_shared<com::chancho::qml::Category>();
    QDate date(2014, 2, 1);
    QString conents = "My new contents";
    QString memo = "My new memo";
    double amount = 23.4;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);
    auto qmlTransaction = std::make_shared<com::chancho::qml::Transaction>();

    QSignalSpy spy(qmlBook.get(), SIGNAL(transactionUpdated(QDate, QDate)));
    QSignalSpy accSpy(qmlBook.get(), SIGNAL(accountUpdated()));

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::TransactionPtr>(_)))
            .Times(0);

    auto updated = qmlBook->updateTransaction(tran.get(), account.get(), category.get(), date, conents, memo, amount);

    QVERIFY(!updated);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(accSpy.count(), 0);
}

void
TestBook::testUpdateTransactionNeedsUpdate() {
    auto tran = std::make_shared<com::chancho::qml::Transaction>();
    com::chancho::AccountPtr accountPtr = std::make_shared<PublicAccount>(QUuid::createUuid());
    auto account = std::make_shared<com::chancho::tests::PublicAccount>(accountPtr);
    com::chancho::CategoryPtr categoryPtr = std::make_shared<PublicCategory>(QUuid::createUuid());
    auto category = std::make_shared<com::chancho::tests::PublicCategory>(categoryPtr);

    QDate date(2014, 2, 1);
    QString conents = "My new contents";
    QString memo = "My new memo";
    double amount = 23.4;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);
    auto qmlTransaction = std::make_shared<com::chancho::qml::Transaction>();

    QSignalSpy spy(qmlBook.get(), SIGNAL(transactionUpdated(QDate, QDate)));
    QSignalSpy accSpy(qmlBook.get(), SIGNAL(accountUpdated()));

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::TransactionPtr>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto updated = qmlBook->updateTransaction(tran.get(), account.get(), category.get(), date, conents, memo, amount);

    QVERIFY(updated);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(accSpy.count(), 1);
}

void
TestBook::testUpdateTransactionBookError() {
    auto tran = std::make_shared<com::chancho::qml::Transaction>();
    auto account = std::make_shared<com::chancho::qml::Account>();
    auto category = std::make_shared<com::chancho::qml::Category>();
    QDate date(2014, 2, 1);
    QString conents = "My new contents";
    QString memo = "My new memo";
    double amount = 23.4;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);
    auto qmlTransaction = std::make_shared<com::chancho::qml::Transaction>();

    QSignalSpy spy(qmlBook.get(), SIGNAL(transactionUpdated(QDate, QDate)));
    QSignalSpy accSpy(qmlBook.get(), SIGNAL(accountUpdated()));

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::TransactionPtr>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(true));

    auto updated = qmlBook->updateTransaction(tran.get(), account.get(), category.get(), date, conents, memo, amount);

    QVERIFY(!updated);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(accSpy.count(), 0);
}

void
TestBook::testStoreAccountInitialAmount() {
    QString name = "BBVA";
    QString memo = "First test account";
    QString color = "#fff";
    double initialAmount = 34.20;

    auto expectedAccount = std::make_shared<com::chancho::Account>(name, initialAmount, memo, color);
    expectedAccount->initialAmount = initialAmount;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);

    QSignalSpy spy(qmlBook.get(), SIGNAL(accountStored()));

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::AccountPtr>(AccountEquals(expectedAccount))))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto stored = qmlBook->storeAccount(name, memo, color, initialAmount);
    QVERIFY(stored);
    QCOMPARE(spy.count(), 1);
}

void
TestBook::testStoreAccountNoInitialAmount() {
    QString name = "BBVA";
    QString memo = "First test account";
    QString color = "#fff";
    double initialAmount = 0;

    auto expectedAccount = std::make_shared<com::chancho::Account>(name, initialAmount, memo, color);
    expectedAccount->initialAmount = initialAmount;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);

    QSignalSpy spy(qmlBook.get(), SIGNAL(accountStored()));

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::AccountPtr>(AccountEquals(expectedAccount))))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto stored = qmlBook->storeAccount(name, memo, color, initialAmount);
    QVERIFY(stored);
    QCOMPARE(spy.count(), 1);
}

void
TestBook::testStoreAccountBookError() {
    QString name = "BBVA";
    QString memo = "First test account";
    QString color = "#fff";
    double initialAmount = 0;

    auto expectedAccount = std::make_shared<com::chancho::Account>(name, initialAmount, memo, color);
    expectedAccount->initialAmount = initialAmount;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);

    QSignalSpy spy(qmlBook.get(), SIGNAL(accountStored()));

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::AccountPtr>(AccountEquals(expectedAccount))))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(true));

    auto stored = qmlBook->storeAccount(name, memo, color, initialAmount);
    QVERIFY(!stored);
    QCOMPARE(spy.count(), 0);
}

void
TestBook::testRemoveAccountWrongObj() {
    auto other = std::make_shared<com::chancho::qml::Category>();

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);

    QSignalSpy spy(qmlBook.get(), SIGNAL(accountRemoved()));

    EXPECT_CALL(*book.get(), remove(Matcher<com::chancho::AccountPtr>(_)))
            .Times(0);

    auto removed = qmlBook->removeAccount(other.get());
    QVERIFY(!removed);
    QCOMPARE(spy.count(), 0);
}

void
TestBook::testRemoveAccount() {
    auto acc = std::make_shared<com::chancho::qml::Account>();

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);

    QSignalSpy spy(qmlBook.get(), SIGNAL(accountRemoved()));

    EXPECT_CALL(*book.get(), remove(Matcher<com::chancho::AccountPtr>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto removed = qmlBook->removeAccount(acc.get());
    QVERIFY(removed);
    QCOMPARE(spy.count(), 1);
}

void
TestBook::testRemoveAccountBookError() {
    auto acc = std::make_shared<com::chancho::qml::Account>();

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);

    QSignalSpy spy(qmlBook.get(), SIGNAL(accountRemoved()));

    EXPECT_CALL(*book.get(), remove(Matcher<com::chancho::AccountPtr>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(true));

    auto removed = qmlBook->removeAccount(acc.get());
    QVERIFY(!removed);
    QCOMPARE(spy.count(), 0);
}

void
TestBook::testUpdateAccountWrongObj() {
    auto other = std::make_shared<com::chancho::qml::Category>();

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);

    QSignalSpy spy(qmlBook.get(), SIGNAL(accountUpdated()));

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::AccountPtr>(_)))
            .Times(0);

    auto updated = qmlBook->updateAccount(other.get(), "New name", "New memo", "#fdf");
    QVERIFY(!updated);
    QCOMPARE(spy.count(), 0);
}

void
TestBook::testUpdateAccountNoNeed() {
    auto accPtr = std::make_shared<com::chancho::Account>("BBVA", 50.0, "Memo", "#ff");
    auto other = std::make_shared<com::chancho::tests::PublicAccount>(accPtr);

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);

    QSignalSpy spy(qmlBook.get(), SIGNAL(accountUpdated()));

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::AccountPtr>(_)))
            .Times(0);

    auto updated = qmlBook->updateAccount(other.get(), accPtr->name, accPtr->memo, accPtr->color);
    QVERIFY(!updated);
    QCOMPARE(spy.count(), 0);
}

void
TestBook::testUpdateAccount() {
    auto accPtr = std::make_shared<com::chancho::Account>("BBVA", 50.0, "Memo", "#ff");
    auto other = std::make_shared<com::chancho::tests::PublicAccount>(accPtr);

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);

    QSignalSpy spy(qmlBook.get(), SIGNAL(accountUpdated()));

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::AccountPtr>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto updated = qmlBook->updateAccount(other.get(), "New Name", accPtr->memo, accPtr->color);
    QVERIFY(updated);
    QCOMPARE(spy.count(), 1);
}

void
TestBook::testUpdateAccountBookError() {
    auto accPtr = std::make_shared<com::chancho::Account>("BBVA", 50.0, "Memo", "#ff");
    auto other = std::make_shared<com::chancho::tests::PublicAccount>(accPtr);

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);

    QSignalSpy spy(qmlBook.get(), SIGNAL(accountUpdated()));

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::AccountPtr>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(true));

    auto updated = qmlBook->updateAccount(other.get(), "New Name", accPtr->memo, accPtr->color);
    QVERIFY(!updated);
    QCOMPARE(spy.count(), 0);
}

void
TestBook::testStoreCategoryIncome() {
    QString name = "Test category";
    QString color = "#123";
    auto type = com::chancho::qml::Book::INCOME;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);

    QSignalSpy spy(qmlBook.get(), SIGNAL(categoryStored(Book::TransactionType)));

    auto cat = std::make_shared<com::chancho::Category>(name, com::chancho::Category::Type::EXPENSE, color);

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::CategoryPtr>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto stored = qmlBook->storeCategory(name, color, type);
    QVERIFY(stored);
    QCOMPARE(spy.count(), 1);
}

void
TestBook::testStoreCategoryExpense() {
    QString name = "Test category";
    QString color = "#123";
    auto type = com::chancho::qml::Book::EXPENSE;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);

    QSignalSpy spy(qmlBook.get(), SIGNAL(categoryStored(Book::TransactionType)));

    auto cat = std::make_shared<com::chancho::Category>(name, com::chancho::Category::Type::EXPENSE, color);

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::CategoryPtr>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto stored = qmlBook->storeCategory(name, color, type);
    QVERIFY(stored);
    QCOMPARE(spy.count(), 1);
}

void
TestBook::testStoreCategoryBookError() {
    QString name = "Test category";
    QString color = "#123";
    auto type = com::chancho::qml::Book::EXPENSE;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);

    QSignalSpy spy(qmlBook.get(), SIGNAL(categoryStored(Book::TransactionType)));

    auto cat = std::make_shared<com::chancho::Category>(name, com::chancho::Category::Type::EXPENSE, color);

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::CategoryPtr>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(true));

    auto stored = qmlBook->storeCategory(name, color, type);
    QVERIFY(!stored);
    QCOMPARE(spy.count(), 0);
}

void
TestBook::testUpdateCategoryWrongObj() {
    auto other = std::make_shared<com::chancho::qml::Account>();
    QString name = "Test name";
    QString color = "Test color";
    auto type = com::chancho::qml::Book::EXPENSE;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);

    QSignalSpy spy(qmlBook.get(), SIGNAL(categoryUpdated(Book::TransactionType)));

    auto cat = std::make_shared<com::chancho::Category>(name, com::chancho::Category::Type::EXPENSE, color);

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::CategoryPtr>(_)))
            .Times(0);

    EXPECT_CALL(*book.get(), isError())
            .Times(0);

    auto updated = qmlBook->updateCategory(other.get(), name, color, type);
    QVERIFY(!updated);
    QCOMPARE(spy.count(), 0);
}

void
TestBook::testUpdateCategoryNoUpdate() {
    QString name = "Test name";
    QString color = "Test color";
    auto type = com::chancho::qml::Book::EXPENSE;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);

    QSignalSpy spy(qmlBook.get(), SIGNAL(categoryUpdated(Book::TransactionType)));

    auto cat = std::make_shared<com::chancho::Category>(name, com::chancho::Category::Type::EXPENSE, color);
    auto qmlCat = std::make_shared<com::chancho::tests::PublicCategory>(cat);

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::CategoryPtr>(_)))
            .Times(0);

    EXPECT_CALL(*book.get(), isError())
            .Times(0);

    auto updated = qmlBook->updateCategory(qmlCat.get(), name, color, type);
    QVERIFY(!updated);
    QCOMPARE(spy.count(), 0);
}

void
TestBook::testUpdateSameType() {
    QString name = "Test name";
    QString color = "Test color";
    auto type = com::chancho::qml::Book::EXPENSE;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);

    QSignalSpy spy(qmlBook.get(), SIGNAL(categoryUpdated(Book::TransactionType)));
    QSignalSpy typeSpy(qmlBook.get(), SIGNAL(categoryTypeUpdated()));

    auto cat = std::make_shared<com::chancho::Category>(name, com::chancho::Category::Type::EXPENSE, color);
    auto qmlCat = std::make_shared<com::chancho::tests::PublicCategory>(cat);

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::CategoryPtr>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto updated = qmlBook->updateCategory(qmlCat.get(), "New name", color, type);
    QVERIFY(updated);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(typeSpy.count(), 0);
}

void
TestBook::testUpdateDiffType() {
    QString name = "Test name";
    QString color = "Test color";

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);

    QSignalSpy spy(qmlBook.get(), SIGNAL(categoryUpdated(Book::TransactionType)));
    QSignalSpy typeSpy(qmlBook.get(), SIGNAL(categoryTypeUpdated()));

    auto cat = std::make_shared<com::chancho::Category>(name, com::chancho::Category::Type::EXPENSE, color);
    auto qmlCat = std::make_shared<com::chancho::tests::PublicCategory>(cat);

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::CategoryPtr>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto updated = qmlBook->updateCategory(qmlCat.get(), "New name", color, com::chancho::qml::Book::INCOME);
    QVERIFY(updated);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(typeSpy.count(), 1);
}

void
TestBook::testUpdateBookError() {
    QString name = "Test name";
    QString color = "Test color";

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);

    QSignalSpy spy(qmlBook.get(), SIGNAL(categoryUpdated(Book::TransactionType)));
    QSignalSpy typeSpy(qmlBook.get(), SIGNAL(categoryTypeUpdated()));

    auto cat = std::make_shared<com::chancho::Category>(name, com::chancho::Category::Type::EXPENSE, color);
    auto qmlCat = std::make_shared<com::chancho::tests::PublicCategory>(cat);

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::CategoryPtr>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(true));

    auto updated = qmlBook->updateCategory(qmlCat.get(), "New name", color, com::chancho::qml::Book::EXPENSE);
    QVERIFY(!updated);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(typeSpy.count(), 0);
}

void
TestBook::testRemoveCategoryWrongObj() {
    auto other = std::make_shared<com::chancho::qml::Account>();

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);

    QSignalSpy spy(qmlBook.get(), SIGNAL(categoryRemoved(Book::TransactionType)));

    EXPECT_CALL(*book.get(), remove(Matcher<com::chancho::CategoryPtr>(_)))
            .Times(0);

    auto removed = qmlBook->removeCategory(other.get());
    QVERIFY(!removed);
    QCOMPARE(spy.count(), 0);
}

void
TestBook::testRemoveCategory() {
    auto qmlCat = std::make_shared<com::chancho::qml::Category>();

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);

    QSignalSpy spy(qmlBook.get(), SIGNAL(categoryRemoved(Book::TransactionType)));

    EXPECT_CALL(*book.get(), remove(Matcher<com::chancho::CategoryPtr>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto removed = qmlBook->removeCategory(qmlCat.get());
    QVERIFY(removed);
    QCOMPARE(spy.count(), 1);
}

void
TestBook::testRemoveCategoryBookError() {
    auto qmlCat = std::make_shared<com::chancho::qml::Category>();

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);

    QSignalSpy spy(qmlBook.get(), SIGNAL(categoryRemoved(Book::TransactionType)));

    EXPECT_CALL(*book.get(), remove(Matcher<com::chancho::CategoryPtr>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(true));

    auto removed = qmlBook->removeCategory(qmlCat.get());
    QVERIFY(!removed);
    QCOMPARE(spy.count(), 0);
}

QTEST_MAIN(TestBook)
