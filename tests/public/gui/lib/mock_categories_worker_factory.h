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

#include <com/chancho/qml/workers/categories.h>

namespace qml = com::chancho::qml;
namespace w = com::chancho::qml::workers;
namespace ca = com::chancho::qml::workers::categories;

namespace com {

namespace chancho {

namespace tests {

namespace categories {


class MockMultiStore : public ca::MultiStore {
 public:
    MockMultiStore(BookPtr book, QVariantList categories) : ca::MultiStore(book, categories) {}

    MOCK_METHOD0(run, void());
};

class MockSingleRemove : public ca::SingleRemove {

 public:
    MockSingleRemove(BookPtr book, com::chancho::CategoryPtr category) : ca::SingleRemove(book, category) {}

    MOCK_METHOD0(run, void());
};

class MockSingleStore : public ca::SingleStore {
 public:
    MockSingleStore(BookPtr book, QString name, QString color, qml::Book::TransactionType type)
            : ca::SingleStore(book, name, color, type) {}

    MOCK_METHOD0(run, void());
};

class MockSingleUpdate : public ca::SingleUpdate {

 public:
    MockSingleUpdate(BookPtr book, com::chancho::CategoryPtr category, QString name, QString color,
                 qml::Book::TransactionType type)
            : ca::SingleUpdate(book, category, name, color, type) {}

    MOCK_METHOD0(run, void());
};

class WorkerFactory : public ca::WorkerFactory {
 public:
    WorkerFactory() : ca::WorkerFactory() {}

    MOCK_METHOD2(storeCategories, w::WorkerThread<ca::MultiStore>*(qml::Book*, QVariantList));
    MOCK_METHOD4(storeCategory, w::WorkerThread<ca::SingleStore>*(qml::Book*, QString, QString,
            qml::Book::TransactionType));
    MOCK_METHOD2(removeCategory, w::WorkerThread<ca::SingleRemove>*(qml::Book*, com::chancho::CategoryPtr));
    MOCK_METHOD5(updateCategory, w::WorkerThread<ca::SingleUpdate>*(qml::Book*, com::chancho::CategoryPtr, QString,
            QString, qml::Book::TransactionType));
};

}
}
}
}
