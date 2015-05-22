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

#include "single_store.h"

namespace com {

namespace chancho {

namespace qml {

namespace workers {

namespace transactions {

SingleStore::SingleStore(BookPtr book, chancho::AccountPtr account, chancho::CategoryPtr category, QDate date,
                         double amount, QString contents, QString memo)
        : workers::Worker(),
          _book(book),
          _account(account),
          _category(category),
          _date(date),
          _amount(amount),
          _contents(contents),
          _memo(memo) {
}

void
SingleStore::run() {
    auto tran = std::make_shared<chancho::Transaction>(
            _account, _amount, _category, _date, _contents, _memo);
    _book->store(tran);

    if (_book->isError()) {
        emit failure();
    } else {
        emit success();
    }
}

}
}
}
}
}
