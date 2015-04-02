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

#include "com/chancho/qml/account.h"
#include "accounts.h"

namespace com {

namespace chancho {

namespace qml {

namespace models {

Accounts::Accounts(QObject* parent)
        : QAbstractListModel(parent),
          _book(std::make_shared<com::chancho::Book>()) {
}

Accounts::Accounts(BookPtr book, QObject* parent)
        : QAbstractListModel(parent),
          _book(book) {
}

Accounts::~Accounts() {
}

int
Accounts::rowCount(const QModelIndex&) const {
    // the parent is not really used
    // use the book to return the count of transactions for the given month
    auto count = _book->numberOfAccounts();
    if (_book->isError()) {
        return 0;
    }
    return count;
}

QVariant
Accounts::data(int row, int role) const {
    DLOG(INFO) << "Requesting data";
    auto count = _book->numberOfAccounts();
    if (_book->isError()) {
        LOG(INFO) << "Error when getting data from the db" << _book->lastError().toStdString();
        return QVariant();
    }
    if (row > count) {
        DLOG(INFO) << "Querying data for to large index";
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        auto accs = _book->accounts(1, row);
        if (_book->isError()) {
            LOG(INFO) << "Error when getting data from the db" << _book->lastError().toStdString();
            return QVariant();
        }
        if (accs.count() > 0) {
            DLOG(INFO) << "Returning account " << accs.at(0)->name.toStdString();
            auto model = new Account(accs.at(0));
            return QVariant::fromValue(model);
        } else {
            DLOG(INFO) << "No account was found.";
            return QVariant();
        }
    } else {
        return QVariant();
    }
}

QVariant
Accounts::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) {
        DLOG(INFO) << "Querying data for not valid index.";
        return QVariant();
    }
    return data(index.row(), role);
}

QVariant
Accounts::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QString("Column %1").arg(section);
    else
        return QString("Row %1").arg(section);
}

QVariant
Accounts::get(int row) {
    return data(row, Qt::DisplayRole);
}

int
Accounts::getIndex(QObject* account) {
    auto qmlAcc = qobject_cast<qml::Account*>(account);
    if (qmlAcc == nullptr) {
        return -1;
    }

    auto accs = _book->accounts();

    if (_book->isError()) {
        LOG(INFO) << "Error when getting data from the db" << _book->lastError().toStdString();
        return -1;
    }

    if (accs.count() > 0 ){
        for(int index=0; index < accs.count(); index++) {
            if (accs.at(index) == qmlAcc->getAccount()) {
                return index;
            }
        }
    }

    return -1;
}

int
Accounts::numberOfAccounts() const {
    auto result = _book->numberOfAccounts();
    if (_book->isError()) {
        return -1;
    }
    return result;
}

void
Accounts::onAccountStored() {
    beginResetModel();
    endResetModel();
}

void
Accounts::onAccountRemoved() {
    beginResetModel();
    endResetModel();
}

void
Accounts::onAccountUpdated() {
    beginResetModel();
    endResetModel();
}

}

}

}

}
