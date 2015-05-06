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

#include <gmock/gmock.h>

#include <com/chancho/qml/workers/transactions.h>

namespace qml = com::chancho::qml;
namespace w = com::chancho::qml::workers;
namespace ta = com::chancho::qml::workers::transactions;

namespace com {

namespace chancho {

namespace tests {

namespace transactions {

class MockGenerateRecurrent : public ta::GenerateRecurrent {
 public:
    MockGenerateRecurrent(BookPtr book) : ta::GenerateRecurrent(book) {}

    MOCK_METHOD0(run, void());
};


class MockSingleRemove : public ta::SingleRemove {

 public:
    MockSingleRemove(BookPtr book, com::chancho::TransactionPtr trans) : ta::SingleRemove(book, trans) {}

    MOCK_METHOD0(run, void());
};

class MockSingleStore : public ta::SingleStore {
 public:
    MockSingleStore() : ta::SingleStore(std::make_shared<com::chancho::Book>(),
                                        std::make_shared<com::chancho::Account>(),
                                        std::make_shared<com::chancho::Category>(),
                                        QDate(), 0, "", "") {}
    MockSingleStore(BookPtr book, chancho::AccountPtr account, chancho::CategoryPtr category, QDate date,
                double amount, QString contents, QString memo)
            : ta::SingleStore(book, account, category, date, amount, contents, memo) {}

    MOCK_METHOD0(run, void());
};

class MockSingleUpdate : public ta::SingleUpdate {

 public:
    MockSingleUpdate(BookPtr book, chancho::TransactionPtr trans, chancho::AccountPtr acc, chancho::CategoryPtr cat,
                 QDate date, QString contents, QString memo, double amount)
            : ta::SingleUpdate(book, trans, acc, cat, date, contents, memo, amount) {}

    MOCK_METHOD0(run, void());
};

class WorkerFactory : public ta::WorkerFactory {
 public:
    WorkerFactory() : ta::WorkerFactory() {}

    MOCK_METHOD7(storeTransaction, w::WorkerThread<ta::SingleStore>*(qml::Book*, chancho::AccountPtr, chancho::CategoryPtr, QDate,
            double, QString, QString));
    MOCK_METHOD2(removeTransaction, w::WorkerThread<ta::SingleRemove>*(qml::Book*, chancho::TransactionPtr));
    MOCK_METHOD8(updateTransaction, w::WorkerThread<ta::SingleUpdate>*(qml::Book*, chancho::TransactionPtr, chancho::AccountPtr,
            chancho::CategoryPtr, QDate, QString, QString, double));
    MOCK_METHOD1(generateRecurrentTransactions, w::WorkerThread<ta::GenerateRecurrent>*(qml::Book*));
};

}
}
}
}
