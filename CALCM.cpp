#include "CALCM.h"
#include <iostream>
#include <windows.h>
#include <QString>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QMessageBox>
#include <QDesktopServices>
#include <QStringConverter>
#include <QDateTime>
#include <QSaveFile>
#include <QEventLoop>
#include <QTimer>
#include <QProcess>
#include <QStandardPaths>
#include <QtGlobal>
#include <QClipboard>
#include <QKeyEvent>
using namespace std;

// 初始化CASDK类对象
CASDK2Ca200* objCa200 = nullptr;
CASDK2Cas* objCas = nullptr;
CASDK2Ca* objCa = nullptr;
CASDK2Probes* objProbes = nullptr;
CASDK2OutputProbes* objOutputProbes = nullptr;
CASDK2Probe* objProbe = nullptr;
CASDK2Memory* objMemory = nullptr;

CALCM::CALCM(QWidget *parent)
    : QWidget(parent), stopTesting(false)
{
    ui.setupUi(this);

// set defuat channel value
    ui.ChannelBx->setValue(4);
// KeyLocker
    ui.DisConnectBt->setEnabled(false);
    ui.ocalBt->setEnabled(false);
    ui.MeasureBt->setEnabled(false);
    ui.gamutBt->setEnabled(false);
    ui.gammaBt->setEnabled(false);
    ui.backlightBt->setEnabled(false);
    ui.exportBt->setEnabled(false);
    ui.tfcolorBt->setEnabled(false);
    ui.stopBt->setEnabled(false);
    ui.scrcpyBt->setEnabled(false);
    ui.clearBt->setEnabled(false);
    ui.delRowBt->setEnabled(false);
    ui.screenBrightnessBt->setEnabled(false);

// TableView
    // 初始化 Model
    tableModel = new QStandardItemModel(this);

    // 设置表头
    tableModel->setHorizontalHeaderLabels({ "RGB", "x", "y", "Lv", "TCP", "Brightness", "Notes"});

    // 将 Model 设置到 UI 文件中的 testDataTv
    ui.testDataTv->setModel(tableModel);

    // 设置表头字体和对齐方式
    QFont headerFont("Arial", 15, QFont::Bold);
    ui.testDataTv->horizontalHeader()->setFont(headerFont);
    ui.testDataTv->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);

    // 设置表头高度
    ui.testDataTv->horizontalHeader()->setFixedHeight(50);

    // 设置表头样式
    ui.testDataTv->setStyleSheet(
        "QHeaderView::section {"
        "    background-color: #AAAA00;" // 表头背景颜色
        "    color: black;"              // 表头文字颜色
        "    font-weight: bold;"         // 表头文字加粗
        "    border: 1px solid #dddddd;" // 表头边框颜色
        "    padding: 4px;"              // 表头内边距
        "    text-align: center;"        // 表头文字居中
        "}"
        "QHeaderView::section:hover {"
        "    background-color: #CCCC00;" // 鼠标悬停时的背景颜色
        "}"
    );

    // 隐藏垂直表头
    //ui.testDataTv->verticalHeader()->setVisible(false);

    // 可选：设置表格属性（如列宽自动调整）
    ui.testDataTv->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.testDataTv->setEditTriggers(QAbstractItemView::NoEditTriggers); // 禁止编辑
    //ui.testDataTv->setSelectionBehavior(QAbstractItemView::SelectRows); // 选择整行
    // 设置选择模式为单元格选择，并允许多选
    ui.testDataTv->setSelectionMode(QAbstractItemView::ExtendedSelection);
    //ui.testDataTv->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.testDataTv->setSelectionBehavior(QAbstractItemView::SelectItems);
}

CALCM::~CALCM()
{
    disConnect();
}

// 函数实现
/// 槽函数
void CALCM::on_ConnectBt_clicked()
{
    connect();
    ui.ConnectBt->setEnabled(false);

    ui.DisConnectBt->setEnabled(true);
    ui.ocalBt->setEnabled(true);
    ui.MeasureBt->setEnabled(true);
    ui.gamutBt->setEnabled(true);
    ui.gammaBt->setEnabled(true);
    ui.backlightBt->setEnabled(true);
    ui.exportBt->setEnabled(true);
    ui.tfcolorBt->setEnabled(true);
    ui.stopBt->setEnabled(true);
    ui.scrcpyBt->setEnabled(true);
    ui.clearBt->setEnabled(true);
    ui.delRowBt->setEnabled(true);
    ui.screenBrightnessBt->setEnabled(true);
}

void CALCM::on_DisConnectBt_clicked()
{
    // 弹出确认对话框
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("确认断开"), tr("您确定要断开连接吗？"),
        QMessageBox::Yes | QMessageBox::No);

    // 如果用户选择 "Yes"，执行断开操作
    if (reply == QMessageBox::Yes) {
        disConnect();

        // 禁用相关按钮
        ui.DisConnectBt->setEnabled(false);
        ui.ocalBt->setEnabled(false);
        ui.MeasureBt->setEnabled(false);
        ui.gamutBt->setEnabled(false);
        ui.gammaBt->setEnabled(false);
        ui.backlightBt->setEnabled(false);
        ui.exportBt->setEnabled(false);
        ui.tfcolorBt->setEnabled(false);
        ui.stopBt->setEnabled(false);
        ui.scrcpyBt->setEnabled(false);
        ui.clearBt->setEnabled(false);
        ui.delRowBt->setEnabled(false);
        ui.screenBrightnessBt->setEnabled(false);

        // 重新启用连接按钮
        ui.ConnectBt->setEnabled(true);
    }
}

