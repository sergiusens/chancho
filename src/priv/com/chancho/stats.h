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

#include <QList>
#include <QPair>

#include <com/chancho/system/database.h>

#include "account.h"
#include "category.h"

namespace com {

namespace chancho {

/*!
   \class Stats
   \brief The Stats class allows to perform queries about different stats related to the data stored in the
          database;
   \since 0.1
*/
class Stats {
 public:

    struct CategoryPercentage {
        CategoryPtr category;
        int count;
        double amount;
    };

    struct CategoryPercentageTotal {
        int count;
        double amount;
    };

    Stats();
    virtual ~Stats();


    /*!
        \fn virtual QList<int> monthsTotalForAccount(AccountPtr acc, int year);

        Returns a list with the values of the total amount used in each month for an app during the \a year. If
        there are no transactions in a month the method will add the needed 0 values. The result can be assume to
        allways return 12 doubles.

    */
    virtual QList<double> monthsTotalForAccount(AccountPtr acc, int year);

    /*!
        \fn virtual QList<CategoryPercentage> categoryPercentages(int month, int year);

        Returns a list of the categories for a month and the number of types the have occurred;
    */
    virtual QPair<CategoryPercentageTotal, QList<CategoryPercentage>> categoryPercentages(int month, int year);

    /*!
        \fn virtual bool isError();

        Returns if there was an error in the execution of a method.
    */
    virtual bool isError();

    /*!
        \fn virtual QString lastError();

        Returns the last error.
    */
    virtual QString lastError();

 protected:
    std::shared_ptr<system::Database> _db;
    QString _lastError = QString::null;
};

}

}
