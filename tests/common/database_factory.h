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

#include <com/chancho/system/database_factory.h>

namespace com {

namespace chancho {

namespace tests {

class MockDatabaseFactory : public com::chancho::system::DatabaseFactory {
 public:
    MockDatabaseFactory() : com::chancho::system::DatabaseFactory() {}

    MOCK_METHOD2(addDatabase, std::shared_ptr<system::Database>(const QString&, const QString&));
    MOCK_METHOD2(addDatabase, std::shared_ptr<system::Database>(QSqlDriver*, const QString&));
    MOCK_METHOD2(cloneDatabase, std::shared_ptr<system::Database>(const system::Database&, const QString&));
    MOCK_METHOD0(connectionNames, QStringList());
    MOCK_METHOD1(contains, bool(const QString&));
    MOCK_METHOD2(database, std::shared_ptr<system::Database>(const QString&, bool));
    MOCK_METHOD0(drivers, QStringList());
    MOCK_METHOD1(isDriverAvailable, bool(const QString&));
    MOCK_METHOD2(registerSqlDriver, void(const QString&, QSqlDriverCreatorBase*));
    MOCK_METHOD1(removeDatabase, void(const QString&));
};

}

}

}
