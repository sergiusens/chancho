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
#include <random>

#include <QDebug>
#include <QFileInfo>
#include <QList>

#include <com/chancho/book.h>
#include <com/chancho/stats.h>

#include "public_account.h"
#include "public_book.h"
#include "public_category.h"
#include "public_transaction.h"

#include "test_stats.h"

namespace chancho = com::chancho;

namespace {
    double lowerRandomBound = 0;
    double upperRandomBound = 10000;
}

void
TestStats::init() {
    BaseTestCase::init();
    PublicBook::initDatabse();

}

void
TestStats::cleanup() {
    BaseTestCase::cleanup();

    auto dbPath = PublicBook::databasePath();
    QFileInfo fi(dbPath);
    if (fi.exists())
        removeDir(fi.absolutePath());

}

void
TestStats::testMonthTotalsForAccountComplete_data() {
    QTest::addColumn<QList<std::shared_ptr<PublicAccount>>>("accounts");
    QTest::addColumn<QList<std::shared_ptr<PublicCategory>>>("categories");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("transactions");
    QTest::addColumn<std::shared_ptr<PublicAccount>>("account");
    QTest::addColumn<int>("year");
    QTest::addColumn<QList<double>>("expected");

    auto firstAcc = std::make_shared<PublicAccount>("Bankia", 0);
    auto secondAcc = std::make_shared<PublicAccount>("BBVA", 0);
    QList<std::shared_ptr<PublicAccount>> accounts;
    accounts.append(firstAcc);
    accounts.append(secondAcc);

    auto firstCat = std::make_shared<PublicCategory>("Food", com::chancho::Category::Type::EXPENSE);
    auto secondCat = std::make_shared<PublicCategory>("Driks", com::chancho::Category::Type::EXPENSE);
    auto thirdCat = std::make_shared<PublicCategory>("Salary", com::chancho::Category::Type::INCOME);
    QList<std::shared_ptr<PublicCategory>> cats;
    cats.append(firstCat);
    cats.append(secondCat);
    cats.append(thirdCat);

    std::uniform_int_distribution<int> catUnif(0, 2);
    std::uniform_int_distribution<int> dayUnif(1, 27);  // up to 27 so that we do not have issues with feb
    std::uniform_int_distribution<int> amountUnif(lowerRandomBound, upperRandomBound);
    std::default_random_engine re;


    QList<std::shared_ptr<PublicTransaction>> firstTransactions;
    QList<double> firstResults;
    int firstYear = 2014;

    for(int month=1; month <= 12; month++) {
        double monthTotal = 0;
        for(int tranIndex=0; tranIndex < 5; tranIndex++) {
            int day = dayUnif(re);
            int catIndex = catUnif(re);
            double amount = amountUnif(re);
            auto tran = std::make_shared<PublicTransaction>(firstAcc, amount, cats.at(catIndex), QDate(firstYear, month, day));
            firstTransactions.append(tran);
            if (tran->category->type == com::chancho::Category::Type::EXPENSE) {
                monthTotal -= amount;
            } else {
                monthTotal += amount;
            }
        }
        firstResults.append(monthTotal);
    }


    QList<std::shared_ptr<PublicTransaction>> secondTransactions;
    QList<double> secondResults;
    int secondYear = 1998;

    for(int month=1; month <= 12; month++) {
        double monthTotal = 0;
        for(int tranIndex=0; tranIndex < 5; tranIndex++) {
            int day = dayUnif(re);
            int catIndex = catUnif(re);
            double amount = amountUnif(re);
            auto tran = std::make_shared<PublicTransaction>(secondAcc, amount, cats.at(catIndex), QDate(secondYear, month, day));
            secondTransactions.append(tran);
            if (tran->category->type == com::chancho::Category::Type::EXPENSE) {
                monthTotal -= amount;
            } else {
                monthTotal += amount;
            }
        }
        secondResults.append(monthTotal);
    }

    QList<std::shared_ptr<PublicTransaction>> thirdTransactions;
    QList<double> thirdResults;
    int thirdYear = 1983;

    for(int month=1; month <= 12; month++) {
        double monthTotal = 0;
        for(int tranIndex=0; tranIndex < 5; tranIndex++) {
            int day = dayUnif(re);
            int catIndex = catUnif(re);
            double amount = amountUnif(re);
            auto tran = std::make_shared<PublicTransaction>(secondAcc, amount, cats.at(catIndex), QDate(thirdYear, month, day));
            thirdTransactions.append(tran);
            if (tran->category->type == com::chancho::Category::Type::EXPENSE) {
                monthTotal -= amount;
            } else {
                monthTotal += amount;
            }
        }
        thirdResults.append(monthTotal);
    }

    QList<std::shared_ptr<PublicTransaction>> allTransactions;
    allTransactions.append(firstTransactions);
    allTransactions.append(secondTransactions);
    allTransactions.append(thirdTransactions);

    QTest::newRow("first-acc") << accounts << cats << allTransactions << firstAcc << firstYear << firstResults;
    QTest::newRow("second-acc") << accounts << cats << allTransactions << secondAcc << secondYear << secondResults;
    QTest::newRow("second-acc-bis") << accounts << cats << allTransactions << secondAcc << thirdYear << thirdResults;

}

