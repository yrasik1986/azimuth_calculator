#-------------------------------------------------
#
# Проект: Калькулятор азимута WGS84
# Компилятор: g++ (Ubuntu 20.04)
# Qt версия: 5.12.8
#
#-------------------------------------------------

QT += core widgets
TARGET = azimuth_calculator
TEMPLATE = app

# Стандарт C++17
CONFIG += c++17
CONFIG -= app_bundle

# Оптимизация для максимальной точности
QMAKE_CXXFLAGS_RELEASE += -O3 -ffast-math -mfpmath=sse -msse2
QMAKE_CXXFLAGS_DEBUG += -O0 -g -ggdb

# Версия Qt
QT_MAJOR_VERSION = 5
QT_MINOR_VERSION = 12
QT_PATCH_VERSION = 8

# Проверка версии Qt
!versionAtLeast(QT_VERSION, 5.12.0) {
    error("Требуется Qt версии 5.12.0 или выше")
}

# Исходные файлы
SOURCES += \
    main.cpp \
    mainwindow.cpp

# Заголовочные файлы
HEADERS += \
    mainwindow.h

# Файлы интерфейса (если используете Qt Designer)
FORMS += \
    #mainwindow.ui

# Ресурсы (если нужны)
# RESOURCES += \
#     resources.qrc

# Установка
target.path = /usr/local/bin
INSTALLS += target

# Дополнительные настройки для Ubuntu
unix:!macx {
    # Пути для библиотек
    LIBS += -lm
    
    # Определение ОС
    DEFINES += OS_LINUX
    
    # Дополнительные флаги
    QMAKE_LFLAGS += -Wl,-rpath,/usr/local/lib
}

# Информация о сборке
message("========================================")
message("Сборка калькулятора азимута WGS84")
message("Qt версия: $$QT_VERSION")
message("Компилятор: $${QMAKE_CXX}")
message("Флаги компиляции: $${QMAKE_CXXFLAGS_RELEASE}")
message("========================================")

# Создание Makefile
CONFIG += create_prl
CONFIG += link_prl
