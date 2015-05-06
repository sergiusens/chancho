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

#include "accounts.h"

namespace com {

namespace chancho {

namespace qml {

namespace workers {

namespace accounts {

WorkerThread<SingleStore>*
WorkerFactory::storeAccount(qml::Book* book, QString name, QString memo, QString color, double initialAmount) {
    auto worker = new WorkerThread<SingleStore>(new SingleStore(book->_book, name, memo, color, initialAmount));
    QObject::connect(worker->implementation(), &SingleStore::success, book, &Book::accountStored);
    QObject::connect(worker->thread(), &QThread::finished, worker, &QObject::deleteLater);
    // TODO: connect the failure signal
    return worker;
}

WorkerThread<MultiStore>*
WorkerFactory::storeAccounts(qml::Book* book, QVariantList accounts) {
    auto worker = new WorkerThread<MultiStore>(new MultiStore(book->_book, accounts));
    QObject::connect(worker->implementation(), &MultiStore::success, book, &Book::accountStored);
    QObject::connect(worker->thread(), &QThread::finished, worker, &QObject::deleteLater);
    // TODO: connect the failure signal
    return worker;
}

WorkerThread<SingleRemove>*
WorkerFactory::removeAccount(qml::Book* book, com::chancho::AccountPtr account) {
    auto worker = new WorkerThread<SingleRemove>(new SingleRemove(book->_book, account));
    QObject::connect(worker->implementation(), &SingleRemove::success, book, &Book::accountRemoved);
    QObject::connect(worker->thread(), &QThread::finished, worker, &QObject::deleteLater);
    // TODO: connect the failure signal
    return worker;
}

WorkerThread<SingleUpdate>*
WorkerFactory::updateAccount(qml::Book* book, com::chancho::AccountPtr account, QString name, QString memo,
                             QString color) {
    auto worker = new WorkerThread<SingleUpdate>(new SingleUpdate(book->_book, account, name, memo, color));
    QObject::connect(worker->implementation(), &SingleUpdate::success, book, &Book::accountUpdated);
    QObject::connect(worker->thread(), &QThread::finished, worker, &QObject::deleteLater);
    // TODO: connect the failure signal
    return worker;
}

}
}
}
}
}
