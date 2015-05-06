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

#include <QThread>

namespace com {

namespace chancho {

namespace qml {

namespace workers {

template<class T>
class WorkerThread : public QObject {

    template<class U>
    class InternalThread : public QThread {
     public:
        InternalThread(U* implementation) : _impl(implementation) { }

        ~InternalThread() {
            _impl->deleteLater();
            _impl = nullptr;
        }

        void run() override {
            _impl->run();
        }

        U* implementation() {
            return _impl;
        }

     private:
        U* _impl;
    };

 public:
    WorkerThread<T>(T* implementation) : _thread(new InternalThread<T>(implementation)) {
    }

    ~WorkerThread() {
        _thread->deleteLater();
    }

    virtual void start() {
        _thread->start();
    }

    virtual T* implementation() {
        return _thread->implementation();
    }

    virtual QThread* thread() {
        return _thread;
    }

 private:
    InternalThread<T>* _thread;
};

}
}
}
}

