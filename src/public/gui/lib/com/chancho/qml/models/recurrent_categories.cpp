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

#include "com/chancho/qml/category.h"

#include "recurrent_categories.h"

namespace com {

namespace chancho {

namespace qml {

class Book;

namespace models {

RecurrentCategories::RecurrentCategories(QObject* parent)
        : RecurrentCategories(std::make_shared<com::chancho::Book>(), parent) {
}

RecurrentCategories::RecurrentCategories(BookPtr book, QObject* parent)
        : QAbstractListModel(parent),
          _book(book) {
}

RecurrentCategories::~RecurrentCategories() {
}

int
RecurrentCategories::numberOfCategories() const {
    auto count = _book->numberOfRecurrentCategories();
    if (_book->isError()) {
        return 0;
    }
    return count;
}

int
RecurrentCategories::rowCount(const QModelIndex&) const {
    return numberOfCategories();
}

QVariant
RecurrentCategories::data(int row, int role) const {
    auto count = _book->numberOfRecurrentCategories();

    if (_book->isError()) {
        return QVariant();
    }

    if (row >= count) {
        DLOG(INFO) << "Querying data for to large index";
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        QList<CategoryPtr> categories = _book->recurrentCategories(1, row);
        if (_book->isError()) {
            LOG(INFO) << "Error when getting data from the db" << _book->lastError().toStdString();
            return QVariant();
        }
        if (categories.count() > 0) {
            auto model = new com::chancho::qml::Category(categories.at(0));
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
RecurrentCategories::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) {
        DLOG(INFO) << "Querying data for not valid index.";
        return QVariant();
    }
    return data(index.row(), role);
}

QVariant
RecurrentCategories::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QString("Column %1").arg(section);
    else
        return QString("Row %1").arg(section);
}

int
RecurrentCategories::getCount() const {
    auto count = _book->numberOfRecurrentCategories();
    return count;
}

void
RecurrentCategories::onRecurrentTransactionUpdated() {
    beginResetModel();
    endResetModel();
}

void
RecurrentCategories::onRecurrentTransactionRemoved() {
    beginResetModel();
    endResetModel();
}

}

}

}

}
