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

#include <com/chancho/book.h>

namespace com {

namespace chancho {

namespace tests {

class MockBook: public com::chancho::Book {
 public:
    MOCK_METHOD1(store, void(AccountPtr));
    MOCK_METHOD1(store, void(CategoryPtr));
    MOCK_METHOD1(store, void(TransactionPtr));
    MOCK_METHOD1(remove, void(AccountPtr));
    MOCK_METHOD1(remove, void(CategoryPtr));
    MOCK_METHOD1(remove, void(TransactionPtr));
    MOCK_METHOD0(accounts, QList<AccountPtr>());
    MOCK_METHOD0(categories, QList<CategoryPtr>());
    MOCK_METHOD2(transactions, QList<TransactionPtr>(int, int));
    MOCK_METHOD4(transactions, QList<TransactionPtr>(int, int, int, int));
    MOCK_METHOD3(transactions, QList<TransactionPtr>(int, int, int));
    MOCK_METHOD5(transactions, QList<TransactionPtr>(int, int, int, int, int));
    MOCK_METHOD2(numberOfTransactions, int(int, int));
    MOCK_METHOD3(numberOfTransactions, int(int, int, int));
    MOCK_METHOD3(transactions, QList<TransactionPtr>(CategoryPtr, boost::optional<int>, boost::optional<int>));
    MOCK_METHOD1(transactions, QList<TransactionPtr>(AccountPtr));
    MOCK_METHOD3(monthsWithTransactions, QList<int>(int, boost::optional<int>, boost::optional<int>));
    MOCK_METHOD1(numberOfMonthsWithTransactions, int(int));
    MOCK_METHOD4(daysWithTransactions, QList<int>(int, int, boost::optional<int>, boost::optional<int>));
    MOCK_METHOD2(numberOfDaysWithTransactions, int(int, int));
    MOCK_METHOD0(isError, bool());
    MOCK_METHOD0(lastError, QString());
};

}

}

}

