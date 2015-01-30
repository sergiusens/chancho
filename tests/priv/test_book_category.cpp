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
#include <QFileInfo>

#include <com/chancho/system/database.h>
#include <com/chancho/system/database_factory.h>

#include "test_book_category.h"

namespace sys = com::chancho::system;

namespace {
    const QString SELECT_CATEGORY_QUERY = "SELECT name, type, parent FROM Categories WHERE uuid=:uuid";
}

void
TestBookCategory::init() {
    BaseTestCase::init();
    PublicBook::initDatabse();
}

void
TestBookCategory::cleanup() {
    BaseTestCase::cleanup();

    auto dbPath = PublicBook::databasePath();
    QFileInfo fi(dbPath);
    if (fi.exists())
        removeDir(fi.absolutePath());
}

void
TestBookCategory::testStoreCategoryNoParent_data() {
    QTest::addColumn<QString>("name");
    QTest::addColumn<chancho::Category::Type>("type");

    QTest::newRow("income-obj") << "Salary" << chancho::Category::Type::INCOME;
    QTest::newRow("expense-obj") << "Food" << chancho::Category::Type::EXPENSE;
}

void
TestBookCategory::testStoreCategoryNoParent() {
    QFETCH(QString, name);
    QFETCH(chancho::Category::Type, type);

    // create the category and store it
    auto category = std::make_shared<PublicCategory>(name, type);
    PublicBook book;
    book.store(category);

    QVERIFY(category->wasStoredInDb());

    // assert that the db is present in the db
    auto dbPath = PublicBook::databasePath();

    auto db = sys::DatabaseFactory::instance()->addDatabase("QSQLITE", QTest::currentTestFunction());
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    // create the required tables and indexes
    auto query = db->createQuery();
    query->prepare(SELECT_CATEGORY_QUERY);
    query->bindValue(":uuid", category->_dbId.toString());
    auto success = query->exec();

    QVERIFY(success);
    QVERIFY(query->next());
    QCOMPARE(query->value("name").toString(), category->name);
    QCOMPARE(query->value("type").toInt(), static_cast<int>(category->type));
    QVERIFY(query->value("parent").isNull());

    db->close();
}

void
TestBookCategory::testStoreCategoryParentPresent_data() {
    QTest::addColumn<QString>("name");
    QTest::addColumn<chancho::Category::Type>("type");

    QTest::newRow("income-obj") << "Salary" << chancho::Category::Type::INCOME;
    QTest::newRow("expense-obj") << "Food" << chancho::Category::Type::EXPENSE;
}

void
TestBookCategory::testStoreCategoryParentPresent() {
    QFETCH(QString, name);
    QFETCH(chancho::Category::Type, type);

    auto parentName = QString("Parent");

    // create the parent category and store it
    auto parent = std::make_shared<PublicCategory>(parentName, chancho::Category::Type::INCOME);
    auto category = std::make_shared<PublicCategory>(name, type, parent);

    PublicBook book;
    book.store(category);

    // assert that both the parent and the category are added
    QVERIFY(parent->wasStoredInDb());
    QVERIFY(category->wasStoredInDb());

    // assert that the db is present in the db
    auto dbPath = PublicBook::databasePath();

    auto db = sys::DatabaseFactory::instance()->addDatabase("QSQLITE", QTest::currentTestFunction());
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    // create the required tables and indexes
    auto query = db->createQuery();
    query->prepare(SELECT_CATEGORY_QUERY);
    query->bindValue(":uuid", category->_dbId.toString());
    auto success = query->exec();

    QVERIFY(success);
    QVERIFY(query->next());
    QCOMPARE(query->value("name").toString(), category->name);
    QCOMPARE(query->value("type").toInt(), static_cast<int>(category->type));
    QCOMPARE(query->value("parent").toString(), parent->_dbId.toString());

    // ensure that the parent is pressent
    query->prepare(SELECT_CATEGORY_QUERY);
    query->bindValue(":uuid", parent->_dbId.toString());
    success = query->exec();

    QVERIFY(success);
    QVERIFY(query->next());
    QCOMPARE(query->value("name").toString(), parent->name);
    QCOMPARE(query->value("type").toInt(), static_cast<int>(parent->type));
    QVERIFY(query->value("parent").isNull());

    db->close();
}

