#include "mainwindow.h"
#include <QMenuBar>
// Если используете Qt Designer, раскомментируйте следующую строку
// #include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(nullptr)  // если не используете .ui файл
{
    // Если используете Qt Designer:
    // ui = new Ui::MainWindow;
    // ui->setupUi(this);
    
    setWindowTitle("Калькулятор азимута (WGS84) - Ubuntu 20.04");
    setMinimumSize(600, 550);
    
    // Создание центрального виджета
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // Создание валидаторов с высокой точностью
    latValidator = new QDoubleValidator(-90.0, 90.0, 12, this);
    lonValidator = new QDoubleValidator(-180.0, 180.0, 12, this);
    
    // Настройка валидаторов для локали Ubuntu
    QLocale locale(QLocale::C);
    locale.setNumberOptions(QLocale::RejectGroupSeparator);
    latValidator->setLocale(locale);
    lonValidator->setLocale(locale);
    
    // Создание полей ввода
    lat1Edit = new QLineEdit(this);
    lat1Edit->setPlaceholderText("Широта точки стояния");
    lat1Edit->setValidator(latValidator);
    lat1Edit->setClearButtonEnabled(true);
    lat1Edit->setToolTip("Введите широту в десятичных градусах\nПример: 55.755831");
    
    lon1Edit = new QLineEdit(this);
    lon1Edit->setPlaceholderText("Долгота точки стояния");
    lon1Edit->setValidator(lonValidator);
    lon1Edit->setClearButtonEnabled(true);
    lon1Edit->setToolTip("Введите долготу в десятичных градусах\nПример: 37.617673");
    
    lat2Edit = new QLineEdit(this);
    lat2Edit->setPlaceholderText("Широта объекта");
    lat2Edit->setValidator(latValidator);
    lat2Edit->setClearButtonEnabled(true);
    
    lon2Edit = new QLineEdit(this);
    lon2Edit->setPlaceholderText("Долгота объекта");
    lon2Edit->setValidator(lonValidator);
    lon2Edit->setClearButtonEnabled(true);
    
    // Выбор точности
    precisionCombo = new QComboBox(this);
    precisionCombo->addItem("6 знаков (метровая точность)", 6);
    precisionCombo->addItem("8 знаков (сантиметровая точность)", 8);
    precisionCombo->addItem("10 знаков (миллиметровая точность)", 10);
    precisionCombo->addItem("12 знаков (максимальная точность)", 12);
    precisionCombo->setCurrentIndex(3);
    precisionCombo->setToolTip("Выберите точность вычислений");
    
    // Создание кнопок
    calculateButton = new QPushButton("Вычислить азимут", this);
    calculateButton->setMinimumHeight(40);
    calculateButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #3d8b40;"
        "}"
    );
    
    swapButton = new QPushButton("⇄ Поменять местами", this);
    swapButton->setMinimumHeight(30);
    swapButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #2196F3;"
        "   color: white;"
        "   font-size: 12px;"
        "   border-radius: 3px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #1976D2;"
        "}"
    );
    
    clearButton = new QPushButton("Очистить", this);
    clearButton->setMinimumHeight(30);
    clearButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #f44336;"
        "   color: white;"
        "   font-size: 12px;"
        "   border-radius: 3px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #d32f2f;"
        "}"
    );
    
    // Создание меток для результатов
    resultLabel = new QLabel("Прямой азимут: --", this);
    resultLabel->setAlignment(Qt::AlignCenter);
    resultLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 18px;"
        "   font-weight: bold;"
        "   color: #1976D2;"
        "   padding: 10px;"
        "   background-color: #f5f5f5;"
        "   border-radius: 5px;"
        "}"
    );
    
    reverseAzimuthLabel = new QLabel("Обратный азимут: --", this);
    reverseAzimuthLabel->setAlignment(Qt::AlignCenter);
    reverseAzimuthLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 14px;"
        "   color: #388E3C;"
        "   padding: 5px;"
        "}"
    );
    
    distanceLabel = new QLabel("Расстояние: --", this);
    distanceLabel->setAlignment(Qt::AlignCenter);
    distanceLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 14px;"
        "   color: #D32F2F;"
        "   padding: 5px;"
        "}"
    );
    
    ellipsoidLabel = new QLabel("Эллипсоид: WGS84 (a=6378137 м, f=1/298.257223563)", this);
    ellipsoidLabel->setAlignment(Qt::AlignCenter);
    ellipsoidLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 10px;"
        "   color: #757575;"
        "   padding: 5px;"
        "}"
    );
    
    // Группа для кнопок управления
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(swapButton);
    buttonLayout->addWidget(clearButton);
    
    // Группа для выбора точности
    QGroupBox *precisionGroup = new QGroupBox("Настройки точности", this);
    QVBoxLayout *precisionLayout = new QVBoxLayout(precisionGroup);
    precisionLayout->addWidget(new QLabel("Точность вычислений:"));
    precisionLayout->addWidget(precisionCombo);
    
    // Группа для точки стояния
    QGroupBox *point1Group = new QGroupBox("Точка стояния", this);
    QVBoxLayout *point1Layout = new QVBoxLayout(point1Group);
    point1Layout->addWidget(new QLabel("Широта:"));
    point1Layout->addWidget(lat1Edit);
    point1Layout->addWidget(new QLabel("Долгота:"));
    point1Layout->addWidget(lon1Edit);
    
    // Группа для объекта
    QGroupBox *point2Group = new QGroupBox("Объект", this);
    QVBoxLayout *point2Layout = new QVBoxLayout(point2Group);
    point2Layout->addWidget(new QLabel("Широта:"));
    point2Layout->addWidget(lat2Edit);
    point2Layout->addWidget(new QLabel("Долгота:"));
    point2Layout->addWidget(lon2Edit);
    
    // Группа для результатов
    QGroupBox *resultGroup = new QGroupBox("Результаты вычислений", this);
    QVBoxLayout *resultLayout = new QVBoxLayout(resultGroup);
    resultLayout->addWidget(resultLabel);
    resultLayout->addWidget(reverseAzimuthLabel);
    resultLayout->addWidget(distanceLabel);
    resultLayout->addWidget(ellipsoidLabel);
    
    // Основной макет
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(point1Group);
    mainLayout->addWidget(point2Group);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(precisionGroup);
    mainLayout->addWidget(calculateButton);
    mainLayout->addWidget(resultGroup);
    mainLayout->addStretch();
    
    // Подключение сигналов
    connect(calculateButton, &QPushButton::clicked, this, &MainWindow::calculateAzimuth);
    connect(swapButton, &QPushButton::clicked, this, &MainWindow::swapPoints);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearFields);
    connect(precisionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &MainWindow::updatePrecision);
    
    // Установка примеров
    lat1Edit->setText("55.755831");     // Москва
    lon1Edit->setText("37.617673");
    lat2Edit->setText("59.934280");     // Санкт-Петербург
    lon2Edit->setText("30.335099");
    
    // Создание меню
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    
    QMenu *fileMenu = menuBar->addMenu("&Файл");
    fileMenu->addAction("&Выход", this, &QWidget::close, QKeySequence::Quit);
    
    QMenu *helpMenu = menuBar->addMenu("&Помощь");
    helpMenu->addAction("&О программе", this, &MainWindow::about);
    
    // Статус бар
    statusBar()->showMessage("Готов к вычислениям");
}