void CALCM::on_ocalBt_clicked()
{
    ExeCalZero();
}

void CALCM::on_MeasureBt_clicked()
{
    ui.MeasureBt->setEnabled(false);
    normalMeasure("", "");
    ui.MeasureBt->setEnabled(true);

}

void CALCM::on_exportBt_clicked()
{
    ui.DisConnectBt->setEnabled(false);
    ui.ocalBt->setEnabled(false);
    ui.MeasureBt->setEnabled(false);
    ui.gamutBt->setEnabled(false);
    ui.gammaBt->setEnabled(false);
    ui.backlightBt->setEnabled(false);
    ui.tfcolorBt->setEnabled(false);
    ui.stopBt->setEnabled(false);
    ui.scrcpyBt->setEnabled(false);
    ui.clearBt->setEnabled(false);
    ui.delRowBt->setEnabled(false);
    ui.exportBt->setEnabled(false);
    ui.screenBrightnessBt->setEnabled(false);

    // 打开文件保存对话框
    //QString fileName = QFileDialog::getSaveFileName(this, tr("导出 CSV 文件"), "", tr("CSV 文件 (*.csv)"));
    
    // 获取当前用户桌面路径
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

    // 打开文件保存对话框，默认路径设置为桌面
    QString fileName = QFileDialog::getSaveFileName(this, tr("导出 CSV 文件"), desktopPath, tr("CSV 文件 (*.csv)"));

    if (fileName.isEmpty()) {
        return; // 如果用户取消操作，则直接返回
    }

    // 创建文件对象
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("错误"), tr("无法创建文件：%1").arg(fileName));
        return;
    }

    // 创建文本流
    QTextStream out(&file);

    // 写入表头，添加序号列
    QStringList headers;
    headers << tr("No. "); // 添加序号列
    for (int col = 0; col < tableModel->columnCount(); ++col) {
        headers << tableModel->headerData(col, Qt::Horizontal).toString();
    }
    out << headers.join(",") << "\n";

    // 写入数据
    for (int row = 0; row < tableModel->rowCount(); ++row) {
        QStringList rowData;
        rowData << QString::number(row + 1); // 添加序号，从 1 开始递增
        for (int col = 0; col < tableModel->columnCount(); ++col) {
            QStandardItem* item = tableModel->item(row, col);
            if (item) {
                rowData << item->text();
            }
            else {
                rowData << ""; // 如果单元格为空，写入空字符串
            }
        }
        out << rowData.join(",") << "\n";
    }

    file.close();

    // 提示用户导出成功
    QMessageBox::information(this, tr("成功"), tr("数据已成功导出到：%1").arg(fileName));
    
    ui.DisConnectBt->setEnabled(true);
    ui.ocalBt->setEnabled(true);
    ui.MeasureBt->setEnabled(true);
    ui.gamutBt->setEnabled(true);
    ui.gammaBt->setEnabled(true);
    ui.backlightBt->setEnabled(true);
    ui.tfcolorBt->setEnabled(true);
    ui.stopBt->setEnabled(true);
    ui.scrcpyBt->setEnabled(true);
    ui.clearBt->setEnabled(true);
    ui.delRowBt->setEnabled(true);
    ui.exportBt->setEnabled(true);
    ui.screenBrightnessBt->setEnabled(true);
}

void CALCM::on_gamutBt_clicked()
{
    ui.DisConnectBt->setEnabled(false);
    ui.ocalBt->setEnabled(false);
    ui.MeasureBt->setEnabled(false);
    ui.gamutBt->setEnabled(false);
    ui.gammaBt->setEnabled(false);
    ui.backlightBt->setEnabled(false);
    ui.tfcolorBt->setEnabled(false);
    ui.stopBt->setEnabled(true);
    ui.scrcpyBt->setEnabled(false);
    ui.clearBt->setEnabled(false);
    ui.delRowBt->setEnabled(false);
    ui.exportBt->setEnabled(false);
    ui.ChannelBx->setEnabled(false);
    ui.screenBrightnessBt->setEnabled(false);


    QStringList adbCommands = {
        // open purepicture app,
        "adb shell settings put system screen_brightness 255"
    };
    cmd(adbCommands);

    // r
    if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
    rgb(255, 0, 0);
    normalMeasure("255 0 0", "ColorGamut");
    double rx = 0.000;
    double ry = 0.000;
    rx = measure_data.x;
    ry = measure_data.y;
    // g
    if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
    rgb(0, 255, 0);
    normalMeasure("0 255 0", "ColorGamut");
    double gx = 0.000;
    double gy = 0.000;
    gx = measure_data.x;
    gy = measure_data.y;
    // b
    if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
    rgb(0, 0, 255);
    normalMeasure("0 0 255", "ColorGamut");
    double bx = 0.000;
    double by = 0.000;
    bx = measure_data.x;
    by = measure_data.y;
    // w
    if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
    rgb(255, 255, 255);
    normalMeasure("255 255 255", "ColorGamut");
    double wx = 0.000;
    double wy = 0.000;
    wx = measure_data.x;
    wy = measure_data.y;

    // Gamut Caculate
    QString gamutData = gamutCacu(rx, ry, gx, gy, bx, by);
    addItem(gamutData);

    // notice measure finish
    QMessageBox::information(this, tr("成功"), tr("色域测试已完成！"));
    ui.DisConnectBt->setEnabled(true);
    ui.ocalBt->setEnabled(true);
    ui.MeasureBt->setEnabled(true);
    ui.gamutBt->setEnabled(true);
    ui.gammaBt->setEnabled(true);
    ui.backlightBt->setEnabled(true);
    ui.tfcolorBt->setEnabled(true);
    ui.stopBt->setEnabled(true);
    ui.scrcpyBt->setEnabled(true);
    ui.clearBt->setEnabled(true);
    ui.delRowBt->setEnabled(true);
    ui.exportBt->setEnabled(true);
    ui.ChannelBx->setEnabled(true);
    ui.screenBrightnessBt->setEnabled(true);
}