void
TestBookCategory::testUpdateCategory_data() {
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("newName");
    QTest::addColumn<chancho::Category::Type>("type");

    QTest::newRow("income-obj") << "Salary" << "Updated Salary" << chancho::Category::Type::INCOME;
    QTest::newRow("expense-obj") << "Food" << "Updated food" << chancho::Category::Type::EXPENSE;
}

void
TestBookCategory::testUpdateCategory() {
    QFETCH(QString, name);
    QFETCH(QString, newName);
    QFETCH(chancho::Category::Type, type);

    // create the category and store it
    auto category = std::make_shared<PublicCategory>(name, type);
    PublicBook book;
    book.store(category);

    QVERIFY(category->wasStoredInDb());

    // update the category and assert the values
    category->name = newName;
    book.store(category);

    QVERIFY(category->wasStoredInDb());

    // assert that the db is present in the db
    auto dbPath = PublicBook::databasePath();

    auto db = sys::DatabaseFactory::instance()->addDatabase("QSQLITE", QTest::currentTestFunction());
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    // create the required tables and indexes
    auto query = db->createQuery();
    query->prepare(SELECT_CATEGORY_QUERY);
    query->bindValue(":uuid", category->_dbId.toString());
    auto success = query->exec();

    QVERIFY(success);
    QVERIFY(query->next());
    QCOMPARE(query->value("name").toString(), newName);  // test against new name to be 100% sure
    QCOMPARE(query->value("type").toInt(), static_cast<int>(category->type));
    QVERIFY(query->value("parent").isNull());

    db->close();

}

void
TestBookCategory::testRemoveCategoryNoParent() {
    QString name = "Salary";
    auto type = chancho::Category::Type::INCOME;

    auto category = std::make_shared<PublicCategory>(name, type);

    PublicBook book;
    book.store(category);

    // assert that both the parent and the category are added
    QVERIFY(category->wasStoredInDb());

    // method under test
    book.remove(category);

    QVERIFY(!category->wasStoredInDb());

    // assert that the db is present in the db
    auto dbPath = PublicBook::databasePath();

    auto db = sys::DatabaseFactory::instance()->addDatabase("QSQLITE", QTest::currentTestFunction());
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    // create the required tables and indexes
    auto query = db->createQuery();
    query->prepare(SELECT_CATEGORY_QUERY);
    query->bindValue(":uuid", category->_dbId.toString());
    auto success = query->exec();

    QVERIFY(success);
    QVERIFY(!query->next());

    db->close();
}

void
TestBookCategory::testRemoveCategoryParent() {
    // remove the category with a parent and ensure that the parent is left there
    auto name = "Salary";
    auto type = chancho::Category::Type::INCOME;
    auto parentName = QString("Parent");

    // create the parent category and store it
    auto parent = std::make_shared<PublicCategory>(parentName, chancho::Category::Type::INCOME);
    auto category = std::make_shared<PublicCategory>(name, type, parent);

    PublicBook book;
    book.store(category);

    // assert that both the parent and the category are added
    QVERIFY(parent->wasStoredInDb());
    QVERIFY(category->wasStoredInDb());

    book.remove(category);
    QVERIFY(parent->wasStoredInDb());
    QVERIFY(!category->wasStoredInDb());

    // assert that the db is present in the db
    auto dbPath = PublicBook::databasePath();

    auto db = sys::DatabaseFactory::instance()->addDatabase("QSQLITE", QTest::currentTestFunction());
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    // create the required tables and indexes
    auto query = db->createQuery();
    query->prepare(SELECT_CATEGORY_QUERY);
    query->bindValue(":uuid", category->_dbId.toString());
    auto success = query->exec();

    QVERIFY(success);
    QVERIFY(!query->next());

    // ensure that the parent is pressent
    query->prepare(SELECT_CATEGORY_QUERY);
    query->bindValue(":uuid", parent->_dbId.toString());
    success = query->exec();

    QVERIFY(success);
    QVERIFY(query->next());
    QCOMPARE(query->value("name").toString(), parent->name);
    QCOMPARE(query->value("type").toInt(), static_cast<int>(parent->type));
    QVERIFY(query->value("parent").isNull());

    db->close();
}

