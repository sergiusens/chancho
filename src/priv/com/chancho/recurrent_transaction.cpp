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

#include <glog/logging.h>

#include "recurrent_transaction.h"

namespace com {

namespace chancho {

RecurrentTransaction::Recurrence::Recurrence(int n, QDate s, QDate e)
    : startDate(s),
      endDate(e),
      _numberOfDays(n) {
}

RecurrentTransaction::Recurrence::Recurrence(int n, QDate s, boost::optional<int> o)
    : startDate(s),
      occurrences(o),
     _numberOfDays(n){

}

RecurrentTransaction::Recurrence::Recurrence(Recurrence::Defaults n, QDate s, QDate e)
    : startDate(s),
      endDate(e),
      _defaults(n) {
}

RecurrentTransaction::Recurrence::Recurrence(Recurrence::Defaults n, QDate s, boost::optional<int> o)
    : startDate(s),
      occurrences(o),
      _defaults(n) {

}

boost::optional<int>
RecurrentTransaction::Recurrence::numberOfDays() const {
    return _numberOfDays;
}

boost::optional<RecurrentTransaction::Recurrence::Defaults>
RecurrentTransaction::Recurrence::defaults() const {
    return _defaults;
}

int
RecurrentTransaction::Recurrence::ocurrencesPassed() {
    // simply, do the reverse of the generateMissingDate, we go from last to start
    int count = 0;
    QDate lastCalculated;

    if(lastGenerated.isValid()) {
        lastCalculated = QDate(lastGenerated);
    } else {
        lastCalculated = startDate;
    }

    if(_defaults) {
        while (lastCalculated > startDate) {
            switch (*_defaults) {
                case Recurrence::Defaults::DAILY:
                    DLOG(INFO) << "Daily increased";
                    lastCalculated = lastCalculated.addDays(-1);
                    break;
                case Recurrence::Defaults::WEEKLY:
                    DLOG(INFO) << "Weekly increased";
                    lastCalculated = lastCalculated.addDays(-7);
                    break;
                default:
                    DLOG(INFO) << "Monthly increased";
                    lastCalculated = lastCalculated.addMonths(-1);
                    break;
            }
            if (lastCalculated >= startDate) {
                count++;
            }
        }
    } else {
        while(lastCalculated > startDate) {
            lastCalculated = lastCalculated.addDays(*_numberOfDays * -1);
            if (lastCalculated >= startDate) {
                count++;
            }
        }
    }
    return count;
}

QList<QDate>
RecurrentTransaction::Recurrence::generateMissingDates() {
    QList<QDate> result;

    // calculate which dates should be used since the last time we had a recurrence created
    auto today = QDate::currentDate();
    if (endDate.isValid() && endDate < today) {
        LOG(INFO) << "Found end date " << endDate.day() << "/" << endDate.month() << "/" << endDate.year();
        today = endDate;
    }

    if (startDate > today) {
        LOG(INFO) << "Returning empty list because the start date is smaller than the current date";
        return result;
    }

    if (lastGenerated.isValid() && lastGenerated == today) {
        LOG(INFO) << "Returning empty list because we are up to date with all the occurrences";
        return result;
    }

    QDate lastCalculated;
    if(lastGenerated.isValid()) {
        lastCalculated = QDate(lastGenerated);
    } else {
        lastCalculated = startDate;
    }

    if(_defaults) {
        while (lastCalculated < today) {
            switch (*_defaults) {
                case Recurrence::Defaults::DAILY:
                    DLOG(INFO) << "Daily increased";
                    lastCalculated = lastCalculated.addDays(1);
                    break;
                case Recurrence::Defaults::WEEKLY:
                    DLOG(INFO) << "Weekly increased";
                    lastCalculated = lastCalculated.addDays(7);
                    break;
                default:
                    DLOG(INFO) << "Monthly increased";
                    lastCalculated = lastCalculated.addMonths(1);
                    break;
            }
            if (lastCalculated <= today) {
                DLOG(INFO) << "New date appended " << lastCalculated.day() << "/" << lastCalculated.month()
                    << "/" << lastCalculated.year();
                result.append(lastCalculated);
            }
        }
    } else {
        while(lastCalculated < today) {
            lastCalculated = lastCalculated.addDays(*_numberOfDays);
            if (lastCalculated <= today) {
                result.append(lastCalculated);
            }
        }
    }

    if (occurrences) {
        auto left = *occurrences - ocurrencesPassed();
        LOG(INFO) << "We have " << left << " occurrences left";
        if (left > 0) {
            return result.mid(0, left);
        } else {
            return QList<QDate>();
        }
    }

    return result;
}

RecurrentTransaction::RecurrentTransaction(TransactionPtr t, RecurrencePtr r)
    : transaction(t),
      recurrence(r) {
}

bool
RecurrentTransaction::wasStoredInDb() const {
    return !_dbId.isNull();
}

QUuid
RecurrentTransaction::initTransactionId() {
    return transaction->_dbId;
}

}

}
