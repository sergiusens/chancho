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

#include <com/chancho/system/database.h>
#include <com/chancho/category.h>
#include <boost/optional/optional.hpp>

namespace com {

namespace chancho {

namespace book {

class Categories {

 public:
    Categories() = delete;

    Categories(system::DatabasePtr db);

    virtual ~Categories() = default;

    /*!
        \fn virtual void store(CategoryPtr cat);

        Stores or updates the given \a cat in the database.

        \note When a category is newly added to the database a new unique identifier is provided for the account.
    */
    virtual void store(CategoryPtr cat) ;

    /*!
        \fn virtual void store(CategoryPtr cat);

        Stores or updates the given \a cats in the database.

        \note When a category is newly added to the database a new unique identifier is provided for the account.
     */
    virtual void store(QList<CategoryPtr> cats);

    /*!
        \fn virtual void remove(CategoryPtr cat);

        Removes the given \a tran from the database.

        \note If the remove category is a parent category all of its children will be also removed.
    */
    virtual void remove(CategoryPtr cat);

    /*!
        \fn virtual QList<CategoryPtr> categories();

        Returns all the categories that can be found in the database. If the categories have children categories it
        those will be found in the parent object.
    */
    virtual QList<CategoryPtr> categories(boost::optional<Category::Type> type=boost::optional<Category::Type>(),
                                          boost::optional<int> linit=boost::optional<int>(),
                                          boost::optional<int> offset=boost::optional<int>());

    /*!
        \fn virtual int numberOfCategories();

        Returns the number of categories that have been added so far to the system.
    */
    virtual int numberOfCategories(boost::optional<Category::Type> type = boost::optional<Category::Type>());

    /*!
        \fn virtual QList<CategoryPtr> recurrentCategories(boost::optional<int> limit = boost::optional<int>(),
                                                   boost::optional<int> offset = boost::optional<int>());

        Returns all those categories that have recurrent transactions.
     */
    virtual QList<CategoryPtr> recurrentCategories(boost::optional<int> limit = boost::optional<int>(),
                                                   boost::optional<int> offset = boost::optional<int>());

    /*!
        \fn virtual int numberOfRecurrentCategories();

        Returns the number of categories that have recurrent transactions.
    */
    virtual int numberOfRecurrentCategories();

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

 private:
    QList<CategoryPtr> parseCategories(std::shared_ptr<system::Query> query);
    bool storeSingleCat(CategoryPtr ptr);

 protected:
    system::DatabasePtr _db;
    QString _lastError = QString::null;
};

}
}
}
