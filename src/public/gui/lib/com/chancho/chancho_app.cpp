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

#include <QLibrary>
#include <QQuickView>
#include <QtQml>

#include <com/chancho/book.h>

#include "book_model.h"
#include "day_model.h"
#include "month_model.h"
#include "transaction_model.h"

#include "chancho_app.h"

namespace com {

namespace chancho {

ChanchoApp::ChanchoApp(int &argc, char **argv)
    : QGuiApplication(argc, argv) {
}

bool
ChanchoApp::setup() {

    /* The testability driver is only loaded by QApplication but not by
     * QGuiApplication.  However, QApplication depends on QWidget which would
     * add some unneeded overhead => Let's load the testability driver on our
     * own.
     */
    if (arguments().contains(QStringLiteral("-testability"))) {
        QLibrary testLib(QStringLiteral("qttestability"));
        if (testLib.load()) {
            typedef void (*TasInitialize)(void);
            TasInitialize initFunction =
                    (TasInitialize)testLib.resolve("qt_testability_init");
            if (initFunction) {
                initFunction();
            } else {
                LOG(INFO) << "Library qttestability resolve failed!";
            }
        } else {
            LOG(INFO) << "Library qttestability load failed!";
        }
    }

    // register all the diff types with the qml engine
    // register the cpp types used in qml
    auto bookProvider = [](QQmlEngine*, QJSEngine*) -> QObject* {
        Book::initDatabse();

        auto model = new BookModel();
        return model;
    };

    qmlRegisterSingletonType<BookModel>("com.chancho", 1, 0, "Book", bookProvider);
    qmlRegisterType<DayModel>("com.chancho", 1, 0, "DayModel");
    qmlRegisterType<MonthModel>("com.chancho", 1, 0, "Month");
    qmlRegisterType<TransactionModel>("com.chancho", 1, 0, "Transaction");

    return true;
}

}

}