MainWindow::~MainWindow()
{
    delete ui;
}

double MainWindow::toRadians(double degrees) const
{
    return degrees * M_PI / 180.0;
}

double MainWindow::toDegrees(double radians) const
{
    return radians * 180.0 / M_PI;
}

double MainWindow::normalizeAzimuth(double azimuth) const
{
    azimuth = fmod(azimuth, 360.0);
    if (azimuth < 0)
        azimuth += 360.0;
    return azimuth;
}

QString MainWindow::formatDirection(double azimuth) const
{
    if (azimuth >= 337.5 || azimuth < 22.5) return "С";
    if (azimuth >= 22.5 && azimuth < 67.5) return "СВ";
    if (azimuth >= 67.5 && azimuth < 112.5) return "В";
    if (azimuth >= 112.5 && azimuth < 157.5) return "ЮВ";
    if (azimuth >= 157.5 && azimuth < 202.5) return "Ю";
    if (azimuth >= 202.5 && azimuth < 247.5) return "ЮЗ";
    if (azimuth >= 247.5 && azimuth < 292.5) return "З";
    return "СЗ";
}

QString MainWindow::formatDistance(double distance, int precision) const
{
    if (distance < 0.001) {
        return QString("%1 мм").arg(distance * 1000.0, 0, 'f', 3);
    } else if (distance < 1.0) {
        return QString("%1 м").arg(distance, 0, 'f', precision > 8 ? 6 : 3);
    } else if (distance < 1000.0) {
        return QString("%1 м").arg(distance, 0, 'f', precision > 8 ? 3 : 2);
    } else if (distance < 1000000.0) {
        return QString("%1 км").arg(distance / 1000.0, 0, 'f', precision > 8 ? 6 : 3);
    } else {
        return QString("%1 км").arg(distance / 1000.0, 0, 'f', precision > 8 ? 3 : 2);
    }
}