void
TestBookCategory::testRemoveParentCategory() {
    // create two cats with the same parent, remove the parent and ensure that the cats have been removed
    // remove the category with a parent and ensure that the parent is left there
    auto parentName = QString("Parent");

    // create the parent category and store it
    auto parent = std::make_shared<PublicCategory>(parentName, chancho::Category::Type::INCOME);
    auto first = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME, parent);
    auto second = std::make_shared<PublicCategory>("Bonus", chancho::Category::Type::INCOME, parent);

    PublicBook book;
    book.store(first);
    book.store(second);

    // assert that both the parent and the category are added
    QVERIFY(parent->wasStoredInDb());
    QVERIFY(first->wasStoredInDb());

    book.remove(parent);

    QVERIFY(!parent->wasStoredInDb());

    // assert that the db is present in the db
    auto dbPath = PublicBook::databasePath();

    auto db = sys::DatabaseFactory::instance()->addDatabase("QSQLITE", QTest::currentTestFunction());
    db->setDatabaseName(dbPath);

    auto opened = db->open();
    QVERIFY(opened);

    // create the required tables and indexes
    auto query = db->createQuery();
    query->prepare(SELECT_CATEGORY_QUERY);
    query->bindValue(":uuid", first->_dbId.toString());
    auto success = query->exec();

    QVERIFY(success);
    QVERIFY(!query->next());

    query->prepare(SELECT_CATEGORY_QUERY);
    query->bindValue(":uuid", second->_dbId.toString());
    success = query->exec();

    QVERIFY(success);
    QVERIFY(!query->next());

    // ensure that the parent is not pressent
    query->prepare(SELECT_CATEGORY_QUERY);
    query->bindValue(":uuid", parent->_dbId.toString());
    success = query->exec();

    QVERIFY(success);
    QVERIFY(!query->next());

    db->close();
}

void
TestBookCategory::testRemoveNotAdded() {
    // ensure that we cannot delete a not present cat
    auto cat = std::make_shared<chancho::Category>("Not presnet", chancho::Category::Type::INCOME);
    PublicBook book;
    book.remove(cat);
    QVERIFY(book.isError());
}

void
TestBookCategory::testGetCategoriesEmpty() {
    PublicBook book;
    auto cats = book.categories();
    QCOMPARE(cats.count(), 0);
}

void
TestBookCategory::testGetCategoriesNoParents() {
    // add several cats and add them to the books
    auto first = std::make_shared<PublicCategory>("Food", chancho::Category::Type::EXPENSE);
    auto second = std::make_shared<PublicCategory>("Vacations", chancho::Category::Type::EXPENSE);
    auto last = std::make_shared<PublicCategory>("Salary", chancho::Category::Type::INCOME);

    PublicBook book;
    book.store(first);
    book.store(second);
    book.store(last);

    QVERIFY(first->wasStoredInDb());
    QVERIFY(second->wasStoredInDb());
    QVERIFY(last->wasStoredInDb());

    auto cats = book.categories();
    QCOMPARE(cats.count(), 3);

    // assert they are all present
    QStringList expectedNames;
    expectedNames.append(first->name);
    expectedNames.append(second->name);
    expectedNames.append(last->name);
    foreach(const chancho::CategoryPtr& cat, cats) {
        QVERIFY(expectedNames.contains(cat->name));
    }
}

