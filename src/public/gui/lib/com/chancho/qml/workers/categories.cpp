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

#include <glog/logging.h>

#include "categories.h"

namespace com {

namespace chancho {

namespace qml {

namespace workers {

namespace categories {

#if QT_VERSION < 0x050300

class MultiStoreExecutor : public QObject {
 public:
    MultiStoreExecutor(qml::Book* book, WorkerThread<MultiStore>* worker, QObject* parent=0)
        : QObject(parent),
          _book(book),
          _worker(worker) {
    }

    void execute() {
        if (_worker->implementation()->wasIncome()) {
            emit _book->categoryStored(qml::Book::TransactionType::INCOME);
        }
        if (_worker->implementation()->wasExpense()) {
            emit _book->categoryStored(qml::Book::TransactionType::EXPENSE);
        }
        deleteLater();
    }

 private:
    qml::Book* _book;
    WorkerThread<MultiStore>* _worker;
};

class SingleStoreExecutor : public QObject {
 public:
    SingleStoreExecutor(qml::Book* book, qml::Book::TransactionType type, QObject* parent=0)
        : QObject(parent),
          _book(book),
          _type(type) {

    }

    void execute() {
        emit _book->categoryStored(_type);
        deleteLater();
    }

 private:
    qml::Book* _book;
    qml::Book::TransactionType _type;
};

class SingleRemoveExecutor : public QObject {
 public:
    SingleRemoveExecutor(qml::Book* book, com::chancho::CategoryPtr category, QObject* parent=0)
        : QObject(parent),
          _book(book),
          _category(category) {

    }

    void execute() {
        if (_category->type == chancho::Category::Type::INCOME) {
            emit _book->categoryRemoved(qml::Book::TransactionType::INCOME);
        } else {
            emit _book->categoryRemoved(qml::Book::TransactionType::EXPENSE);
        }
        deleteLater();
    }

 private:
    qml::Book* _book;
    com::chancho::CategoryPtr _category;
};

class SingleUpdateExecutor : public QObject {
 public:
    SingleUpdateExecutor(qml::Book* book, com::chancho::Category::Type oldType, qml::Book::TransactionType newType,
                         QObject* parent=0)
        : QObject(parent),
          _book(book),
          _oldType(oldType),
          _newType((newType == qml::Book::EXPENSE)?
                   com::chancho::Category::Type::EXPENSE: com::chancho::Category::Type::INCOME){
    }

    void execute() {
        auto typeChanged = _newType != _oldType;
        if (typeChanged) {
            LOG(INFO) << "Type updated!";
            emit _book->categoryTypeUpdated();
        } else {
            emit _book->categoryUpdated((_newType == com::chancho::Category::Type::EXPENSE)?
                qml::Book::EXPENSE: qml::Book::INCOME);
        }
        deleteLater();
    }
 private:
    qml::Book* _book;
    com::chancho::Category::Type _oldType;
    com::chancho::Category::Type _newType;
};

#endif

WorkerThread<MultiStore>*
WorkerFactory::storeCategories(qml::Book* book, QVariantList categories) {
    auto worker = new WorkerThread<MultiStore>(new MultiStore(book->_book, categories));

#if QT_VERSION >= 0x050300

    CHECK(QObject::connect(worker->implementation(), &MultiStore::success, book, [book, worker](){
        if (worker->implementation()->wasIncome()) {
            emit book->categoryStored(qml::Book::TransactionType::INCOME);
        }
        if (worker->implementation()->wasExpense()) {
            emit book->categoryStored(qml::Book::TransactionType::EXPENSE);
        }
    })) << "Could not connect to the success signal";

#else

    auto executor = new MultiStoreExecutor(book, worker);
    CHECK(QObject::connect(worker->implementation(), &SingleStore::success,
                           executor, &MultiStoreExecutor::execute)) << "Could not connect executor";

#endif

    CHECK(QObject::connect(worker->thread(), &QThread::finished, worker, &QObject::deleteLater))
            << "Could not connect to the finished signal";
    // TODO: connect the failure signal
    return worker;
}

WorkerThread<SingleStore>*
WorkerFactory::storeCategory(qml::Book* book, QString name, QString color, qml::Book::TransactionType type) {
    auto worker = new WorkerThread<SingleStore>(new SingleStore(book->_book, name, color, type));
#if QT_VERSION >= 0x050300

    CHECK(QObject::connect(worker->implementation(), &SingleStore::success, book, [book, type](){
        emit book->categoryStored(type);
    })) << "Could not connect to the success signal";

#else

    auto executor = new SingleStoreExecutor(book, type);
    CHECK(QObject::connect(worker->implementation(), &SingleStore::success,
                           executor, &SingleStoreExecutor::execute)) << "Could not connect executor";

#endif

    CHECK(QObject::connect(worker->thread(), &QThread::finished, worker, &QObject::deleteLater))
            << "Could not connect to the finished signal";
    // TODO: connect the failure signal
    return worker;
}

WorkerThread<SingleRemove>*
WorkerFactory::removeCategory(qml::Book* book, com::chancho::CategoryPtr category) {
    auto worker = new WorkerThread<SingleRemove>(new SingleRemove(book->_book, category));
#if QT_VERSION >= 0x050300

    CHECK(QObject::connect(worker->implementation(), &SingleRemove::success, book, [book, category](){
        if (category->type == chancho::Category::Type::INCOME) {
            emit book->categoryRemoved(qml::Book::TransactionType::INCOME);
        } else {
            emit book->categoryRemoved(qml::Book::TransactionType::EXPENSE);
        }
    })) << "Could not connect to the success signal";

#else

    auto executor = new SingleRemoveExecutor(book, category);
    CHECK(QObject::connect(worker->implementation(), &SingleStore::success,
                           executor, &SingleRemoveExecutor::execute)) << "Could not connect executor";

#endif

    CHECK(QObject::connect(worker->thread(), &QThread::finished, worker, &QObject::deleteLater))
        << "Could not connect to the finished signal";
    // TODO: connect the failure signal
    return worker;
}

WorkerThread<SingleUpdate>*
WorkerFactory::updateCategory(qml::Book* book, com::chancho::CategoryPtr category, QString name, QString color,
                              qml::Book::TransactionType type) {
    auto worker = new WorkerThread<SingleUpdate>(new SingleUpdate(book->_book, category, name, color, type));
    auto oldType = category->type;

#if QT_VERSION >= 0x050300
    CHECK(QObject::connect(worker->implementation(), &SingleRemove::success, book, [book, oldType, type](){
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
    })) << "Could not connect to the success signal";
#else

    auto executor = new SingleUpdateExecutor(book, oldType, type);
    CHECK(QObject::connect(worker->implementation(), &SingleStore::success,
                           executor, &SingleUpdateExecutor::execute)) << "Could not connect executor";

#endif

    CHECK(QObject::connect(worker->thread(), &QThread::finished, worker, &QObject::deleteLater))
            << "Could not connect to the finished signal";
    // TODO: connect the failure signal
    return worker;
}

}
}
}
}
}
