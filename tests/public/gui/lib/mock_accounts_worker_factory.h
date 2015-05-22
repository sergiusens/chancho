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

#include <com/chancho/qml/workers/accounts.h>

namespace qml = com::chancho::qml;
namespace w = com::chancho::qml::workers;
namespace ac = com::chancho::qml::workers::accounts;

namespace com {

namespace chancho {

namespace tests {

namespace accounts {

class MockMultiStore : public ac::MultiStore {
 public:
    MockMultiStore(BookPtr book, QVariantList accounts)
            : ac::MultiStore(book, accounts) {
    }

    MOCK_METHOD0(run, void());
};

class MockSingleRemove : public ac::SingleRemove {
 public:
    MockSingleRemove(BookPtr book, com::chancho::AccountPtr account)
            : ac::SingleRemove(book, account) {
    }

    MOCK_METHOD0(run, void());
};

class MockSingleStore : public ac::SingleStore {
 public:
    MockSingleStore(BookPtr book, QString name, QString memo, QString color, double initialAmount)
            : ac::SingleStore(book, name, memo, color, initialAmount) {

    }

    MOCK_METHOD0(run, void());
};

class MockSingleUpdate : public ac::SingleUpdate {
 public:
    MockSingleUpdate(BookPtr book, com::chancho::AccountPtr account, QString name, QString memo,
                 QString color) : ac::SingleUpdate(book, account, name, memo, color) {
    }

    MOCK_METHOD0(run, void());
};

class WorkerFactory : public ac::WorkerFactory {
 public:
    WorkerFactory() : ac::WorkerFactory() {}

    MOCK_METHOD5(storeAccount, w::WorkerThread<ac::SingleStore>*(qml::Book*, QString, QString, QString, double));
    MOCK_METHOD2(storeAccounts, w::WorkerThread<ac::MultiStore>*(qml::Book*, QVariantList));
    MOCK_METHOD2(removeAccount, w::WorkerThread<ac::SingleRemove>*(qml::Book*, com::chancho::AccountPtr));
    MOCK_METHOD5(updateAccount, w::WorkerThread<ac::SingleUpdate>*(qml::Book*, com::chancho::AccountPtr, QString, QString, QString));
};

}
}
}
}
