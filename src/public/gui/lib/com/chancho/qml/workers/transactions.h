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

#include "com/chancho/qml/book.h"
#include "com/chancho/qml/workers/worker_thread.h"

// make the include simpler
#include "transactions/generate_recurrent.h"
#include "transactions/single_remove.h"
#include "transactions/single_store.h"
#include "transactions/single_update.h"


namespace com {

namespace chancho {

namespace qml {

namespace workers {

namespace transactions {

class WorkerFactory {
 public:
    WorkerFactory() = default;
    virtual ~WorkerFactory() = default;

    virtual WorkerThread<SingleStore>* storeTransaction(qml::Book* book, chancho::AccountPtr account,
                                                        chancho::CategoryPtr category, QDate date, double amount,
                                                        QString contents, QString memo, QVariantMap recurrence);
    virtual WorkerThread<SingleRemove>* removeTransaction(qml::Book* book, chancho::TransactionPtr trans);
    virtual WorkerThread<SingleUpdate>* updateTransaction(qml::Book* book, chancho::TransactionPtr trans,
                                                          chancho::AccountPtr acc, chancho::CategoryPtr cat, QDate date,
                                                          QString contents, QString memo, double amount);
    virtual WorkerThread<GenerateRecurrent>* generateRecurrentTransactions(qml::Book* book);
};

}
}
}
}
}

