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
#include "recurrent_transaction.h"


namespace com {

namespace chancho {

class Stats;
class BookLock;

 /*!
    \class Book
    \brief The Book class allows to perform the queries to the backend databse used to store the different transactions
           that a user has registered with the application.
    \since 0.1
*/
class Book {
    friend class Stats;
    friend class BookLock;

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

        \fn virtual void store(AccountPtr acc);

        Stores or updates the given \a accs in the database.

        \note When an account is newly added to the database a new unique identifier is provided for the account.
     */
    virtual void store(QList<AccountPtr> accs);

    /*!
        \fn virtual void store(CategoryPtr cat);

        Stores or updates the given \a cat in the database.

        \note When a category is newly added to the database a new unique identifier is provided for the account.
    */
    virtual void store(CategoryPtr cat);

    /*!
        \fn virtual void store(CategoryPtr cat);

        Stores or updates the given \a cats in the database.

        \note When a category is newly added to the database a new unique identifier is provided for the account.
     */
    virtual void store(QList<CategoryPtr> cats);

    /*!
        \fn virtual void store(TransactionPtr tran);

        Stores or updates the given \a tran in the database.

        \note When a transaction is newly added to the database a new unique identifier is provided for the account.
    */
    virtual void store(TransactionPtr tran);

    /*!
        \fn virtual void store(TransactionPtr tran);

        Stores or updates the given \a trans in the database.

        \note When a transaction is newly added to the database a new unique identifier is provided for the account.
    */
    virtual void store(QList<TransactionPtr> trans);

    /*!
        \fn virtual void store(RecurrentTransactionPtr tran);

        Stores or updates the given recurrent \a tran in the database.

        \note When a transaction is newly added to the database a new unique identifier is provided for the account.
    */
    virtual void store(RecurrentTransactionPtr tran, bool updatePast=false);

    /*!
        \fn virtual void store(TransactionPtr tran);

        Stores or updates the given recurrent \a trans in the database.

        \note When a transaction is newly added to the database a new unique identifier is provided for the account.
    */
    virtual void store(QList<RecurrentTransactionPtr> trans);

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
        \fn virtual void remove(RecurrentTransactionPtr tran);

        Removes the given \a tran from the database.
    */
    virtual void remove(RecurrentTransactionPtr tran);

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
        \fn virtual QList<TransactionPtr> transactions(RecurrentTransactionPtr recurrent);

        Returns all transactions that have been generated by the given recurrent transaction.
    */
    virtual QList<TransactionPtr> transactions(RecurrentTransactionPtr recurrent,
                                               boost::optional<int> limit = boost::optional<int>(),
                                               boost::optional<int> offset = boost::optional<int>());

    /*!
        \fn virtual int numberOfTransactions();

        Returns the number of all transactions that have been stored in the database.
    */
    virtual int numberOfTransactions();

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
        \fn virtual int numberOfTransactions(RecurrentTransactionPtr recurrent);

        Returns the number of transactions that have been generated by the given recurrent transactions.
    */
    virtual int numberOfTransactions(RecurrentTransactionPtr recurrent);

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

    /*!
        \fn virtual QList<RecurrentTransactionPtr> recurrent_transactions(
        boost::optional<int> limit = boost::optional<int>(), boost::optional<int> offset = boost::optional<int>());

        Returns a list with all the recurrent transactions that have been created in the system.
     */
    virtual QList<RecurrentTransactionPtr> recurrentTransactions(boost::optional<int> limit = boost::optional<int>(),
                                                                 boost::optional<int> offset = boost::optional<int>());

    /*!
        \fn virtual QList<RecurrentTransactionPtr> recurrentTransactions(CategoryPtr cat,
                                                                 boost::optional<int> limit = boost::optional<int>(),
                                                                 boost::optional<int> offset = boost::optional<int>());

        Returns a list with all the recurrent transactions that have the given category.
     */
    virtual QList<RecurrentTransactionPtr> recurrentTransactions(CategoryPtr cat,
                                                                 boost::optional<int> limit = boost::optional<int>(),
                                                                 boost::optional<int> offset = boost::optional<int>());
    virtual QList<CategoryPtr> recurrentCategories(boost::optional<int> limit = boost::optional<int>(),
                                                   boost::optional<int> offset = boost::optional<int>());

    /*!
        \fn virtual int numberOfRecurrentTransactions()

        Returns the number of recurrent transactions in the system.
    */
    virtual int numberOfRecurrentTransactions();

    /*!
        \fn virtual int numberOfRecurrentTransactions(CategoryPtr cat);

        Returns the number of recurrent transactions in the system with the given category.
    */
    virtual int numberOfRecurrentTransactions(CategoryPtr cat);

    /*!
        \fn virtual int numberOfRecurrentCategories();

        Returns the number of categories that have recurrent transactions.
    */
    virtual int numberOfRecurrentCategories();

    /*!
        \fn void generateRecurrentTransactions();

        Generates the recurrent transactions that have not been added since the last time the
        application was used.
     */
    virtual void generateRecurrentTransactions();

    /*!
        \fn virtual int incomeForDay(int day, int month, int year);

        Returns income that was added in an specific day.
    */
    virtual double incomeForDay(int day, int month, int year);

    /*!
        \fn virtual int expenseForDay(int day, int month, int year);

        Returns expense that was added in an specific day.
    */
    virtual double expenseForDay(int day, int month, int year);

    virtual std::shared_ptr<Stats> stats();

    /*!
        \fn static void initDatabse();

        Initializes the databse and ensures it is ready to be used.
     */
    static void initDatabse();

    /*!
        \fn static QStringList tables();

        Returns the tables that are expected to be present in the database.
     */
    static QStringList tables();

    /*!
        \fn static QStringList triggers();

        Returns the triggers that are expected to be present in the database.
     */
    static QStringList triggers();

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

    /*!
        \fn static QString databasePath();

        Returns the path of the database.
     */
    static QString databasePath();

    static double DB_VERSION;

 protected:
    static std::set<QString> TABLES;
    double amountForTypeInDay(int day, int month, int year, Category::Type type);

 private:
    QList<CategoryPtr> parseCategories(std::shared_ptr<system::Query> query);
    QList<TransactionPtr> parseTransactions(std::shared_ptr<system::Query> query);
    QList<RecurrentTransactionPtr> parseRecurrentTransactions(std::shared_ptr<system::Query> query);
    QList<TransactionPtr> transactions(int year, int month, boost::optional<int> day, boost::optional<int> limit,
           boost::optional<int> offset);
    bool storeSingleAcc(AccountPtr ptr);
    bool storeSingleCat(CategoryPtr ptr);
    bool storeSingleTransactions(TransactionPtr ptr);
    bool storeSingleRecurrentTransactions(RecurrentTransactionPtr tran);
    void storeGeneratedTransactions(QMap<RecurrentTransactionPtr, QList<TransactionPtr>> trans);
    void storeRecurrentNoUpdates(RecurrentTransactionPtr recurrent);
    void storeRecurrentWithUpdate(RecurrentTransactionPtr recurrent);

    static QStringList getTriggers(std::shared_ptr<system::Database> db);


 protected:
    system::DatabasePtr _db;
    std::mutex _dbMutex;
    QString _lastError = QString::null;

 private:
    static std::mutex _initMutex;
};

typedef std::shared_ptr<Book> BookPtr;

}

}
