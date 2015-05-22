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

#include <com/chancho/transaction.h>

namespace chancho = com::chancho;

class PublicTransaction : public chancho::Transaction {
 public:
    PublicTransaction()
            : chancho::Transaction() {}

    PublicTransaction(const chancho::AccountPtr& acc,
                      double amount,
                      const chancho::CategoryPtr& cat,
                      const QString& cont=QString::null,
                      const QString& memo=QString::null)
            : chancho::Transaction(acc, amount, cat, cont, memo) {}

    PublicTransaction(const chancho::AccountPtr& acc,
                      double amount,
                      const chancho::CategoryPtr& cat,
                      const QDate& date,
                      const QString& cont=QString::null,
                      const QString& memo=QString::null)
            : chancho::Transaction(acc, amount, cat, date, cont, memo) {}

    virtual ~PublicTransaction() = default;

    using chancho::Transaction::_dbId;
};

typedef std::shared_ptr<PublicTransaction> PublicTransactionPtr;

Q_DECLARE_METATYPE(std::shared_ptr<PublicTransaction>)
