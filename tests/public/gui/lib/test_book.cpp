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

#include <QDebug>
#include <gmock/gmock.h>

#include <com/chancho/qml/account.h>
#include <com/chancho/qml/book.h>
#include <com/chancho/qml/category.h>
#include <com/chancho/qml/transaction.h>

#include "book.h"
#include "matchers.h"
#include "mock_accounts_worker_factory.h"
#include "mock_categories_worker_factory.h"
#include "mock_transactions_worker_factory.h"
#include "mock_worker_thread.h"
#include "public_account.h"
#include "public_category.h"
#include "public_qml_account.h"
#include "public_qml_book.h"
#include "public_qml_category.h"
#include "public_qml_transaction.h"

#include "test_book.h"

using ::testing::_;
using ::testing::Mock;
using ::testing::Matcher;
using ::testing::AnyNumber;
using ::testing::Return;
using ::testing::AnyOf;

namespace t = com::chancho::tests;
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
    auto book = std::make_shared<t::MockBook>();
    auto accounts = std::make_shared<t::accounts::WorkerFactory>();
    auto categories = std::make_shared<t::categories::WorkerFactory>();
    auto transactions = std::make_shared<t::transactions::WorkerFactory>();
    auto singleStore = std::make_shared<t::MockWorkerThread<com::chancho::qml::workers::transactions::SingleStore>>();
    auto qmlBook = std::make_shared<t::PublicBook>(book, accounts, categories, transactions);

    auto account = new com::chancho::qml::Account();
    auto category = new com::chancho::qml::Category();
    QDate date(2014, 2, 1);
    double amount = 2.5;
    QString contents = "The contents of the transaction";
    QString memo = "The memo";

    EXPECT_CALL(*transactions.get(), storeTransaction(_, _, _, date, amount, contents, memo, QVariantMap()))
            .Times(1)
            .WillOnce(Return(singleStore.get()));

    EXPECT_CALL(*singleStore.get(), start())
            .Times(1);

    auto stored = qmlBook->storeTransaction(account, category, date, amount, contents, memo, QVariantMap());
    QVERIFY(stored);
}

void
TestBook::testStoreTransactionWrongAccount() {
    auto book = std::make_shared<t::MockBook>();
    auto accounts = std::make_shared<t::accounts::WorkerFactory>();
    auto categories = std::make_shared<t::categories::WorkerFactory>();
    auto transactions = std::make_shared<t::transactions::WorkerFactory>();
    auto singleStore = std::make_shared<t::MockWorkerThread<com::chancho::qml::workers::transactions::SingleStore>>();
    auto qmlBook = std::make_shared<t::PublicBook>(book, accounts, categories, transactions);

    auto account = new com::chancho::qml::Category();
    auto category = new com::chancho::qml::Category();
    QDate date(2014, 2, 1);
    double amount = 2.5;
    QString contents = "The contents of the transaction";
    QString memo = "The memo";

    EXPECT_CALL(*transactions.get(), storeTransaction(_, _, _, date, amount, contents, memo, QVariantMap()))
            .Times(0);

    auto stored = qmlBook->storeTransaction(account, category, date, amount, contents, memo, QVariantMap());

    QVERIFY(!stored);
}

void
TestBook::testStoreTransactionWrongCategory() {
    auto book = std::make_shared<t::MockBook>();
    auto accounts = std::make_shared<t::accounts::WorkerFactory>();
    auto categories = std::make_shared<t::categories::WorkerFactory>();
    auto transactions = std::make_shared<t::transactions::WorkerFactory>();
    auto singleStore = std::make_shared<t::MockWorkerThread<com::chancho::qml::workers::transactions::SingleStore>>();
    auto qmlBook = std::make_shared<t::PublicBook>(book, accounts, categories, transactions);

    auto account = new com::chancho::qml::Account();
    auto category = new com::chancho::qml::Account();
    QDate date(2014, 2, 1);
    double amount = 2.5;
    QString contents = "The contents of the transaction";
    QString memo = "The memo";

    EXPECT_CALL(*transactions.get(), storeTransaction(_, _, _, date, amount, contents, memo, QVariantMap()))
            .Times(0);

    auto stored = qmlBook->storeTransaction(account, category, date, amount, contents, memo, QVariantMap());

    QVERIFY(!stored);
}

