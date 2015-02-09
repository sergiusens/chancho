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

#include <memory>

#include <QDate>
#include <QMetaType>
#include <QString>

#include "account.h"
#include "category.h"

namespace com {

namespace chancho {

class Transaction {

 public:
    Transaction() = default;
    Transaction(const AccountPtr& acc,
            double amount,
            const CategoryPtr& cat,
            const QString& cont=QString::null,
            const QString& memo=QString::null);

    Transaction(const AccountPtr& acc,
            double amount,
            const CategoryPtr& cat,
            const QDate& date,
            const QString& cont=QString::null,
            const QString& memo=QString::null);
    virtual ~Transaction() = default;

    virtual Category::Type type() const;

 public:
    AccountPtr account = std::shared_ptr<Account>();
    double amount = 0;
    CategoryPtr category = std::shared_ptr<Category>();
    QDate date = QDate();
    QString contents = QString::null;
    QString memo = QString::null;

    virtual bool wasStoredInDb() const;

 protected:
    // optional so that we know if a category was added to the db or not
    QUuid _dbId;

};

typedef std::shared_ptr<Category> TransactionPtr;

}

}

Q_DECLARE_METATYPE(std::shared_ptr<com::chancho::Transaction>)
