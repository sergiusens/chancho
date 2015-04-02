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

#include <QQuickView>
#include <QCoreApplication>
#include <QQmlEngine>
#include <QtQml>

#include "com/chancho/chancho_app.h"

int main(int argc, char** argv) {
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    QCoreApplication::setOrganizationName("TheMacaque");
    QCoreApplication::setOrganizationDomain("themacaque.com");
    QCoreApplication::setApplicationName("Chancho Personal Finance");

    com::chancho::ChanchoApp application(argc, argv);

    if (!application.setup()) {
        return 0;
    }

    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.engine()->addImportPath(":/imports");

/*    QObject::connect(view.engine(), SIGNAL(quit()), this, SLOT(quit()), Qt::QueuedConnection);
    qmlRegisterType<QAbstractItemModel>();
    qmlRegisterType<SystemSettings::PluginManager>("SystemSettings", 1, 0, "PluginManager");
    view.engine()->rootContext()->setContextProperty("Utilities", new Utilities());
    view.engine()->addImportPath(PLUGIN_PRIVATE_MODULE_DIR);
    view.engine()->addImportPath(PLUGIN_QML_DIR);
    view.rootContext()->setContextProperty("defaultPlugin", defaultPlugin);
    view.rootContext()->setContextProperty("i18nDirectory", I18N_DIRECTORY);
    view.rootContext()->setContextProperty("pluginOptions", pluginOptions);
*/
    view.setSource(QUrl("qrc:/qml/MainWindow.qml"));
    view.show();

    return application.exec();
}