void
TestBook::testRemoveTransaction() {
    auto book = std::make_shared<t::MockBook>();
    auto accounts = std::make_shared<t::accounts::WorkerFactory>();
    auto categories = std::make_shared<t::categories::WorkerFactory>();
    auto transactions = std::make_shared<t::transactions::WorkerFactory>();
    auto singleRemove = std::make_shared<t::MockWorkerThread<com::chancho::qml::workers::transactions::SingleRemove>>();
    auto qmlBook = std::make_shared<t::PublicBook>(book, accounts, categories, transactions);
    auto qmlTransaction = std::make_shared<com::chancho::qml::Transaction>();

    EXPECT_CALL(*transactions.get(), removeTransaction(_, _))
            .Times(1)
            .WillOnce(Return(singleRemove.get()));

    EXPECT_CALL(*singleRemove.get(), start())
            .Times(1);

    auto removed = qmlBook->removeTransaction(qmlTransaction.get());

    QVERIFY(removed);
}

void
TestBook::testRemoveTransactionWrongObject() {
    auto book = std::make_shared<t::MockBook>();
    auto accounts = std::make_shared<t::accounts::WorkerFactory>();
    auto categories = std::make_shared<t::categories::WorkerFactory>();
    auto transactions = std::make_shared<t::transactions::WorkerFactory>();
    auto singleRemove = std::make_shared<t::MockWorkerThread<com::chancho::qml::workers::transactions::SingleRemove>>();
    auto qmlBook = std::make_shared<t::PublicBook>(book, accounts, categories, transactions);
    auto qmlTransaction = std::make_shared<com::chancho::qml::Category>();

    EXPECT_CALL(*transactions.get(), removeTransaction(_, _))
            .Times(0);

    auto removed = qmlBook->removeTransaction(qmlTransaction.get());

    QVERIFY(!removed);
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

    auto book = std::make_shared<t::MockBook>();
    auto accounts = std::make_shared<t::accounts::WorkerFactory>();
    auto categories = std::make_shared<t::categories::WorkerFactory>();
    auto transactions = std::make_shared<t::transactions::WorkerFactory>();
    auto singleUpdate = std::make_shared<t::MockWorkerThread<com::chancho::qml::workers::transactions::SingleRemove>>();
    auto qmlBook = std::make_shared<t::PublicBook>(book, accounts, categories, transactions);
    auto qmlTransaction = std::make_shared<com::chancho::qml::Transaction>();

    EXPECT_CALL(*transactions.get(), updateTransaction(_, _, _, _, date, conents, memo, amount))
            .Times(0);

    auto updated = qmlBook->updateTransaction(tran.get(), account.get(), category.get(), date, conents, memo, amount);

    QVERIFY(!updated);
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

    auto book = std::make_shared<t::MockBook>();
    auto accounts = std::make_shared<t::accounts::WorkerFactory>();
    auto categories = std::make_shared<t::categories::WorkerFactory>();
    auto transactions = std::make_shared<t::transactions::WorkerFactory>();
    auto singleUpdate = std::make_shared<t::MockWorkerThread<com::chancho::qml::workers::transactions::SingleRemove>>();
    auto qmlBook = std::make_shared<t::PublicBook>(book, accounts, categories, transactions);
    auto qmlTransaction = std::make_shared<com::chancho::qml::Transaction>();

    EXPECT_CALL(*transactions.get(), updateTransaction(_, _, _, _, date, conents, memo, amount))
            .Times(0);

    auto updated = qmlBook->updateTransaction(tran.get(), account.get(), category.get(), date, conents, memo, amount);

    QVERIFY(!updated);
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

    auto book = std::make_shared<t::MockBook>();
    auto accounts = std::make_shared<t::accounts::WorkerFactory>();
    auto categories = std::make_shared<t::categories::WorkerFactory>();
    auto transactions = std::make_shared<t::transactions::WorkerFactory>();
    auto singleUpdate = std::make_shared<t::MockWorkerThread<com::chancho::qml::workers::transactions::SingleRemove>>();
    auto qmlBook = std::make_shared<t::PublicBook>(book, accounts, categories, transactions);
    auto qmlTransaction = std::make_shared<com::chancho::qml::Account>();

    EXPECT_CALL(*transactions.get(), updateTransaction(_, _, _, _, date, conents, memo, amount))
            .Times(0);

    auto updated = qmlBook->updateTransaction(tran.get(), account.get(), category.get(), date, conents, memo, amount);

    QVERIFY(!updated);
}

