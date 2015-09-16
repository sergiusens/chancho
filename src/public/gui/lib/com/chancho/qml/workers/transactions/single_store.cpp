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

#include <glog/logging.h>

#include <boost/optional.hpp>

#include <com/chancho/book.h>
#include <com/chancho/transaction.h>
#include <com/chancho/recurrent_transaction.h>
#include <com/chancho/qml/book.h>

#include "single_store.h"

namespace com {

namespace chancho {

namespace qml {

namespace workers {

namespace transactions {

namespace {
    const QString RECURRENCE_TYPE_KEY = "type";
    const QString RECURRENCE_DAYS_KEY = "days";
    const QString END_KEY = "end";
    const QString END_DATE_KEY = "date";
    const QString END_OCCURRENCES_KEY = "ocurrences";
}

SingleStore::SingleStore(BookPtr book, chancho::AccountPtr account, chancho::CategoryPtr category, QDate date,
                         double amount, QString contents, QString memo, QVariantMap recurrence)
        : workers::Worker(),
          _book(book),
          _account(account),
          _category(category),
          _date(date),
          _amount(amount),
          _contents(contents),
          _memo(memo),
          _recurrence(recurrence) {
}

void
SingleStore::run() {
    DLOG(INFO) << __PRETTY_FUNCTION__;
    if (_recurrence.count() > 0) {
        DLOG(INFO) << "Storing new recurrenct transaction";
        storeRecurrentTransaction();
    } else {
        DLOG(INFO) << "Storing new transaction";
        storeTransaction();
    }
}

void
SingleStore::storeTransaction() {
    auto tran = std::make_shared<chancho::Transaction>(
            _account, _amount, _category, _date, _contents, _memo);
    _book->store(tran);

    if (_book->isError()) {
        emit failure();
    } else {
        emit success();
    }
}

void
SingleStore::storeRecurrentTransaction() {
    // the expected representation of the QVariantMap is the following:
    //
    // 1. A recurrence with a known type:
    // {
    //     "type": DAILY|WEEKLY|MONTHLY
    // }
    //
    // 2. One with a specific number of days
    // {
    //     "days": int
    // }
    //
    // One of the two must be present. The end key can specify when the recurrent data terminates, this is an optional
    // key that can contains a property specifying the data.
    //
    // 1. Specific date:
    // {
    //     "type": DAILY|WEEKLY|MONTHLY,
    //     "end": {
    //         "date": QDate
    //     }
    // }
    //
    // 2. Occurrences
    // {
    //     "type": DAILY|WEEKLY|MONTHLY,
    //     "end": {
    //         "occurrences": int
    //     }
    // }
    DLOG(INFO) << __PRETTY_FUNCTION__;
    if (_recurrence.count() == 0
            || (!_recurrence.contains(RECURRENCE_TYPE_KEY)
                       && !_recurrence.contains(RECURRENCE_DAYS_KEY))) {
        LOG(WARNING) << "Could not create a recurrent data because the recurrence type of number of days"
                << " are missing.";
        emit failure();
        return;
    }

    auto tran = std::make_shared<chancho::Transaction>(_account, _amount, _category, _date, _contents, _memo);
    com::chancho::RecurrentTransaction::RecurrencePtr recurrence;

    QDate endDate = QDate();
    // something fishy is going on with gcc and this line.
    #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
    boost::optional<int> occurrences = boost::none;
    #pragma GCC diagnostic pop

    if (_recurrence.contains(END_KEY)) {
        auto endMap = _recurrence[END_KEY].toMap();
        if (endMap.contains(END_DATE_KEY) && endMap.contains(END_OCCURRENCES_KEY)) {
            LOG(WARNING) << "Recurrence cannot have an end date and a number of ocurrences";
            emit failure();
            return;
        }
        if (_recurrence.contains(END_DATE_KEY)) {
            endDate = endMap[END_DATE_KEY].toDate();
        } else if (_recurrence.contains(END_OCCURRENCES_KEY)) {
            occurrences = endMap[END_OCCURRENCES_KEY].toInt();
        }
    }

    if (_recurrence.contains(RECURRENCE_TYPE_KEY)) {
        auto qmlEnum = static_cast<qml::Book::RecurrenceType>(_recurrence[RECURRENCE_TYPE_KEY].toInt());
        com::chancho::RecurrentTransaction::Recurrence::Defaults defaults;
        switch (qmlEnum) {
            case qml::Book::DAILY:
                defaults = com::chancho::RecurrentTransaction::Recurrence::Defaults::DAILY;
                break;
            case qml::Book::WEEKLY:
                defaults = com::chancho::RecurrentTransaction::Recurrence::Defaults::WEEKLY;
                break;
            case qml::Book::MONTHLY:
                defaults = com::chancho::RecurrentTransaction::Recurrence::Defaults::MONTHLY;
                break;
            default:
                defaults = com::chancho::RecurrentTransaction::Recurrence::Defaults::OTHER;
                break;
        }
        if (endDate.isValid()) {
            recurrence = std::make_shared<com::chancho::RecurrentTransaction::Recurrence>(defaults, _date, endDate);
        } else if (occurrences) {
            recurrence = std::make_shared<com::chancho::RecurrentTransaction::Recurrence>(defaults, _date, occurrences);
        } else {
            recurrence = std::make_shared<com::chancho::RecurrentTransaction::Recurrence>(defaults, _date);
        }
    } else if (_recurrence.contains(RECURRENCE_DAYS_KEY)) {
        auto days = _recurrence[RECURRENCE_DAYS_KEY].toInt();
        if (endDate.isValid()) {
            recurrence = std::make_shared<com::chancho::RecurrentTransaction::Recurrence>(days, _date, endDate);
        } else if (occurrences) {
            recurrence = std::make_shared<com::chancho::RecurrentTransaction::Recurrence>(days, _date, occurrences);
        } else {
            recurrence = std::make_shared<com::chancho::RecurrentTransaction::Recurrence>(days, _date);
        }
    } else {
        LOG(WARNING) << "Recurrence type is missing.";
        emit failure();
        return;
    }

    auto recurrent = std::make_shared<com::chancho::RecurrentTransaction>(tran, recurrence);

    // store the transaction AND generate which ever new transaction that has to be added
    _book->store(recurrent);
    DLOG(INFO) << "Store the recurrent transaction";
    if (_book->isError()) {
        emit failure();
        return;;
    }

    _book->generateRecurrentTransactions();
    DLOG(INFO) << "Generate the needed transactions.";
    if (_book->isError()) {
        emit failure();
        return;
    }
    emit success();
}

}
}
}
}
}