void CALCM::on_gammaBt_clicked()
{
    ui.DisConnectBt->setEnabled(false);
    ui.ocalBt->setEnabled(false);
    ui.MeasureBt->setEnabled(false);
    ui.gamutBt->setEnabled(false);
    ui.gammaBt->setEnabled(false);
    ui.backlightBt->setEnabled(false);
    ui.tfcolorBt->setEnabled(false);
    ui.stopBt->setEnabled(true);
    ui.scrcpyBt->setEnabled(false);
    ui.clearBt->setEnabled(false);
    ui.delRowBt->setEnabled(false);
    ui.exportBt->setEnabled(false);
    ui.ChannelBx->setEnabled(false);
    ui.screenBrightnessBt->setEnabled(false);


    QStringList adbCommands = {
    // open purepicture app
    };
    cmd(adbCommands);

    int gammaRed = 0;

    int& gammaGreen = gammaRed;
    int& gammaBlue = gammaRed;

    for (gammaRed = 0; gammaRed <= 255; gammaRed++)
    {
        rgb(gammaRed, gammaGreen, gammaBlue);
        normalMeasure(
            QString::number(gammaRed) + " " + QString::number(gammaGreen) + " " + QString::number(gammaGreen),
            "Gray" + QString::number(gammaRed)
        );

        if (stopTesting) { // 检查是否需要终止测试
            stopTesting = false; // 重置标志，防止影响后续操作
            return; // 退出函数
        }
    }

    // notice measure finish
    QMessageBox::information(this, tr("成功"), tr("伽马测试已完成！"));
    ui.DisConnectBt->setEnabled(true);
    ui.ocalBt->setEnabled(true);
    ui.MeasureBt->setEnabled(true);
    ui.gamutBt->setEnabled(true);
    ui.gammaBt->setEnabled(true);
    ui.backlightBt->setEnabled(true);
    ui.tfcolorBt->setEnabled(true);
    ui.stopBt->setEnabled(true);
    ui.scrcpyBt->setEnabled(true);
    ui.clearBt->setEnabled(true);
    ui.delRowBt->setEnabled(true);
    ui.exportBt->setEnabled(true);
    ui.ChannelBx->setEnabled(true);
    ui.screenBrightnessBt->setEnabled(true);

}

void CALCM::on_scrcpyBt_clicked()
{
    // 获取 scrcpy.exe 的路径
    QString scrcpyPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/scrcpy.exe");

    // 检查 scrcpy.exe 是否存在
    if (!QFile::exists(scrcpyPath)) {
        QMessageBox::warning(this, tr("错误"), tr("无法找到 scrcpy 程序，请检查路径：%1").arg(scrcpyPath));
        return;
    }
    QProcess* scrcpyProcess = nullptr; // 用于管理 scrcpy 进程

    // 创建 QProcess 对象
    scrcpyProcess = new QProcess(this);
   
    // 启动 scrcpy.exe
    scrcpyProcess->start(scrcpyPath);

    // 检查是否启动成功
    if (!scrcpyProcess->waitForStarted()) {
        QMessageBox::warning(this, tr("错误"), tr("启动 scrcpy 失败，请检查配置！"));
        return;
    }
}

