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
}

void
TestBook::testStoreTransactionWrongAccount() {
    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);

    QSignalSpy spy(qmlBook.get(), SIGNAL(transactionStored(QDate)));

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
}

void
TestBook::testStoreTransactionWrongCategory() {
    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);

    QSignalSpy spy(qmlBook.get(), SIGNAL(transactionStored(QDate)));

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
}

void
TestBook::testStoreTransactionBookError() {
    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);

    QSignalSpy spy(qmlBook.get(), SIGNAL(transactionStored(QDate)));

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
}

void
TestBook::testRemoveTransaction() {
    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);
    auto qmlTransaction = std::make_shared<com::chancho::qml::Transaction>();

    QSignalSpy spy(qmlBook.get(), SIGNAL(transactionRemoved(QDate)));

    EXPECT_CALL(*book.get(), remove(Matcher<com::chancho::TransactionPtr>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto removed = qmlBook->removeTransaction(qmlTransaction.get());

    QVERIFY(removed);
    QCOMPARE(spy.count(), 1);
}

void
TestBook::testRemoveTransactionWrongObject() {
    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);
    auto qmlTransaction = std::make_shared<com::chancho::qml::Category>();

    QSignalSpy spy(qmlBook.get(), SIGNAL(transactionRemoved(QDate)));

    EXPECT_CALL(*book.get(), remove(Matcher<com::chancho::TransactionPtr>(_)))
            .Times(0);

    auto removed = qmlBook->removeTransaction(qmlTransaction.get());

    QVERIFY(!removed);
    QCOMPARE(spy.count(), 0);
}

void
TestBook::testRemoveTransactionBookError() {
    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto qmlBook = std::make_shared<com::chancho::qml::Book>(book);
    auto qmlTransaction = std::make_shared<com::chancho::qml::Transaction>();

    QSignalSpy spy(qmlBook.get(), SIGNAL(transactionRemoved(QDate)));

    EXPECT_CALL(*book.get(), remove(Matcher<com::chancho::TransactionPtr>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(true));

    auto removed = qmlBook->removeTransaction(qmlTransaction.get());

    QVERIFY(!removed);
    QCOMPARE(spy.count(), 0);
}
/*
 *
bool
Book::updateTransaction(QObject* tranObj, QObject* accObj, QObject* catObj, QDate date,
                        QString contents, QString memo, double amount) {
    TransactionPtr tran;
    AccountPtr acc;
    CategoryPtr cat;

    auto tranModel = qobject_cast<qml::Transaction*>(tranObj);
    if (tranModel != nullptr) {
        tran = tranModel->getTransaction();
    } else {
        LOG(ERROR) << "Method called with wrong object type as a transaction model";
        return false;
    }

    auto accModel = qobject_cast<qml::Account*>(accObj);
    if (accModel != nullptr) {
        acc = accModel->getAccount();
    } else {
        LOG(ERROR) << "Method called with wrong object type as an account model";
        return false;
    }

    auto catModel = qobject_cast<qml::Category*>(catObj);
    if (catModel != nullptr) {
        cat = catModel->getCategory();
    } else {
        LOG(ERROR) << "Method called with wrong object type as a category model";
        return false;
    }

    // decide if we need to perform the update
    auto requiresUpdate = tran->account != acc || tran->category != cat || tran->date != date
        || tran->contents != contents || tran->memo != memo || tran->amount != amount;

    if (requiresUpdate) {
        auto oldDate = tran->date;
        tran->account = acc;
        tran->category = cat;
        tran->date = date;
        tran->contents = contents;
        tran->memo = memo;
        tran->amount = amount;

        _book->store(tran);
        if (_book->isError()) {
            return false;
        } else {
            emit transactionUpdated(oldDate, date);
            return true;
        }

    } else {
        return false;
    }

}
 */
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

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::TransactionPtr>(_)))
            .Times(0);

    auto updated = qmlBook->updateTransaction(tran.get(), account.get(), category.get(), date, conents, memo, amount);

    QVERIFY(!updated);
    QCOMPARE(spy.count(), 0);
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

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::TransactionPtr>(_)))
            .Times(0);

    auto updated = qmlBook->updateTransaction(tran.get(), account.get(), category.get(), date, conents, memo, amount);

    QVERIFY(!updated);
    QCOMPARE(spy.count(), 0);
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

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::TransactionPtr>(_)))
            .Times(0);

    auto updated = qmlBook->updateTransaction(tran.get(), account.get(), category.get(), date, conents, memo, amount);

    QVERIFY(!updated);
    QCOMPARE(spy.count(), 0);
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

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::TransactionPtr>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto updated = qmlBook->updateTransaction(tran.get(), account.get(), category.get(), date, conents, memo, amount);

    QVERIFY(updated);
    QCOMPARE(spy.count(), 1);
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

    EXPECT_CALL(*book.get(), store(Matcher<com::chancho::TransactionPtr>(_)))
            .Times(1);

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(true));

    auto updated = qmlBook->updateTransaction(tran.get(), account.get(), category.get(), date, conents, memo, amount);

    QVERIFY(!updated);
    QCOMPARE(spy.count(), 0);
}

QTEST_MAIN(TestBook)