void
TestStats::testMonthTotalsForAccountComplete() {
    QFETCH(QList<std::shared_ptr<PublicAccount>>, accounts);
    QFETCH(QList<std::shared_ptr<PublicCategory>>, categories);
    QFETCH(QList<std::shared_ptr<PublicTransaction>>, transactions);
    QFETCH(std::shared_ptr<PublicAccount>, account);
    QFETCH(int, year);
    QFETCH(QList<double>, expected);

    chancho::Book book;
    chancho::Stats stats;

    // store all the required data for the test
    foreach(const std::shared_ptr<PublicAccount>& acc, accounts) {
        if (acc->wasStoredInDb()) {
            acc->_dbId = QUuid();
        }
        book.store(acc);
        QVERIFY(acc->wasStoredInDb());
        QVERIFY(!book.isError());
    }

    foreach(const std::shared_ptr<PublicCategory>& cat, categories) {
        if (cat->wasStoredInDb()) {
            cat->_dbId = QUuid();
        }
        book.store(cat);
        QVERIFY(cat->wasStoredInDb());
        QVERIFY(!book.isError());
    }

    foreach(const std::shared_ptr<PublicTransaction>& tran, transactions) {
        if (tran->wasStoredInDb()) {
            tran->_dbId = QUuid();
        }
        book.store(tran);
        QVERIFY(tran->wasStoredInDb());
        QVERIFY(!book.isError());
    }
    auto result = stats.monthsTotalForAccount(account, year);
    QCOMPARE(result, expected);
}

