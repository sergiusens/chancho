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

#include "categories.h"

namespace com {

namespace chancho {

namespace book {

namespace {
    const QString INSERT_CATEGORY = "INSERT INTO Categories(uuid, parent, name, type, color) " \
        "VALUES (:uuid, :parent, :name, :type, :color)";
    const QString UPDATE_CATEGORY = "UPDATE Categories SET parent=:parent, name=:name, type=:type, color=:color " \
        "WHERE uuid=:uuid";
    const QString DELETE_CATEGORY = "DELETE FROM Categories WHERE uuid=:uuid";
    const QString SELECT_ALL_CATEGORIES = "SELECT uuid, parent, name, type, color FROM Categories ORDER BY name ASC";
    const QString SELECT_ALL_CATEGORIES_LIMIT = "SELECT uuid, parent, name, type, color FROM Categories ORDER BY name ASC "\
        "LIMIT :limit OFFSET :offset";
    const QString SELECT_ALL_CATEGORIES_TYPE = "SELECT uuid, parent, name, type, color FROM Categories WHERE type=:type "\
        "ORDER BY name ASC";
    const QString SELECT_ALL_CATEGORIES_TYPE_LIMIT = "SELECT uuid, parent, name, type, color FROM Categories "
            "WHERE type=:type ORDER BY name ASC LIMIT :limit OFFSET :offset";
    const QString SELECT_CATEGORIES_COUNT = "SELECT count(*) FROM Categories";
    const QString SELECT_CATEGORIES_COUNT_TYPE = "SELECT count(*) FROM Categories WHERE type=:type";
    const QString SELECT_CATEGORIES_RECURRENT = "SELECT uuid, parent, name, type, color FROM Categories WHERE uuid IN "\
        "(SELECT category from RecurrentTransactions GROUP BY category) ORDER BY name";
    const QString SELECT_CATEGORIES_RECURRENT_LIMIT = "SELECT uuid, parent, name, type, color FROM Categories WHERE uuid IN "\
        "(SELECT category from RecurrentTransactions GROUP BY category) ORDER BY name LIMIT :limit OFFSET :offset";
    const QString SELECT_CATEGORIES_RECURRENT_COUNT = "SELECT count(*) FROM (SELECT category FROM "\
        "RecurrentTransactions GROUP BY category)";
}

Categories::Categories(system::DatabasePtr db)
    : _db(db){
}

bool
Categories::storeSingleCat(CategoryPtr cat) {
    auto isPresent = cat->wasStoredInDb();
    if (!isPresent) {
        cat->_dbId = QUuid::createUuid();
    }

    // create the command and execute it
    auto query = _db->createQuery();
    if (!isPresent) {
        query->prepare(INSERT_CATEGORY);
    } else {
        query->prepare(UPDATE_CATEGORY);
    }
    query->bindValue(":uuid", cat->_dbId.toString());
    query->bindValue(":name", cat->name);
    query->bindValue(":type", static_cast<int>(cat->type));
    query->bindValue(":color", cat->color);

    if (cat->parent) {
        query->bindValue(":parent", cat->parent->_dbId.toString());
    } else {
        query->bindValue(":parent", QVariant());
    }

    // no need to use a transaction since is a single insert
    auto success = query->exec();
    if (!success) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << _lastError.toStdString();
        cat->_dbId = QUuid();
    }
    return success;
}

void
Categories::store(CategoryPtr cat) {
    // if the cate has a parent and was not stored, we store it
    if (cat->parent && !cat->parent->wasStoredInDb()) {
        LOG(INFO) << "Storing parent that was not present already";
        store(cat->parent);
    }

    storeSingleCat(cat);
}

void
Categories::store(QList<CategoryPtr> cats) {
    // grab all parents, add the to the list of cats to store
    QList<CategoryPtr> parents;
    foreach(const CategoryPtr& cat, cats) {
        if (cat->parent && !cat->parent->wasStoredInDb()) {
            parents.append(cat->parent);
        }
    }

    bool transaction = _db->transaction();
    if (!transaction) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << "Error creating the transaction " << _lastError.toStdString();
        return;
    }

