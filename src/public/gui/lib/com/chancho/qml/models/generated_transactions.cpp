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

#include "com/chancho/qml/transaction.h"
#include "generated_transactions.h"

namespace com {

namespace chancho {

namespace qml {

class Book;

namespace models {

GeneratedTransactions::GeneratedTransactions(QObject* parent)
        : GeneratedTransactions(std::make_shared<com::chancho::Book>(), parent) {
}

GeneratedTransactions::GeneratedTransactions(qml::RecurrentTransaction* tran, QObject* parent)
        : GeneratedTransactions(tran, std::make_shared<com::chancho::Book>(), parent) {

}

GeneratedTransactions::GeneratedTransactions(BookPtr book, QObject* parent)
        : QAbstractListModel(parent),
          _book(book) {

}

GeneratedTransactions::GeneratedTransactions(qml::RecurrentTransaction* tran, BookPtr book, QObject* parent)
        : QAbstractListModel(parent),
          _tran(tran->getTransaction()),
          _book(book) {

}

GeneratedTransactions::~GeneratedTransactions() {

}

int
GeneratedTransactions::numberOfTransactions() const {
    if (_tran) {
        int count = _book->numberOfTransactions(_tran);
        if (_book->isError()) {
            return 0;
        }
        DLOG(INFO) << "Number of transactions " << count;
        return count;
    }
    DLOG(INFO) << "0 transactions";
    return 0;
}

int
GeneratedTransactions::rowCount(const QModelIndex&) const {
    return numberOfTransactions();
}

QVariant
GeneratedTransactions::data(int row, int role) const {
    if (_tran) {
        int count = _book->numberOfTransactions(_tran);

        if (_book->isError()) {
            return QVariant();
        }

        if (row >= count) {
            DLOG(INFO) << "Querying data for to large index";
            return QVariant();
        }

        if (role == Qt::DisplayRole) {
            auto transactions = _book->transactions(_tran, 1, row);
            if (_book->isError()) {
                LOG(INFO) << "Error when getting data from the db" << _book->lastError().toStdString();
                return QVariant();
            }
            if (transactions.count() > 0) {
                auto model = new com::chancho::qml::Transaction(transactions.at(0));
                return QVariant::fromValue(model);
            } else {
                DLOG(INFO) << "No transaction was found.";
                return QVariant();
            }
        } else {
            return QVariant();
        }
    } else {
        return QVariant();
    }
}

QVariant
GeneratedTransactions::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) {
        DLOG(INFO) << "Querying data for not valid index.";
        return QVariant();
    }
    return data(index.row(), role);
}

QVariant
GeneratedTransactions::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QString("Column %1").arg(section);
    else
        return QString("Row %1").arg(section);
}

QObject*
GeneratedTransactions::getRecurrentTransaction() {
    return new qml::RecurrentTransaction(_tran);
}

void
GeneratedTransactions::setRecurrentTransaction(QObject* recurrent) {
    auto reccurrentModel = qobject_cast<qml::RecurrentTransaction *>(recurrent);
    _tran = reccurrentModel->getTransaction();
    // let the system know we need to recalculate the transactions
    beginResetModel();
    endResetModel();
}

}

}

}

}
