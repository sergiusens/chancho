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

#include <memory>

#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>

#include <com/chancho/category.h>

#include "matchers.h"
#include "public_account.h"
#include "public_transaction.h"
#include "test_transactions_mocked.h"

using ::testing::_;
using ::testing::Mock;
using ::testing::Matcher;
using ::testing::AnyNumber;
using ::testing::Return;
using ::testing::AnyOf;

void
TestTransactionsMocked::init() {
    BaseTestCase::init();
    _dbFactory = new tests::MockDatabaseFactory();

    chancho::system::DatabaseFactory::setInstance(_dbFactory);
}

void
TestTransactionsMocked::cleanup() {
    BaseTestCase::cleanup();
    QFileInfo fi(PublicBook::databasePath());
    if (fi.exists()) {
        QFile::remove(PublicBook::databasePath());
    }
    chancho::system::DatabaseFactory::deleteInstance();
}

void
TestTransactionsMocked::testStoreTransactionNoAccountNoCategory() {
    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testStoreNewTransaction_data() {

}

void
TestTransactionsMocked::testStoreNewTransaction() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testStoreUpdateTransaction_data() {

}

void
TestTransactionsMocked::testStoreUpdateTransaction() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testStoreTransactionTransactionError() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testStoreTransactionExecError() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testStoreTransactionWithAttachment_data() {

}

void
TestTransactionsMocked::testStoreTransactionWithAttachment() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testStoreTransactionList() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testStoreTransactionListTransactionError() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testSToreTransactionListExecError() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testStoreRecurrentNoAccountNoCategory() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testStoreRecurrentExecError() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testStoreRecurrentUpdateGenerated() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testStoreRecurrentUpdateExecError() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testStoreRecurrentNoUpdateGenerated() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testStoreRecurrentNoUpdateGeneratedExecError() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testTransactionsMonthNoLimitNoOffset() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testTransactionsMonthLimitNoOffset() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testTransactionsMonthLimitOffset() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testTransactionsMonthLimitOffsetExecError() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testTransactionsDayNoLimitNoOffset() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testTransactionsDayLimitNoOffset() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testTransactionsDayLimitOffset() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testTransactionsDayLimitOffsetExecError() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testGeneratedTransactionsNoLimitNoOffset() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testGeneratedTransactionsLimitNoOffset() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testGeneratedTransactionsLimitOffset() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testGeneratedTransactionsLimitOffsetExecError() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testNumberOfTransactions_data() {

}

void
TestTransactionsMocked::testNumberOfTransactions() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testNumberOfTransactionsExecError() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testNumberOfTransactionsMonth_data() {

}

void
TestTransactionsMocked::testNumberOfTransactionsMonth() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testNumberOfTransactionsMonthExecError() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testNumberOfTransactionsDay_data() {

}

void
TestTransactionsMocked::testNumberOfTransactionsDay() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testNumberOfTransactionsDayExecError() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testNumberOfTransactionsRecurrent_data() {

}

void
TestTransactionsMocked::testNumberOfTransactionsRecurrent() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testNumberOfTransactionsRecurrentExecError() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testTransactionsCategoryNoLimitNoOffset() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testTransactionsCategoryLimitNoOffset() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testTransactionsCategoryLimitOffset() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testTransactionsCategoryLimitOffsetExecError() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testTransactionsAccount() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testTransactionsAccountExecError() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testMontWithTransactionsNoLimitNoOffset() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testMontWithTransactionsLimitNoOffset() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testMontWithTransactionsLimitOffset() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testMontWithTransactionsLimitOffsetExecError() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testNumberOfMonthsWithTransactions() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testNumberOfMonthsWithTransactionsExecError() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testDaysWithTransactionsNoLimitNoOffset() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testDaysWithTransactionsLimitNoOffset() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testDaysWithTransactionsLimitOffset() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testDaysWithTransactionsLimitOffsetExecError() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testNumberOfDaysWithTransactions() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testNumberOfDaysWithTransactionsExecError() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testRecurrentTransactionsNoLimitNoOffset() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testRecurrentTransactionsLimitNoOffset() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testRecurrentTransactionsLimitOffset() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testRecurrentTransactionsLimitOffsetExecError() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testRecurrentTransactionsCategoryNoLimitNoOffset() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testRecurrentTransactionsCategoryLimitNoOffset() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testRecurrentTransactionsCategoryLimitOffset() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testRecurrentTransactionsCategoryLimitOffsetExecError() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testNumberOfRecurrentTransactions() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testNumberOfRecurrentTransactionsExecError() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testNumberOfRecurrentTransactionsCategory() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testNumberOfRecurrentTransactionsCategoryExecError() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testStoreRecurrentNoPastUpdates() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testStoreRecurrentNoPastUpdatesExecError() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testStoreRecurrentWithPastUpdates() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testStoreRecurrentWithPastUpdatesExecError() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testStoreRecurrentList() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testStoreRecurrentListExecError() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testRemoveTransaction_data() {

}

void
TestTransactionsMocked::testRemoveTransaction() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testRemoveRecurrent() {

    QSKIP("NOt implemented");
}

void
TestTransactionsMocked::testRemoveRecurrentWithGenerated() {

    QSKIP("NOt implemented");
}

QTEST_MAIN(TestTransactionsMocked)
