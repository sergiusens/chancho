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
#include "single_store.h"

namespace com {

namespace chancho {

namespace qml {

namespace workers {

namespace categories {

SingleStore::SingleStore(BookPtr book, QString name, QString color, qml::Book::TransactionType type)
    : workers::Worker(),
      _book(book),
      _name(name),
      _color(color),
      _type(type) {

}

void
SingleStore::run() {
    com::chancho::Category::Type catType;
    if (_type == qml::Book::TransactionType::EXPENSE) {
        catType = com::chancho::Category::Type::EXPENSE;
    } else {
        catType = com::chancho::Category::Type::INCOME;
    }

    auto cat = std::make_shared<com::chancho::Category>(_name, catType, _color);
    _book->store(cat);

    if (_book->isError()) {
        emit failure();
        return;
    }
    emit success();
}

}
}
}
}
}
