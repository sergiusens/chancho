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

#include <memory>

#include <com/chancho/qml/models/recurrent_categories.h>

#include "book.h"
#include "base_testcase.h"
#include "public_recurrent_categories_model.h"
#include "public_category.h"
#include "public_qml_category.h"

class TestRecurrentCategoriesModel : public BaseTestCase {
   Q_OBJECT

 public:
   explicit TestRecurrentCategoriesModel(QObject *parent = 0)
           : BaseTestCase("TestRecurrentCategoriesModel", parent) { }

 private slots:

   void init() override;
   void cleanup() override;

   void testRowCount();
   void testRowCountError();

   void testDataNotValidIndex();
   void testDataOutOfIndex();
   void testDataBookError();
   void testDataNoData();
   void testDataGetCategory();
};
