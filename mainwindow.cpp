#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(nullptr)
{
    setWindowTitle("Калькулятор азимута (WGS84) - Поддержка карт");
    setMinimumSize(700, 750);

    // Создание центрального виджета
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Создание валидаторов
    latValidator = new QDoubleValidator(-90.0, 90.0, 12, this);
    lonValidator = new QDoubleValidator(-180.0, 180.0, 12, this);

    // Настройка локали
    QLocale locale(QLocale::C);
    locale.setNumberOptions(QLocale::RejectGroupSeparator);
    latValidator->setLocale(locale);
    lonValidator->setLocale(locale);

    // === Группа для вставки с карты ===
    QGroupBox *pasteGroup = new QGroupBox("Вставка координат с карты", this);
    pasteGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #2196F3; }");

    QVBoxLayout *pasteLayout = new QVBoxLayout(pasteGroup);

    // Пояснение по форматам
    formatHintLabel = new QLabel(
        "<b>Поддерживаемые форматы:</b><br>"
        "• 53.9273, 27.6674 (десятичные градусы)<br>"
        "• 53°55'38.28\"N, 27°40'2.64\"E (градусы/минуты/секунды)<br>"
        "• 53.92730011265664, 27.667402828338364 (максимальная точность)<br>"
        "• 53.9273 27.6674 (с пробелом)<br>"
        "• 53.9273; 27.6674 (с точкой с запятой)", this);
    formatHintLabel->setWordWrap(true);
    formatHintLabel->setStyleSheet("QLabel { color: #666; font-size: 10px; padding: 5px; }");
    pasteLayout->addWidget(formatHintLabel);

    // Поле для вставки
    bulkPasteEdit = new QTextEdit(this);
    bulkPasteEdit->setPlaceholderText("Вставьте координаты с карты (можно несколько пар)...\n"
                                      "Пример:\n"
                                      "53.92730011265664, 27.667402828338364\n"
                                      "55.755831, 37.617673\n"
                                      "59°56'37.2\"N 30°18'58.8\"E");
    bulkPasteEdit->setMaximumHeight(100);
    bulkPasteEdit->setFont(QFont("Monospace", 10));
    pasteLayout->addWidget(bulkPasteEdit);

    // Кнопки для вставки
    QHBoxLayout *pasteButtonsLayout = new QHBoxLayout();

    pasteToPoint1Button = new QPushButton("← В точку стояния", this);
    pasteToPoint1Button->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; padding: 5px; }"
        "QPushButton:hover { background-color: #45a049; }"
    );

    pasteToPoint2Button = new QPushButton("→ В объект", this);
    pasteToPoint2Button->setStyleSheet(
        "QPushButton { background-color: #FF9800; color: white; padding: 5px; }"
        "QPushButton:hover { background-color: #F57C00; }"
    );

    // Кнопка для вставки из буфера в поле массового ввода
    QPushButton *pasteFromClipboardButton = new QPushButton("📋 Вставить из буфера", this);
    pasteFromClipboardButton->setStyleSheet(
        "QPushButton { background-color: #9C27B0; color: white; padding: 5px; }"
        "QPushButton:hover { background-color: #7B1FA2; }"
    );
    connect(pasteFromClipboardButton, &QPushButton::clicked, this, &MainWindow::onPasteButtonClicked);

    pasteButtonsLayout->addWidget(pasteToPoint1Button);
    pasteButtonsLayout->addWidget(pasteToPoint2Button);
    pasteButtonsLayout->addWidget(pasteFromClipboardButton);
    pasteLayout->addLayout(pasteButtonsLayout);

    // Выбор режима ввода
    QHBoxLayout *modeLayout = new QHBoxLayout();
    modeLayout->addWidget(new QLabel("Режим ввода:"));

    decimalRadio = new QRadioButton("Десятичные", this);
    dmsRadio = new QRadioButton("Градусы/мин/сек", this);
    bothRadio = new QRadioButton("Оба формата", this);
    bothRadio->setChecked(true);

    inputModeGroup = new QButtonGroup(this);
    inputModeGroup->addButton(decimalRadio, 0);
    inputModeGroup->addButton(dmsRadio, 1);
    inputModeGroup->addButton(bothRadio, 2);

    modeLayout->addWidget(decimalRadio);
    modeLayout->addWidget(dmsRadio);
    modeLayout->addWidget(bothRadio);
    modeLayout->addStretch();
    pasteLayout->addLayout(modeLayout);

    // Автоматическая вставка
    autoPasteCheck = new QCheckBox("Автоматически парсить при вставке", this);
    autoPasteCheck->setChecked(true);
    pasteLayout->addWidget(autoPasteCheck);

    // === Основные поля ввода ===
    // Группа для точки стояния
    QGroupBox *point1Group = new QGroupBox("Точка стояния", this);
    point1Group->setStyleSheet("QGroupBox { font-weight: bold; color: #4CAF50; }");
    QGridLayout *point1Layout = new QGridLayout(point1Group);

    point1Layout->addWidget(new QLabel("Широта:"), 0, 0);
    lat1Edit = new QLineEdit(this);
    lat1Edit->setPlaceholderText("53.92730011265664");
    lat1Edit->setValidator(latValidator);
    lat1Edit->setClearButtonEnabled(true);
    point1Layout->addWidget(lat1Edit, 0, 1);

    point1Layout->addWidget(new QLabel("Долгота:"), 1, 0);
    lon1Edit = new QLineEdit(this);
    lon1Edit->setPlaceholderText("27.667402828338364");
    lon1Edit->setValidator(lonValidator);
    lon1Edit->setClearButtonEnabled(true);
    point1Layout->addWidget(lon1Edit, 1, 1);

    // Группа для объекта
    QGroupBox *point2Group = new QGroupBox("Объект", this);
    point2Group->setStyleSheet("QGroupBox { font-weight: bold; color: #FF9800; }");
    QGridLayout *point2Layout = new QGridLayout(point2Group);

    point2Layout->addWidget(new QLabel("Широта:"), 0, 0);
    lat2Edit = new QLineEdit(this);
    lat2Edit->setPlaceholderText("55.755831");
    lat2Edit->setValidator(latValidator);
    lat2Edit->setClearButtonEnabled(true);
    point2Layout->addWidget(lat2Edit, 0, 1);

    point2Layout->addWidget(new QLabel("Долгота:"), 1, 0);
    lon2Edit = new QLineEdit(this);
    lon2Edit->setPlaceholderText("37.617673");
    lon2Edit->setValidator(lonValidator);
    lon2Edit->setClearButtonEnabled(true);
    point2Layout->addWidget(lon2Edit, 1, 1);

    // === Группа для результатов ===
    QGroupBox *resultGroup = new QGroupBox("Результаты вычислений", this);
    resultGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #9C27B0; }");
    QVBoxLayout *resultLayout = new QVBoxLayout(resultGroup);

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
    resultLayout->addWidget(resultLabel);

    reverseAzimuthLabel = new QLabel("Обратный азимут: --", this);
    reverseAzimuthLabel->setAlignment(Qt::AlignCenter);
    reverseAzimuthLabel->setStyleSheet("QLabel { font-size: 14px; color: #388E3C; }");
    resultLayout->addWidget(reverseAzimuthLabel);

    distanceLabel = new QLabel("Расстояние: --", this);
    distanceLabel->setAlignment(Qt::AlignCenter);
    distanceLabel->setStyleSheet("QLabel { font-size: 14px; color: #D32F2F; }");
    resultLayout->addWidget(distanceLabel);

    // === Группа настроек ===
    QGroupBox *settingsGroup = new QGroupBox("Настройки", this);
    QHBoxLayout *settingsLayout = new QHBoxLayout(settingsGroup);

    settingsLayout->addWidget(new QLabel("Точность:"));
    precisionCombo = new QComboBox(this);
    precisionCombo->addItem("6 знаков (метровая)", 6);
    precisionCombo->addItem("8 знаков (сантиметровая)", 8);
    precisionCombo->addItem("10 знаков (миллиметровая)", 10);
    precisionCombo->addItem("12 знаков (макс. точность)", 12);
    precisionCombo->setCurrentIndex(3);
    settingsLayout->addWidget(precisionCombo);

    settingsLayout->addStretch();

    swapButton = new QPushButton("⇄ Поменять местами", this);
    settingsLayout->addWidget(swapButton);

    clearButton = new QPushButton("Очистить", this);
    settingsLayout->addWidget(clearButton);

    // Кнопка вычисления
    calculateButton = new QPushButton("ВЫЧИСЛИТЬ АЗИМУТ", this);
    calculateButton->setMinimumHeight(50);
    calculateButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #9C27B0;"
        "   color: white;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "   border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #7B1FA2;"
        "}"
    );

    // Информация об эллипсоиде
    ellipsoidLabel = new QLabel(
        "Эллипсоид: WGS84 (a=6378137 м, f=1/298.257223563)",
        this
    );
    ellipsoidLabel->setAlignment(Qt::AlignCenter);
    ellipsoidLabel->setStyleSheet("QLabel { color: #757575; font-size: 9px; }");

    // Основной макет
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(pasteGroup);
    mainLayout->addWidget(point1Group);
    mainLayout->addWidget(point2Group);
    mainLayout->addWidget(settingsGroup);
    mainLayout->addWidget(calculateButton);
    mainLayout->addWidget(resultGroup);
    mainLayout->addWidget(ellipsoidLabel);

    // Подключение сигналов
    connect(calculateButton, &QPushButton::clicked, this, &MainWindow::calculateAzimuth);
    connect(swapButton, &QPushButton::clicked, this, &MainWindow::swapPoints);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearFields);
    connect(precisionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::updatePrecision);
    connect(pasteToPoint1Button, &QPushButton::clicked, this, &MainWindow::pasteToPoint1);
    connect(pasteToPoint2Button, &QPushButton::clicked, this, &MainWindow::pasteToPoint2);

    // Исправленное подключение для QButtonGroup
    connect(inputModeGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
            [this](QAbstractButton *button) {
                int id = inputModeGroup->id(button);
                setInputMode(id);
            });

    // Установка примеров
    lat1Edit->setText("53.92730011265664");
    lon1Edit->setText("27.667402828338364");
    lat2Edit->setText("55.755831");
    lon2Edit->setText("37.617673");

    // Создание меню
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    QMenu *fileMenu = menuBar->addMenu("&Файл");
    fileMenu->addAction("&Выход", this, &QWidget::close, QKeySequence::Quit);

    QMenu *editMenu = menuBar->addMenu("&Правка");
    editMenu->addAction("&Копировать результат", [this]() {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(resultLabel->text() + "\n" +
                          reverseAzimuthLabel->text() + "\n" +
                          distanceLabel->text());
        statusBar()->showMessage("Результат скопирован", 2000);
    });

    QMenu *helpMenu = menuBar->addMenu("&Помощь");
    helpMenu->addAction("&О программе", this, &MainWindow::about);

    // Статус бар
    statusBar()->showMessage("Готов к вычислениям. Поддерживается вставка с карты");
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Новая функция для вставки из буфера
void MainWindow::onPasteButtonClicked()
{
    QString clipboardText = QApplication::clipboard()->text();
    if (clipboardText.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Буфер обмена пуст!");
        return;
    }
    bulkPasteEdit->setText(clipboardText);
    statusBar()->showMessage("Текст вставлен в поле массового ввода", 2000);

    if (autoPasteCheck->isChecked()) {
        auto pairs = parseMultiplePairs(clipboardText);
        if (pairs.size() >= 2) {
            lat1Edit->setText(QString::number(pairs[0].first, 'f', 12));
            lon1Edit->setText(QString::number(pairs[0].second, 'f', 12));
            lat2Edit->setText(QString::number(pairs[1].first, 'f', 12));
            lon2Edit->setText(QString::number(pairs[1].second, 'f', 12));
            statusBar()->showMessage("Автоматически распознаны 2 точки", 3000);
        } else if (pairs.size() == 1) {
            lat1Edit->setText(QString::number(pairs[0].first, 'f', 12));
            lon1Edit->setText(QString::number(pairs[0].second, 'f', 12));
            statusBar()->showMessage("Автоматически распознана 1 точка", 3000);
        }
    }
}

