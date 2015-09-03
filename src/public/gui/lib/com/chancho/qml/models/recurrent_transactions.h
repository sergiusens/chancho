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

#include <QAbstractListModel>
#include <QModelIndex>

#include <com/chancho/book.h>
#include <com/chancho/category.h>
#include "com/chancho/qml/category.h"

namespace com {

namespace chancho {

namespace qml {

class Book;

namespace models {

class RecurrentTransactions : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(QObject* category READ getCategory NOTIFY categoryChanged)

    friend class com::chancho::qml::Book;

 public:
    explicit RecurrentTransactions(QObject* parent = 0);
    RecurrentTransactions(qml::Category* cat, QObject* parent = 0);
    virtual ~RecurrentTransactions();

    // methods to override to allow the model to be used from qml
    Q_INVOKABLE int numberOfTransactions() const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    QVariant data(int row, int role) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE QObject* getCategory() const;

 signals:
    void categoryChanged(QObject* day);

 protected:
    RecurrentTransactions(BookPtr book, QObject* parent = 0);
    RecurrentTransactions(qml::Category* cat, BookPtr book, QObject* parent = 0);

 private:
    CategoryPtr _cat;
    BookPtr _book;

};

}

}

}

}

