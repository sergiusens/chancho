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

#include "transactions.h"

namespace com {

namespace chancho {

namespace qml {

namespace workers {

namespace transactions {

WorkerThread<SingleStore>*
WorkerFactory::storeTransaction(qml::Book* book, chancho::AccountPtr account, chancho::CategoryPtr category,
                                QDate date, double amount, QString contents, QString memo) {

    auto worker = new WorkerThread<SingleStore>(new SingleStore(book->_book, account, category, date, amount, contents, memo));

    QObject::connect(worker->implementation(), &SingleStore::success, book, &Book::accountUpdated);
    // XXX: Are we leaking a handler here?
    QObject::connect(worker->implementation(), &SingleStore::success, book, [book, date]() {
        emit book->transactionStored(date);
    });
    QObject::connect(worker->thread(), &QThread::finished, worker, &QObject::deleteLater);
    // TODO: connect the failure signal
    return worker;
}

WorkerThread<SingleRemove>*
WorkerFactory::removeTransaction(qml::Book* book, chancho::TransactionPtr trans) {
    auto worker = new WorkerThread<SingleRemove>(new SingleRemove(book->_book, trans));
    auto date = trans->date;
    QObject::connect(worker->implementation(), &SingleRemove::success, book, &Book::accountUpdated);
    QObject::connect(worker->implementation(), &SingleRemove::success, book, [book, date]() {
        emit book->transactionRemoved(date);
    });
    QObject::connect(worker->thread(), &QThread::finished, worker, &QObject::deleteLater);
    return worker;
}

WorkerThread<SingleUpdate>*
WorkerFactory::updateTransaction(qml::Book* book, chancho::TransactionPtr trans, chancho::AccountPtr acc,
                                 chancho::CategoryPtr cat, QDate date, QString contents, QString memo, double amount) {
    auto oldDate = trans->date;
    auto worker = new WorkerThread<SingleUpdate>(new SingleUpdate(book->_book, trans, acc, cat, date, contents, memo, amount));
    QObject::connect(worker->implementation(), &SingleUpdate::success, book, &Book::accountUpdated);
    QObject::connect(worker->implementation(), &SingleUpdate::success, book, [book, oldDate, date]() {
        emit book->transactionUpdated(oldDate, date);
    });
    QObject::connect(worker->thread(), &QThread::finished, worker, &QObject::deleteLater);

    return worker;
}

WorkerThread<GenerateRecurrent>*
WorkerFactory::generateRecurrentTransactions(qml::Book* book) {
    auto worker = new WorkerThread<GenerateRecurrent>(new GenerateRecurrent(book->_book));
    QObject::connect(worker->thread(), &QThread::finished, worker, &QObject::deleteLater);
    return worker;
}

}
}
}
}
}