    foreach(const CategoryPtr& cat, parents) {
        bool success = storeSingleCat(cat);
        if (!success) {
            _db->rollback();
            return;;
        }
    }

    foreach(const CategoryPtr& cat, cats) {
        bool success = storeSingleCat(cat);
        if (!success) {
            _db->rollback();
            return;;
        }
    }
    _db->commit();
}

void
Categories::remove(CategoryPtr cat) {
    if (cat->_dbId.isNull()) {
        LOG(ERROR) << "Cannot delete category '" << cat->name.toStdString()
        << "' with a NULL id";
        _lastError = "Cannot delete Category that was not added to the db";
        return;
    }

    // ensure that we have a transaction so that we do not have the db in a non stable state
    auto success = _db->transaction();
    if (!success) {
        _lastError = _db->lastError().text();
        LOG(ERROR) << "Error creating the transaction " << _lastError.toStdString();
        return;
    }

    auto deleteCat = _db->createQuery();
    deleteCat->prepare(DELETE_CATEGORY);
    deleteCat->bindValue(":uuid", cat->_dbId.toString());
    success &= deleteCat->exec();

    if (success) {
        DLOG(INFO) << "Commiting transaction";
        _db->commit();
        // set the _dbId to be a null uuid
        cat->_dbId = QUuid();
    } else {
        _lastError = _db->lastError().text();
        LOG(ERROR) << "Rolliing back transaction after error: '" << _lastError.toStdString() << "'";
        _db->rollback();
    }
}

QList<CategoryPtr>
Categories::parseCategories(std::shared_ptr<system::Query> query) {
    QMap<QUuid, QList<QUuid>> parentChildMap;
    QMap<QUuid, CategoryPtr> catsMap;
    QList<QUuid> orderedIds;

    auto success = query->exec();

    if (!success) {
        _lastError = _db->lastError().text();
        LOG(INFO) << "Error retrieving the categories " << _lastError.toStdString();
        QList<CategoryPtr> cats;
        return cats;
    }


    // SELECT_ALL_CATEGORIES = "SELECT uuid, parent, name, type, color FROM Categories ORDER BY name ASC";
    // therefore
    // index 0 => uuid
    // index 1 => parent
    // index 2 => name
    // index 3 => type
    // index 4 => color
    // is more efficient to use indexes that strings
    while (query->next()) {
        auto uuid = QUuid(query->value(0).toString());
        auto parentVar = query->value(1);
        auto name = query->value(2).toString();
        auto type = static_cast<Category::Type>(query->value(3).toInt());
        auto color = query->value(4).toString();
        auto cat = std::make_shared<Category>(name, type, color);
        cat->_dbId = uuid;
        DLOG(INFO) << "Category found with id " << cat->_dbId.toString().toStdString();
        catsMap[cat->_dbId] = cat;

        if(!parentVar.isNull()) {
            auto parent = QUuid(parentVar.toString());
            DLOG(INFO) << "Parent relationship foudn with " << parent.toString().toStdString();

            if(parentChildMap.contains(parent)) {
                parentChildMap[parent].append(cat->_dbId);
            } else {
                QList<QUuid> childList;
                childList.append(cat->_dbId);
                parentChildMap[parent] = childList;
            }
        } else {
            DLOG(INFO) << "Parent not found";
        }
        orderedIds.append(uuid);
    }

    // set the parent child relationship
    foreach(const QUuid& parentId, parentChildMap.keys()) {
        if (!catsMap.contains(parentId)) {
            LOG(ERROR) << "DB consintency error categori with id '" << parentId.toString().toStdString()
            << "' not found.";
            continue;
        }
        foreach(const QUuid& childId, parentChildMap[parentId]) {
            if (!catsMap.contains(childId)) {
                LOG(ERROR) << "DB consintency error categori with id '" << parentId.toString().toStdString()
                    << "' not found.";
                continue;
            }
            // set the parent, do not worry about points since we are using std::shared_ptr
            catsMap[childId]->parent = catsMap[parentId];
        }
    }

    // QMap and QHash will not return the results in the right order, we have the QList of uuids to get them in
    // the order that the select returned
    QList<CategoryPtr> orderedCats;
    foreach(const QUuid& id, orderedIds) {
        orderedCats.append(catsMap[id]);
    }
    return orderedCats;
}

QList<CategoryPtr>
Categories::categories(boost::optional<Category::Type> type, boost::optional<int> limit, boost::optional<int> offset) {
    auto query = _db->createQuery();

    if (type) {
        if (limit) {
            // SELECT_ALL_CATEGORIES_TYPE_LIMIT = "SELECT uuid, parent, name, type FROM Categories ORDER BY name ASC
            //     WHERE type=:type LIMIT :limit OFFSET :offset
            query->prepare(SELECT_ALL_CATEGORIES_TYPE_LIMIT);
            query->bindValue(":type", static_cast<int>(*type));
            query->bindValue(":limit", *limit);

            if (offset) {
                query->bindValue(":offset", *offset);
            } else {
                query->bindValue(":offset", 0);
            }
        } else {
            // SELECT_ALL_CATEGORIES_TYPE = SELECT uuid, parent, name, type FROM Categories ORDER BY name ASC
            // WHERE type=:type
            query->prepare(SELECT_ALL_CATEGORIES_TYPE);
            query->bindValue(":type", static_cast<int>(*type));
        }
    } else {
        if (limit) {
            // SELECT_ALL_CATEGORIES_LIMIT = SELECT uuid, parent, name, type FROM Categories ORDER BY name ASC
            //     LIMIT :limit OFFSET :offset;
            query->prepare(SELECT_ALL_CATEGORIES_LIMIT);
            query->bindValue(":limit", *limit);

            if (offset) {
                query->bindValue(":offset", *offset);
            } else {
                query->bindValue(":offset", 0);
            }
        } else {
            query->prepare(SELECT_ALL_CATEGORIES);
        }
    }

    auto cats = parseCategories(query);
    return cats;
}

int
Categories::numberOfCategories(boost::optional<Category::Type> type) {
    int count = -1;

    auto query = _db->createQuery();

    if (type) {
        // SELECT_CATEGORIES_COUNT_TYPE = SELECT count(*) FROM Categories WHERE type=:type;
        query->prepare(SELECT_CATEGORIES_COUNT_TYPE);
        query->bindValue(":type", static_cast<int>(*type));
    } else {
        // SELECT_CATEGORIES_COUNT = "SELECT count(*) FROM Categories";
        query->prepare(SELECT_CATEGORIES_COUNT);
    }

    auto success = query->exec();

    if (!success) {
        _lastError = _db->lastError().text();
        LOG(INFO) << "Error retrieving the transactions count" << _lastError.toStdString();
    } else if (query->next()) {
        count = query->value(0).toInt();
    }

    return count;
}

QList<CategoryPtr>
Categories::recurrentCategories(boost::optional<int> limit, boost::optional<int> offset) {
    auto query = _db->createQuery();

    if (limit) {
        query->prepare(SELECT_CATEGORIES_RECURRENT_LIMIT);
        query->bindValue(":limit", *limit);

        if (offset) {
            query->bindValue(":offset", *offset);
        } else {
            query->bindValue(":offset", 0);
        }
    } else {
        query->prepare(SELECT_CATEGORIES_RECURRENT);
    }

    auto cats = parseCategories(query);
    return cats;
}

int
Categories::numberOfRecurrentCategories() {
    int count = -1;

    auto query = _db->createQuery();
    query->prepare(SELECT_CATEGORIES_RECURRENT_COUNT);
    auto success = query->exec();

    if (!success) {
        _lastError = _db->lastError().text();
        LOG(INFO) << "Error retrieving the transactions count" << _lastError.toStdString();
    } else if (query->next()) {
        count = query->value(0).toInt();
    }

    return count;
}

bool
Categories::isError() {
    return _lastError != QString::null;
}

QString
Categories::lastError() {
    return _lastError;
}

}
}
}