void CALCM::on_backlightBt_clicked()
{
    ui.DisConnectBt->setEnabled(false);
    ui.ocalBt->setEnabled(false);
    ui.MeasureBt->setEnabled(false);
    ui.gamutBt->setEnabled(false);
    ui.gammaBt->setEnabled(false);
    ui.backlightBt->setEnabled(false);
    ui.tfcolorBt->setEnabled(false);
    ui.stopBt->setEnabled(true);
    ui.scrcpyBt->setEnabled(false);
    ui.clearBt->setEnabled(false);
    ui.delRowBt->setEnabled(false);
    ui.exportBt->setEnabled(false);
    ui.ChannelBx->setEnabled(false);
    ui.screenBrightnessBt->setEnabled(false);


    QStringList adbCommands = {
        // open purepicture app
    };
    cmd(adbCommands);
    rgb(255, 255, 255);

    // 定义 adb 命令
    QString adbMaxBrightnessCmd = "adb shell cat /sys/class/leds/lcd-backlight/max_brightness";
    QString adbMinBrightnessCmd = "adb shell cat /sys/class/leds/lcd-backlight/min_brightness";
    QString adbSetBrightnessCmd = "adb shell echo %1 > /sys/class/leds/lcd-backlight/brightness";

    // 读取最大亮度
    int maxBrightness = cmdPro(adbMaxBrightnessCmd).trimmed().toInt();
    if (maxBrightness <= 0) {
        QMessageBox::warning(this, tr("错误"), tr("无法读取最大亮度值，请检查设备连接！"));
        return;
    }

    // 读取最小亮度
    int minBrightness = cmdPro(adbMinBrightnessCmd).trimmed().toInt();
    if (minBrightness < 0) {
        QMessageBox::warning(this, tr("错误"), tr("无法读取最小亮度值，请检查设备连接！"));
        return;
    }

    // 遍历亮度值
    for (int brightness = minBrightness; brightness <= maxBrightness; ++brightness) {
 
        if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
        // 设置亮度
        QString setBrightnessCmd = adbSetBrightnessCmd.arg(brightness);
        cmdPro(setBrightnessCmd);

        // 延迟一段时间，确保亮度设置生效
        delayMs(500);

        // 测量亮度
        QString item = "255 255 255";
        QString note = QString("Write BL %1").arg(brightness);
        normalMeasure(item, note);
    }

    // 提示用户完成
    QMessageBox::information(this, tr("成功"), tr("亮度曲线采集已完成！"));
    ui.DisConnectBt->setEnabled(true);
    ui.ocalBt->setEnabled(true);
    ui.MeasureBt->setEnabled(true);
    ui.gamutBt->setEnabled(true);
    ui.gammaBt->setEnabled(true);
    ui.backlightBt->setEnabled(true);
    ui.tfcolorBt->setEnabled(true);
    ui.stopBt->setEnabled(true);
    ui.scrcpyBt->setEnabled(true);
    ui.clearBt->setEnabled(true);
    ui.delRowBt->setEnabled(true);
    ui.exportBt->setEnabled(true);
    ui.ChannelBx->setEnabled(true);
    ui.screenBrightnessBt->setEnabled(true);

}

void CALCM::on_screenBrightnessBt_clicked()
{
    ui.DisConnectBt->setEnabled(false);
    ui.ocalBt->setEnabled(false);
    ui.MeasureBt->setEnabled(false);
    ui.gamutBt->setEnabled(false);
    ui.gammaBt->setEnabled(false);
    ui.backlightBt->setEnabled(false);
    ui.tfcolorBt->setEnabled(false);
    ui.stopBt->setEnabled(true);
    ui.scrcpyBt->setEnabled(false);
    ui.clearBt->setEnabled(false);
    ui.delRowBt->setEnabled(false);
    ui.exportBt->setEnabled(false);
    ui.ChannelBx->setEnabled(false);
    ui.screenBrightnessBt->setEnabled(false);


    QStringList preTestCMD = {
    // open purepicture app,
    "adb shell am broadcast - a com.tcl.colorcorrecting --ei r 255 --ei g 255 --ei b 255",
    "adb shell settings put system screen_brightness 1"
    };
    cmd(preTestCMD);
    delayMs(5000);

    // 遍历亮度值
    for (int ScreenBrightness = 1; ScreenBrightness <= 255; ScreenBrightness++)
    {
        QStringList adbCommands = {
            "adb shell settings put system screen_brightness " + QString::number(ScreenBrightness)
        };
        cmd(adbCommands);

        int _screenbl = getScreenBrightness();

        normalMeasure("255 255 255", "screenbl=" + QString::number(_screenbl));

        if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
    }


    ui.DisConnectBt->setEnabled(true);
    ui.ocalBt->setEnabled(true);
    ui.MeasureBt->setEnabled(true);
    ui.gamutBt->setEnabled(true);
    ui.gammaBt->setEnabled(true);
    ui.backlightBt->setEnabled(true);
    ui.tfcolorBt->setEnabled(true);
    ui.stopBt->setEnabled(true);
    ui.scrcpyBt->setEnabled(true);
    ui.clearBt->setEnabled(true);
    ui.delRowBt->setEnabled(true);
    ui.exportBt->setEnabled(true);
    ui.ChannelBx->setEnabled(true);
    ui.screenBrightnessBt->setEnabled(true);

}

