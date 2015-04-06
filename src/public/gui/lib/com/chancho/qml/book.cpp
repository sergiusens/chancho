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
#include "models/month.h"

#include "account.h"
#include "transaction.h"

#include "book.h"

namespace com {

namespace chancho {

namespace qml {

Book::Book(QObject* parent)
    : QObject(parent),
      _book(std::make_shared<com::chancho::Book>()) {
    qRegisterMetaType<Book::TransactionType>("Book::TransactionType");
}

Book::Book(BookPtr book, QObject* parent)
    : QObject(parent),
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

    double amount = 0;
    if (initialAmount != 0) {
        amount = initialAmount;
    }
    auto acc = std::make_shared<com::chancho::Account>(name, amount, memo, color);
    acc->initialAmount = initialAmount;
    _book->store(acc);
    if (_book->isError()) {
        return false;
    } else {
        emit accountStored();
        return true;
    }
}

bool
Book::removeAccount(QObject* account) {
    auto acc = qobject_cast<qml::Account*>(account);
    if (acc == nullptr) {
        return false;
    }
    _book->remove(acc->getAccount());
    if (_book->isError()) {
        return false;
    } else {
        emit accountRemoved();
        return true;
    }
}

bool
Book::updateAccount(QObject* account, QString name, QString memo, QString color) {
    auto acc = qobject_cast<qml::Account*>(account);
    if (acc == nullptr) {
        return false;
    }
    auto accPtr = acc->getAccount();
    if (accPtr->name != name
            || accPtr->memo != memo
            || accPtr->color != color) {
        accPtr->name = name;
        accPtr->memo = memo;
        accPtr->color = color;
        _book->store(accPtr);
        if (_book->isError()) {
            return false;
        } else {
            emit accountUpdated();
            return true;
        }
    } else {
        return false;
    }
}

bool
Book::storeTransaction(QObject* account, QObject* category, QDate date, double amount, QString contents,
        QString memo) {
    auto acc = qobject_cast<qml::Account*>(account);
    if (acc == nullptr) {
        return false;
    }

    auto cat = qobject_cast<qml::Category*>(category);
    if (cat == nullptr) {
        return false;
    }

    auto tran = std::make_shared<chancho::Transaction>(
            acc->getAccount(), amount, cat->getCategory(), date, contents, memo);
    _book->store(tran);

    if (_book->isError()) {
        return false;
    } else {
        emit accountUpdated();
        emit transactionStored(date);
        return true;
    }
}

bool
Book::removeTransaction(QObject* transaction) {
    auto tran = qobject_cast<qml::Transaction*>(transaction);
    if (tran == nullptr) {
        return false;
    }
    _book->remove(tran->getTransaction());
    if (_book->isError()) {
        return false;
    } else {
        emit accountUpdated();
        emit transactionRemoved(tran->getTransaction()->date);
        return true;
    }
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

    // decide if we need to perform the update
    auto requiresUpdate = tran->account != acc || tran->category != cat || tran->date != date
        || tran->contents != contents || tran->memo != memo || tran->amount != amount;

    if (requiresUpdate) {
        auto oldDate = tran->date;
        tran->account = acc;
        tran->category = cat;
        tran->date = date;
        tran->contents = contents;
        tran->memo = memo;
        tran->amount = amount;

        _book->store(tran);
        if (_book->isError()) {
            return false;
        } else {
            emit accountUpdated();
            emit transactionUpdated(oldDate, date);
            return true;
        }

    } else {
        return false;
    }

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
    DLOG(INFO) << __PRETTY_FUNCTION__ << " " << name.toStdString() << " " << color.toStdString();
    com::chancho::Category::Type catType;
    if (type == TransactionType::EXPENSE) {
        catType = com::chancho::Category::Type::EXPENSE;
    } else {
        catType = com::chancho::Category::Type::INCOME;
    }

    auto cat = std::make_shared<com::chancho::Category>(name, catType, color);
    _book->store(cat);

    if (_book->isError()) {
        return false;
    } else {
        LOG(INFO) << "Category stored being emited";
        emit categoryStored(type);
        return true;
    }
}

bool
Book::updateCategory(QObject* category, QString name, QString color, Book::TransactionType type) {
    auto catModel = qobject_cast<qml::Category*>(category);
    if (catModel == nullptr) {
        return false;
    }
    auto cat = catModel->getCategory();

    com::chancho::Category::Type catType;
    if (type == TransactionType::EXPENSE) {
        catType = com::chancho::Category::Type::EXPENSE;
    } else {
        catType = com::chancho::Category::Type::INCOME;
    }
    auto typeChanged = catType != cat->type;

    if (cat->name != name
            || cat->color != color
            || cat->type != catType) {
        cat->name = name;
        cat->type = catType;
        cat->color = color;

        _book->store(cat);

        if (_book->isError()) {
            return false;
        } else {
            if (typeChanged) {
                LOG(INFO) << "Type updated!";
                emit categoryTypeUpdated();
            } else {
                emit categoryUpdated(type);
            }
            return true;
        }
    } else {
        return false;
    }
}

bool
Book::removeCategory(QObject* category) {
    auto catModel = qobject_cast<qml::Category*>(category);
    if (catModel == nullptr) {
        return false;
    }
    auto cat = catModel->getCategory();
    _book->remove(cat);

    if (_book->isError()) {
        return false;
    } else {
        emit categoryRemoved(catModel->getType());
        return true;
    }
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