void
TestBookCategory::testGetCategoriesOneLevelParents() {
    QString firstParentName = "Food";
    QString secondParentName = "Salary";
    // create two parents to be provided to the diff children and assert that the parents are correctly set
    auto firstParent = std::make_shared<PublicCategory>(firstParentName, chancho::Category::Type::EXPENSE);
    auto secondParent = std::make_shared<PublicCategory>(secondParentName, chancho::Category::Type::INCOME);

    auto first = std::make_shared<PublicCategory>("Restaurant", chancho::Category::Type::EXPENSE, firstParent);
    auto second = std::make_shared<PublicCategory>("Bar", chancho::Category::Type::EXPENSE, firstParent);
    auto last = std::make_shared<PublicCategory>("Bonus", chancho::Category::Type::INCOME, secondParent);

    // store the diff cats and ensure that we do get the parents correctly
    PublicBook book;
    book.store(first);
    book.store(second);
    book.store(last);

    QVERIFY(first->wasStoredInDb());
    QVERIFY(second->wasStoredInDb());
    QVERIFY(last->wasStoredInDb());

    auto cats = book.categories();
    QCOMPARE(cats.count(), 5);

    // assert they are all present
    QStringList firstParentCats;
    firstParentCats.append(first->name);
    firstParentCats.append(second->name);

    QStringList secondParentCats;
    secondParentCats.append(last->name);

    QStringList expectedNames;
    expectedNames.append(firstParentCats);
    expectedNames.append(secondParentCats);
    expectedNames.append(firstParent->name);
    expectedNames.append(secondParent->name);

    foreach(const chancho::CategoryPtr& cat, cats) {
        QVERIFY(expectedNames.contains(cat->name));
        if (cat->name != firstParentName && cat->name != secondParentName) {
            if (cat->parent) {
                if (cat->parent->name == firstParentName) {
                    QVERIFY(firstParentCats.contains(cat->name));
                }
                if (cat->parent->name == secondParentName) {
                    QVERIFY(secondParentCats.contains(cat->name));
                }
            } else {
                QFAIL(QString("Missing parent for category %1").arg(cat->name).toStdString().c_str());
            }
        }
    }
}

void
TestBookCategory::testGetCategoriesSeveralLevels() {
    // add categories with parents and add a top parent for all
    QString masterName = "Master";
    QString firstParentName = "Salary";
    QString secondParentName = "Food";

    auto masterCatgeroy = std::make_shared<PublicCategory>(masterName, chancho::Category::Type::INCOME);
    auto firstParent = std::make_shared<PublicCategory>(
            firstParentName, chancho::Category::Type::INCOME, masterCatgeroy);
    auto secondParent =
            std::make_shared<PublicCategory>(secondParentName, chancho::Category::Type::EXPENSE, masterCatgeroy);

    auto first = std::make_shared<PublicCategory>("Restaurant", chancho::Category::Type::EXPENSE, firstParent);
    auto second = std::make_shared<PublicCategory>("Bar", chancho::Category::Type::EXPENSE, firstParent);
    auto last = std::make_shared<PublicCategory>("Bonus", chancho::Category::Type::INCOME, secondParent);

    // store the diff cats and ensure that we do get the parents correctly
    PublicBook book;
    book.store(first);
    book.store(second);
    book.store(last);

    QVERIFY(masterCatgeroy->wasStoredInDb());
    QVERIFY(firstParent->wasStoredInDb());
    QVERIFY(secondParent->wasStoredInDb());
    QVERIFY(first->wasStoredInDb());
    QVERIFY(second->wasStoredInDb());
    QVERIFY(last->wasStoredInDb());

    auto cats = book.categories();
    QCOMPARE(cats.count(), 6);

    // assert they are all present
    QStringList firstParentCats;
    firstParentCats.append(first->name);
    firstParentCats.append(second->name);

    QStringList secondParentCats;
    secondParentCats.append(last->name);

    QStringList expectedNames;
    expectedNames.append(firstParentCats);
    expectedNames.append(secondParentCats);
    expectedNames.append(firstParent->name);
    expectedNames.append(secondParent->name);
    expectedNames.append(masterCatgeroy->name);

    foreach(const chancho::CategoryPtr& cat, cats) {
        QVERIFY(expectedNames.contains(cat->name));
        if (cat->name != firstParentName && cat->name != secondParentName && cat->name != masterName) {
            if (cat->parent) {
                if (cat->parent->name == firstParentName) {
                    QVERIFY(firstParentCats.contains(cat->name));
                }
                if (cat->parent->name == secondParentName) {
                    QVERIFY(secondParentCats.contains(cat->name));
                }
            } else {
                QFAIL(QString("Missing parent for category %1").arg(cat->name).toStdString().c_str());
            }
        } else if (cat->name == firstParentName) {
            if (cat->parent) {
                QCOMPARE(cat->parent->name, masterName);
            } else {
                QFAIL("Missing parent for the first parent category.");
            }
        } else if (cat->name == secondParentName) {
            if (cat->parent) {
                QCOMPARE(cat->parent->name, masterName);
            } else {
                QFAIL("Missing parent for the second parent category.");
            }
        } else if (cat->name == masterName) {
            if (cat->parent) {
                QFAIL("Master category has parent");
            }
        }
    }
}

QTEST_MAIN(TestBookCategory)
