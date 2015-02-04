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

#include <boost/multiprecision/cpp_dec_float.hpp>

#include <com/chancho/account.h>

#include "base_testcase.h"


class PublicSetters : public com::chancho::Account {
 public:
    PublicSetters() : com::chancho::Account() {}
    PublicSetters(const QString& n, cpp_dec_float_50 a=0, const QString& m=QString::null)
        : com::chancho::Account(n, a, m) {}

    using com::chancho::Account::_dbId;
};

class TestAccount : public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestAccount(QObject *parent = 0)
            : BaseTestCase("TestAccount", parent) { }

 private slots:

    void init() override;
    void cleanup() override;

    void testNameConstructor_data();
    void testNameConstructor();

    void testNameAmountConstructor_data();
    void testNameAmountConstructor();

    void testNameAmountMemo_data();
    void testNameAmountMemo();

    void testWasDbStored_data();
    void testWasDbStored();
};
