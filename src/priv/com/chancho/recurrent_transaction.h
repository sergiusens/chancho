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

#include <boost/optional.hpp>

#include <QDate>
#include <QMetaType>

#include "transaction.h"

namespace com {

namespace chancho {

class RecurrentTransaction {

   friend class Book;

 public:
    class Recurrence {

       friend class Book;

     public:
        enum class Defaults {
           DAILY,
           WEEKLY,
           MONTHLY,
           OTHER
        };

        Recurrence() = default;
        Recurrence(int n, QDate s, QDate e=QDate());
        Recurrence(int n, QDate s, boost::optional<int> o);
        Recurrence(Recurrence::Defaults n, QDate s, QDate e=QDate());
        Recurrence(Recurrence::Defaults n, QDate s, boost::optional<int> o);
        virtual ~Recurrence() = default;

        QDate startDate = QDate();  // the date when we started repeating
        QDate lastGenerated = QDate();  // last time we added a transaction in the db, needed to not recreate
        QDate endDate = QDate();  // shall we stop at some date
        boost::optional<int> occurrences;  // shall we stop after a number of times

        virtual boost::optional<int> numberOfDays() const;
        virtual boost::optional<Recurrence::Defaults> defaults() const;

     protected:
        virtual int ocurrencesPassed();
        virtual QList<QDate> generateMissingDates();

     private:
        boost::optional<int> _numberOfDays = boost::none;
        boost::optional<Recurrence::Defaults> _defaults = boost::none;
    };

    typedef std::shared_ptr<Recurrence> RecurrencePtr;

    RecurrentTransaction() = default;
    RecurrentTransaction(TransactionPtr t, RecurrencePtr r);
    ~RecurrentTransaction() = default;

    virtual bool wasStoredInDb() const;

    TransactionPtr transaction;  // example transaction to be used to repeat
    RecurrencePtr recurrence;

 protected:
   // optional so that we know if a category was added to the db or not
   QUuid _dbId;

   // allow a way to get the init transaction id, mainly for tests
   QUuid initTransactionId();

};

typedef std::shared_ptr<RecurrentTransaction> RecurrentTransactionPtr;

}

}

Q_DECLARE_METATYPE(com::chancho::RecurrentTransaction::RecurrencePtr)
Q_DECLARE_METATYPE(com::chancho::RecurrentTransaction::Recurrence::Defaults)