void CALCM::on_tfcolorBt_clicked()
{
    ui.DisConnectBt->setEnabled(false);
    ui.ocalBt->setEnabled(false);
    ui.MeasureBt->setEnabled(false);
    ui.gamutBt->setEnabled(false);
    ui.gammaBt->setEnabled(false);
    ui.backlightBt->setEnabled(false);
    ui.tfcolorBt->setEnabled(false);
    ui.stopBt->setEnabled(true);
    ui.scrcpyBt->setEnabled(false);
    ui.clearBt->setEnabled(false);
    ui.delRowBt->setEnabled(false);
    ui.exportBt->setEnabled(false);
    ui.ChannelBx->setEnabled(false);
    ui.screenBrightnessBt->setEnabled(false);


    QStringList adbCommands = {
    // open purepicture app
    };
    cmd(adbCommands);

    // color 1 -- 255, 255, 255
    rgb(255, 255, 255);
    normalMeasure("255 255 255", "Color 1");

    // color 2 -- 230, 230, 230
    if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
    rgb(230, 230, 230);
    normalMeasure("230 230 230", "Color 2");

    // color 3 -- 209, 209, 209
    if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
    rgb(209, 209, 209);
    normalMeasure("209 209 209", "Color 3");

    // color 4 -- 186, 186, 186
    if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
    rgb(186, 186, 186);
    normalMeasure("186 186 186", "Color 4");

    // color 5 -- 158, 158, 158
    if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
    rgb(158, 158, 158);
    normalMeasure("158 158 158", "Color 5");

    // color 6 -- 0, 0, 0
    if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
    rgb(0, 0, 0);
    normalMeasure("0 0 0", "Color 6");

    // color 7 -- 115, 82, 66
    if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
    rgb(115, 82, 66);
    normalMeasure("115 82 66", "Color 7");

    // color 8 -- 194, 150, 130
    if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
    rgb(194, 150, 130);
    normalMeasure("194 150 130", "Color 8");

    // color 9 -- 94, 122, 156
    if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
    rgb(94, 122, 156);
    normalMeasure("94 122 156", "Color 9");

    // color 10 -- 89, 107, 66
    if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
    rgb(89, 107, 66);
    normalMeasure("89 107 66", "Color 10");
    
    // color 11 -- 130, 128, 176
    if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
    rgb(130, 128, 176);
    normalMeasure("130 128 176", "Color 11");

    // color 12 -- 99, 189, 168
    if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
    rgb(99, 189, 168);
    normalMeasure("99 189 168", "Color 12");

    // color 13 -- 217, 120, 41
    if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
    rgb(217, 120, 41);
    normalMeasure("217 120 41", "Color 13");

    // color 14 -- 74, 92, 163
    if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
    rgb(74, 92, 163);
    normalMeasure("74 92 163", "Color 14");

    // color 15 -- 194, 84, 97
    if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
    rgb(194, 84, 97);
    normalMeasure("194 84 97", "Color 15");

    // color 16 -- 92, 61, 107
    if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
    rgb(92, 61, 107);
    normalMeasure("92 61 107", "Color 16");

    // color 17 -- 158, 186, 64
    if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
    rgb(158, 186, 64);
    normalMeasure("158 186 64", "Color 17");

    // color 18 -- 230, 161, 46
    if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
    rgb(230, 161, 46);
    normalMeasure("230 161 46", "Color 18");

    // color 19 -- 51, 61, 150
    if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
    rgb(51, 61, 150);
    normalMeasure("51 61 150", "Color 19");

    // color 20 -- 51, 61, 150
    if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
    rgb(51, 61, 150);
    normalMeasure("51 61 150", "Color 20");

    // color 21 -- 176, 48, 59
    if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
    rgb(176, 48, 59);
    normalMeasure("176 48 59", "Color 21");

    // color 22 -- 237, 199, 33
    if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
    rgb(237, 199, 33);
    normalMeasure("237 199 33", "Color 22");

    // color 23 -- 186, 84, 145
    if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
    rgb(186, 84, 145);
    normalMeasure("186 84 145", "Color 23");

    // color 24 -- 0, 133, 163
    if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试
    rgb(0, 133, 163);
    normalMeasure("0 133 163", "Color 24");
    if (stopTesting) { stopTesting = false; return; } // 检查是否需要终止测试

    // notice measure finish
    QMessageBox::information(this, tr("成功"), tr("24色测试已完成！"));
    ui.DisConnectBt->setEnabled(true);
    ui.ocalBt->setEnabled(true);
    ui.MeasureBt->setEnabled(true);
    ui.gamutBt->setEnabled(true);
    ui.gammaBt->setEnabled(true);
    ui.backlightBt->setEnabled(true);
    ui.tfcolorBt->setEnabled(true);
    ui.stopBt->setEnabled(true);
    ui.scrcpyBt->setEnabled(true);
    ui.clearBt->setEnabled(true);
    ui.delRowBt->setEnabled(true);
    ui.exportBt->setEnabled(true);
    ui.ChannelBx->setEnabled(true);
    ui.screenBrightnessBt->setEnabled(true);

}

void CALCM::on_delRowBt_clicked()
{
    deleteSelectedOrLastRow();
    // 设置最新行的样式
    int lastRow = tableModel->rowCount() - 1; // 最新行的索引
    for (int col = 0; col < tableModel->columnCount(); ++col) {
        QStandardItem* item = tableModel->item(lastRow, col);
        if (item) {
            item->setBackground(QBrush(Qt::yellow)); // 设置背景颜色为黄色
            item->setForeground(QBrush(Qt::black)); // 设置字体颜色为黑色
            item->setFont(QFont("Arial", 10, QFont::Bold)); // 设置字体为加粗
        }
    }
}

void CALCM::on_clearBt_clicked()
{
    clearTableView();
}