void
TestStats::testMonthTotalsForAccountOnlyFirst_data() {
    QTest::addColumn<QList<std::shared_ptr<PublicAccount>>>("accounts");
    QTest::addColumn<QList<std::shared_ptr<PublicCategory>>>("categories");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("transactions");
    QTest::addColumn<std::shared_ptr<PublicAccount>>("account");
    QTest::addColumn<int>("year");
    QTest::addColumn<QList<double>>("expected");

    auto firstAcc = std::make_shared<PublicAccount>("Bankia", 0);
    auto secondAcc = std::make_shared<PublicAccount>("BBVA", 0);
    QList<std::shared_ptr<PublicAccount>> accounts;
    accounts.append(firstAcc);
    accounts.append(secondAcc);

    auto firstCat = std::make_shared<PublicCategory>("Food", com::chancho::Category::Type::EXPENSE);
    auto secondCat = std::make_shared<PublicCategory>("Driks", com::chancho::Category::Type::EXPENSE);
    auto thirdCat = std::make_shared<PublicCategory>("Salary", com::chancho::Category::Type::INCOME);
    QList<std::shared_ptr<PublicCategory>> cats;
    cats.append(firstCat);
    cats.append(secondCat);
    cats.append(thirdCat);

    std::uniform_int_distribution<int> catUnif(0, 2);
    std::uniform_int_distribution<int> dayUnif(1, 27);  // up to 27 so that we do not have issues with feb
    std::uniform_int_distribution<int> amountUnif(lowerRandomBound, upperRandomBound);
    std::default_random_engine re;


    QList<std::shared_ptr<PublicTransaction>> firstTransactions;
    QList<double> firstResults;
    int firstYear = 2014;

    for(int month=1; month <= 6; month++) {
        double monthTotal = 0;
        for(int tranIndex=0; tranIndex < 5; tranIndex++) {
            int day = dayUnif(re);
            int catIndex = catUnif(re);
            double amount = amountUnif(re);
            auto tran = std::make_shared<PublicTransaction>(firstAcc, amount, cats.at(catIndex), QDate(firstYear, month, day));
            firstTransactions.append(tran);
            if (tran->category->type == com::chancho::Category::Type::EXPENSE) {
                monthTotal -= amount;
            } else {
                monthTotal += amount;
            }
        }
        firstResults.append(monthTotal);
    }
    for(int index=0; index < 6; index++)
        firstResults.append(0);


    QList<std::shared_ptr<PublicTransaction>> secondTransactions;
    QList<double> secondResults;
    int secondYear = 1998;

    for(int month=1; month <= 4; month++) {
        double monthTotal = 0;
        for(int tranIndex=0; tranIndex < 5; tranIndex++) {
            int day = dayUnif(re);
            int catIndex = catUnif(re);
            double amount = amountUnif(re);
            auto tran = std::make_shared<PublicTransaction>(secondAcc, amount, cats.at(catIndex), QDate(secondYear, month, day));
            secondTransactions.append(tran);
            if (tran->category->type == com::chancho::Category::Type::EXPENSE) {
                monthTotal -= amount;
            } else {
                monthTotal += amount;
            }
        }
        secondResults.append(monthTotal);
    }

    for(int index=0; index < 8; index++)
        secondResults.append(0);

    QList<std::shared_ptr<PublicTransaction>> thirdTransactions;
    QList<double> thirdResults;
    int thirdYear = 1983;

    for(int month=1; month <= 11; month++) {
        double monthTotal = 0;
        for(int tranIndex=0; tranIndex < 5; tranIndex++) {
            int day = dayUnif(re);
            int catIndex = catUnif(re);
            double amount = amountUnif(re);
            auto tran = std::make_shared<PublicTransaction>(secondAcc, amount, cats.at(catIndex), QDate(thirdYear, month, day));
            thirdTransactions.append(tran);
            if (tran->category->type == com::chancho::Category::Type::EXPENSE) {
                monthTotal -= amount;
            } else {
                monthTotal += amount;
            }
        }
        thirdResults.append(monthTotal);
    }
    thirdResults.append(0);

    QList<std::shared_ptr<PublicTransaction>> allTransactions;
    allTransactions.append(firstTransactions);
    allTransactions.append(secondTransactions);
    allTransactions.append(thirdTransactions);

    QTest::newRow("first-acc") << accounts << cats << allTransactions << firstAcc << firstYear << firstResults;
    QTest::newRow("second-acc") << accounts << cats << allTransactions << secondAcc << secondYear << secondResults;
    QTest::newRow("second-acc-bis") << accounts << cats << allTransactions << secondAcc << thirdYear << thirdResults;
}

