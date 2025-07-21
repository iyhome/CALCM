#pragma once
#pragma execution_character_set("utf-8")  // 仅用于MSVC编译器

#include <QtWidgets/QWidget>
#include "ui_CALCM.h"
#include "CASDK2.h"
#include <QTableView>
#include <QVBoxLayout>
#include <QStandardItemModel>


// 声明CASDK对象
extern CASDK2Ca200* objCa200;
extern CASDK2Cas* objCas;
extern CASDK2Ca* objCa;
extern CASDK2Probes* objProbes;
extern CASDK2OutputProbes* objOutputProbes;
extern CASDK2Probe* objProbe;
extern CASDK2Memory* objMemory;


class CALCM : public QWidget
{
    Q_OBJECT

public:
    CALCM(QWidget *parent = nullptr);
    ~CALCM();

// 核心函数
    void GetErrorMessage(int errornum);
    void ExeCalZero();
    // void SetZeroCalEvent();

// 功能函数
    void connect();
    void normalMeasure(QString item, QString note);
    void flickerMeasure();
    void disConnect();
    QString cmd(const QStringList&);
    QString cmdPro(const QString&);
    void rgb(int, int, int);
    void delayMs(int);
    void addItem(QString);
    QString gamutCacu(double, double, double, double, double, double);
    int getScreenBrightness();
    int getBrightness();
    void copySelectedCells();
    void keyPressEvent(QKeyEvent*);
    void deleteSelectedOrLastRow();
    void clearTableView();
    void deviceInit();

private slots:
    void on_ConnectBt_clicked();
    void on_DisConnectBt_clicked();
    void on_MeasureBt_clicked();
    void on_ocalBt_clicked();
    void on_exportBt_clicked();
    void on_gamutBt_clicked();
    void on_gammaBt_clicked();
    void on_tfcolorBt_clicked();
    void on_backlightBt_clicked();
    void on_screenBrightnessBt_clicked();
    void on_scrcpyBt_clicked();
    void on_delRowBt_clicked();
    void on_clearBt_clicked();
    void on_stopBt_clicked();
private:
    Ui::CALCMClass ui;

    bool stopTesting; // 标志变量，用于控制测试是否终止

// TableView
    QTableView* tableView;               // 用于显示测量数据
    QStandardItemModel* tableModel;      // 数据模型

// CASDK
    int err = 0;
    int channelNum = 0;

    struct data
    {
        double Lv;
        double x;
        double y;
        double tcp;
        double duv;
        double u;
        double v;
        double flicker;
    } measure_data = {
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
    };

    struct dispMode
    {
        int lv_x_y;
        int tcp_duv;
        int u_v_lv;
        int jeita_flicker;
        int vd_Pe_Lv;
        int waveform;
    } ca_dispMode = {
        0, 1, 5, 8, 9, 10
    };

    struct syncMode {
        int ntsc;
        int pal;
        int ext;
        int univ;
        int int_;
        int manual;
        double int_freq; //40.0 to 240.0 in INT
    } ca_syncMode = {
        0, 1, 2, 3, 4, 5, 60.0
    };

    struct measurementSpeed {
        int slow;
        int fast;
        int ltd_auto;
        int _auto;
    } ca_measurementSpeed = {
        0, 1, 2, 3
    };

    struct lvUnit
    {
        int fl; //
        int nit; // cd/m^2
    } ca_lvUnit = {
        0, 1
    };

};