// Функции парсинга координат

QString MainWindow::cleanCoordinateString(const QString &text)
{
    QString cleaned = text;
    // Замена различных разделителей
    cleaned.replace(QRegularExpression("[,\\s;]+"), " ");
    // Замена градусных обозначений
    cleaned.replace("°", " ");
    cleaned.replace("'", " ");
    cleaned.replace("\"", " ");
    // Удаление лишних пробелов
    cleaned = cleaned.simplified();
    return cleaned;
}

bool MainWindow::parseDecimal(const QString &text, double &value)
{
    bool ok;
    value = text.toDouble(&ok);
    return ok;
}

bool MainWindow::parseDMS(const QString &text, double &degrees)
{
    // Парсинг формата: 53°55'38.28"N
    QRegularExpression dmsRegex(
        "([+-]?\\d+)[°\\s]+(\\d+)[′'\\s]+(\\d+\\.?\\d*)[″\"\\s]*([NSEWЮЗВС])?"
    );

    QRegularExpressionMatch match = dmsRegex.match(text);
    if (!match.hasMatch())
        return false;

    bool ok1, ok2, ok3;
    double d = match.captured(1).toDouble(&ok1);
    double m = match.captured(2).toDouble(&ok2);
    double s = match.captured(3).toDouble(&ok3);

    if (!ok1 || !ok2 || !ok3)
        return false;

    degrees = d + m/60.0 + s/3600.0;

    // Учет направления
    if (match.lastCapturedIndex() >= 4) {
        QString dir = match.captured(4);
        if (dir == "S" || dir == "W" || dir == "Ю" || dir == "З")
            degrees = -degrees;
    }

    return true;
}

