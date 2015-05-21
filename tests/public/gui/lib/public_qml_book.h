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

#include <memory>

#include <com/chancho/qml/book.h>

namespace workers = com::chancho::qml::workers;

namespace com {

namespace chancho {

namespace tests {

class PublicBook : public com::chancho::qml::Book {
 public:

    PublicBook(com::chancho::BookPtr book, QObject* parent=0) : com::chancho::qml::Book(book, parent) {}
    PublicBook(com::chancho::BookPtr book, std::shared_ptr<workers::accounts::WorkerFactory> accounts,
               std::shared_ptr<workers::categories::WorkerFactory> categories,
               std::shared_ptr<workers::transactions::WorkerFactory> transactions,
               QObject* parent=0) : com::chancho::qml::Book(book, accounts, categories, transactions, parent) {}
};

}
}
}