int MainWindow::vincentyInverse(double lat1, double lon1, double lat2, double lon2,
                                double &azimuth1, double &azimuth2, double &distance)
{
    double phi1 = toRadians(lat1);
    double lambda1 = toRadians(lon1);
    double phi2 = toRadians(lat2);
    double lambda2 = toRadians(lon2);
    
    double f = WGS84_F;
    double a = WGS84_A;
    double b = WGS84_B;
    
    double sinPhi1 = sin(phi1);
    double cosPhi1 = cos(phi1);
    double sinPhi2 = sin(phi2);
    double cosPhi2 = cos(phi2);
    
    double L = lambda2 - lambda1;
    double lambda = L;
    double sinLambda, cosLambda;
    
    double sinSigma, cosSigma, sigma, sinAlpha, cosSqAlpha, cos2SigmaM, C;
    double lambdaPrev;
    int iterations = 0;
    
    do {
        sinLambda = sin(lambda);
        cosLambda = cos(lambda);
        
        double sinSigma1 = cosPhi2 * sinLambda;
        double sinSigma2 = cosPhi1 * sinPhi2 - sinPhi1 * cosPhi2 * cosLambda;
        sinSigma = sqrt(sinSigma1 * sinSigma1 + sinSigma2 * sinSigma2);
        
        if (sinSigma == 0) {
            azimuth1 = 0;
            azimuth2 = 0;
            distance = 0;
            return 0;
        }
        
        cosSigma = sinPhi1 * sinPhi2 + cosPhi1 * cosPhi2 * cosLambda;
        sigma = atan2(sinSigma, cosSigma);
        
        double sinAlphaPart = cosPhi1 * cosPhi2 * sinLambda / sinSigma;
        sinAlpha = sinAlphaPart;
        cosSqAlpha = 1 - sinAlpha * sinAlpha;
        
        if (cosSqAlpha != 0) {
            cos2SigmaM = cosSigma - 2 * sinPhi1 * sinPhi2 / cosSqAlpha;
        } else {
            cos2SigmaM = 0;
        }
        
        C = f / 16 * cosSqAlpha * (4 + f * (4 - 3 * cosSqAlpha));
        
        lambdaPrev = lambda;
        lambda = L + (1 - C) * f * sinAlpha * 
                (sigma + C * sinSigma * (cos2SigmaM + C * cosSigma * (-1 + 2 * cos2SigmaM * cos2SigmaM)));
        
        iterations++;
    } while (fabs(lambda - lambdaPrev) > EPSILON && iterations < MAX_ITERATIONS);
    
    if (iterations >= MAX_ITERATIONS) {
        return -1; // Не сошлось
    }
    
    double uSq = cosSqAlpha * (a * a - b * b) / (b * b);
    double A = 1 + uSq / 16384 * (4096 + uSq * (-768 + uSq * (320 - 175 * uSq)));
    double B = uSq / 1024 * (256 + uSq * (-128 + uSq * (74 - 47 * uSq)));
    
    double deltaSigma = B * sinSigma * (cos2SigmaM + B / 4 * 
                        (cosSigma * (-1 + 2 * cos2SigmaM * cos2SigmaM) - 
                         B / 6 * cos2SigmaM * (-3 + 4 * sinSigma * sinSigma) * 
                         (-3 + 4 * cos2SigmaM * cos2SigmaM)));
    
    distance = b * A * (sigma - deltaSigma);
    
    double az1 = atan2(cosPhi2 * sinLambda, cosPhi1 * sinPhi2 - sinPhi1 * cosPhi2 * cosLambda);
    double az2 = atan2(cosPhi1 * sin(lambda), -sinPhi1 * cosPhi2 + cosPhi1 * sinPhi2 * cos(lambda));
    
    azimuth1 = normalizeAzimuth(toDegrees(az1));
    azimuth2 = normalizeAzimuth(toDegrees(az2));
    
    return iterations;
}

double MainWindow::calculateAzimuthWGS84(double lat1, double lon1, double lat2, double lon2, 
                                         double &distance, double &reverseAzimuth)
{
    double azimuth;
    int result = vincentyInverse(lat1, lon1, lat2, lon2, azimuth, reverseAzimuth, distance);
    
    if (result < 0) {
        // Если метод Винсенти не сошелся, используем упрощенную формулу
        double lat1Rad = toRadians(lat1);
        double lon1Rad = toRadians(lon1);
        double lat2Rad = toRadians(lat2);
        double lon2Rad = toRadians(lon2);
        
        double deltaLon = lon2Rad - lon1Rad;
        double y = sin(deltaLon) * cos(lat2Rad);
        double x = cos(lat1Rad) * sin(lat2Rad) - sin(lat1Rad) * cos(lat2Rad) * cos(deltaLon);
        
        azimuth = normalizeAzimuth(toDegrees(atan2(y, x)));
        
        // Упрощенное расстояние (сферическое)
        double a = sin((lat2Rad - lat1Rad) / 2) * sin((lat2Rad - lat1Rad) / 2) +
                   cos(lat1Rad) * cos(lat2Rad) * sin(deltaLon / 2) * sin(deltaLon / 2);
        double c = 2 * atan2(sqrt(a), sqrt(1 - a));
        distance = WGS84_A * c;
        
        reverseAzimuth = normalizeAzimuth(azimuth + 180);
    }
    
    return azimuth;
}