void
TestBook::testUpdateTransactionNeedsUpdate() {
    com::chancho::AccountPtr accountPtr = std::make_shared<PublicAccount>(QUuid::createUuid());
    auto account = std::make_shared<t::PublicAccount>(accountPtr);
    com::chancho::CategoryPtr categoryPtr = std::make_shared<PublicCategory>(QUuid::createUuid());
    auto category = std::make_shared<t::PublicCategory>(categoryPtr);

    QDate date(2014, 2, 1);
    QString conents = "My new contents";
    QString memo = "My new memo";
    double amount = 23.4;

    auto tran = std::make_shared<com::chancho::Transaction>(accountPtr, amount, categoryPtr, date, conents, memo);

    auto book = std::make_shared<t::MockBook>();
    auto accounts = std::make_shared<t::accounts::WorkerFactory>();
    auto categories = std::make_shared<t::categories::WorkerFactory>();
    auto transactions = std::make_shared<t::transactions::WorkerFactory>();
    auto singleUpdate = std::make_shared<t::MockWorkerThread<com::chancho::qml::workers::transactions::SingleUpdate>>();
    auto qmlBook = std::make_shared<t::PublicBook>(book, accounts, categories, transactions);
    auto qmlTransaction = std::make_shared<t::PublicTransaction>(tran);

    EXPECT_CALL(*transactions.get(), updateTransaction(_, _, _, _, date, conents, memo, amount))
            .Times(1)
            .WillOnce(Return(singleUpdate.get()));

    EXPECT_CALL(*singleUpdate.get(), start())
            .Times(1);

    auto updated = qmlBook->updateTransaction(qmlTransaction.get(), account.get(), category.get(), date, conents,
                                              memo, amount);

    QVERIFY(updated);
}

void
TestBook::testStoreAccountInitialAmount() {
    QString name = "BBVA";
    QString memo = "First test account";
    QString color = "#fff";
    double initialAmount = 34.20;

    auto book = std::make_shared<t::MockBook>();
    auto accounts = std::make_shared<t::accounts::WorkerFactory>();
    auto categories = std::make_shared<t::categories::WorkerFactory>();
    auto transactions = std::make_shared<t::transactions::WorkerFactory>();
    auto singleStore = std::make_shared<t::MockWorkerThread<com::chancho::qml::workers::accounts::SingleStore>>();
    auto qmlBook = std::make_shared<t::PublicBook>(book, accounts, categories, transactions);

    EXPECT_CALL(*accounts.get(), storeAccount(_, name, memo, color, initialAmount))
            .Times(1)
            .WillOnce(Return(singleStore.get()));

    EXPECT_CALL(*singleStore.get(), start())
            .Times(1);

    auto stored = qmlBook->storeAccount(name, memo, color, initialAmount);
    QVERIFY(stored);
}

void
TestBook::testRemoveAccountWrongObj() {
    auto other = std::make_shared<com::chancho::qml::Category>();

    auto book = std::make_shared<t::MockBook>();
    auto accounts = std::make_shared<t::accounts::WorkerFactory>();
    auto categories = std::make_shared<t::categories::WorkerFactory>();
    auto transactions = std::make_shared<t::transactions::WorkerFactory>();
    auto singleRemove = std::make_shared<t::MockWorkerThread<com::chancho::qml::workers::accounts::SingleRemove>>();
    auto qmlBook = std::make_shared<t::PublicBook>(book, accounts, categories, transactions);

    EXPECT_CALL(*accounts.get(), removeAccount(_, _))
            .Times(0);

    auto removed = qmlBook->removeAccount(other.get());
    QVERIFY(!removed);
}

