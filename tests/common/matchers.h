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

#include <gmock/gmock.h>

#include <QList>
#include <QMap>
#include <QString>

#include <com/chancho/account.h>

MATCHER_P(QStringEqual, value, "Returns if two QStrings are equal.") {
    auto argStr = static_cast<QString>(arg);
    auto valueStr = static_cast<QString>(value);
    return argStr == valueStr;
}


MATCHER_P(AccountEquals, value, "Returns if the two accounts are equal.") {
    return arg->name == value->name && arg->amount == value->amount
            && arg->initialAmount == value->initialAmount && arg->memo == value->memo
            && arg->color == value->color;
}

MATCHER_P(AccountListEquals, value, "Returns if the two list of accounts are equal using the account name as uuid.") {
    QMap<QString, com::chancho::AccountPtr> valueSorted;
    foreach(const com::chancho::AccountPtr& acc, value) {
        valueSorted[acc->name] = acc;
    }

    foreach(const com::chancho::AccountPtr& acc, arg) {
        if (valueSorted.contains(acc->name)) {
            auto expected = valueSorted[acc->name];
            if (acc->name != expected->name
                || acc->amount != expected->amount
                || acc->initialAmount != expected->initialAmount
                || acc->memo != expected->memo
                || acc->color != expected->color) {
                return false;
            }
        } else {
            return false;
        }
    }
    return true;
}

MATCHER_P(CategoryEquals, value, "Returns if the two categories are equal.") {
    return arg->name == value->name && arg->type == value->type && arg->color == value->color;
}

MATCHER_P(CategoryListEquals, value, "Returns if the two list of categories are equal using the account name as uuid.") {
    QMap<QString, com::chancho::CategoryPtr> valueSorted;
    foreach(const com::chancho::CategoryPtr& cat, value) {
        valueSorted[cat->name] = cat;
    }

    foreach(const com::chancho::CategoryPtr& cat, arg) {
        if (valueSorted.contains(cat->name)) {
            auto expected = valueSorted[cat->name];
            if (cat->name != cat->name
                || cat->type != expected->type
                || cat->color != expected->color) {
                return false;
            }
        } else {
            return false;
        }
    }
    return true;
}

MATCHER_P(TransactionEquals, value, "Returns if the two categories are equal.") {
    return arg->account->name == value->account->name && arg->amount == value->amount
           && arg->category->name == value->category->name && arg->date == value->date
           && arg->contents == value->contents && arg->memo == value->memo;
}

MATCHER_P(RecurrentTransactionEquals, value, "Returns if the two transactions are equal.") {
    auto argTran = arg->transaction;
    auto valueTran = value->transaction;
    auto transEqual = argTran->account->name == valueTran->account->name && argTran->amount == valueTran->amount
           && argTran->category->name == valueTran->category->name && argTran->date == valueTran->date
           && argTran->contents == valueTran->contents && argTran->memo == valueTran->memo;

    auto argRecurrence = arg->recurrence;
    auto valueRecurrence = value->recurrence;

    auto recurrenceEqual = argRecurrence->startDate == valueRecurrence->startDate
        && argRecurrence->lastGenerated  == valueRecurrence->lastGenerated
        && argRecurrence->endDate  == valueRecurrence->endDate
        && argRecurrence->occurrences == valueRecurrence->occurrences
        && argRecurrence->numberOfDays() == valueRecurrence->numberOfDays()
        && argRecurrence->defaults() == valueRecurrence->defaults();

    return transEqual && recurrenceEqual;
}
