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

#include "public_category.h"
#include "public_qml_category.h"
#include "test_categories.h"

using ::testing::_;
using ::testing::Mock;
using ::testing::Matcher;
using ::testing::AnyNumber;
using ::testing::Return;
using ::testing::AnyOf;

void
TestCategoriesModel::init() {
    BaseTestCase::init();
}

void
TestCategoriesModel::cleanup() {
    BaseTestCase::cleanup();
}

void
TestCategoriesModel::testRowCount() {
    int count = 5;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicCategoriesModel>(book);

    EXPECT_CALL(*book.get(), numberOfCategories(_))
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
TestCategoriesModel::testRowCountError() {
    int count = 5;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicCategoriesModel>(book);

    EXPECT_CALL(*book.get(), numberOfCategories(_))
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
TestCategoriesModel::testDataNotValidIndex() {
    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicCategoriesModel>(book);
    auto result = model->data(QModelIndex(), Qt::DisplayRole);

    QVERIFY(!result.isValid());
    QVERIFY(Mock::VerifyAndClearExpectations(book.get()));
}

void
TestCategoriesModel::testDataOutOfIndex() {
    int count = 5;
    int index = count + 1;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicCategoriesModel>(book);

    EXPECT_CALL(*book.get(), numberOfCategories(_))
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
TestCategoriesModel::testDataBookError() {
    int count = 5;
    int index = count - 1;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicCategoriesModel>(book);

    EXPECT_CALL(*book.get(), numberOfCategories(_))
            .Times(1)
            .WillOnce(Return(count));

    EXPECT_CALL(*book.get(), categories(boost::optional<com::chancho::Category::Type>(), boost::optional<int>(1),
                boost::optional<int>(index)))
            .Times(1)
            .WillOnce(Return(QList<com::chancho::CategoryPtr>()));

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
TestCategoriesModel::testDataNoData() {
    int count = 5;
    int index = count - 1;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicCategoriesModel>(book);

    EXPECT_CALL(*book.get(), numberOfCategories(_))
            .Times(1)
            .WillOnce(Return(count));

    EXPECT_CALL(*book.get(), categories(boost::optional<com::chancho::Category::Type>(), boost::optional<int>(1),
                boost::optional<int>(index)))
            .Times(1)
            .WillOnce(Return(QList<com::chancho::CategoryPtr>()));

    EXPECT_CALL(*book.get(), isError())
            .Times(2)
            .WillOnce(Return(false))
            .WillOnce(Return(false));

    auto result = model->data(index, Qt::DisplayRole);
    QVERIFY(!result.isValid());

    QVERIFY(Mock::VerifyAndClearExpectations(book.get()));
}

void
TestCategoriesModel::testDataGetCategory() {
    int count = 5;
    int index = count - 1;

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicCategoriesModel>(book);
    QList<com::chancho::CategoryPtr> list;
    list.append(std::make_shared<com::chancho::Category>());

    EXPECT_CALL(*book.get(), numberOfCategories(_))
            .Times(1)
            .WillOnce(Return(count));

    EXPECT_CALL(*book.get(), categories(boost::optional<com::chancho::Category::Type>(), boost::optional<int>(1),
                boost::optional<int>(index)))
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
TestCategoriesModel::testGetIndex() {
    com::chancho::CategoryPtr firstCat = std::make_shared<PublicCategory>(QUuid::createUuid());
    com::chancho::CategoryPtr secondCat = std::make_shared<PublicCategory>(QUuid::createUuid());

    auto qmlCat = new com::chancho::tests::PublicCategory(secondCat);

    QList<com::chancho::CategoryPtr> cats;
    cats.append(firstCat);
    cats.append(secondCat);

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicCategoriesModel>(book);

    EXPECT_CALL(*book.get(), categories(boost::optional<com::chancho::Category::Type>(), boost::optional<int>(),
                    boost::optional<int>()))
            .Times(1)
            .WillOnce(Return(cats));

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto index = model->getIndex(qmlCat);
    QCOMPARE(index, 1);
}

void
TestCategoriesModel::testGetIndexMissing() {
    com::chancho::CategoryPtr firstCat = std::make_shared<PublicCategory>(QUuid::createUuid());
    com::chancho::CategoryPtr secondCat = std::make_shared<PublicCategory>(QUuid::createUuid());
    com::chancho::CategoryPtr notPresentCat = std::make_shared<PublicCategory>(QUuid::createUuid());

    auto qmlCat = new com::chancho::tests::PublicCategory(notPresentCat);

    QList<com::chancho::CategoryPtr> cats;
    cats.append(firstCat);
    cats.append(secondCat);

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicCategoriesModel>(book);

    EXPECT_CALL(*book.get(), categories(boost::optional<com::chancho::Category::Type>(), boost::optional<int>(),
                boost::optional<int>()))
            .Times(1)
            .WillOnce(Return(cats));

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(false));

    auto index = model->getIndex(qmlCat);
    QCOMPARE(index, -1);
}

void
TestCategoriesModel::testGetIndexError() {
    auto other = new com::chancho::qml::Account();

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicCategoriesModel>(book);

    auto index = model->getIndex(other);
    QCOMPARE(index, -1);
}

void
TestCategoriesModel::testGetIndexBookError() {
    // the db will return an error and -1 most be returned
    auto cat = new com::chancho::qml::Category();

    auto book = std::make_shared<com::chancho::tests::MockBook>();
    auto model = std::make_shared<com::chancho::tests::PublicCategoriesModel>(book);

    EXPECT_CALL(*book.get(), categories(boost::optional<com::chancho::Category::Type>(), boost::optional<int>(),
                boost::optional<int>()))
            .Times(1)
            .WillOnce(Return(QList<com::chancho::CategoryPtr>()));

    EXPECT_CALL(*book.get(), isError())
            .Times(1)
            .WillOnce(Return(true));

    EXPECT_CALL(*book.get(), lastError())
            .Times(1)
            .WillOnce(Return(QString("Foo")));

    auto index = model->getIndex(cat);
    QCOMPARE(index, -1);
}

QTEST_MAIN(TestCategoriesModel)

