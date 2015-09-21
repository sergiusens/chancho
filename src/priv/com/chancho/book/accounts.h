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

#include <mutex>

#include <QList>

#include <boost/optional/optional.hpp>

#include <com/chancho/account.h>
#include <com/chancho/system/database.h>

namespace com {

namespace chancho {

namespace book {

class Accounts {

 public:
    Accounts() = delete;
    Accounts(system::DatabasePtr db);
    virtual ~Accounts() = default;

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
        \fn virtual void remove(AccountPtr acc);

        Removes the given \a acc from the database.
    */
    virtual void remove(AccountPtr acc);

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
    bool storeSingleAcc(AccountPtr ptr);

 protected:
    system::DatabasePtr _db;
    QString _lastError = QString::null;
};

}
}
}
