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

#include "single_update.h"

namespace com {

namespace chancho {

namespace qml {

namespace workers {

namespace categories {

SingleUpdate::SingleUpdate(BookPtr book, com::chancho::CategoryPtr category, QString name, QString color,
                           qml::Book::TransactionType type)
        : Worker(),
          _book(book),
          _category(category),
          _name(name),
          _color(color),
          _type(type) {
}

void
SingleUpdate::run() {
    com::chancho::Category::Type catType;
    if (_type == qml::Book::TransactionType::EXPENSE) {
        catType = com::chancho::Category::Type::EXPENSE;
    } else {
        catType = com::chancho::Category::Type::INCOME;
    }

    if (_category->name != _name
        || _category->color != _color
        || _category->type != catType) {
        _category->name = _name;
        _category->type = catType;
        _category->color = _color;

        _book->store(_category);

        if (_book->isError()) {
            emit failure();
            return;
        }
    }
    emit success();
}

}
}
}
}
}
