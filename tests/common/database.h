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

#include <com/chancho/system/database.h>

namespace com {

namespace chancho {

namespace tests {

class MockDatabase : public com::chancho::system::Database {
 public:
    MockDatabase() : com::chancho::system::Database() {}

    MOCK_METHOD0(close, void());
    MOCK_METHOD0(commit, bool());
    MOCK_CONST_METHOD0(connectOptions, QString());
    MOCK_CONST_METHOD0(connectionName, QString());
    MOCK_CONST_METHOD0(createQuery, std::shared_ptr<system::Query>());
    MOCK_CONST_METHOD0(databaseName, QString());
    MOCK_CONST_METHOD0(driverName, QString());
    MOCK_CONST_METHOD1(exec, QSqlQuery(const QString&));
    MOCK_CONST_METHOD0(hostName, QString());
    MOCK_CONST_METHOD0(isOpen, bool());
    MOCK_CONST_METHOD0(isOpenError, bool());
    MOCK_CONST_METHOD0(isValid, bool());
    MOCK_CONST_METHOD0(lastError, QSqlError());
    MOCK_METHOD0(open, bool());
    MOCK_METHOD2(open, bool (const QString&, const QString&));
    MOCK_CONST_METHOD0(password, QString());
    MOCK_CONST_METHOD0(port, int());
    MOCK_CONST_METHOD1(primaryIndex, QSqlIndex(const QString&));
    MOCK_CONST_METHOD1(record, QSqlRecord(const QString&));
    MOCK_METHOD0(rollback, bool());
    MOCK_METHOD1(setConnectOptions, void(const QString& options));
    MOCK_METHOD1(setDatabaseName, void(const QString&));
    MOCK_METHOD1(setHostName, void(const QString&));
    MOCK_METHOD1(setNumericalPrecisionPolicy, void(QSql::NumericalPrecisionPolicy));
    MOCK_METHOD1(setPassword, void(const QString&));
    MOCK_METHOD1(setPort, void(int));
    MOCK_METHOD1(setUserName, void(const QString&));
    MOCK_CONST_METHOD1(tables, QStringList(QSql::TableType type));
    MOCK_METHOD0(transaction, bool());
    MOCK_CONST_METHOD0(userName, QString());
    MOCK_METHOD0(addSqlite3Extensions, bool());
};

}

}

}
