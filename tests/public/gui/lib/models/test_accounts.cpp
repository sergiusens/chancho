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
#include <com/chancho/qml/category.h>

#include "public_account.h"
#include "public_qml_account.h"
#include "test_accounts.h"

using ::testing::_;
using ::testing::Mock;
using ::testing::Matcher;
using ::testing::AnyNumber;
using ::testing::Return;
using ::testing::AnyOf;

void
TestAccountsModel::init() {
    BaseTestCase::init();
}

void
TestAccountsModel::cleanup() {
    BaseTestCase::cleanup();
}

void
TestAccountsModel::testRowCount() {
    int count = 5;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicAccountsModel>(book);

    EXPECT_CALL(*book.get(), numberOfAccounts())
            .Times(1)
            .WillOnce(Return(count));

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto result = model->rowCount(QModelIndex());

    QVERIFY(Mock::VerifyAndClearExpectations(book.get()));
    QCOMPARE(result, count);
}

void
TestAccountsModel::testRowCountError() {
    int count = 5;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicAccountsModel>(book);

    EXPECT_CALL(*book.get(), numberOfAccounts())
            .Times(1)
            .WillOnce(Return(count));

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(true));

    auto result = model->rowCount(QModelIndex());

    QVERIFY(Mock::VerifyAndClearExpectations(book.get()));
    QCOMPARE(result, 0);  // not equal to count because we had an error
}

void
TestAccountsModel::testDataNotValidIndex() {
    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicAccountsModel>(book);
    auto result = model->data(QModelIndex(), Qt::DisplayRole);

    QVERIFY(!result.isValid());
    QVERIFY(Mock::VerifyAndClearExpectations(book.get()));
}

void
TestAccountsModel::testDataOutOfIndex() {
    int count = 5;
    int index = count + 1;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicAccountsModel>(book);

    EXPECT_CALL(*book.get(), numberOfAccounts())
            .Times(1)
            .WillOnce(Return(count));

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto result = model->data(index, Qt::DisplayRole);
    QVERIFY(!result.isValid());

    QVERIFY(Mock::VerifyAndClearExpectations(book.get()));
}

void
TestAccountsModel::testDataBookError() {
    int count = 5;
    int index = count - 1;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicAccountsModel>(book);

    EXPECT_CALL(*book.get(), numberOfAccounts())
            .Times(1)
            .WillOnce(Return(count));

    EXPECT_CALL(*book.get(), accounts(boost::optional<int>(1), boost::optional<int>(index)))
            .Times(1)
            .WillOnce(Return(QList<com::chancho::AccountPtr>()));

    EXPECT_CALL(*book.get(), isError())
            .Times(2)
            .WillOnce(Return(false))
            .WillOnce(Return(true));

    EXPECT_CALL(*book.get(), lastError())
            .Times(1)
            .WillOnce(Return(QString("Foo")));

    auto result = model->data(index, Qt::DisplayRole);
    QVERIFY(!result.isValid());

    QVERIFY(Mock::VerifyAndClearExpectations(book.get()));
}

void
TestAccountsModel::testDataNoData() {
    int count = 5;
    int index = count - 1;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicAccountsModel>(book);

    EXPECT_CALL(*book.get(), numberOfAccounts())
            .Times(1)
            .WillOnce(Return(count));

    EXPECT_CALL(*book.get(), accounts(boost::optional<int>(1), boost::optional<int>(index)))
            .Times(1)
            .WillOnce(Return(QList<com::chancho::AccountPtr>()));

    EXPECT_CALL(*book.get(), isError())
            .Times(2)
            .WillOnce(Return(false))
            .WillOnce(Return(false));

    auto result = model->data(index, Qt::DisplayRole);
    QVERIFY(!result.isValid());

    QVERIFY(Mock::VerifyAndClearExpectations(book.get()));
}

