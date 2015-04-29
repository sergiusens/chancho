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
#include "public_category.h"
#include "base_testcase.h"

namespace chancho = com::chancho;

class TestBookRecurrentTransaction : public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestBookRecurrentTransaction(QObject *parent = 0)
            : BaseTestCase("TestBookRecurrentTransaction", parent) { }

 private slots:

    void init() override;
    void cleanup() override;

    void testStoreAccountNotStored();
    void testStoreCategoryNotStored();
    void testStoreLastGeneratedPresent_data();
    void testStoreLastGeneratedPresent();
    void testStoreLasteGeneratedMissing_data();
    void testStoreLasteGeneratedMissing();
    void testStoreEndDatePresent_data();
    void testStoreEndDatePresent();
    void testStoreEndDateMissing_data();
    void testStoreEndDateMissing();
    void testStoreDefaultsUsed_data();
    void testStoreDefaultsUsed();
    void testStoreNonDefaultsUsed_data();
    void testStoreNonDefaultsUsed();
    void testStoreOcurrencesPresent_data();
    void testStoreOcurrencesPresent();
    void testStoreOcurrencesMissing_data();
    void testStoreOcurrencesMissing();
    void testRecurrentTransactions_data();
    void testRecurrentTransactions();
    void testRecurrentTransactionsLimit_data();
    void testRecurrentTransactionsLimit();
    void testRecurrentTransactionsOffset_data();
    void testRecurrentTransactionsOffset();
    void testRemoveStoredTransaction();
    void testRemoveMissingTransaction();
};

Q_DECLARE_METATYPE(QList<com::chancho::RecurrentTransactionPtr>)
