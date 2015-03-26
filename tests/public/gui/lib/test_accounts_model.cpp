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
#include "test_accounts_model.h"

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

QTEST_MAIN(TestAccountsModel)