void
TestBook::testRemoveAccount() {
    auto acc = std::make_shared<com::chancho::qml::Account>();

    auto book = std::make_shared<t::MockBook>();
    auto accounts = std::make_shared<t::accounts::WorkerFactory>();
    auto categories = std::make_shared<t::categories::WorkerFactory>();
    auto transactions = std::make_shared<t::transactions::WorkerFactory>();
    auto singleRemove = std::make_shared<t::MockWorkerThread<com::chancho::qml::workers::accounts::SingleRemove>>();
    auto qmlBook = std::make_shared<t::PublicBook>(book, accounts, categories, transactions);

    EXPECT_CALL(*accounts.get(), removeAccount(_, _))
            .Times(1)
            .WillOnce(Return(singleRemove.get()));

    EXPECT_CALL(*singleRemove.get(), start())
            .Times(1);

    auto removed = qmlBook->removeAccount(acc.get());
    QVERIFY(removed);
}

void
TestBook::testUpdateAccountWrongObj() {
    QString name("New Name");
    QString memo("New Memo");
    QString color("#ccc");
    auto other = std::make_shared<com::chancho::qml::Category>();

    auto book = std::make_shared<t::MockBook>();
    auto accounts = std::make_shared<t::accounts::WorkerFactory>();
    auto categories = std::make_shared<t::categories::WorkerFactory>();
    auto transactions = std::make_shared<t::transactions::WorkerFactory>();
    auto qmlBook = std::make_shared<t::PublicBook>(book, accounts, categories, transactions);

    EXPECT_CALL(*accounts.get(), updateAccount(_, _, name, memo, color))
            .Times(0);

    auto updated = qmlBook->updateAccount(other.get(), name, memo, color);
    QVERIFY(!updated);
}

void
TestBook::testUpdateAccount() {
    QString name("New Name");
    QString memo("New Memo");
    QString color("#ccc");
    auto accPtr = std::make_shared<com::chancho::Account>("BBVA", 50.0, "Memo", "#ff");
    auto other = std::make_shared<com::chancho::tests::PublicAccount>(accPtr);

    auto book = std::make_shared<t::MockBook>();
    auto accounts = std::make_shared<t::accounts::WorkerFactory>();
    auto categories = std::make_shared<t::categories::WorkerFactory>();
    auto transactions = std::make_shared<t::transactions::WorkerFactory>();
    auto singleUpdate = std::make_shared<t::MockWorkerThread<com::chancho::qml::workers::accounts::SingleUpdate>>();
    auto qmlBook = std::make_shared<t::PublicBook>(book, accounts, categories, transactions);

    EXPECT_CALL(*accounts.get(), updateAccount(_, _, name, memo, color))
            .Times(1)
            .WillOnce(Return(singleUpdate.get()));

    EXPECT_CALL(*singleUpdate.get(), start())
            .Times(1);

    auto updated = qmlBook->updateAccount(other.get(), name, memo, color);
    QVERIFY(updated);
}

void
TestBook::testStoreCategory() {
    QString name = "Test category";
    QString color = "#123";
    auto type = com::chancho::tests::PublicBook::INCOME;

    auto book = std::make_shared<t::MockBook>();
    auto accounts = std::make_shared<t::accounts::WorkerFactory>();
    auto categories = std::make_shared<t::categories::WorkerFactory>();
    auto transactions = std::make_shared<t::transactions::WorkerFactory>();
    auto singleStore = std::make_shared<t::MockWorkerThread<com::chancho::qml::workers::categories::SingleStore>>();
    auto qmlBook = std::make_shared<t::PublicBook>(book, accounts, categories, transactions);

    auto cat = std::make_shared<com::chancho::Category>(name, com::chancho::Category::Type::EXPENSE, color);

    EXPECT_CALL(*categories.get(), storeCategory(_, name, color, type))
            .Times(1)
            .WillOnce(Return(singleStore.get()));

    EXPECT_CALL(*singleStore.get(), start())
            .Times(1);

    auto stored = qmlBook->storeCategory(name, color, type);
    QVERIFY(stored);
}

