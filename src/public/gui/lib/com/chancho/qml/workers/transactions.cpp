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

#include "transactions.h"

namespace com {

namespace chancho {

namespace qml {

namespace workers {

namespace transactions {

// the LTS released used in the CI bot does not allow to use a new compiler meaning that we
// cannot pass lambdas that capture variables to the QObject::connect method, this class wrap the required
// data and allow to provide an empty slot
#if QT_VERSION < 0x050300

class StoreTransactionExecutor : public QObject {
 public:
    StoreTransactionExecutor(qml::Book* book, QDate date, QObject* parent=0)
        : QObject(parent),
          _book(book),
          _date(date) {

    };

    void execute() {
        emit _book->transactionStored(_date);
        deleteLater();
    }

 private:
    qml::Book* _book = nullptr;
    QDate _date;
};

class RemoveTransactionExecutor : public QObject {
 public:
    RemoveTransactionExecutor(qml::Book* book, QDate date, QObject* parent=0)
        : QObject(parent),
          _book(book),
          _date(date) {
    };

    void execute() {
        emit _book->transactionRemoved(_date);
        deleteLater();
    }

 private:
    qml::Book* _book = nullptr;
    QDate _date;
};

class UpdateTransactionExecutor : public QObject {
 public:
    UpdateTransactionExecutor(qml::Book* book, QDate oldDate, QDate newDate, QObject* parent=0)
            : QObject(parent),
              _book(book),
              _oldDate(oldDate),
              _newDate(newDate) {
    };

    void execute() {
        emit _book->transactionUpdated(_oldDate, _newDate);
        deleteLater();
    }

 private:
    qml::Book* _book = nullptr;
    QDate _oldDate;
    QDate _newDate;
};


#endif

WorkerThread<SingleStore>*
WorkerFactory::storeTransaction(qml::Book* book, chancho::AccountPtr account, chancho::CategoryPtr category,
                                QDate date, double amount, QString contents, QString memo, QVariantMap recurrence) {

    auto worker = new WorkerThread<SingleStore>(new SingleStore(book->_book, account, category, date, amount,
                                                                contents, memo, recurrence));

    CHECK(QObject::connect(worker->implementation(), &SingleStore::success, book, &Book::accountUpdated))
          << "Could not connect to the success signal!";
    // XXX: Are we leaking a handler here?
#if QT_VERSION >= 0x050300

    CHECK(QObject::connect(worker->implementation(), &SingleStore::success, book, [book, date]() {
        emit book->transactionStored(date);
    })) << "Could not connecto the success signal!";

#else

    auto executor = new StoreTransactionExecutor(book, date);
    CHECK(QObject::connect(worker->implementation(), &SingleStore::success,
        executor, &StoreTransactionExecutor::execute)) << "Could not connect executor";

#endif

    CHECK(QObject::connect(worker->thread(), &QThread::finished, worker, &QObject::deleteLater))
            << "Could not connect to the finished signal!";
    // TODO: connect the failure signal
    return worker;
}

WorkerThread<SingleRemove>*
WorkerFactory::removeTransaction(qml::Book* book, chancho::TransactionPtr trans) {
    auto worker = new WorkerThread<SingleRemove>(new SingleRemove(book->_book, trans));
    auto date = trans->date;
    QObject::connect(worker->implementation(), &SingleRemove::success, book, &Book::accountUpdated);
#if QT_VERSION >= 0x050300

    QObject::connect(worker->implementation(), &SingleRemove::success, book, [book, date]() {
        emit book->transactionRemoved(date);
    });

#else

    auto executor = new RemoveTransactionExecutor(book, date);
    CHECK(QObject::connect(worker->implementation(), &SingleStore::success,
        executor, &RemoveTransactionExecutor::execute)) << "Could not connect executor";

#endif
    QObject::connect(worker->thread(), &QThread::finished, worker, &QObject::deleteLater);
    return worker;
}

WorkerThread<SingleUpdate>*
WorkerFactory::updateTransaction(qml::Book* book, chancho::TransactionPtr trans, chancho::AccountPtr acc,
                                 chancho::CategoryPtr cat, QDate date, QString contents, QString memo, double amount) {
    auto oldDate = trans->date;
    auto worker = new WorkerThread<SingleUpdate>(new SingleUpdate(book->_book, trans, acc, cat, date, contents, memo, amount));
    QObject::connect(worker->implementation(), &SingleUpdate::success, book, &Book::accountUpdated);
#if QT_VERSION >= 0x050300

    QObject::connect(worker->implementation(), &SingleUpdate::success, book, [book, oldDate, date]() {
        emit book->transactionUpdated(oldDate, date);
    });

#else

    auto executor = new UpdateTransactionExecutor(book, oldDate, date);
    CHECK(QObject::connect(worker->implementation(), &SingleStore::success,
        executor, &UpdateTransactionExecutor::execute)) << "Could not connect executor";

#endif
    QObject::connect(worker->thread(), &QThread::finished, worker, &QObject::deleteLater);

    return worker;
}

WorkerThread<GenerateRecurrent>*
WorkerFactory::generateRecurrentTransactions(qml::Book* book) {
    auto worker = new WorkerThread<GenerateRecurrent>(new GenerateRecurrent(book->_book));
    QObject::connect(worker->implementation(), &SingleUpdate::success, book, &Book::recurrentTransactionsGenerated);
    QObject::connect(worker->thread(), &QThread::finished, worker, &QObject::deleteLater);
    return worker;
}

}
}
}
}
}
