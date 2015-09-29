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

#include <com/chancho/book.h>

#include "base_testcase.h"
#include "database_factory.h"
#include "database.h"
#include "query.h"
#include "public_book.h"
#include "public_category.h"

namespace chancho = com::chancho;
namespace tests = com::chancho::tests;

namespace com {

namespace chancho {

namespace tests {

struct CategoryDbRow {
 public:
    CategoryDbRow(QUuid id, QUuid parent, QString name, chancho::Category::Type  type, QString color)
            : uuid(id.toString()),
              name(name),
              type(static_cast<int>(type)),
              color(color) {
        if (parent.isNull()) {
            this->parent = QVariant();
        } else {
            this->parent = parent;
        }
    }

    QVariant uuid;
    QVariant parent;
    QVariant name;
    QVariant type;
    QVariant color;
};

class TestCategoriesMocked : public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestCategoriesMocked(QObject *parent = 0)
            : BaseTestCase("TestCategoriesMocked", parent) { }

 private slots:

    void init() override;
    void cleanup() override;

    void testStoreCategoryNew_data();
    void testStoreCategoryNew();
    void testStoreCategoryUpdate_data();
    void testStoreCategoryUpdate();
    void testStoreCategoryExecError();

    void testStoreCategoryListTransactionError();
    void testStoreCategoryListExecError();

    void testRemoveNotPresent();
    void testRemoveTransactionError();
    void testRemoveTransactionExecError();
    void testRemoveTransaction_data();
    void testRemoveTransaction();

    void testCategories_data();
    void testCategories();
    void testCategoriesLimitNoOffset_data();
    void testCategoriesLimitNoOffset();
    void testCategoriesLimitOffset_data();
    void testCategoriesLimitOffset();
    void testCategoriesExecError();

    void testNumberOfCategoriesNoType_data();
    void testNumberOfCategoriesNoType();
    void testNumberOfCategoriesType_data();
    void testNumberOfCategoriesType();
    void testNumberOfCategoriesExecError();

 private:
    tests::MockDatabaseFactory* _dbFactory;
};

}
}
}


Q_DECLARE_METATYPE(QList<com::chancho::tests::CategoryDbRow>)
