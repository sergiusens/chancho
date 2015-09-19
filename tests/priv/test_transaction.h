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

#include <com/chancho/transaction.h>

#include "base_testcase.h"

class TestTransaction : public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestTransaction(QObject *parent = 0)
            : BaseTestCase("TestTransaction", parent) { }

 private slots:

    void init() override;
    void cleanup() override;

    void testConstructorWithDate_data();
    void testConstructorWithDate();
    void testContrutorCurrentDate_data();
    void testContrutorCurrentDate();

    void testWasDbStored_data();
    void testWasDbStored();

    void testType_data();
    void testType();

    void testTypeMissingCat_data();
    void testTypeMissingCat();

    void testAddAttachment_data();
    void testAddAttachment();
    void testRemoveAttachment();
    void testRemoveAttachmentName();
};
