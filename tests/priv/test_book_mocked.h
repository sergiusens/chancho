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

class TestBookMocked : public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestBookMocked(QObject *parent = 0)
            : BaseTestCase("TestBookMocked", parent) { }

 private slots:

    void init() override;
    void cleanup() override;

    void testDatabasePathMissing();

    void testInitDatbaseMissingTables();
    void testInitDatbaseMissingTablesError();
    void testInitDatabasePresentTables_data();
    void testInitDatabasePresentTables();

    void testStoreCategoryOpenError();
    void testStoreCategoryExecError();

    void testRemoveCategoryOpenError();
    void testRemoveCategoryChildsExecError();
    void testRemoveCategoryDeleteExecError();

    void testStoreAccountsOpenError();
    void testStoreAccountExecError();

    void testRemoveAccountOpenError();
    void testRemoveAccountExecError();

    void testAccountsOpenError();
    void testAccountsExecError();

    void testCategoriesOpenError();
    void testCategoriesExecError();

    void testStoreTransactionOpenError();
    void testStoreTransactionExecError();

    void testRemoveTransactionOpenError();
    void testRemoveTransactionExecError();

    void testTransactionsMonthOpenError();
    void testTransactionsMonthExecError();

    void testTransactionsCategoryOpenError();
    void testTransactionsCategoryExecError();

    void testTransactionsCategoryMonthOpenError();
    void testTransactionsCategoryMonthExecError();

    void testTransactionsAccountOpenError();
    void testTransactionsAccountExecError();

 private:
    tests::MockDatabaseFactory* _dbFactory;
};
