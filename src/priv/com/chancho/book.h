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
#include <mutex>
#include <set>

#include <boost/optional.hpp>

#include <QList>

#include <com/chancho/static_init.h>
#include <com/chancho/system/database.h>

#include "account.h"
#include "category.h"
#include "transaction.h"


namespace com {

namespace chancho {

 /*!
    \class Book
    \brief The Book class allows to perform the queries to the backend databse used to store the different transactions
           that a user has registered with the application.
    \since 0.1
*/
class Book {
 public:
    Book();
    virtual ~Book();

    DECLARE_STATIC_INIT(Book);

    /*!
        \fn virtual void store(AccountPtr acc);

        Stores or updates the given \a acc in the database.

        \note When an account is newly added to the database a new unique identifier is provided for the account.
    */
    virtual void store(AccountPtr acc);

    /*!
        \fn virtual void store(CategoryPtr cat);

        Stores or updates the given \a cat in the database.

        \note When a category is newly added to the database a new unique identifier is provided for the account.
    */
    virtual void store(CategoryPtr cat);

    /*!
        \fn virtual void store(TransactionPtr tran);

        Stores or updates the given \a tran in the database.

        \note When a transaction is newly added to the database a new unique identifier is provided for the account.
    */
    virtual void store(TransactionPtr tran);

    /*!
        \fn virtual void remove(AccountPtr acc);

        Removes the given \a acc from the database.
    */
    virtual void remove(AccountPtr acc);

    /*!
        \fn virtual void remove(CategoryPtr cat);

        Removes the given \a tran from the database.

        \note If the remove category is a parent category all of its children will be also removed.
    */
    virtual void remove(CategoryPtr cat);

    /*!
        \fn virtual void remove(TransactionPtr tran);

        Removes the given \a tran from the database.
    */
    virtual void remove(TransactionPtr tran);

    /*!
        \fn virtual QList<AccountPtr> accounts();

        Returns all the accounts that can be found in the database.
    */
    virtual QList<AccountPtr> accounts(boost::optional<int> limit=boost::optional<int>(),
            boost::optional<int> offset=boost::optional<int>());

    /*!
        \fn virtual int numberOfAccounts();

        Returns the number of accounts that have been added so far to the system.
    */
    virtual int numberOfAccounts();

    /*!
        \fn virtual QList<CategoryPtr> categories();

        Returns all the categories that can be found in the database. If the categories have children categories it
        those will be found in the parent object.
    */
    virtual QList<CategoryPtr> categories(boost::optional<Category::Type> type=boost::optional<Category::Type>(),
            boost::optional<int> linit=boost::optional<int>(), boost::optional<int> offset=boost::optional<int>());

    /*!
        \fn virtual int numberOfCategories();

        Returns the number of categories that have been added so far to the system.
    */
    virtual int numberOfCategories(boost::optional<Category::Type> type = boost::optional<Category::Type>());

    /*!
        \fn virtual QList<TransactionPtr> transactions(int month, int year)

        Returns all the transactions that can be found in the database for the given month.
    */
    virtual QList<TransactionPtr> transactions(int month, int year) {
        return transactions(year, month, boost::optional<int>(), boost::optional<int>(), boost::optional<int>());
    }

    /*!
        \fn virtual QList<TransactionPtr> transactions(int month, int year, int limit, int offset)

        Returns all the transactions that can be found in the database for the given month with the given limit and
        offset from the top result.
    */
    virtual QList<TransactionPtr> transactions(int month, int year, int limit, int offset) {
        return transactions(year, month, boost::optional<int>(), boost::optional<int>(limit),
                boost::optional<int>(offset));
    }

    /*!
        \fn virtual QList<TransactionPtr> transactions(int day, int month, int year)

        Returns all the transactions that can be found in the database for the given day.
    */
    virtual QList<TransactionPtr> transactions(int day, int month, int year) {
        return transactions(year, month, boost::optional<int>(day), boost::optional<int>(), boost::optional<int>());
    }

    /*!
        \fn virtual QList<TransactionPtr> transactions(int day, int month, int year, int limit, int offset)

        Returns all the transactions that can be found in the database for the given day with the given limit and
        offset from the top result.
    */
    virtual QList<TransactionPtr> transactions(int day, int month, int year, int limit, int offset) {
        // some times the compiler needs some help not to get in a stupid loop (calling this func over an over)
        return transactions(year, month, boost::optional<int>(day), boost::optional<int>(limit),
                boost::optional<int>(offset));
    }

    /*!
        \fn virtual int numberOfTransactions(int month, int year);

        Returns the number of transactions that have been stored in an specific month.
    */
    virtual int numberOfTransactions(int month, int year);

    /*!
        \fn virtual int numberOfTransactions(int day, int month, int year);

        Returns the number of transactions that have been stored in an specific day.
    */
    virtual int numberOfTransactions(int day, int month, int year);

    /*!
        \fn virtual QList<TransactionPtr> transactions(CategoryPtr cat, int month, int year);

        Returns all the transactions that have be registered with the given category in a specific month.
    */
    virtual QList<TransactionPtr> transactions(CategoryPtr cat, boost::optional<int> month=boost::optional<int>(),
            boost::optional<int> year=boost::optional<int>());

    /*!
        \fn virtual QList<TransactionPtr> transactions(AccountPtr acc);

        Returns all the transactions that have be registered to the given account.
    */
    virtual QList<TransactionPtr> transactions(AccountPtr acc);

    /*!
        \fn virtual QList<int> monthsWithTransactions(int year, int limit, int offset);

        Returns all the months in a year that have registered transactions limited to the given amount and with the
        given offset from the top result.
    */
    virtual QList<int> monthsWithTransactions(int year, boost::optional<int> limit=boost::optional<int>(),
            boost::optional<int> offset=boost::optional<int>());

    /*!
        \fn virtual int numberOfMonthsWithTransactions(int year);

        Returns the number of months that have transactions in the database for the given year.
    */
    virtual int numberOfMonthsWithTransactions(int year);

    /*!
        \fn virtual QList<int> daysWithTransactions(int month, int year, int limit, int offset);

        Returns all the days in a month that have registered transactions limited to the given amount and with the
        given offset from the top result.
    */
    virtual QList<int> daysWithTransactions(int month, int year, boost::optional<int> limit=boost::optional<int>(),
            boost::optional<int> offset=boost::optional<int>());

    /*!
        \fn virtual int numberOfDaysWithTransactions(int month, int year);

        Returns the number of days in a month that have registered transactions.
    */
    virtual int numberOfDaysWithTransactions(int month, int year);

    static void initDatabse();

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

    static double DB_VERSION;

 protected:
    static std::set<QString> TABLES;
    static QString databasePath();

 private:
   QList<TransactionPtr> parseTransactions(std::shared_ptr<system::Query> query);
   QList<TransactionPtr> transactions(int year, int month, boost::optional<int> day, boost::optional<int> limit,
           boost::optional<int> offset);

 protected:
    std::shared_ptr<system::Database> _db;
    QString _lastError = QString::null;

 private:
    std::mutex _transactionMutex;
    std::mutex _categoriesMutex;
    std::mutex _accountsMutex;
};

typedef std::shared_ptr<Book> BookPtr;

}

}