void
TestStats::testMonthTotalsForAccountOnlyFirst() {
    QFETCH(QList<std::shared_ptr<PublicAccount>>, accounts);
    QFETCH(QList<std::shared_ptr<PublicCategory>>, categories);
    QFETCH(QList<std::shared_ptr<PublicTransaction>>, transactions);
    QFETCH(std::shared_ptr<PublicAccount>, account);
    QFETCH(int, year);
    QFETCH(QList<double>, expected);

    chancho::Book book;
    chancho::Stats stats;

    // store all the required data for the test
    foreach(const std::shared_ptr<PublicAccount>& acc, accounts) {
        if (acc->wasStoredInDb()) {
            acc->_dbId = QUuid();
        }
        book.store(acc);
        QVERIFY(acc->wasStoredInDb());
        QVERIFY(!book.isError());
    }

    foreach(const std::shared_ptr<PublicCategory>& cat, categories) {
        if (cat->wasStoredInDb()) {
            cat->_dbId = QUuid();
        }
        book.store(cat);
        QVERIFY(cat->wasStoredInDb());
        QVERIFY(!book.isError());
    }

    foreach(const std::shared_ptr<PublicTransaction>& tran, transactions) {
        if (tran->wasStoredInDb()) {
            tran->_dbId = QUuid();
        }
        book.store(tran);
        QVERIFY(tran->wasStoredInDb());
        QVERIFY(!book.isError());
    }

    auto result = stats.monthsTotalForAccount(account, year);
    QCOMPARE(result, expected);
}

void
TestStats::testMonthTotalsForAccountOnlyLast_data() {
    QTest::addColumn<QList<std::shared_ptr<PublicAccount>>>("accounts");
    QTest::addColumn<QList<std::shared_ptr<PublicCategory>>>("categories");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("transactions");
    QTest::addColumn<std::shared_ptr<PublicAccount>>("account");
    QTest::addColumn<int>("year");
    QTest::addColumn<QList<double>>("expected");

    auto firstAcc = std::make_shared<PublicAccount>("Bankia", 0);
    auto secondAcc = std::make_shared<PublicAccount>("BBVA", 0);
    QList<std::shared_ptr<PublicAccount>> accounts;
    accounts.append(firstAcc);
    accounts.append(secondAcc);

    auto firstCat = std::make_shared<PublicCategory>("Food", com::chancho::Category::Type::EXPENSE);
    auto secondCat = std::make_shared<PublicCategory>("Driks", com::chancho::Category::Type::EXPENSE);
    auto thirdCat = std::make_shared<PublicCategory>("Salary", com::chancho::Category::Type::INCOME);
    QList<std::shared_ptr<PublicCategory>> cats;
    cats.append(firstCat);
    cats.append(secondCat);
    cats.append(thirdCat);

    std::uniform_int_distribution<int> catUnif(0, 2);
    std::uniform_int_distribution<int> dayUnif(1, 27);  // up to 27 so that we do not have issues with feb
    std::uniform_int_distribution<int> amountUnif(lowerRandomBound, upperRandomBound);
    std::default_random_engine re;


    QList<std::shared_ptr<PublicTransaction>> firstTransactions;
    QList<double> firstResults;
    int firstYear = 2014;

    for(int index=0; index < 5; index++)
        firstResults.append(0);
    for(int month=6; month <= 12; month++) {
        double monthTotal = 0;
        for(int tranIndex=0; tranIndex < 5; tranIndex++) {
            int day = dayUnif(re);
            int catIndex = catUnif(re);
            double amount = amountUnif(re);
            auto tran = std::make_shared<PublicTransaction>(firstAcc, amount, cats.at(catIndex), QDate(firstYear, month, day));
            firstTransactions.append(tran);
            if (tran->category->type == com::chancho::Category::Type::EXPENSE) {
                monthTotal -= amount;
            } else {
                monthTotal += amount;
            }
        }
        firstResults.append(monthTotal);
    }


    QList<std::shared_ptr<PublicTransaction>> secondTransactions;
    QList<double> secondResults;
    int secondYear = 1998;

    for(int index=0; index < 7; index++)
        secondResults.append(0);
    for(int month=8; month <= 12; month++) {
        double monthTotal = 0;
        for(int tranIndex=0; tranIndex < 5; tranIndex++) {
            int day = dayUnif(re);
            int catIndex = catUnif(re);
            double amount = amountUnif(re);
            auto tran = std::make_shared<PublicTransaction>(secondAcc, amount, cats.at(catIndex), QDate(secondYear, month, day));
            secondTransactions.append(tran);
            if (tran->category->type == com::chancho::Category::Type::EXPENSE) {
                monthTotal -= amount;
            } else {
                monthTotal += amount;
            }
        }
        secondResults.append(monthTotal);
    }


    QList<std::shared_ptr<PublicTransaction>> thirdTransactions;
    QList<double> thirdResults;
    int thirdYear = 1983;

    thirdResults.append(0);
    for(int month=2; month <= 12; month++) {
        double monthTotal = 0;
        for(int tranIndex=0; tranIndex < 5; tranIndex++) {
            int day = dayUnif(re);
            int catIndex = catUnif(re);
            double amount = amountUnif(re);
            auto tran = std::make_shared<PublicTransaction>(secondAcc, amount, cats.at(catIndex), QDate(thirdYear, month, day));
            thirdTransactions.append(tran);
            if (tran->category->type == com::chancho::Category::Type::EXPENSE) {
                monthTotal -= amount;
            } else {
                monthTotal += amount;
            }
        }
        thirdResults.append(monthTotal);
    }

    QList<std::shared_ptr<PublicTransaction>> allTransactions;
    allTransactions.append(firstTransactions);
    allTransactions.append(secondTransactions);
    allTransactions.append(thirdTransactions);

    QTest::newRow("first-acc") << accounts << cats << allTransactions << firstAcc << firstYear << firstResults;
    QTest::newRow("second-acc") << accounts << cats << allTransactions << secondAcc << secondYear << secondResults;
    QTest::newRow("second-acc-bis") << accounts << cats << allTransactions << secondAcc << thirdYear << thirdResults;
}