void
TestBook::testUpdateCategoryWrongObj() {
    auto other = std::make_shared<com::chancho::qml::Account>();
    QString name = "Test name";
    QString color = "Test color";
    auto type = com::chancho::tests::PublicBook::EXPENSE;

    auto book = std::make_shared<t::MockBook>();
    auto accounts = std::make_shared<t::accounts::WorkerFactory>();
    auto categories = std::make_shared<t::categories::WorkerFactory>();
    auto transactions = std::make_shared<t::transactions::WorkerFactory>();
    auto qmlBook = std::make_shared<t::PublicBook>(book, accounts, categories, transactions);

    EXPECT_CALL(*categories.get(), updateCategory(_, _, name, color, type))
            .Times(0);

    auto updated = qmlBook->updateCategory(other.get(), name, color, type);
    QVERIFY(!updated);
}

void
TestBook::testUpdateCategoryNoUpdate() {
    QString name = "Test name";
    QString color = "Test color";
    auto type = com::chancho::tests::PublicBook::EXPENSE;

    auto book = std::make_shared<t::MockBook>();
    auto accounts = std::make_shared<t::accounts::WorkerFactory>();
    auto categories = std::make_shared<t::categories::WorkerFactory>();
    auto transactions = std::make_shared<t::transactions::WorkerFactory>();
    auto singleUpdate = std::make_shared<t::MockWorkerThread<com::chancho::qml::workers::categories::SingleUpdate>>();
    auto qmlBook = std::make_shared<t::PublicBook>(book, accounts, categories, transactions);


    auto cat = std::make_shared<com::chancho::Category>(name, com::chancho::Category::Type::EXPENSE, color);
    auto qmlCat = std::make_shared<com::chancho::tests::PublicCategory>(cat);

    EXPECT_CALL(*categories.get(), updateCategory(_, _, name, color, type))
            .Times(1)
            .WillOnce(Return(singleUpdate.get()));

    EXPECT_CALL(*singleUpdate.get(), start())
            .Times(1);

    auto updated = qmlBook->updateCategory(qmlCat.get(), name, color, type);
    QVERIFY(updated);
}

void
TestBook::testRemoveCategoryWrongObj() {
    auto other = std::make_shared<com::chancho::qml::Account>();

    auto book = std::make_shared<t::MockBook>();
    auto accounts = std::make_shared<t::accounts::WorkerFactory>();
    auto categories = std::make_shared<t::categories::WorkerFactory>();
    auto transactions = std::make_shared<t::transactions::WorkerFactory>();
    auto singleRemove = std::make_shared<t::MockWorkerThread<com::chancho::qml::workers::categories::SingleRemove>>();
    auto qmlBook = std::make_shared<t::PublicBook>(book, accounts, categories, transactions);

    EXPECT_CALL(*categories.get(), removeCategory(_, _))
            .Times(0);

    auto removed = qmlBook->removeCategory(other.get());
    QVERIFY(!removed);
}

void
TestBook::testRemoveCategory() {
    auto qmlCat = std::make_shared<com::chancho::qml::Category>();

    auto book = std::make_shared<t::MockBook>();
    auto accounts = std::make_shared<t::accounts::WorkerFactory>();
    auto categories = std::make_shared<t::categories::WorkerFactory>();
    auto transactions = std::make_shared<t::transactions::WorkerFactory>();
    auto singleRemove = std::make_shared<t::MockWorkerThread<com::chancho::qml::workers::categories::SingleRemove>>();
    auto qmlBook = std::make_shared<t::PublicBook>(book, accounts, categories, transactions);

    EXPECT_CALL(*categories.get(), removeCategory(_, _))
            .Times(1)
            .WillOnce(Return(singleRemove.get()));

    EXPECT_CALL(*singleRemove.get(), start())
            .Times(1);

    auto removed = qmlBook->removeCategory(qmlCat.get());
    QVERIFY(removed);
}

QTEST_MAIN(TestBook)
