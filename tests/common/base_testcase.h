/*
 * Copyright (c) 2014 Manuel de la Peña <mandel@themacaque.com>
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

#include <QEventLoop>
#include <QSignalSpy>
#include <QTest>
#include <QTimer>
#include <QObject>

// util methods used to minimize the issues with signals in diff platforms
static inline bool waitForSignal(const QObject* obj, const char* signal, int timeout = -1) {
    QEventLoop loop;
    QObject::connect(obj, signal, &loop, SLOT(quit()));
    QTimer timer;
    QSignalSpy timeoutSpy(&timer, SIGNAL(timeout()));
    if (timeout > 0) {
        QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
        timer.setSingleShot(true);
        timer.start(timeout);
    }
    loop.exec();
    return timeoutSpy.isEmpty();
}

class SignalBarrier : public QSignalSpy {
 public:
    SignalBarrier(const QObject* obj, const char* aSignal)
        : QSignalSpy(obj, aSignal),
          _obj(obj),
          _signal(aSignal) { }

    bool ensureSignalEmitted() {
        bool result = count() > 0;
        if (!result) {
            result = waitForSignal(_obj, _signal, -1);
        }
        return result;
    }
 private:
    const QObject* _obj;
    const char* _signal;
};

class BaseTestCase : public QObject {
    Q_OBJECT

 public:
    BaseTestCase(const QString& testName, QObject *parent = 0);

    QString testDirectory();
    QString dataDirectory();

 protected slots:  // NOLINT(whitespace/indent)

    virtual void init();
    virtual void cleanup();

 protected:
    bool removeDir(const QString& dirName);
};