void
TestStats::testMonthTotalsForAccountOnlyLast() {
    QFETCH(QList<std::shared_ptr<PublicAccount>>, accounts);
    QFETCH(QList<std::shared_ptr<PublicCategory>>, categories);
    QFETCH(QList<std::shared_ptr<PublicTransaction>>, transactions);
    QFETCH(std::shared_ptr<PublicAccount>, account);
    QFETCH(int, year);
    QFETCH(QList<double>, expected);

    chancho::Book book;
    chancho::Stats stats;

    // store all the required data for the test
    foreach(const std::shared_ptr<PublicAccount>& acc, accounts) {
        if (acc->wasStoredInDb()) {
            acc->_dbId = QUuid();
        }
        book.store(acc);
        QVERIFY(acc->wasStoredInDb());
        QVERIFY(!book.isError());
    }

    foreach(const std::shared_ptr<PublicCategory>& cat, categories) {
        if (cat->wasStoredInDb()) {
            cat->_dbId = QUuid();
        }
        book.store(cat);
        QVERIFY(cat->wasStoredInDb());
        QVERIFY(!book.isError());
    }

    foreach(const std::shared_ptr<PublicTransaction>& tran, transactions) {
        if (tran->wasStoredInDb()) {
            tran->_dbId = QUuid();
        }
        book.store(tran);
        QVERIFY(tran->wasStoredInDb());
        QVERIFY(!book.isError());
    }

    auto result = stats.monthsTotalForAccount(account, year);
    QCOMPARE(result, expected);
}

