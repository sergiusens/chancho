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

namespace com {

namespace chancho {

namespace tests {

struct AccountDbRow {
 public:
    AccountDbRow(QUuid id, QString name, QString memo, QString color, double initialAmount, double amount)
            : uuid(id.toString()),
              name(name),
              memo(memo),
              color(color),
              initialAmount(initialAmount),
              amount(amount) {
    }

    QVariant uuid;
    QVariant name;
    QVariant memo;
    QVariant color;
    QVariant initialAmount;
    QVariant amount;
};

class TestAccountsMocked : public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestAccountsMocked(QObject *parent = 0)
            : BaseTestCase("TestAccountsMocked", parent) { }

 private slots:

    void init() override;
    void cleanup() override;

    void testStoreAccountExecError();
    void testStoreAccount_data();
    void testStoreAccount();

    void testStoreAccountListTransactionError();
    void testStoreAccountListExecError();
    void testStoreAccountList_data();
    void testStoreAccountList();

    void testRemoveAccountNotStored();
    void testRemoveAccountExecError();
    void testRemoveAccount_data();
    void testRemoveAccount();

    void testAccountsNoLimitNoOffset_data();
    void testAccountsNoLimitNoOffset();
    void testAccountsNoLimitNoOffsetError();
    void testAccountsLimitNoOffset_data();
    void testAccountsLimitNoOffset();
    void testAccountsLimitNoOffsetError();
    void testAccountsLimitOffset_data();
    void testAccountsLimitOffset();
    void testAccountsLimitOffsetError();

    void testNumberOfAccountsExecError();
    void testNumberOfAccountsExec_data();
    void testNumberOfAccountsExec();

 private:
    tests::MockDatabaseFactory* _dbFactory;
};

}
}
}

Q_DECLARE_METATYPE(QList<com::chancho::tests::AccountDbRow>)
