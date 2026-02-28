#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QDoubleValidator>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolTip>
#include <QClipboard>
#include <QApplication>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QTextEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QMenuBar>
#include <QMenu>
#include <cmath>
#include <limits>
#include <QPair>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// Структура для хранения координат
struct Coordinates {
    double latitude;
    double longitude;
    bool isValid;
    QString format; // формат исходной строки
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void calculateAzimuth();
    void updatePrecision();
    void swapPoints();
    void clearFields();
    void about();
    void pasteToPoint1();
    void pasteToPoint2();
    void setInputMode(int mode);
    void onPasteButtonClicked();

private:
    Ui::MainWindow *ui;

    // Компоненты интерфейса
    QLineEdit *lat1Edit;
    QLineEdit *lon1Edit;
    QLineEdit *lat2Edit;
    QLineEdit *lon2Edit;
    QComboBox *precisionCombo;
    QLabel *resultLabel;
    QLabel *reverseAzimuthLabel;
    QLabel *distanceLabel;
    QLabel *ellipsoidLabel;
    QLabel *formatHintLabel;
    QPushButton *calculateButton;
    QPushButton *swapButton;
    QPushButton *clearButton;
    QPushButton *pasteToPoint1Button;
    QPushButton *pasteToPoint2Button;
    QRadioButton *decimalRadio;
    QRadioButton *dmsRadio;
    QRadioButton *bothRadio;
    QButtonGroup *inputModeGroup;
    QCheckBox *autoPasteCheck;

    // Текстовое поле для вставки координат
    QTextEdit *bulkPasteEdit;

    // Валидаторы
    QDoubleValidator *latValidator;
    QDoubleValidator *lonValidator;

    // Функции вычисления
    double calculateAzimuthWGS84(double lat1, double lon1, double lat2, double lon2,
                                  double &distance, double &reverseAzimuth);

    // Вспомогательные функции
    double toRadians(double degrees) const;
    double toDegrees(double radians) const;
    double normalizeAzimuth(double azimuth) const;
    QString formatDirection(double azimuth) const;
    QString formatDistance(double distance, int precision) const;
    QString formatCoordinates(double lat, double lon, int precision, bool withComma = true) const;
    QString formatDMS(double degrees, bool isLatitude) const;

    // Функции парсинга координат
    Coordinates parseCoordinateString(const QString &text);
    bool parsePair(const QString &text, double &lat, double &lon);
    bool parseDMS(const QString &text, double &degrees);
    bool parseDecimal(const QString &text, double &value);
    QString cleanCoordinateString(const QString &text);
    QList<QPair<double, double>> parseMultiplePairs(const QString &text);

    // Формула Винсенти для максимальной точности
    int vincentyInverse(double lat1, double lon1, double lat2, double lon2,
                        double &azimuth1, double &azimuth2, double &distance);

    // Параметры эллипсоида WGS84
    static constexpr double WGS84_A = 6378137.0;
    static constexpr double WGS84_F = 1.0 / 298.257223563;
    static constexpr double WGS84_B = WGS84_A * (1 - WGS84_F);
    static constexpr double WGS84_E2 = 2 * WGS84_F - WGS84_F * WGS84_F;

    // Константы точности
    static constexpr double EPSILON = 1e-12;
    static constexpr int MAX_ITERATIONS = 200;
};

#endif // MAINWINDOW_H
