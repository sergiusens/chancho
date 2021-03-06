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

#include <com/chancho/category.h>

namespace chancho = com::chancho;

class PublicCategory : public chancho::Category {
 public:
    PublicCategory()
            : chancho::Category() {}

    PublicCategory(QUuid id)
            : chancho::Category() {
        _dbId = id;
    }

    PublicCategory(const QString& n, Category::Type t, const QString& c=QString::null)
            : chancho::Category(n, t, c) {}

    PublicCategory(const QString& n, Category::Type t, std::shared_ptr<Category> p, const QString& c=QString::null)
            : chancho::Category(n, t, p, c) {}

    virtual ~PublicCategory() = default;

    using chancho::Category::_dbId;
};

typedef std::shared_ptr<PublicCategory> PublicCategoryPtr;

Q_DECLARE_METATYPE(std::shared_ptr<PublicCategory>)