void MainWindow::updatePrecision()
{
    int precision = precisionCombo->currentData().toInt();
    latValidator->setDecimals(precision);
    lonValidator->setDecimals(precision);
    statusBar()->showMessage(QString("Точность установлена: %1 знаков").arg(precision));
}

void MainWindow::swapPoints()
{
    QString lat1 = lat1Edit->text();
    QString lon1 = lon1Edit->text();
    QString lat2 = lat2Edit->text();
    QString lon2 = lon2Edit->text();
    
    lat1Edit->setText(lat2);
    lon1Edit->setText(lon2);
    lat2Edit->setText(lat1);
    lon2Edit->setText(lon1);
    
    statusBar()->showMessage("Точки поменяны местами");
}

void MainWindow::clearFields()
{
    lat1Edit->clear();
    lon1Edit->clear();
    lat2Edit->clear();
    lon2Edit->clear();
    
    resultLabel->setText("Прямой азимут: --");
    reverseAzimuthLabel->setText("Обратный азимут: --");
    distanceLabel->setText("Расстояние: --");
    
    lat1Edit->setFocus();
    statusBar()->showMessage("Поля очищены");
}

void MainWindow::about()
{
    QMessageBox::about(this, "О программе",
                       "<h2>Калькулятор азимута WGS84</h2>"
                       "<p>Версия: 1.0</p>"
                       "<p>Платформа: Ubuntu 20.04 LTS</p>"
                       "<p>Qt версия: 5.12.8</p>"
                       "<p>Компилятор: g++ 9.4.0</p>"
                       "<p><b>Особенности:</b></p>"
                       "<ul>"
                       "<li>Максимальная точность вычислений (до 12 знаков)</li>"
                       "<li>Формула Винсенти для эллипсоида WGS84</li>"
                       "<li>Прямой и обратный азимут</li>"
                       "<li>Геодезическое расстояние</li>"
                       "<li>8 направлений (румбы)</li>"
                       "</ul>"
                       "<p>© 2024</p>");
}

void MainWindow::calculateAzimuth()
{
    // Проверка заполненности полей
    if (lat1Edit->text().isEmpty() || lon1Edit->text().isEmpty() ||
        lat2Edit->text().isEmpty() || lon2Edit->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните все поля координат!");
        return;
    }
    
    bool ok1, ok2, ok3, ok4;
    
    // Преобразование строк в числа
    double lat1 = lat1Edit->text().toDouble(&ok1);
    double lon1 = lon1Edit->text().toDouble(&ok2);
    double lat2 = lat2Edit->text().toDouble(&ok3);
    double lon2 = lon2Edit->text().toDouble(&ok4);
    
    if (!ok1 || !ok2 || !ok3 || !ok4) {
        QMessageBox::warning(this, "Ошибка", "Некорректный формат координат!");
        return;
    }
    
    // Проверка диапазонов
    if (lat1 < -90.0 || lat1 > 90.0 || lat2 < -90.0 || lat2 > 90.0) {
        QMessageBox::warning(this, "Ошибка", "Широта должна быть в диапазоне [-90°, +90°]!");
        return;
    }
    
    if (lon1 < -180.0 || lon1 > 180.0 || lon2 < -180.0 || lon2 > 180.0) {
        QMessageBox::warning(this, "Ошибка", "Долгота должна быть в диапазоне [-180°, +180°]!");
        return;
    }
    
    // Вычисление
    double distance, reverseAzimuth;
    double azimuth = calculateAzimuthWGS84(lat1, lon1, lat2, lon2, distance, reverseAzimuth);
    
    int precision = precisionCombo->currentData().toInt();
    
    // Форматирование результатов
    QString direction = formatDirection(azimuth);
    QString reverseDirection = formatDirection(reverseAzimuth);
    
    resultLabel->setText(QString("Прямой азимут: %1° %2")
                         .arg(azimuth, 0, 'f', precision)
                         .arg(direction));
    
    reverseAzimuthLabel->setText(QString("Обратный азимут: %1° %2")
                                 .arg(reverseAzimuth, 0, 'f', precision)
                                 .arg(reverseDirection));
    
    distanceLabel->setText(QString("Расстояние: %1")
                           .arg(formatDistance(distance, precision)));
    
    statusBar()->showMessage(QString("Вычислено за %1 итераций").arg(MAX_ITERATIONS));
}
