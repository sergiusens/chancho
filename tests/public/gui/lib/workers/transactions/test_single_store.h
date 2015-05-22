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

#include <memory>

#include "base_testcase.h"

namespace com {

namespace chancho {

namespace qml {

namespace workers {

namespace transactions {

namespace tests {

class TestSingleStore : public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestSingleStore(QObject *parent = 0)
            : BaseTestCase("TestSingleStore", parent) { }

 private slots:

    void init() override;
    void cleanup() override;

    void testRun_data();
    void testRun();

    void testRunBookError();

    void testRecurrenceStoreTypeMissing();
    void testRecurrenceStoreBothEndPresent();
    void testRecurrenctStore_data();
    void testRecurrenctStore();
};

}
}
}
}
}
}

