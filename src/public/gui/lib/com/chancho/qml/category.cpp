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

#include "category.h"

namespace com {

namespace chancho {

namespace qml {

Category::Category(QObject* parent)
    : QObject(parent),
      _cat(std::make_shared<com::chancho::Category>()){
}

Category::Category(QString name, qml::Book::TransactionType type, QObject* parent)
    : QObject(parent),
      _cat(std::make_shared<com::chancho::Category>(name, (type == qml::Book::EXPENSE)?
                      chancho::Category::Type::EXPENSE:chancho::Category::Type::INCOME)) {
}

Category::Category(QString name, qml::Book::TransactionType type, QString color, QObject* parent)
        : QObject(parent),
          _cat(std::make_shared<com::chancho::Category>(name, (type == qml::Book::EXPENSE)?
                          chancho::Category::Type::EXPENSE:chancho::Category::Type::INCOME, color)) {
}

Category::Category(CategoryPtr cat, QObject* parent)
    : QObject(parent),
     _cat(cat) {
}

QString
Category::getName() const {
    return _cat->name;
}

void
Category::setName(QString name) {
    if (name != _cat->name) {
        _cat->name = name;
        emit nameChanged(_cat->name);
    }
}

com::chancho::qml::Book::TransactionType
Category::getType() const {
    if (_cat->type == com::chancho::Category::Type::EXPENSE) {
        return com::chancho::qml::Book::TransactionType::EXPENSE;
    } else {
        return com::chancho::qml::Book::TransactionType::INCOME;
    }
}

void
Category::setType(com::chancho::qml::Book::TransactionType type) {
    if (type == com::chancho::qml::Book::EXPENSE && _cat->type != com::chancho::Category::Type::EXPENSE) {
        _cat->type = com::chancho::Category::Type::EXPENSE;
        emit typeChanged(type);
    } else if (type == com::chancho::qml::Book::INCOME && _cat->type != com::chancho::Category::Type::INCOME) {
        _cat->type = com::chancho::Category::Type::EXPENSE;
        emit typeChanged(type);
    }
}

QString
Category::getColor() const {
    return _cat->color;
}

void
Category::setColor(QString color) {
    if(_cat->color != color) {
        _cat->color = color;
        emit colorChanged(color);
    }
}

CategoryPtr
Category::getCategory() const {
    return _cat;
}

}

}

}
