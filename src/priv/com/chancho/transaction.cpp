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

#include <QFile>
#include <QFileInfo>
#include <glog/logging.h>

#include "transaction.h"

namespace com {

namespace chancho {

Transaction::Attachment::Attachment(QString attachmentName, QByteArray attachmentData)
        : name(attachmentName),
          data(attachmentData),
          _dbId(QUuid::createUuid()){
}

bool
Transaction::Attachment::isValid() {
    return !_dbId.isNull();
}

std::shared_ptr<Transaction::Attachment>
Transaction::Attachment::fromFile(QString file) {
    QFileInfo info(file);
    if (info.exists()) {

#if QT_VERSION >= 0x050300
        QScopedPointer<QFile, QScopedPointerDeleteLater> fd(new QFile(file));
#else
        auto file = new QFile(file);
#endif
        fd->open(QFile::ReadOnly);
        if (fd->error() == QFile::NoError) {
            auto data = fd->readAll();
            fd->close();
#if QT_VERSION >= 0x050300
            fd->deleteLater();
#endif
            return std::make_shared<Attachment>(file, data);
        } else {
            LOG(WARNING) << "Could not open file.";
        }
    }
    return std::make_shared<Attachment>();
}

Transaction::Transaction(const AccountPtr& acc,
        double a,
        const CategoryPtr& cat,
        const QString& cont,
        const QString& m)
    : account(acc),
      amount(a),
      category(cat),
      date(QDate::currentDate()),
      contents(cont),
      memo(m) {
}

Transaction::Transaction(const AccountPtr& acc,
        double a,
        const CategoryPtr& cat,
        const QDate& d,
        const QString& cont,
        const QString& m)
    : account(acc),
      amount(a),
      category(cat),
      date(d),
      contents(cont),
      memo(m) {
}

Category::Type
Transaction::type() const {
    if (category) {
        return category->type;
    }
    return Category::Type::EXPENSE;
}

bool
Transaction::wasStoredInDb() const {
    return !_dbId.isNull();
}

QList<std::shared_ptr<Transaction::Attachment>>
Transaction::attachments() {
    return _attachments.values();
}

void
Transaction::attach(std::shared_ptr<Transaction::Attachment> attachment) {
    _attachments[attachment->_dbId.toString()] = attachment;
}

void
Transaction::attach(QString attachmentPath) {
    auto attachment = Attachment::fromFile(attachmentPath);
    if (attachment->isValid()) {
        _attachments[attachment->_dbId.toString()] = attachment;
    }
}

void
Transaction::detach(std::shared_ptr<Transaction::Attachment> attachment) {
    _attachments.remove(attachment->_dbId.toString());
}

void
Transaction::detach(QString attachmentId) {
    _attachments.remove(attachmentId);
}

}

}
