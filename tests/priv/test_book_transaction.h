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

#include "public_book.h"
#include "public_transaction.h"
#include "base_testcase.h"

namespace chancho = com::chancho;

class TestBookTransaction : public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestBookTransaction(QObject *parent = 0)
            : BaseTestCase("TestBookTransaction", parent) { }

 private slots:

    void init() override;
    void cleanup() override;

    void testStoreTransaction_data();
    void testStoreTransaction();

    void testUpdateTransaction_data();
    void testUpdateTransaction();

    void testStoreTransactionNoAccount();
    void testStoreTransactionNoCategory();

    void testRemoveTransaction_data();
    void testRemoveTransaction();

    void testTransactionsMonth_data();
    void testTransactionsMonth();

    void testTransactionsMonthCount_data();
    void testTransactionsMonthCount();

    void testTransactionsMonthLimited_data();
    void testTransactionsMonthLimited();

    void testTransactionsDay_data();
    void testTransactionsDay();

    void testTransactionsDayCount_data();
    void testTransactionsDayCount();

    void testTransactionsDayLimited_data();
    void testTransactionsDayLimited();

    void testGetMonthsWithTransactions_data();
    void testGetMonthsWithTransactions();

    void testGetMonthsWithTransactionsCount_data();
    void testGetMonthsWithTransactionsCount();

    void testGetDaysWithTransactions_data();
    void testGetDaysWithTransactions();

    void testGetDaysWithTransactionsCount_data();
    void testGetDaysWithTransactionsCount();

    void testTransactionsCategory_data();
    void testTransactionsCategory();

    void testTransactionsCategoryAndMonth_data();
    void testTransactionsCategoryAndMonth();

    void testTransactionsAccount_data();
    void testTransactionsAccount();

    void testDeleteAccountTransactions_data();
    void testDeleteAccountTransactions();
};