bool MainWindow::parsePair(const QString &text, double &lat, double &lon)
{
    QString cleaned = cleanCoordinateString(text);
    QStringList parts = cleaned.split(" ", Qt::SkipEmptyParts);

    if (parts.size() < 2)
        return false;

    // Пробуем разные форматы для каждой части
    bool latOk = false, lonOk = false;

    // Сначала пробуем как десятичные
    lat = parts[0].toDouble(&latOk);
    if (parts.size() >= 2) {
        lon = parts[1].toDouble(&lonOk);
    }

    // Если не получилось, пробуем как DMS
    if (!latOk)
        latOk = parseDMS(parts[0], lat);
    if (!lonOk && parts.size() >= 2)
        lonOk = parseDMS(parts[1], lon);

    return latOk && lonOk;
}

QList<QPair<double, double>> MainWindow::parseMultiplePairs(const QString &text)
{
    QList<QPair<double, double>> results;
    QStringList lines = text.split("\n", Qt::SkipEmptyParts);

    for (const QString &line : lines) {
        double lat, lon;
        if (parsePair(line, lat, lon)) {
            results.append(qMakePair(lat, lon));
        }
    }

    return results;
}

Coordinates MainWindow::parseCoordinateString(const QString &text)
{
    Coordinates result = {0, 0, false, "unknown"};

    // Очистка строки
    QString cleaned = text.trimmed();

    // Проверка различных форматов
    QRegularExpression decimalPair("^[-+]?\\d+\\.\\d+\\s*[,;\\s]\\s*[-+]?\\d+\\.\\d+$");
    QRegularExpression dmsPair("^\\d+[°]\\d+[']\\d+\\.?\\d*[\"][NSEWЮЗВС]?\\s*[,;\\s]\\s*\\d+[°]\\d+[']\\d+\\.?\\d*[\"][NSEWЮЗВС]?$");

    if (decimalPair.match(cleaned).hasMatch()) {
        result.format = "decimal";
        result.isValid = parsePair(cleaned, result.latitude, result.longitude);
    }
    else if (dmsPair.match(cleaned).hasMatch()) {
        result.format = "dms";
        result.isValid = parsePair(cleaned, result.latitude, result.longitude);
    }
    else {
        // Пробуем общий парсинг
        result.isValid = parsePair(cleaned, result.latitude, result.longitude);
        result.format = result.isValid ? "auto" : "invalid";
    }

    return result;
}