void CALCM::on_stopBt_clicked()
{
    stopTesting = true; // 设置标志为 true，表示需要终止测试
    ui.DisConnectBt->setEnabled(true);
    ui.ocalBt->setEnabled(true);
    ui.MeasureBt->setEnabled(true);
    ui.gamutBt->setEnabled(true);
    ui.gammaBt->setEnabled(true);
    ui.backlightBt->setEnabled(true);
    ui.tfcolorBt->setEnabled(true);
    ui.stopBt->setEnabled(false);
    ui.scrcpyBt->setEnabled(true);
    ui.clearBt->setEnabled(true);
    ui.delRowBt->setEnabled(true);
    ui.exportBt->setEnabled(true);
    ui.ChannelBx->setEnabled(true);
    ui.screenBrightnessBt->setEnabled(true);

}

/// CA核心函数
void CALCM::GetErrorMessage(int errornum)
{
    wchar_t errormessage[100] = { 0 };
    unsigned long int size = 100;
    if (errornum)
    {
        //Get Error message from Error number
        err = CASDK2_GetLocalizedErrorMsgFromErrorCode(0, errornum, errormessage, &size);

        /// ***todo：做弹窗提示
        ///wcout << errormessage << endl;
    }
}

void CALCM::ExeCalZero()
{
    channelNum = ui.ChannelBx->value();
    GetErrorMessage(objMemory->put_ChannelNO(channelNum));	// 设置通道
    
    rgb(0, 0, 0);
    GetErrorMessage(objCa->CalZero());		// 校零
    rgb(255, 255, 255);
}

/// 功能函数
//// AutoConnect, 需传入通道值 channelNum
void CALCM::connect()
{
 // Android Connect
    deviceInit();

// CA410 disconncet
    //disConnect();

 // CA410 Connect   
    // Connect
    objCa200 = new CASDK2Ca200();	// 生成应用对象
    GetErrorMessage(objCa200->AutoConnect());

    // Substitute object variables(nesicery)
    GetErrorMessage(objCa200->get_Cas(&objCas));
    GetErrorMessage(objCa200->get_SingleCa(&objCa));
    GetErrorMessage(objCa->get_Probes(&objProbes));
    GetErrorMessage(objCa->get_OutputProbes(&objOutputProbes));
    GetErrorMessage(objCa->get_Memory(&objMemory));
    GetErrorMessage(objCa->get_SingleProbe(&objProbe));

    // Set MeasureMent Condituions
    //ExeCalZero();	// 校零
    GetErrorMessage(objCa->put_DisplayProbe("P1"));	// 配置探头为P1
    GetErrorMessage(objCa->put_SyncMode(ca_syncMode.ntsc));		// 配置同步模式，且仅当同步模式为INT时，需要添加频率参数ca_syncMode.int_freq，60Hz
    GetErrorMessage(objCa->put_AveragingMode(ca_measurementSpeed._auto));	// 配置测量速度

    // Calibrate
    GetErrorMessage(objCa->put_BrightnessUnit(ca_lvUnit.nit));	// 配置测量单位

    ExeCalZero();	// 校零
}

void CALCM::disConnect()
{
    Sleep(2);
    GetErrorMessage(objCa200->AutoDisconnect());

    delete objCa200;
    objCa200 = NULL;
}

void CALCM::normalMeasure(QString item, QString note)
{
    GetErrorMessage(objCa->put_DisplayMode(ca_dispMode.tcp_duv));
    GetErrorMessage(objCa->Measure());
    GetErrorMessage(objProbe->get_T(&measure_data.tcp));
    //GetErrorMessage(objProbe->get_duv(&measure_data.duv));

    GetErrorMessage(objCa->put_DisplayMode(ca_dispMode.lv_x_y));
    GetErrorMessage(objCa->Measure());
    GetErrorMessage(objProbe->get_Lv(&measure_data.Lv));
    GetErrorMessage(objProbe->get_sx(&measure_data.x));
    GetErrorMessage(objProbe->get_sy(&measure_data.y));

    measure_data.tcp = QString::number(measure_data.tcp, 'f', 0).toDouble();
    measure_data.Lv = QString::number(measure_data.Lv, 'f', 1).toDouble();
    measure_data.x = QString::number(measure_data.x, 'f', 3).toDouble();
    measure_data.y = QString::number(measure_data.y, 'f', 3).toDouble();

    int _brightness = getBrightness();
    //int _screenBrightness = getScreenBrightness();

    // 将数据插入到 TableModel 中
    QList<QStandardItem*> row;
    row.append(new QStandardItem(item));
    row.append(new QStandardItem(QString::number(measure_data.x)));
    row.append(new QStandardItem(QString::number(measure_data.y)));
    row.append(new QStandardItem(QString::number(measure_data.Lv)));

    if (qIsInf(measure_data.tcp))
    {
        row.append(new QStandardItem("-"));
    }
    else
    {
        row.append(new QStandardItem(QString::number(measure_data.tcp)));
    }

    row.append(new QStandardItem(QString::number(_brightness)));
    //row.append(new QStandardItem(QString::number(_screenBrightness)));
    row.append(new QStandardItem(note));

    tableModel->appendRow(row);

    // 强制刷新表格
    ui.testDataTv->repaint();

    // 恢复之前行的样式
    int previousRow = tableModel->rowCount() - 2; // 倒数第二行的索引
    if (previousRow >= 0) {
        for (int col = 0; col < tableModel->columnCount(); ++col) {
            QStandardItem* item = tableModel->item(previousRow, col);
            if (item) {
                item->setBackground(QBrush(Qt::white)); // 恢复背景颜色为白色
                item->setForeground(QBrush(Qt::black)); // 恢复字体颜色为黑色
                item->setFont(QFont("Arial", 10)); // 恢复字体样式
            }
        }
    }

    // 设置最新行的样式
    int lastRow = tableModel->rowCount() - 1; // 最新行的索引
    for (int col = 0; col < tableModel->columnCount(); ++col) {
        QStandardItem* item = tableModel->item(lastRow, col);
        if (item) {
            item->setBackground(QBrush(Qt::yellow)); // 设置背景颜色为黄色
            item->setForeground(QBrush(Qt::black)); // 设置字体颜色为黑色
            item->setFont(QFont("Arial", 10, QFont::Bold)); // 设置字体为加粗
        }
    }

    // 滚动到最后一行
    if (tableModel->rowCount() > 0) {
        ui.testDataTv->scrollTo(
            tableModel->index(tableModel->rowCount() - 1, 0),
            QAbstractItemView::PositionAtBottom
        );
    }
    delayMs(500);
}

