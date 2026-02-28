#include "mainwindow.h"
#include <QApplication>
#include <QFont>
#include <QStyleFactory>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Установка информации о приложении
    app.setApplicationName("Калькулятор азимута WGS84");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("Geodesy Lab");

    // Установка стиля для Ubuntu
    app.setStyle(QStyleFactory::create("Fusion"));

    // Установка шрифта по умолчанию
    QFont defaultFont("Ubuntu", 10);
    app.setFont(defaultFont);

    // Вывод информации о сборке
    qDebug() << "Запуск калькулятора азимута";
    qDebug() << "Qt версия:" << qVersion();
    qDebug() << "С++ стандарт:" << __cplusplus;

    MainWindow w;
    w.show();

    return app.exec();
}