void MainWindow::pasteToPoint1()
{
    QString clipboardText = QApplication::clipboard()->text();
    if (clipboardText.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Буфер обмена пуст!");
        return;
    }

    double lat, lon;
    if (parsePair(clipboardText, lat, lon)) {
        lat1Edit->setText(QString::number(lat, 'f', 12));
        lon1Edit->setText(QString::number(lon, 'f', 12));
        statusBar()->showMessage("Координаты вставлены в точку стояния", 2000);
    } else {
        // Если не получилось распарсить, вставляем в поле для массовой вставки
        bulkPasteEdit->setText(clipboardText);
        QMessageBox::information(this, "Формат не распознан",
                                "Не удалось распознать формат координат.\n"
                                "Текст вставлен в поле для массового ввода.");
    }
}

void MainWindow::pasteToPoint2()
{
    QString clipboardText = QApplication::clipboard()->text();
    if (clipboardText.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Буфер обмена пуст!");
        return;
    }

    double lat, lon;
    if (parsePair(clipboardText, lat, lon)) {
        lat2Edit->setText(QString::number(lat, 'f', 12));
        lon2Edit->setText(QString::number(lon, 'f', 12));
        statusBar()->showMessage("Координаты вставлены в объект", 2000);
    } else {
        bulkPasteEdit->setText(clipboardText);
        QMessageBox::information(this, "Формат не распознан",
                                "Не удалось распознать формат координат.\n"
                                "Текст вставлен в поле для массового ввода.");
    }
}