void CALCM::flickerMeasure()
{
    GetErrorMessage(objCa->put_DisplayMode(ca_dispMode.jeita_flicker));
    GetErrorMessage(objCa->Measure());
    GetErrorMessage(objProbe->get_FlckrJEITA(&measure_data.flicker));

    GetErrorMessage(objCa->put_DisplayMode(ca_dispMode.lv_x_y));
}

QString CALCM::cmd(const QStringList& commands)
{
    QProcess process;
    QString output;

    // 设置不弹出命令窗口
    process.setProcessChannelMode(QProcess::MergedChannels); // 合并标准输出和标准错误
    process.setCreateProcessArgumentsModifier([](QProcess::CreateProcessArguments* args) {
        args->flags |= CREATE_NO_WINDOW; // 确保命令窗口不弹出
        });

    for (const QString& command : commands) {
        // 执行 adb 命令
        process.start(command);
        if (!process.waitForFinished(-1)) { // 等待命令执行完成，-1 表示无限等待
            return QString("Error: Failed to execute command: %1").arg(command);
        }

        // 获取命令输出
        output += process.readAll();
        output += "\n"; // 每条命令的输出用换行分隔
    }

    return output; // 返回所有命令的输出
}

QString CALCM::cmdPro(const QString& command)
{
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels); // 合并标准输出和标准错误
    process.start(command);

    if (!process.waitForFinished(-1)) { // 等待命令执行完成，-1 表示无限等待
        return QString("Error: Failed to execute command: %1").arg(command);
    }

    // 返回命令输出
    return process.readAll().trimmed();
}

void CALCM::rgb(int r, int g, int b)
{
    QStringList rgbCommands = {
    "adb shell am broadcast -a com.tcl.colorcorrecting --ei r " + QString::number(r) + " --ei g " + QString::number(g) + " --ei b " +QString::number(b)
    };
    cmd(rgbCommands);
    delayMs(500);
}

void CALCM::delayMs(int milliseconds)
{
    QEventLoop loop;
    QTimer::singleShot(milliseconds, &loop, &QEventLoop::quit);
    loop.exec(); // 进入事件循环，直到计时器超时
}

QString CALCM::gamutCacu(double rx, double ry, double gx, double gy, double bx, double by)
{
    double _gamutNum = 0.000;
    _gamutNum = (rx * gy + gx * by + bx * ry - ry * gx - gy * bx - by * rx) / 2;
    // NTSC
    double _ntsc = 0.000;
    _ntsc = _gamutNum / 0.1582;
    QString ntsc_qst = "NTSC Gamut is "+QString::number(_ntsc * 100, 'f', 2) + "%";
    // SRGB
    double _srgb = 0.000;
    _srgb = _gamutNum / 0.11205;
    QString srgb_qst = "SRGB Gamut is " + QString::number(_srgb * 100, 'f', 2) + "%";

    QString Gamut_Qst = ntsc_qst + " , " + srgb_qst;
    return Gamut_Qst;
}

