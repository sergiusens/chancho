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

#pragma once

#include <com/chancho/book.h>

#include "base_testcase.h"
#include "database_factory.h"
#include "database.h"
#include "query.h"
#include "public_book.h"
#include "public_category.h"

namespace chancho = com::chancho;
namespace tests = com::chancho::tests;

class TestTransactionsMocked : public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestTransactionsMocked(QObject *parent = 0)
            : BaseTestCase("TestTransactionsMocked", parent) { }

 private slots:

    void init() override;
    void cleanup() override;

    void testStoreTransactionNoAccountNoCategory();
    void testStoreNewTransaction_data();
    void testStoreNewTransaction();
    void testStoreUpdateTransaction_data();
    void testStoreUpdateTransaction();
    void testStoreTransactionTransactionError();
    void testStoreTransactionExecError();
    void testStoreTransactionWithAttachment_data();
    void testStoreTransactionWithAttachment();

    void testStoreTransactionList();
    void testStoreTransactionListTransactionError();
    void testSToreTransactionListExecError();

    void testStoreRecurrentNoAccountNoCategory();
    void testStoreRecurrentExecError();
    void testStoreRecurrentUpdateGenerated();
    void testStoreRecurrentUpdateExecError();
    void testStoreRecurrentNoUpdateGenerated();
    void testStoreRecurrentNoUpdateGeneratedExecError();

    void testTransactionsMonthNoLimitNoOffset();
    void testTransactionsMonthLimitNoOffset();
    void testTransactionsMonthLimitOffset();
    void testTransactionsMonthLimitOffsetExecError();
    void testTransactionsDayNoLimitNoOffset();
    void testTransactionsDayLimitNoOffset();
    void testTransactionsDayLimitOffset();
    void testTransactionsDayLimitOffsetExecError();

    void testGeneratedTransactionsNoLimitNoOffset();
    void testGeneratedTransactionsLimitNoOffset();
    void testGeneratedTransactionsLimitOffset();
    void testGeneratedTransactionsLimitOffsetExecError();

    void testNumberOfTransactions_data();
    void testNumberOfTransactions();
    void testNumberOfTransactionsExecError();

    void testNumberOfTransactionsMonth_data();
    void testNumberOfTransactionsMonth();
    void testNumberOfTransactionsMonthExecError();

    void testNumberOfTransactionsDay_data();
    void testNumberOfTransactionsDay();
    void testNumberOfTransactionsDayExecError();

    void testNumberOfTransactionsRecurrent_data();
    void testNumberOfTransactionsRecurrent();
    void testNumberOfTransactionsRecurrentExecError();

    void testTransactionsCategoryNoLimitNoOffset();
    void testTransactionsCategoryLimitNoOffset();
    void testTransactionsCategoryLimitOffset();
    void testTransactionsCategoryLimitOffsetExecError();

    void testTransactionsAccount();
    void testTransactionsAccountExecError();

    void testMontWithTransactionsNoLimitNoOffset();
    void testMontWithTransactionsLimitNoOffset();
    void testMontWithTransactionsLimitOffset();
    void testMontWithTransactionsLimitOffsetExecError();

    void testNumberOfMonthsWithTransactions();
    void testNumberOfMonthsWithTransactionsExecError();

    void testDaysWithTransactionsNoLimitNoOffset();
    void testDaysWithTransactionsLimitNoOffset();
    void testDaysWithTransactionsLimitOffset();
    void testDaysWithTransactionsLimitOffsetExecError();

    void testNumberOfDaysWithTransactions();
    void testNumberOfDaysWithTransactionsExecError();

    void testRecurrentTransactionsNoLimitNoOffset();
    void testRecurrentTransactionsLimitNoOffset();
    void testRecurrentTransactionsLimitOffset();
    void testRecurrentTransactionsLimitOffsetExecError();

    void testRecurrentTransactionsCategoryNoLimitNoOffset();
    void testRecurrentTransactionsCategoryLimitNoOffset();
    void testRecurrentTransactionsCategoryLimitOffset();
    void testRecurrentTransactionsCategoryLimitOffsetExecError();

    void testNumberOfRecurrentTransactions();
    void testNumberOfRecurrentTransactionsExecError();

    void testNumberOfRecurrentTransactionsCategory();
    void testNumberOfRecurrentTransactionsCategoryExecError();

    void testStoreRecurrentNoPastUpdates();
    void testStoreRecurrentNoPastUpdatesExecError();

    void testStoreRecurrentWithPastUpdates();
    void testStoreRecurrentWithPastUpdatesExecError();

    void testStoreRecurrentList();
    void testStoreRecurrentListExecError();

    void testRemoveTransaction_data();
    void testRemoveTransaction();

    void testRemoveRecurrent();
    void testRemoveRecurrentWithGenerated();

 private:
    tests::MockDatabaseFactory* _dbFactory;
};

