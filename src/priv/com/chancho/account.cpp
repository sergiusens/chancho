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

#include "account.h"

namespace com {

namespace chancho {

Account::Account(const QString& n, double a, const QString& m)
    : name(n),
      amount(a),
      memo(m) {
}

bool
Account::wasStoredInDb() const {
    return !_dbId.isNull();
}

bool
Account::operator==(const Account& rhs) {
    return _dbId == rhs._dbId;
}

bool
Account::operator!=(const Account& rhs) {
    return _dbId != rhs._dbId;
}

bool operator==(const AccountPtr& lhs, const AccountPtr& rhs) {
    if (lhs && rhs) {
        return *lhs.get() == *rhs.get();
    } else {
        return lhs.get() == rhs.get();
    }
}

bool operator!=(const AccountPtr& lhs, const AccountPtr& rhs) {
    if (lhs && rhs) {
        return *lhs.get() != *rhs.get();
    } else {
        return lhs.get() != rhs.get();
    }
}

}

}