void CALCM::addItem(QString newContent)
{
    //// 将数据插入到 TableModel 中
    //QList<QStandardItem*> row;
    //row.append(new QStandardItem(newContent));
    ////row.append(new QStandardItem(QString内容));

    //tableModel->appendRow(row);

    // 插入一行
    int newRow = tableModel->rowCount();
    tableModel->insertRow(newRow);

    // 合并当前行的所有单元格
    ui.testDataTv->setSpan(newRow, 0, 1, tableModel->columnCount());

    // 设置合并单元格的内容
    QStandardItem* item = new QStandardItem(newContent);
    item->setTextAlignment(Qt::AlignCenter); // 居中显示
    QFont font("Arial", 12, QFont::Bold);   // 设置字体样式
    item->setFont(font);
    tableModel->setItem(newRow, 0, item);

    // 强制刷新表格
    ui.testDataTv->repaint();


    // 恢复之前行的样式
    int previousRow = tableModel->rowCount() - 2; // 倒数第二行的索引
    if (previousRow >= 0) {
        for (int col = 0; col < tableModel->columnCount(); ++col) {
            QStandardItem* item = tableModel->item(previousRow, col);
            if (item) {
                item->setBackground(QBrush(Qt::white)); // 恢复背景颜色为白色
                item->setForeground(QBrush(Qt::black)); // 恢复字体颜色为黑色
                item->setFont(QFont("Arial", 10)); // 恢复字体样式
            }
        }
    }

    // 设置最新行的样式
    int lastRow = tableModel->rowCount() - 1; // 最新行的索引
    for (int col = 0; col < tableModel->columnCount(); ++col) {
        QStandardItem* item = tableModel->item(lastRow, col);
        if (item) {
            item->setBackground(QBrush(Qt::yellow)); // 设置背景颜色为黄色
            item->setForeground(QBrush(Qt::black)); // 设置字体颜色为黑色
            item->setFont(QFont("Arial", 10, QFont::Bold)); // 设置字体为加粗
        }
    }

    // 滚动到最后一行
    if (tableModel->rowCount() > 0) {
        ui.testDataTv->scrollTo(
            tableModel->index(tableModel->rowCount() - 1, 0),
            QAbstractItemView::PositionAtBottom
        );
    }
}

int CALCM::getBrightness()
{
    QStringList adbCommands = {
        "adb shell cat /sys/class/leds/lcd-backlight/brightness"
    };
    QString brightness_Qst = cmd(adbCommands);

    int brightness = brightness_Qst.toInt();
    return brightness;
}

int CALCM::getScreenBrightness()
{
    QStringList adbCommands = {
        "adb shell settings get system screen_brightness"
    };
    QString screenBrightness_Qst = cmd(adbCommands);

    int screenBrightness = screenBrightness_Qst.toInt();
    return screenBrightness;
}

// 捕获键盘事件，添加复制功能
void CALCM::keyPressEvent(QKeyEvent* event)
{
    if (event->matches(QKeySequence::Copy)) {
        copySelectedCells();
    }
    else {
        QWidget::keyPressEvent(event); // 传递给父类处理
    }
}

// 复制选中单元格内容到剪贴板
void CALCM::copySelectedCells()
{
    QItemSelectionModel* selectionModel = ui.testDataTv->selectionModel();
    if (!selectionModel) return;

    QModelIndexList selectedIndexes = selectionModel->selectedIndexes();
    if (selectedIndexes.isEmpty()) return;

    // 按行排序选中的单元格
    std::sort(selectedIndexes.begin(), selectedIndexes.end());

    QString copiedText;
    int currentRow = selectedIndexes.first().row();

    for (const QModelIndex& index : selectedIndexes) {
        if (index.row() != currentRow) {
            copiedText.append('\n'); // 换行
            currentRow = index.row();
        }
        else if (!copiedText.isEmpty()) {
            copiedText.append('\t'); // 单元格之间用制表符分隔
        }
        copiedText.append(index.data().toString());
    }

    // 将内容复制到剪贴板
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(copiedText);
}

void CALCM::deleteSelectedOrLastRow()
{
    QItemSelectionModel* selectionModel = ui.testDataTv->selectionModel();
    if (!selectionModel) return;

    // 获取选中的单元格
    QModelIndexList selectedIndexes = selectionModel->selectedIndexes();

    if (!selectedIndexes.isEmpty()) {
        // 如果有选中的单元格，提取这些单元格所在的行
        QSet<int> rowsToDelete; // 使用 QSet 确保行号唯一
        for (const QModelIndex& index : selectedIndexes) {
            rowsToDelete.insert(index.row());
        }

        // 将行号从大到小排序，避免索引错乱
        QList<int> sortedRows = rowsToDelete.values();
        std::sort(sortedRows.begin(), sortedRows.end(), std::greater<int>());

        // 删除选中的行
        for (int row : sortedRows) {
            tableModel->removeRow(row);
        }
    }
    else {
        // 如果没有选中任何单元格，删除最后一行
        int lastRow = tableModel->rowCount() - 1;
        if (lastRow >= 0) {
            tableModel->removeRow(lastRow);
        }
    }
}

void CALCM::clearTableView()
{
    // 清空所有行
    tableModel->clear();

    // 重新设置表头
    tableModel->setHorizontalHeaderLabels({ "RGB", "x", "y", "Lv", "TCP", "Brightness", "Notes" });
}

void CALCM::deviceInit()
{
    QStringList adbCommands = {
        "adb shell getprop persist.sys.brightness_max"
    };
    QString brightness = cmd(adbCommands);


    QString apkPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/ColorCorrecting.apk");
    QStringList adbCommands1 = {
        "adb root",
        "adb remount",
        "adb shell settings put system screen_brightness_mode 0 ",
        "adb shell settings put system screen_off_timeout 2147483647",
        "adb shell settings put system screen_brightness" + brightness,
        "adb install " + apkPath,
        // open purepicture app
    };
    cmd(adbCommands1);
}

