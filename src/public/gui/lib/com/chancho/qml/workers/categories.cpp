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

#include "categories.h"

namespace com {

namespace chancho {

namespace qml {

namespace workers {

namespace categories {


WorkerThread<MultiStore>*
WorkerFactory::storeCategories(qml::Book* book, QVariantList categories) {
    auto worker = new WorkerThread<MultiStore>(new MultiStore(book->_book, categories));
    QObject::connect(worker->implementation(), &MultiStore::success, book, [book, worker](){
        if (worker->implementation()->wasIncome()) {
            emit book->categoryStored(qml::Book::TransactionType::INCOME);
        }
        if (worker->implementation()->wasExpense()) {
            emit book->categoryStored(qml::Book::TransactionType::EXPENSE);
        }
    });
    QObject::connect(worker->thread(), &QThread::finished, worker, &QObject::deleteLater);
    // TODO: connect the failure signal
    return worker;
}

WorkerThread<SingleStore>*
WorkerFactory::storeCategory(qml::Book* book, QString name, QString color, qml::Book::TransactionType type) {
    auto worker = new WorkerThread<SingleStore>(new SingleStore(book->_book, name, color, type));
    QObject::connect(worker->implementation(), &SingleStore::success, book, [book, type](){
        emit book->categoryStored(type);
    });
    QObject::connect(worker->thread(), &QThread::finished, worker, &QObject::deleteLater);
    // TODO: connect the failure signal
    return worker;
}

WorkerThread<SingleRemove>*
WorkerFactory::removeCategory(qml::Book* book, com::chancho::CategoryPtr category) {
    auto worker = new WorkerThread<SingleRemove>(new SingleRemove(book->_book, category));
    QObject::connect(worker->implementation(), &SingleRemove::success, book, [book, category](){
        if (category->type == chancho::Category::Type::INCOME) {
            emit book->categoryRemoved(qml::Book::TransactionType::INCOME);
        } else {
            emit book->categoryRemoved(qml::Book::TransactionType::EXPENSE);
        }
    });
    QObject::connect(worker->thread(), &QThread::finished, worker, &QObject::deleteLater);
    // TODO: connect the failure signal
    return worker;
}

WorkerThread<SingleUpdate>*
WorkerFactory::updateCategory(qml::Book* book, com::chancho::CategoryPtr category, QString name, QString color,
                              qml::Book::TransactionType type) {
    auto worker = new WorkerThread<SingleUpdate>(new SingleUpdate(book->_book, category, name, color, type));
    auto oldType = category->type;
    QObject::connect(worker->implementation(), &SingleRemove::success, book, [book, oldType, type](){
        com::chancho::Category::Type catType;
        if (type == qml::Book::TransactionType::EXPENSE) {
            catType = com::chancho::Category::Type::EXPENSE;
        } else {
            catType = com::chancho::Category::Type::INCOME;
        }
        auto typeChanged = catType != oldType;
        if (typeChanged) {
            LOG(INFO) << "Type updated!";
            emit book->categoryTypeUpdated();
        } else {
            emit book->categoryUpdated(type);
        }
    });
    QObject::connect(worker->thread(), &QThread::finished, worker, &QObject::deleteLater);
    // TODO: connect the failure signal
    return worker;
}

}
}
}
}
}
