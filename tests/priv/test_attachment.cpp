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

#include <QFile>
#include <QTextStream>
#include <QVariant>
#include "test_attachment.h"

namespace chancho = com::chancho;

void
TestAttachment::init() {
    BaseTestCase::init();

}

void
TestAttachment::cleanup() {
    BaseTestCase::cleanup();
}

void
TestAttachment::testDefaultConstructor() {
    auto attachment = std::make_shared<com::chancho::Transaction::Attachment>();
    QVERIFY(!attachment->isValid());
}

void
TestAttachment::testIsValid() {
    auto attachment = std::make_shared<com::chancho::Transaction::Attachment>();
    QVERIFY(!attachment->isValid());

    QString name("Attachment");
    QByteArray data;

    attachment = std::make_shared<com::chancho::Transaction::Attachment>(name, data);
    QVERIFY(attachment->isValid());
}

void
TestAttachment::testFromFile() {
    QFile file("out.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        QSKIP("Could not create the test file.");

    QTextStream out(&file);
    out << "The magic number is: " << 49 << "\n";
    out.flush();
    auto attachment = PublicAttachment::fromFile(file.fileName());
    QVERIFY(attachment->isValid());
    QVERIFY(attachment->data.size() != 0);
}

QTEST_MAIN(TestAttachment)
