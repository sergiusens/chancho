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

#include <com/chancho/stats.h>

#include "models/accounts.h"
#include "models/categories.h"
#include "models/day.h"
#include "models/generated_transactions.h"
#include "models/month.h"
#include "models/recurrent_categories.h"
#include "models/recurrent_transactions.h"

#include "workers/accounts.h"
#include "workers/categories.h"
#include "workers/transactions.h"

#include "account.h"
#include "transaction.h"
#include "recurrent_transaction.h"

#include "book.h"

namespace com {

namespace chancho {

namespace qml {

Book::Book(QObject* parent)
    : Book(std::make_shared<com::chancho::Book>(), parent) {
}

Book::Book(BookPtr book, QObject* parent)
    : Book(book, std::make_shared<workers::accounts::WorkerFactory>(),
           std::make_shared<workers::categories::WorkerFactory>(),
           std::make_shared<workers::transactions::WorkerFactory>(),
           parent){

}

Book::Book(BookPtr book, std::shared_ptr<workers::accounts::WorkerFactory> accounts,
           std::shared_ptr<workers::categories::WorkerFactory> categories,
           std::shared_ptr<workers::transactions::WorkerFactory> transactions,
           QObject* parent)
    : QObject(parent),
     _accountWorkersFactory(accounts),
     _categoryWorkersFactory(categories),
     _transactionWorkersFactory(transactions),
      _book(book) {
    qRegisterMetaType<Book::TransactionType>("Book::TransactionType");
}

QObject*
Book::accountsModel() {
    auto model = new models::Accounts(_book);
    connect(this, &Book::accountStored, model, &models::Accounts::onAccountStored);
    connect(this, &Book::accountRemoved, model, &models::Accounts::onAccountRemoved);
    connect(this, &Book::accountUpdated, model, &models::Accounts::onAccountUpdated);
    connect(this, &Book::categoryTypeUpdated, model, &models::Accounts::onCategoryTypeUpdated);

    return model;
}

QVariantList
Book::accounts() {
    QVariantList result;
    auto accs = _book->accounts();

    if (_book->isError()) {
        return result;
    }

    foreach(const com::chancho::AccountPtr& acc, accs) {
        auto qmlAcc = new com::chancho::qml::Account(acc);
        result.append(QVariant::fromValue(qmlAcc));
    }
    return result;
}

QVariantList
Book::monthsTotalForAccount(QObject* acc, int year) {
    QVariantList result;
    auto stats = _book->stats();
    auto qmlAcc = qobject_cast<qml::Account *>(acc);
    if (qmlAcc == nullptr) {
        return result;
    }
    auto doubleList = stats->monthsTotalForAccount(qmlAcc->getAccount(), year);
    foreach(double amount, doubleList) {
        result.append(QVariant(amount));
    }

    return result;
}

bool
Book::storeAccount(QString name, QString memo, QString color, double initialAmount) {
    auto worker = _accountWorkersFactory->storeAccount(this, name, memo, color, initialAmount);
    worker->start();
    return true;
}

bool
Book::storeAccounts(QVariantList accounts) {
    auto worker = _accountWorkersFactory->storeAccounts(this, accounts);
    worker->start();
    return true;
}

bool
Book::removeAccount(QObject* account) {
    auto acc = qobject_cast<qml::Account*>(account);
    if (acc == nullptr) {
        return false;
    }

    auto worker = _accountWorkersFactory->removeAccount(this, acc->getAccount());
    worker->start();
    return true;
}

bool
Book::updateAccount(QObject* account, QString name, QString memo, QString color) {
    auto acc = qobject_cast<qml::Account*>(account);
    if (acc == nullptr) {
        return false;
    }
    auto worker = _accountWorkersFactory->updateAccount(this, acc->getAccount(), name, memo, color );
    worker->start();
    return true;
}

void
Book::generateRecurrentTransactions() {
    auto worker = _transactionWorkersFactory->generateRecurrentTransactions(this);
    worker->start();  // the factory makes sure that when the thread is done the resource is cleaned
}

bool
Book::storeTransaction(QObject* account, QObject* category, QDate date, double amount, QString contents,
        QString memo, QVariantMap recurrence) {
    auto acc = qobject_cast<qml::Account*>(account);
    if (acc == nullptr) {
        return false;
    }

    auto cat = qobject_cast<qml::Category*>(category);
    if (cat == nullptr) {
        return false;
    }

    DLOG(INFO) << __PRETTY_FUNCTION__ << " " << acc->getMemo().toStdString() << " " << cat->getName().toStdString()
        << " " << amount << " " << contents.toStdString() << " " << memo.toStdString();
    auto worker = _transactionWorkersFactory->storeTransaction(this, acc->getAccount(), cat->getCategory(), date,
                                                               amount, contents, memo, recurrence);
    worker->start();
    return true;
}

bool
Book::removeTransaction(QObject* transaction) {
    auto tran = qobject_cast<qml::Transaction*>(transaction);
    if (tran == nullptr) {
        return false;
    }
    auto worker = _transactionWorkersFactory->removeTransaction(this, tran->getTransaction());
    worker->start();
    return true;
}

bool
Book::updateTransaction(QObject* tranObj, QObject* accObj, QObject* catObj, QDate date,
                        QString contents, QString memo, double amount) {
    TransactionPtr tran;
    AccountPtr acc;
    CategoryPtr cat;

    auto tranModel = qobject_cast<qml::Transaction*>(tranObj);
    if (tranModel != nullptr) {
        tran = tranModel->getTransaction();
    } else {
        LOG(ERROR) << "Method called with wrong object type as a transaction model";
        return false;
    }

    auto accModel = qobject_cast<qml::Account*>(accObj);
    if (accModel != nullptr) {
        acc = accModel->getAccount();
    } else {
        LOG(ERROR) << "Method called with wrong object type as an account model";
        return false;
    }

    auto catModel = qobject_cast<qml::Category*>(catObj);
    if (catModel != nullptr) {
        cat = catModel->getCategory();
    } else {
        LOG(ERROR) << "Method called with wrong object type as a category model";
        return false;
    }

    auto worker = _transactionWorkersFactory->updateTransaction(this, tran, acc, cat, date, contents, memo, amount);
    worker->start();
    return true;
}

QObject*
Book::recurrentTransactionsModel(QObject* category) {
    CategoryPtr cat;

    auto catModel = qobject_cast<qml::Category*>(category);
    if (catModel == nullptr) {
        LOG(ERROR) << "Method called with wrong object type as a category model";
        return nullptr;
    }
    auto model = new models::RecurrentTransactions(catModel, _book);
    return model;
}

QObject*
Book::recurrentCategoriesModel() {
    auto model = new models::RecurrentCategories(_book);
    connect(this, &Book::recurrentTransactionUpdated, model,
            &models::RecurrentCategories::onRecurrentTransactionUpdated);
    connect(this, &Book::recurrentTransactionRemoved, model,
            &models::RecurrentCategories::onRecurrentTransactionRemoved);
    return model;
}

QObject*
Book::generatedTransactions(QObject* recurrentTransactionObj) {
    if (recurrentTransactionObj != nullptr) {
        auto reccurrentModel = qobject_cast<qml::RecurrentTransaction *>(recurrentTransactionObj);
        if (reccurrentModel == nullptr) {
            auto model = new models::GeneratedTransactions(reccurrentModel, _book);
            return model;
        }
    }
    LOG(INFO) << "Recurrent tran is null";
    return new models::GeneratedTransactions(new qml::RecurrentTransaction(), _book);
}

bool
Book::updateRecurrentTransaction(QObject* recurrent, QObject* accObj, QObject* catObj, QDate date,
                                 QString contents, QString memo, double amount, bool updateAll) {
    RecurrentTransactionPtr tran;
    AccountPtr acc;
    CategoryPtr cat;

    auto tranModel = qobject_cast<qml::RecurrentTransaction*>(recurrent);
    if (tranModel != nullptr) {
        tran = tranModel->getTransaction();
    } else {
        LOG(ERROR) << "Method called with wrong object type as a transaction model";
        return false;
    }

    auto accModel = qobject_cast<qml::Account*>(accObj);
    if (accModel != nullptr) {
        acc = accModel->getAccount();
    } else {
        LOG(ERROR) << "Method called with wrong object type as an account model";
        return false;
    }

    auto catModel = qobject_cast<qml::Category*>(catObj);
    if (catModel != nullptr) {
        cat = catModel->getCategory();
    } else {
        LOG(ERROR) << "Method called with wrong object type as a category model";
        return false;
    }

    auto worker = _transactionWorkersFactory->updateTransaction(this, tran, acc, cat, date, contents, memo, amount,
                                                                updateAll);
    LOG(INFO) << "Starting worker thread to do the update.";
    worker->start();
    return true;
}

bool
Book::removeRecurrentTransaction(QObject* transaction, bool removeGenerated) {
    auto tran = qobject_cast<qml::RecurrentTransaction*>(transaction);
    if (tran == nullptr) {
        return false;
    }
    auto worker = _transactionWorkersFactory->removeTransaction(this, tran->getTransaction(), removeGenerated);
    worker->start();
    return true;
}

int
Book::numberOfCategories(TransactionType type) {
    boost::optional<com::chancho::Category::Type> chanchoType;
    if (type == TransactionType::EXPENSE) {
        chanchoType = com::chancho::Category::Type::EXPENSE;
    } else {
        chanchoType = com::chancho::Category::Type::INCOME;
    }
    auto number = _book->numberOfCategories(chanchoType);
    if (_book->isError()) {
        return -1;
    }
    return number;
}

QObject*
Book::categoriesModel() {
    auto model = new models::Categories(qml::Book::TransactionType::EXPENSE,  _book);
    connect(this, &Book::categoryStored, model, &models::Categories::onCategoryStored);
    connect(this, &Book::categoryUpdated, model, &models::Categories::onCategoryUpdated);
    connect(this, &Book::categoryRemoved, model, &models::Categories::onCategoryRemoved);
    connect(this, &Book::categoryTypeUpdated, model, &models::Categories::onCategoryTypeUpdated);

    void categoryTypeUpdated();
    return model;
}

QObject*
Book::categoriesModelForType(TransactionType type) {
    auto model = new models::Categories(type,  _book);
    connect(this, &Book::categoryStored, model, &models::Categories::onCategoryStored);
    connect(this, &Book::categoryUpdated, model, &models::Categories::onCategoryUpdated);
    connect(this, &Book::categoryRemoved, model, &models::Categories::onCategoryRemoved);
    connect(this, &Book::categoryTypeUpdated, model, &models::Categories::onCategoryTypeUpdated);
    return model;
}

QVariantList
Book::categoryPercentagesForMonth(int month, int year) {
    QVariantList result;
    auto stats = _book->stats();
    auto statsForMonth = stats->categoryPercentages(month, year);
    foreach(const com::chancho::Stats::CategoryPercentage info, statsForMonth.second) {
        // create a variant map that will contains the category and the value
        QVariantMap map;
        map["category"] = QVariant::fromValue(new com::chancho::qml::Category(info.category));
        map["amount"] = (info.amount < 0)? -1 * info.amount: info.amount;
        result.append(map);
    }

    return result;
}

QVariantList
Book::monthsTotalForCategory(QObject* category, int year) {
    QVariantList result;
    auto stats = _book->stats();
    auto qmlCat = qobject_cast<qml::Category*>(category);
    if (qmlCat == nullptr) {
        return result;
    }
    auto doubleList = stats->monthsTotalForCategory(qmlCat->getCategory(), year);
    foreach(double amount, doubleList) {
        result.append(QVariant(amount));
    }

    return result;
}

bool
Book::storeCategory(QString name, QString color, Book::TransactionType type) {
    auto worker = _categoryWorkersFactory->storeCategory(this, name, color, type);
    worker->start();
    return true;
}

bool
Book::storeCategories(QVariantList categories) {
    auto worker = _categoryWorkersFactory->storeCategories(this, categories);
    worker->start();
    return true;
}

bool
Book::updateCategory(QObject* category, QString name, QString color, Book::TransactionType type) {
    auto catModel = qobject_cast<qml::Category*>(category);
    if (catModel == nullptr) {
        return false;
    }
    auto cat = catModel->getCategory();
    auto worker = _categoryWorkersFactory->updateCategory(this, cat, name, color, type);
    worker->start();
    return true;
}

bool
Book::removeCategory(QObject* category) {
    auto catModel = qobject_cast<qml::Category*>(category);
    if (catModel == nullptr) {
        return false;
    }
    auto worker = _categoryWorkersFactory->removeCategory(this, catModel->getCategory());
    worker->start();
    return true;
}

QObject*
Book::dayModel(int day, int month, int year) {
    return new models::Day(day, month, year, _book);
}

QObject*
Book::monthModel(QDate date) {
    auto model = new models::Month(date.month(), date.year(), _book);
    connect(this, &Book::transactionStored, model, &models::Month::onTransactionStored);
    connect(this, &Book::transactionRemoved, model, &models::Month::onTransactionRemoved);
    connect(this, &Book::transactionUpdated, model, &models::Month::onTransactionUpdated);
    connect(this, &Book::categoryTypeUpdated, model, &models::Month::onCategoryTypeUpdated);
    return model;
}

}
}

}