void
TestAccountsModel::testDataGetAccount() {
    int count = 5;
    int index = count - 1;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicAccountsModel>(book);
    QList<com::chancho::AccountPtr> list;
    list.append(std::make_shared<com::chancho::Account>());

    EXPECT_CALL(*book.get(), numberOfAccounts())
            .Times(1)
            .WillOnce(Return(count));

    EXPECT_CALL(*book.get(), accounts(boost::optional<int>(1), boost::optional<int>(index)))
            .Times(1)
            .WillOnce(Return(list));

    EXPECT_CALL(*book.get(), isError())
            .Times(2)
            .WillOnce(Return(false))
            .WillOnce(Return(false));

    auto result = model->data(index, Qt::DisplayRole);

    QVERIFY(result.isValid());

    QVERIFY(Mock::VerifyAndClearExpectations(book.get()));
}

void
TestAccountsModel::testGetIndex() {
    com::chancho::AccountPtr firstAcc = std::make_shared<PublicAccount>(QUuid::createUuid());
    com::chancho::AccountPtr secondAcc = std::make_shared<PublicAccount>(QUuid::createUuid());

    auto qmlAcc = new com::chancho::tests::PublicAccount(secondAcc);

    QList<com::chancho::AccountPtr> accs;
    accs.append(firstAcc);
    accs.append(secondAcc);

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicAccountsModel>(book);

    EXPECT_CALL(*book.get(), accounts(boost::optional<int>(), boost::optional<int>()))
            .Times(1)
            .WillOnce(Return(accs));

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto index = model->getIndex(qmlAcc);
    QCOMPARE(index, 1);
}

void
TestAccountsModel::testGetIndexMissing() {
    com::chancho::AccountPtr firstAcc = std::make_shared<PublicAccount>(QUuid::createUuid());
    com::chancho::AccountPtr secondAcc = std::make_shared<PublicAccount>(QUuid::createUuid());
    com::chancho::AccountPtr notPresentAcc = std::make_shared<PublicAccount>(QUuid::createUuid());

    auto qmlAcc = new com::chancho::tests::PublicAccount(notPresentAcc);

    QList<com::chancho::AccountPtr> accs;
    accs.append(firstAcc);
    accs.append(secondAcc);

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicAccountsModel>(book);

    EXPECT_CALL(*book.get(), accounts(boost::optional<int>(), boost::optional<int>()))
            .Times(1)
            .WillOnce(Return(accs));

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto index = model->getIndex(qmlAcc);
    QCOMPARE(index, -1);
}

void
TestAccountsModel::testGetIndexError() {
    // use a diff qobject poninter and assert that we return -1
    auto other = new com::chancho::qml::Category();

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicAccountsModel>(book);

    auto index = model->getIndex(other);
    QCOMPARE(index, -1);
}

void
TestAccountsModel::testGetIndexBookError() {
    // the db will return an error and -1 most be returned
    auto acc = new com::chancho::qml::Account();

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicAccountsModel>(book);

    EXPECT_CALL(*book.get(), accounts(boost::optional<int>(), boost::optional<int>()))
            .Times(1)
            .WillOnce(Return(QList<com::chancho::AccountPtr>()));

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*book.get(), lastError())
            .Times(1)
            .WillOnce(Return(QString("Foo")));

    auto index = model->getIndex(acc);
    QCOMPARE(index, -1);
}

void
TestAccountsModel::testNumberOfAccounts() {
    auto count = 5;
    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicAccountsModel>(book);

    EXPECT_CALL(*book.get(), numberOfAccounts())
            .Times(1)
            .WillOnce(Return(count));

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto result = model->numberOfAccounts();
    QCOMPARE(result, count);
}

void
TestAccountsModel::testNumberOfAccountsBookError() {
    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicAccountsModel>(book);

    EXPECT_CALL(*book.get(), numberOfAccounts())
            .Times(1)
            .WillOnce(Return(0));

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(true));

    auto count = model->numberOfAccounts();
    QCOMPARE(count, -1);
}

QTEST_MAIN(TestAccountsModel)

