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

#include <QSqlQuery>
#include <QString>
#include <QVariant>

namespace com {

namespace chancho {

namespace system {

class Query {

 public:
    Query() = default;
    Query(const QSqlQuery& q)
        : _query(q) {

    }
    virtual ~Query() = default;

    virtual void addBindValue(const QVariant& val, QSql::ParamType paramType=QSql::In) {
        _query.addBindValue(val, paramType);
    }

    virtual int at() const {
        return _query.at();
    }

    virtual void bindValue(const QString& placeholder, const QVariant& val, QSql::ParamType paramType=QSql::In) {
        _query.bindValue(placeholder, val, paramType);
    }

    virtual void bindValue(int pos, const QVariant& val, QSql::ParamType paramType=QSql::In) {
        _query.bindValue(pos, val, paramType);
    }

    virtual QVariant boundValue(const QString& placeholder) const {
        return _query.boundValue(placeholder);
    }

    virtual QVariant boundValue(int pos) const {
        return _query.boundValue(pos);
    }

    virtual QMap<QString, QVariant> boundValues() const {
        return _query.boundValues();
    }

    virtual void clear() {
        _query.clear();
    }

    virtual const QSqlDriver* driver() const {
        return _query.driver();
    }

    virtual bool exec(const QString& query) {
        return _query.exec(query);
    }

    virtual bool exec() {
        return _query.exec();
    }

    virtual bool execBatch(QSqlQuery::BatchExecutionMode mode=QSqlQuery::ValuesAsRows) {
        return _query.execBatch(mode);
    }

    virtual QString executedQuery() const {
        return _query.executedQuery();
    }

    virtual void finish() {
        _query.finish();
    }

    virtual bool first() {
        return _query.first();
    }

    virtual bool isActive() const {
        return _query.isActive();
    }

    virtual bool isForwardOnly() const {
        return _query.isForwardOnly();
    }

    virtual bool isNull(int field) const {
        return _query.isNull(field);
    }

    virtual bool isSelect() const {
        return _query.isSelect();
    }

    virtual bool isValid() const {
        return _query.isValid();
    }

    virtual bool last() {
        return _query.last();
    }

    virtual QSqlError lastError() const {
        return _query.lastError();
    }

    virtual QVariant lastInsertId() const {
        return _query.lastInsertId();
    }

    virtual QString lastQuery() const {
        return _query.lastQuery();
    }

    virtual bool next() {
        return _query.next();
    }

    virtual bool nextResult() {
        return _query.nextResult();
    }

    virtual int numRowsAffected() const {
        return _query.numRowsAffected();
    }

    virtual QSql::NumericalPrecisionPolicy numericalPrecisionPolicy() const {
        return _query.numericalPrecisionPolicy();
    }

    virtual bool prepare(const QString& query) {
        return _query.prepare(query);
    }

    virtual bool previous() {
        return _query.previous();
    }

    virtual QSqlRecord record() const {
        return _query.record();
    }

    virtual const QSqlResult* result() const {
        return _query.result();
    }

    virtual bool seek(int index, bool relative=false) {
        return _query.seek(index, relative);
    }

    virtual void setForwardOnly(bool forward) {
        _query.setForwardOnly(forward);
    }

    virtual void setNumericalPrecisionPolicy(QSql::NumericalPrecisionPolicy precisionPolicy) {
        _query.setNumericalPrecisionPolicy(precisionPolicy);
    }

    virtual int size() const {
        return _query.size();
    }

    virtual QVariant value(int index) const {
        return _query.value(index);
    }

    virtual QVariant value(QString name) const {
        return _query.value(name);
    }

 private:
    QSqlQuery _query;
};

}

}

}