void
TestStats::testMonthTotalsForAccountScattered_data() {
    QTest::addColumn<QList<std::shared_ptr<PublicAccount>>>("accounts");
    QTest::addColumn<QList<std::shared_ptr<PublicCategory>>>("categories");
    QTest::addColumn<QList<std::shared_ptr<PublicTransaction>>>("transactions");
    QTest::addColumn<std::shared_ptr<PublicAccount>>("account");
    QTest::addColumn<int>("year");
    QTest::addColumn<QList<double>>("expected");

    auto firstAcc = std::make_shared<PublicAccount>("Bankia", 0);
    auto secondAcc = std::make_shared<PublicAccount>("BBVA", 0);
    QList<std::shared_ptr<PublicAccount>> accounts;
    accounts.append(firstAcc);
    accounts.append(secondAcc);

    auto firstCat = std::make_shared<PublicCategory>("Food", com::chancho::Category::Type::EXPENSE);
    auto secondCat = std::make_shared<PublicCategory>("Driks", com::chancho::Category::Type::EXPENSE);
    auto thirdCat = std::make_shared<PublicCategory>("Salary", com::chancho::Category::Type::INCOME);
    QList<std::shared_ptr<PublicCategory>> cats;
    cats.append(firstCat);
    cats.append(secondCat);
    cats.append(thirdCat);

    std::uniform_int_distribution<int> catUnif(0, 2);
    std::uniform_int_distribution<int> dayUnif(1, 27);  // up to 27 so that we do not have issues with feb
    std::uniform_int_distribution<int> amountUnif(lowerRandomBound, upperRandomBound);
    std::default_random_engine re;


    QList<std::shared_ptr<PublicTransaction>> firstTransactions;
    QList<double> firstResults;
    int firstYear = 2014;

    for(int index=0; index < 3; index++)
        firstResults.append(0);
    for(int month=4; month <= 11; month++) {
        double monthTotal = 0;
        for(int tranIndex=0; tranIndex < 5; tranIndex++) {
            int day = dayUnif(re);
            int catIndex = catUnif(re);
            double amount = amountUnif(re);
            auto tran = std::make_shared<PublicTransaction>(firstAcc, amount, cats.at(catIndex), QDate(firstYear, month, day));
            firstTransactions.append(tran);
            if (tran->category->type == com::chancho::Category::Type::EXPENSE) {
                monthTotal -= amount;
            } else {
                monthTotal += amount;
            }
        }
        firstResults.append(monthTotal);
    }
    firstResults.append(0);


    QList<std::shared_ptr<PublicTransaction>> secondTransactions;
    QList<double> secondResults;
    int secondYear = 1998;

    for(int index=0; index < 7; index++)
        secondResults.append(0);
    for(int month=8; month <= 10; month++) {
        double monthTotal = 0;
        for(int tranIndex=0; tranIndex < 5; tranIndex++) {
            int day = dayUnif(re);
            int catIndex = catUnif(re);
            double amount = amountUnif(re);
            auto tran = std::make_shared<PublicTransaction>(secondAcc, amount, cats.at(catIndex), QDate(secondYear, month, day));
            secondTransactions.append(tran);
            if (tran->category->type == com::chancho::Category::Type::EXPENSE) {
                monthTotal -= amount;
            } else {
                monthTotal += amount;
            }
        }
        secondResults.append(monthTotal);
    }
    secondResults.append(0);
    secondResults.append(0);


    QList<std::shared_ptr<PublicTransaction>> thirdTransactions;
    QList<double> thirdResults;
    int thirdYear = 1983;

    thirdResults.append(0);
    for(int month=2; month <= 12; month++) {
        double monthTotal = 0;
        for(int tranIndex=0; tranIndex < 5; tranIndex++) {
            int day = dayUnif(re);
            int catIndex = catUnif(re);
            double amount = amountUnif(re);
            auto tran = std::make_shared<PublicTransaction>(secondAcc, amount, cats.at(catIndex), QDate(thirdYear, month, day));
            thirdTransactions.append(tran);
            if (tran->category->type == com::chancho::Category::Type::EXPENSE) {
                monthTotal -= amount;
            } else {
                monthTotal += amount;
            }
        }
        thirdResults.append(monthTotal);
    }

    QList<std::shared_ptr<PublicTransaction>> allTransactions;
    allTransactions.append(firstTransactions);
    allTransactions.append(secondTransactions);
    allTransactions.append(thirdTransactions);

    QTest::newRow("first-acc") << accounts << cats << allTransactions << firstAcc << firstYear << firstResults;
    QTest::newRow("second-acc") << accounts << cats << allTransactions << secondAcc << secondYear << secondResults;
    QTest::newRow("second-acc-bis") << accounts << cats << allTransactions << secondAcc << thirdYear << thirdResults;
}

