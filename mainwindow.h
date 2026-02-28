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
#include <QMessageBox>
#include <QStatusBar>
#include <QToolTip>
#include <cmath>
#include <limits>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

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
    QPushButton *calculateButton;
    QPushButton *swapButton;
    QPushButton *clearButton;
    
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
    
    // Формула Винсенти для максимальной точности
    int vincentyInverse(double lat1, double lon1, double lat2, double lon2,
                        double &azimuth1, double &azimuth2, double &distance);
    
    // Параметры эллипсоида WGS84
    static constexpr double WGS84_A = 6378137.0;           // большая полуось (м)
    static constexpr double WGS84_F = 1.0 / 298.257223563; // сжатие
    static constexpr double WGS84_B = WGS84_A * (1 - WGS84_F); // малая полуось (м)
    static constexpr double WGS84_E2 = 2 * WGS84_F - WGS84_F * WGS84_F; // квадрат эксцентриситета
    
    // Константы точности
    static constexpr double EPSILON = 1e-12;
    static constexpr int MAX_ITERATIONS = 200;
};

#endif // MAINWINDOW_H