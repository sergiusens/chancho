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

#include <QObject>

#include <com/chancho/category.h>
#include "com/chancho/qml/book.h"

namespace com {

namespace chancho {

namespace qml {

class Book;
class Transaction;

namespace models {

class Categories;

}

class Category : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(com::chancho::qml::Book::TransactionType type READ getType WRITE setType NOTIFY typeChanged)

    friend class models::Categories;
    friend class qml::Book;
    friend class qml::Transaction;

 public:
    explicit Category(QObject* parent=0);
    Category(QString name, qml::Book::TransactionType type, QObject* parent =0);

    QString getName() const;
    void setName(QString name);

    com::chancho::qml::Book::TransactionType getType() const;
    void setType(com::chancho::qml::Book::TransactionType type);

 signals:
    void nameChanged(QString);
    void typeChanged(com::chancho::qml::Book::TransactionType type);

 protected:
    Category(CategoryPtr cat, QObject* parent=0);
    CategoryPtr getCategory() const;

 private:
    CategoryPtr _cat;
};

}

}

}

