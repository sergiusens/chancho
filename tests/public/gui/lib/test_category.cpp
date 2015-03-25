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

#include <QSignalSpy>

#include <com/chancho/qml/book.h>
#include <com/chancho/qml/category.h>
#include "test_category.h"


void
TestCategory::init() {
    BaseTestCase::init();
    qRegisterMetaType<com::chancho::qml::Book::TransactionType>("com::chancho::qml::Book::TransactionType");
}

void
TestCategory::cleanup() {
    BaseTestCase::cleanup();
}

void
TestCategory::testGetName() {
    QString name = "Bankia";
    auto type = com::chancho::qml::Book::EXPENSE;

    auto cat = std::make_shared<com::chancho::qml::Category>(name, type);
    QCOMPARE(cat->getName(), name);
}

void
TestCategory::testSetNameNoSignal() {
    QString name = "Bankia";
    auto type = com::chancho::qml::Book::EXPENSE;

    auto cat = std::make_shared<com::chancho::qml::Category>(name, type);
    QCOMPARE(cat->getName(), name);

    QSignalSpy spy(cat.get(), SIGNAL(nameChanged(QString)));
    cat->setName(name);
    QCOMPARE(spy.count(), 0);
}

void
TestCategory::testSetNameSignal() {
    QString name = "Bankia";
    auto type = com::chancho::qml::Book::EXPENSE;

    auto cat = std::make_shared<com::chancho::qml::Category>(name, type);
    QCOMPARE(cat->getName(), name);

    QSignalSpy spy(cat.get(), SIGNAL(nameChanged(QString)));
    cat->setName("New name");
    QCOMPARE(spy.count(), 1);
}

void
TestCategory::testGetType() {
    QString name = "Bankia";
    auto type = com::chancho::qml::Book::EXPENSE;

    auto cat = std::make_shared<com::chancho::qml::Category>(name, type);
    QCOMPARE(cat->getType(), type);
}

void
TestCategory::testSetTypeNoSignal() {
    QString name = "Bankia";
    auto type = com::chancho::qml::Book::EXPENSE;

    auto cat = std::make_shared<com::chancho::qml::Category>(name, type);
    QCOMPARE(cat->getType(), type);

    QSignalSpy spy(cat.get(), SIGNAL(typeChanged(com::chancho::qml::Book::TransactionType)));
    cat->setType(type);
    QCOMPARE(spy.count(), 0);
}

void
TestCategory::testSetTypeSignal() {
    QString name = "Bankia";
    auto type = com::chancho::qml::Book::EXPENSE;

    auto cat = std::make_shared<com::chancho::qml::Category>(name, type);
    QCOMPARE(cat->getType(), type);

    QSignalSpy spy(cat.get(), SIGNAL(typeChanged(com::chancho::qml::Book::TransactionType)));
    cat->setType(com::chancho::qml::Book::INCOME);
    QCOMPARE(spy.count(), 1);
}

QTEST_MAIN(TestCategory)

