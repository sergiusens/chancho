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

#include <com/chancho/system/query.h>

namespace com {

namespace chancho {

namespace tests {

class MockQuery : public com::chancho::system::Query {
 public:
    MockQuery() : com::chancho::system::Query() {}

    MOCK_METHOD2(addBindValue, void(const QVariant&, QSql::ParamType));
    MOCK_CONST_METHOD0(at, int());
    MOCK_METHOD3(bindValue, void(const QString&, const QVariant&, QSql::ParamType));
    MOCK_METHOD3(bindValue, void(int, const QVariant&, QSql::ParamType));
    MOCK_CONST_METHOD1(boundValue, QVariant(const QString&));
    MOCK_CONST_METHOD1(boundValue, QVariant(int));
    MOCK_CONST_METHOD0(boundValues, QMap<QString, QVariant>());
    MOCK_METHOD0(clear, void());
    MOCK_CONST_METHOD0(driver, const QSqlDriver*());
    MOCK_METHOD1(exec, bool(const QString&));
    MOCK_METHOD0(exec, bool());
    MOCK_METHOD1(execBatch, bool(QSqlQuery::BatchExecutionMode));
    MOCK_CONST_METHOD0(executedQuery, QString());
    MOCK_METHOD0(finish, void());
    MOCK_METHOD0(first, bool());
    MOCK_CONST_METHOD0(isActive, bool());
    MOCK_CONST_METHOD0(isForwardOnly, bool());
    MOCK_CONST_METHOD1(isNull, bool(int));
    MOCK_CONST_METHOD0(isSelect, bool());
    MOCK_CONST_METHOD0(isValid, bool());
    MOCK_METHOD0(last, bool());
    MOCK_CONST_METHOD0(lastError, QSqlError());
    MOCK_CONST_METHOD0(lastInsertId, QVariant());
    MOCK_CONST_METHOD0(lastQuery, QString());
    MOCK_METHOD0(next, bool());
    MOCK_METHOD0(nextResult, bool());
    MOCK_CONST_METHOD0(numRowsAffected, int());
    MOCK_CONST_METHOD0(numericalPrecisionPolicy, QSql::NumericalPrecisionPolicy());
    MOCK_METHOD1(prepare, bool(const QString&));
    MOCK_METHOD0(previous, bool());
    MOCK_CONST_METHOD0(record, QSqlRecord());
    MOCK_CONST_METHOD0(result, const QSqlResult*());
    MOCK_METHOD2(seek, bool(int, bool));
    MOCK_METHOD1(setForwardOnly, void(bool));
    MOCK_METHOD1(setNumericalPrecisionPolicy, void(QSql::NumericalPrecisionPolicy));
    MOCK_CONST_METHOD0(size, int());
    MOCK_CONST_METHOD1(value, QVariant(int));
    MOCK_CONST_METHOD1(value, QVariant(QString));
};

}

}

}
