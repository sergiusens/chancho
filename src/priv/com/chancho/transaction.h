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

#include <memory>

#include <QByteArray>
#include <QDate>
#include <QMap>
#include <QMetaType>
#include <QString>

#include "account.h"
#include "category.h"

namespace com {

namespace chancho {

class Transaction {

 friend class Book;
 friend class RecurrentTransaction;

 public:

    class Attachment {
        friend class Transaction;

     public:
        Attachment() = default;
        Attachment(QString attachmentName, QByteArray attachmentData);

        virtual ~Attachment() = default;

        virtual bool isValid();

        QString name = QString::null;
        QByteArray data = QByteArray();

     protected:
        static std::shared_ptr<Attachment> fromFile(QString file);

     protected:
        QUuid _dbId;
    };

    Transaction() = default;
    Transaction(const AccountPtr& acc,
            double amount,
            const CategoryPtr& cat,
            const QString& cont=QString::null,
            const QString& memo=QString::null);

    Transaction(const AccountPtr& acc,
            double amount,
            const CategoryPtr& cat,
            const QDate& date,
            const QString& cont=QString::null,
            const QString& memo=QString::null);
    virtual ~Transaction() = default;

    virtual Category::Type type() const;

 public:
    AccountPtr account = std::shared_ptr<Account>();
    double amount = 0;
    CategoryPtr category = std::shared_ptr<Category>();
    QDate date = QDate();
    QString contents = QString::null;
    QString memo = QString::null;
    bool is_recurrent = false;

    virtual bool wasStoredInDb() const;

    /*!
        \fn virtual QList<std::shared_ptr<Attachment>> attachments();

        Returns all the attachments related with this transaction.
     */
    virtual QList<std::shared_ptr<Attachment>> attachments();

    /*!
        \fn virtual void attach(std::shared_ptr<Attachment> attachment);

        Adds a new attachment to the transaction.
     */
    virtual void attach(std::shared_ptr<Attachment> attachment);

    /*!
        \fn virtual void attach(QString attachmentPath);

        Adds a new attachment to the transaction which contains the data of the provided path.
     */
    virtual void attach(QString attachmentPath);

    /*!
        \fn virtual void detach(std::shared_ptr<Attachment> attachment);

        Removes the given attachment from the transaction.
     */
    virtual void detach(std::shared_ptr<Attachment> attachment);

    /*!
        \fn virtual void detach(QString attachmentName);

        Removes the given attachment from the transaction with the given name.
     */
    virtual void detach(QString attachmentId);

 protected:
    // optional so that we know if a category was added to the db or not
    QUuid _dbId;
    QMap<QString, std::shared_ptr<Attachment>> _attachments;
};

typedef std::shared_ptr<Transaction> TransactionPtr;
typedef std::shared_ptr<Transaction::Attachment> AttachmentPtr;

}

}

Q_DECLARE_METATYPE(std::shared_ptr<com::chancho::Transaction>)
