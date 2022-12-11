#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QVariant>

#include <Core/Bilibili.h>

int main(int argc, char *argv[])
{
    using namespace DanmuCore;

    QGuiApplication app(argc, argv);

    Bilibili bili;
    QQmlApplicationEngine engine;

    // 注册到QML，要写在 engine.load(url) 之前
    QQmlContext *rootContext = engine.rootContext();
    rootContext->setContextProperty("Bilibili", &bili);

    const QUrl url(u"qrc:/DMB_GUI_test/home/ccslykx/Projects/BilibiliDMB_QML/GUI/qml/main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    QObject *qml = engine.rootObjects().at(0);
    
    // 下面内容还可以参考 https://doc.qt.io/qt-6/qtqml-cppintegration-exposecppattributes.html#exposing-methods-including-qt-slots
    if (qml) 
    {
        QObject::connect(qml, SIGNAL(connect(QString)),
                         &bili, SLOT(connect(QString)));
        QObject::connect(qml, SIGNAL(disconnect()),
                         &bili, SLOT(disconnect()));
    }

    return app.exec();
}
