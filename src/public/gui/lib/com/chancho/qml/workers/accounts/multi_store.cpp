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

#include "multi_store.h"

namespace com {

namespace chancho {

namespace qml {

namespace workers {

namespace accounts {

namespace {
    const QString NAME_KEY = "name";
    const QString MEMO_KEY = "memo";
    const QString COLOR_KEY = "color";
    const QString AMOUNT_KEY = "amount";
}

MultiStore::MultiStore(BookPtr book, QVariantList accounts)
    : workers::Worker(),
      _book(book),
      _accounts(accounts) {
}

void
MultiStore::run() {
    QList<com::chancho::AccountPtr> accs;
    foreach(const QVariant &var, _accounts) {
        auto map = var.toMap();
        auto name = map[NAME_KEY].toString();
        auto memo = map[MEMO_KEY].toString();
        auto color = map[COLOR_KEY].toString();
        auto amount = map[AMOUNT_KEY].toDouble();
        auto current = std::make_shared<com::chancho::Account>(name, amount, memo, color);
        current->initialAmount = amount;
        accs.append(current);
    }
    _book->store(accs);
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
