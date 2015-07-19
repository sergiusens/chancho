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

#include "public_generated_transactions_model.h"
#include "public_qml_recurrent_transaction.h"

#include "test_generated_transactions.h"

using ::testing::_;
using ::testing::Mock;
using ::testing::Matcher;
using ::testing::AnyNumber;
using ::testing::Return;
using ::testing::AnyOf;

void
TestGeneratedTransactionsModel::init() {
    BaseTestCase::init();
}

void
TestGeneratedTransactionsModel::cleanup() {
    BaseTestCase::cleanup();
}

void
TestGeneratedTransactionsModel::testRowCount() {
    int count = 5;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto recurrent = std::make_shared<com::chancho::RecurrentTransaction>();
    auto qmlRecurrent = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrent);
    auto model = std::make_shared<com::chancho::tests::PublicGeneratedTransactionsModel>(qmlRecurrent.get(), book);

    EXPECT_CALL(*book.get(), numberOfTransactions(recurrent))
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
TestGeneratedTransactionsModel::testRowCountError() {
    int count = 5;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto recurrent = std::make_shared<com::chancho::RecurrentTransaction>();
    auto qmlRecurrent = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrent);
    auto model = std::make_shared<com::chancho::tests::PublicGeneratedTransactionsModel>(qmlRecurrent.get(), book);

    EXPECT_CALL(*book.get(), numberOfTransactions(recurrent))
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
TestGeneratedTransactionsModel::testDataNotValidIndex() {
    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto recurrent = std::make_shared<com::chancho::RecurrentTransaction>();
    auto qmlRecurrent = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrent);
    auto model = std::make_shared<com::chancho::tests::PublicGeneratedTransactionsModel>(qmlRecurrent.get(), book);
    auto result = model->data(QModelIndex(), Qt::DisplayRole);

    QVERIFY(!result.isValid());
    QVERIFY(Mock::VerifyAndClearExpectations(book.get()));
}

void
TestGeneratedTransactionsModel::testDataOutOfIndex() {
    int count = 5;
    int index = count + 1;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto recurrent = std::make_shared<com::chancho::RecurrentTransaction>();
    auto qmlRecurrent = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrent);
    auto model = std::make_shared<com::chancho::tests::PublicGeneratedTransactionsModel>(qmlRecurrent.get(), book);

    EXPECT_CALL(*book.get(), numberOfTransactions(recurrent))
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
TestGeneratedTransactionsModel::testDataBookError() {
    int count = 5;
    int index = count - 1;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto recurrent = std::make_shared<com::chancho::RecurrentTransaction>();
    auto qmlRecurrent = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrent);
    auto model = std::make_shared<com::chancho::tests::PublicGeneratedTransactionsModel>(qmlRecurrent.get(), book);

    EXPECT_CALL(*book.get(), numberOfTransactions(recurrent))
            .Times(1)
            .WillOnce(Return(count));

    EXPECT_CALL(*book.get(), transactions(Matcher<com::chancho::RecurrentTransactionPtr>(recurrent),
                                          boost::optional<int>(1), boost::optional<int>(index)))
            .Times(1)
            .WillOnce(Return(QList<com::chancho::TransactionPtr>()));

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
TestGeneratedTransactionsModel::testDataNoData() {
    int count = 5;
    int index = count - 1;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto recurrent = std::make_shared<com::chancho::RecurrentTransaction>();
    auto qmlRecurrent = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrent);
    auto model = std::make_shared<com::chancho::tests::PublicGeneratedTransactionsModel>(qmlRecurrent.get(), book);

    EXPECT_CALL(*book.get(), numberOfTransactions(recurrent))
            .Times(1)
            .WillOnce(Return(count));

    EXPECT_CALL(*book.get(), transactions(recurrent, boost::optional<int>(1), boost::optional<int>(index)))
            .Times(1)
            .WillOnce(Return(QList<com::chancho::TransactionPtr>()));

    EXPECT_CALL(*book.get(), isError())
            .Times(2)
            .WillOnce(Return(false))
            .WillOnce(Return(false));

    auto result = model->data(index, Qt::DisplayRole);
    QVERIFY(!result.isValid());

    QVERIFY(Mock::VerifyAndClearExpectations(book.get()));
}

void
TestGeneratedTransactionsModel::testDataGetTransaction() {
    int count = 5;
    int index = count - 1;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto recurrent = std::make_shared<com::chancho::RecurrentTransaction>();
    auto qmlRecurrent = std::make_shared<com::chancho::tests::PublicRecurrentTransaction>(recurrent);
    auto model = std::make_shared<com::chancho::tests::PublicGeneratedTransactionsModel>(qmlRecurrent.get(), book);

    auto transaction = std::make_shared<com::chancho::Transaction>();
    QList<com::chancho::TransactionPtr> list;
    list.append(transaction);

    EXPECT_CALL(*book.get(), numberOfTransactions(recurrent))
            .Times(1)
            .WillOnce(Return(count));

    EXPECT_CALL(*book.get(), transactions(recurrent, boost::optional<int>(1), boost::optional<int>(index)))
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

QTEST_MAIN(TestGeneratedTransactionsModel)