void
TestStats::testMonthTotalsForAccountScattered() {
    QFETCH(QList<std::shared_ptr<PublicAccount>>, accounts);
    QFETCH(QList<std::shared_ptr<PublicCategory>>, categories);
    QFETCH(QList<std::shared_ptr<PublicTransaction>>, transactions);
    QFETCH(std::shared_ptr<PublicAccount>, account);
    QFETCH(int, year);
    QFETCH(QList<double>, expected);

    chancho::Book book;
    chancho::Stats stats;

    // store all the required data for the test
    foreach(const std::shared_ptr<PublicAccount>& acc, accounts) {
        if (acc->wasStoredInDb()) {
            acc->_dbId = QUuid();
        }
        book.store(acc);
        QVERIFY(acc->wasStoredInDb());
        QVERIFY(!book.isError());
    }

    foreach(const std::shared_ptr<PublicCategory>& cat, categories) {
        if (cat->wasStoredInDb()) {
            cat->_dbId = QUuid();
        }
        book.store(cat);
        QVERIFY(cat->wasStoredInDb());
        QVERIFY(!book.isError());
    }

    foreach(const std::shared_ptr<PublicTransaction>& tran, transactions) {
        if (tran->wasStoredInDb()) {
            tran->_dbId = QUuid();
        }
        book.store(tran);
        QVERIFY(tran->wasStoredInDb());
        QVERIFY(!book.isError());
    }

    auto result = stats.monthsTotalForAccount(account, year);
    QCOMPARE(result, expected);
}

void
TestStats::testCategoriesPercentage() {
    chancho::Book book;
    chancho::Stats stats;

    // create a number of cats and a number of transactions
    auto acc = std::make_shared<PublicAccount>("Bankia", 0);
    book.store(acc);
    QVERIFY(!book.isError());
    QVERIFY(acc->wasStoredInDb());

    auto firstCat = std::make_shared<PublicCategory>("Food", com::chancho::Category::Type::EXPENSE);
    book.store(firstCat);
    QVERIFY(!book.isError());
    QVERIFY(firstCat->wasStoredInDb());

    auto secondCat = std::make_shared<PublicCategory>("Driks", com::chancho::Category::Type::EXPENSE);
    book.store(secondCat);
    QVERIFY(!book.isError());
    QVERIFY(secondCat->wasStoredInDb());

    auto thirdCat = std::make_shared<PublicCategory>("Salary", com::chancho::Category::Type::INCOME);
    book.store(thirdCat);
    QVERIFY(!book.isError());
    QVERIFY(thirdCat->wasStoredInDb());

    // add a few transactions and ensure that the returned data is the expected
    auto onlyOne = std::make_shared<PublicTransaction>(acc, 100, firstCat, QDate(2015, 4, 21));
    book.store(onlyOne);

    auto onlyOneResult = stats.categoryPercentages(4, 2015);
    QCOMPARE(onlyOneResult.first.count, 1);
    QCOMPARE(onlyOneResult.first.amount, -1 * onlyOne->amount);
    QCOMPARE(onlyOneResult.second.count(), 1);
    QCOMPARE(onlyOneResult.second.at(0).count, 1);
    QCOMPARE(onlyOneResult.second.at(0).amount, -1 * onlyOne->amount);

    auto severalFirst = std::make_shared<PublicTransaction>(acc, 100, firstCat, QDate(2015, 2, 11));
    book.store(severalFirst);

    auto severalSecond = std::make_shared<PublicTransaction>(acc, 23, firstCat, QDate(2015, 2, 1));
    book.store(severalSecond);

    auto severalThird = std::make_shared<PublicTransaction>(acc, 89.50, firstCat, QDate(2015, 2, 2));
    book.store(severalThird);

    auto severalFourth = std::make_shared<PublicTransaction>(acc, 12.34, secondCat, QDate(2015, 2, 18));
    book.store(severalFourth);

    auto severalFith = std::make_shared<PublicTransaction>(acc, 120.89, thirdCat, QDate(2015, 2, 21));
    book.store(severalFith);

    auto severalLast = std::make_shared<PublicTransaction>(acc, 10.40, thirdCat, QDate(2015, 2, 22));
    book.store(severalLast);

    auto severalResults = stats.categoryPercentages(2, 2015);
    QCOMPARE(severalResults.first.count, 3);
    QCOMPARE(severalResults.second.count(), 3);
}

QTEST_MAIN(TestStats)

