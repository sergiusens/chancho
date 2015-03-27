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
#include "com/chancho/qml/book.h"

#include "categories.h"

namespace com {

namespace chancho {

namespace qml {

namespace models {

Categories::Categories(QObject* parent)
    : QAbstractListModel(parent),
      _book(std::make_shared<com::chancho::Book>()) {
}

Categories::Categories(qml::Book::TransactionType type, QObject* parent)
    : QAbstractListModel(parent),
      _type(type),
      _book(std::make_shared<com::chancho::Book>()) {
}

Categories::Categories(BookPtr book, QObject* parent)
    : QAbstractListModel(parent),
      _book(book) {
}

Categories::Categories(qml::Book::TransactionType type, BookPtr book, QObject* parent)
    : QAbstractListModel(parent),
      _type(type),
      _book(book) {
}

Categories::~Categories() {
}

int
Categories::rowCount(const QModelIndex&) const {
    // the parent is not really used
    // use the book to return the count of transactions for the given month
    int count = 0;
    if (_type) {
        if (_type == qml::Book::EXPENSE) {
            count = _book->numberOfCategories(com::chancho::Category::Type::EXPENSE);
        } else {
            count = _book->numberOfCategories(com::chancho::Category::Type::INCOME);
        }
    } else {
        count = _book->numberOfCategories();
    }

    if (_book->isError()) {
        return 0;
    }
    return count;
}

QVariant
Categories::data(int row, int role) const {
    LOG(INFO) << "Requesting data";
    if (_type) {
        LOG(INFO) << "Filtering per type";
    }
    auto count = _book->numberOfCategories();
    if (_book->isError()) {
        LOG(INFO) << "Error when getting data from the db" << _book->lastError().toStdString();
        return QVariant();
    }
    if (row > count) {
        LOG(INFO) << "Querying data for to large index";
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        auto type = boost::optional<chancho::Category::Type>();
        if (_type) {
            LOG(INFO) << "Filtering categories per type " << *_type;
            if (*_type == qml::Book::TransactionType::EXPENSE) {
                type = chancho::Category::Type::EXPENSE;
            } else {
                type = chancho::Category::Type::INCOME;
            }
        }
        auto cats = _book->categories(type, 1, row);
        if (_book->isError()) {
            LOG(INFO) << "Error when getting data from the db" << _book->lastError().toStdString();
            return QVariant();
        }
        if (cats.count() > 0) {
            LOG(INFO) << "Returning category " << cats.at(0)->name.toStdString();
            auto model = new qml::Category(cats.at(0));
            return QVariant::fromValue(model);
        } else {
            LOG(INFO) << "No category was found.";
            return QVariant();
        }
    } else {
        return QVariant();
    }
}

QVariant
Categories::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) {
        LOG(INFO) << "Querying data for not valid index.";
        return QVariant();
    }
    return data(index.row(), role);
}

QVariant
Categories::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QString("Column %1").arg(section);
    else
        return QString("Row %1").arg(section);
}

qml::Book::TransactionType
Categories::getType() const {
    if (_type) {
        return *_type;
    } else {
        return qml::Book::EXPENSE;
    }
}

void
Categories::setType(qml::Book::TransactionType type) {
    if (type != *_type) {
        // model will be reset because we are changing the type of categories that
        // will be shown and therefore a repaint is needed in all attached views
        beginResetModel();
        _type = type;
        emit typeChanged(*_type);
        // let views know they can query the model again for the repaint
        endResetModel();
    }
}

QVariant
Categories::get(int row) {
    return data(row, Qt::DisplayRole);
}

int
Categories::getIndex(QObject* category) {
    auto qmlCat = qobject_cast<qml::Category*>(category);
    if (qmlCat == nullptr) {
        return -1;
    }

    auto type = boost::optional<chancho::Category::Type>();

    if (_type) {
        if (*_type == qml::Book::TransactionType::EXPENSE) {
            type = chancho::Category::Type::EXPENSE;
        } else {
            type = chancho::Category::Type::INCOME;
        }
    }

    auto cats = _book->categories(type);

    if (_book->isError()) {
        LOG(INFO) << "Error when getting data from the db" << _book->lastError().toStdString();
        return -1;
    }

    if (cats.count() > 0 ){
        for(int index=0; index < cats.count(); index++) {
            if (cats.at(index) == qmlCat->getCategory()) {
                return index;
            }
        }
    }

    return -1;
}

}

}

}

}

