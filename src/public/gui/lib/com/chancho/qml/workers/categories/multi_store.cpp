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

#include "com/chancho/qml/book.h"

#include "multi_store.h"

namespace com {

namespace chancho {

namespace qml {

namespace workers {

namespace categories {

namespace {
    const QString NAME_KEY = "name";
    const QString COLOR_KEY = "color";
    const QString TYPE_KEY = "type";
}

MultiStore::MultiStore(BookPtr book, QVariantList categories)
        : workers::Worker(),
          _book(book),
          _categories(categories) {
}

void
MultiStore::run() {
    QList<com::chancho::CategoryPtr> cats;
    _wasExpense = false;
    _wasIncome = false;
    foreach(const QVariant& var, _categories) {
        auto map = var.toMap();
        auto name = map[NAME_KEY].toString();
        auto color = map[COLOR_KEY].toString();
        auto qmlType = map[TYPE_KEY].toInt();
        auto type = com::chancho::Category::Type::EXPENSE;
        if (qmlType == qml::Book::TransactionType::INCOME) {
            type = com::chancho::Category::Type::INCOME;
            _wasIncome = true;
        } else {
            _wasExpense = true;
        }

        auto current = std::make_shared<com::chancho::Category>(name, type, color);
        cats.append(current);
    }
    _book->store(cats);

    if (_book->isError()) {
        emit failure();
        return;
    }

    emit success();
}

bool
MultiStore::wasIncome() {
    return _wasIncome;
}

bool
MultiStore::wasExpense() {
    return _wasExpense;
}

}
}
}
}
}