void MainWindow::setInputMode(int mode)
{
    QString hint;
    switch (mode) {
        case 0: // decimal
            hint = "Формат: 53.9273, 27.6674";
            break;
        case 1: // dms
            hint = "Формат: 53°55'38.28\"N, 27°40'2.64\"E";
            break;
        case 2: // both
            hint = "Любой формат (автоопределение)";
            break;
    }
    formatHintLabel->setText("<b>Режим:</b> " + hint);
    statusBar()->showMessage("Режим ввода изменен: " + hint, 2000);
}

QString MainWindow::formatDMS(double degrees, bool isLatitude) const
{
    char dir;
    if (isLatitude) {
        dir = degrees >= 0 ? 'N' : 'S';
    } else {
        dir = degrees >= 0 ? 'E' : 'W';
    }

    degrees = fabs(degrees);
    int d = static_cast<int>(degrees);
    double minutes = (degrees - d) * 60;
    int m = static_cast<int>(minutes);
    double seconds = (minutes - m) * 60;

    return QString("%1°%2'%3\"%4")
            .arg(d, 2, 10, QChar('0'))
            .arg(m, 2, 10, QChar('0'))
            .arg(seconds, 5, 'f', 2, QChar('0'))
            .arg(dir);
}

QString MainWindow::formatCoordinates(double lat, double lon, int precision, bool withComma) const
{
    return QString("%1%2%3")
        .arg(lat, 0, 'f', precision)
        .arg(withComma ? ", " : " ")
        .arg(lon, 0, 'f', precision);
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
    bulkPasteEdit->clear();

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
                       "<p><b>Особенности:</b></p>"
                       "<ul>"
                       "<li>Максимальная точность вычислений (до 12 знаков)</li>"
                       "<li>Формула Винсенти для эллипсоида WGS84</li>"
                       "<li>Прямой и обратный азимут</li>"
                       "<li>Геодезическое расстояние</li>"
                       "<li>8 направлений (румбы)</li>"
                       "<li>Поддержка вставки с карты (десятичные и DMS форматы)</li>"
                       "</ul>"
                       "<p>© 2024</p>");
}

void MainWindow::calculateAzimuth()
{
    // Проверка заполненности полей
    if (lat1Edit->text().isEmpty() || lon1Edit->text().isEmpty() ||
        lat2Edit->text().isEmpty() || lon2Edit->text().isEmpty()) {

        // Если основные поля пусты, пробуем взять из поля массовой вставки
        if (!bulkPasteEdit->toPlainText().isEmpty()) {
            auto pairs = parseMultiplePairs(bulkPasteEdit->toPlainText());
            if (pairs.size() >= 2) {
                lat1Edit->setText(QString::number(pairs[0].first, 'f', 12));
                lon1Edit->setText(QString::number(pairs[0].second, 'f', 12));
                lat2Edit->setText(QString::number(pairs[1].first, 'f', 12));
                lon2Edit->setText(QString::number(pairs[1].second, 'f', 12));
            } else if (pairs.size() == 1) {
                lat1Edit->setText(QString::number(pairs[0].first, 'f', 12));
                lon1Edit->setText(QString::number(pairs[0].second, 'f', 12));
                QMessageBox::information(this, "Информация",
                                        "Найдена одна пара координат.\n"
                                        "Она вставлена в точку стояния.\n"
                                        "Введите координаты объекта вручную.");
                return;
            } else {
                QMessageBox::warning(this, "Ошибка",
                                    "Не удалось распознать координаты!\n"
                                    "Проверьте формат ввода.");
                return;
            }
        } else {
            QMessageBox::warning(this, "Ошибка", "Заполните поля координат!");
            return;
        }
    }

    bool ok1, ok2, ok3, ok4;
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

    statusBar()->showMessage(
        QString("Точка1: %1 | Точка2: %2")
            .arg(formatCoordinates(lat1, lon1, 6, false))
            .arg(formatCoordinates(lat2, lon2, 6, false)),
        5000
    );
}
