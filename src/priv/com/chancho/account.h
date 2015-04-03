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

#include <QMetaType>
#include <QString>
#include <QUuid>

namespace com {

namespace chancho {

class Account {
    friend class Book;
    friend class Stats;

 public:
    Account() = default;
    Account(const QString& n, double a=0, const QString& m=QString::null, const QString& c=QString::null);
    virtual ~Account() = default;

 public:
    QString name = QString::null;
    double amount = 0;
    double initialAmount = 0;
    QString memo = QString::null;
    QString color = QString::null;

    virtual bool wasStoredInDb() const;
    bool operator==(const Account& rhs);
    bool operator!=(const Account& rhs);

 protected:
    // optional so that we know if a category was added to the db or not
    QUuid _dbId;
};

typedef std::shared_ptr<Account> AccountPtr;

bool operator==(const AccountPtr& lhs, const AccountPtr& rhs);
bool operator!=(const AccountPtr& lhs, const AccountPtr& rhs);

}

}

Q_DECLARE_METATYPE(std::shared_ptr<com::chancho::Account>)
