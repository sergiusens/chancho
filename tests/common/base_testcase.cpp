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

#include <QDir>
#include <QtGlobal>
#include <QStandardPaths>
#include <string.h>

#include "base_testcase.h"

void
noMessageOutput(QtMsgType type,
                const QMessageLogContext& context,
                const QString &message) {
    Q_UNUSED(type);
    Q_UNUSED(context);
    Q_UNUSED(message);
    // do nothing
}

BaseTestCase::BaseTestCase(const QString& testName, QObject *parent)
    : QObject(parent) {
    setObjectName(testName);
}

QString
BaseTestCase::testDirectory() {
    // return the test directory using the standard paths
    QString dataPath = QStandardPaths::writableLocation(
        QStandardPaths::DataLocation);
    QStringList pathComponents;
    pathComponents << dataPath << objectName();
    QString path = pathComponents.join(QDir::separator());

    if (!QDir().exists(path))
        QDir().mkpath(path);

    return path;
}

QString
BaseTestCase::dataDirectory() {
    // get the file name and use it to get the data path that is in the
    // same dir
    QDir dir(".");
    dir.cdUp();
    dir.cdUp();
    dir.makeAbsolute();

    return dir.path() + "/data";
}

bool
BaseTestCase::removeDir(const QString& dirName) {
    bool result = true;
    QDir dir(dirName);

    QFlags<QDir::Filter> filter =  QDir::NoDotAndDotDot | QDir::System
        | QDir::Hidden  | QDir::AllDirs | QDir::Files;
    if (dir.exists(dirName)) {
        foreach(QFileInfo info, dir.entryInfoList(filter, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            } else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }

    return result;
}

void
BaseTestCase::init() {
//    qInstallMessageHandler(noMessageOutput);
    QStandardPaths::enableTestMode(true);
}

void
BaseTestCase::cleanup() {
    // remove the test dir if exists
    removeDir(testDirectory());
}
