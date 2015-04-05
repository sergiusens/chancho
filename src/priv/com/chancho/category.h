/*
 * Copyright (c) 2014 Manuel de la Peña <mandel@themacaque.com>
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

#include <QString>
#include <QMetaType>
#include <QUuid>

namespace com {

namespace chancho {

class Book;

class Category {

 friend class Book;
 friend class Stats;

 public:
    enum class Type {
        INCOME,
        EXPENSE
    };

    Category() = default;
    Category(const QString& n, Category::Type t, const QString& c=QString::null);
    Category(const QString& n, Category::Type t, std::shared_ptr<Category> p, const QString& c=QString::null);
    Category(const Category& other);
    virtual ~Category() = default;

 public:
    QString name = QString::null;
    Category::Type type;
    std::shared_ptr<Category> parent;
    QString color = QString::null;

    virtual bool wasStoredInDb() const;
    bool operator==(const Category& rhs);
    bool operator!=(const Category& rhs);

 protected:
    // optional so that we know if a category was added to the db or not
    QUuid _dbId;
};

typedef std::shared_ptr<Category> CategoryPtr;

bool operator==(const CategoryPtr& lhs, const CategoryPtr& rhs);
bool operator!=(const CategoryPtr& lhs, const CategoryPtr& rhs);

}

}

Q_DECLARE_METATYPE(std::shared_ptr<com::chancho::Category>)
Q_DECLARE_METATYPE(com::chancho::Category::Type)

