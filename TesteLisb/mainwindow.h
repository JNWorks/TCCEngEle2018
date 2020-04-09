#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

class QCPGraph;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    void graph(QCustomPlot *widget);

    ~MainWindow();

    static const int sc = 100;

    void sensorInfo();

    void armazenaMudanca();

private slots:
  void realtimeDataSlot();

  void on_checkBox_toggled(bool checked);

  void on_SerialBox_currentIndexChanged(const QString &arg1);

  void on_Pause_clicked(bool checked);

  void on_RangeDefSlider_valueChanged(int value);

  void on_RangeDefNumber_textChanged(const QString &arg1);

  void on_RangeDefNumber_returnPressed();

  void on_RangeDefNumber_selectionChanged();

  void on_AmoDefSlider_valueChanged(int value);

  void on_AmoDefNumber_textChanged(const QString &arg1);

  void on_AmoDefNumber_returnPressed();

  void on_MainWindow_iconSizeChanged(const QSize &iconSize);

private:
    Ui::MainWindow *ui;
    QTimer dataTimer;
    int cont = 0;
    bool flag = false;
    double current = 0;
    double key = 0;
    QSerialPort serial;
    QString port_name;
    bool passa[8] = {false, false, false, false, false, false, false, false};
    bool passa2[2] = {false, false};
    double defasagem = 0;
    bool pause = false;
    int TDefasagem = 0;
    int Range = 2;
    QString TolDefstr = "10";
    int TDefAux = 0;
    int contaexec = 0;
    double media = 0;
    int Rangeaux = 0;
    int Amostras = 5;
    int Amostrasaux = 5;
    bool resetgraph = false;
    double mediaanterior = 0;
};

#endif // MAINWINDOW_H
