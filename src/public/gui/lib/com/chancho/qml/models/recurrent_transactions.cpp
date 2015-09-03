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

#include "com/chancho/qml/recurrent_transaction.h"
#include "recurrent_transactions.h"

namespace com {

namespace chancho {

namespace qml {

class Book;

namespace models {

RecurrentTransactions::RecurrentTransactions(QObject* parent)
    : RecurrentTransactions(std::make_shared<com::chancho::Book>(), parent) {
}

RecurrentTransactions::RecurrentTransactions(qml::Category* cat, QObject* parent)
    : RecurrentTransactions(cat, std::make_shared<com::chancho::Book>(), parent) {

}

RecurrentTransactions::RecurrentTransactions(BookPtr book, QObject* parent)
    : QAbstractListModel(parent),
      _book(book) {
}

RecurrentTransactions::RecurrentTransactions(qml::Category* cat, BookPtr book, QObject* parent)
        : QAbstractListModel(parent),
          _cat(cat->getCategory()),
          _book(book) {

}

RecurrentTransactions::~RecurrentTransactions() {
}

int
RecurrentTransactions::numberOfTransactions() const {
    int count;
    if (_cat) {
        count = _book->numberOfRecurrentTransactions(_cat);
    } else {
        count = _book->numberOfRecurrentTransactions();
    }
    if (_book->isError()) {
        return 0;
    }
    return count;
}

int
RecurrentTransactions::rowCount(const QModelIndex&) const {
    return numberOfTransactions();
}

QVariant
RecurrentTransactions::data(int row, int role) const {
    int count;
    if (_cat) {
        count = _book->numberOfRecurrentTransactions(_cat);
    } else {
        count = _book->numberOfRecurrentTransactions();
    }
    if (_book->isError()) {
        return QVariant();
    }

    if (row >= count) {
        DLOG(INFO) << "Querying data for to large index";
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        QList<RecurrentTransactionPtr> transactions;
        if (_cat) {
            transactions = _book->recurrentTransactions(_cat, 1, row);
        } else {
            transactions = _book->recurrentTransactions(1, row);
        }
        if (_book->isError()) {
            LOG(INFO) << "Error when getting data from the db" << _book->lastError().toStdString();
            return QVariant();
        }
        if (transactions.count() > 0) {
            auto model = new com::chancho::qml::RecurrentTransaction(transactions.at(0));
            return QVariant::fromValue(model);
        } else {
            DLOG(INFO) << "No transaction was found.";
            return QVariant();
        }
    } else {
        return QVariant();
    }
}

QVariant
RecurrentTransactions::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) {
        DLOG(INFO) << "Querying data for not valid index.";
        return QVariant();
    }
    return data(index.row(), role);
}

QVariant
RecurrentTransactions::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QString("Column %1").arg(section);
    else
        return QString("Row %1").arg(section);
}

QObject*
RecurrentTransactions::getCategory() const {
    if (_cat) {
        return new qml::Category(_cat);
    } else {
        return nullptr;
    }
}

}

}

}

}
