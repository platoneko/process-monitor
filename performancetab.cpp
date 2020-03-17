#include "mainwindow.h"
#include "./ui_mainwindow.h"


void MainWindow::initPerformanceTab() {
    cpuChart->legend()->hide();
    cpuChart->addSeries(cpuSeries);
    cpuChart->createDefaultAxes();
    cpuChart->axisX()->setRange(1, POINT_NUM);
    cpuChart->axisY()->setRange(0, 1);
    cpuChart->axisX()->hide();
    ui->cpuChartView->setChart(cpuChart);
    ui->cpuChartView->setRenderHint(QPainter::Antialiasing);

    memChart->legend()->hide();
    memChart->addSeries(memSeries);
    memChart->createDefaultAxes();
    memChart->axisX()->setRange(1, POINT_NUM);
    memChart->axisY()->setRange(0, 1);
    memChart->axisX()->hide();
    ui->memChartView->setChart(memChart);
    ui->memChartView->setRenderHint(QPainter::Antialiasing);

    swapChart->legend()->hide();
    swapChart->addSeries(swapSeries);
    swapChart->createDefaultAxes();
    swapChart->axisX()->setRange(1, POINT_NUM);
    swapChart->axisY()->setRange(0, 1);
    swapChart->axisX()->hide();
    ui->swapChartView->setChart(swapChart);
    ui->swapChartView->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::updateCpuChart() {
    cpuHistory->erase(cpuHistory->begin());
    cpuHistory->push_back(cpuUsed/100);
    cpuSeries->clear();
    int x = 1;
    for (auto &it: *cpuHistory) {
        cpuSeries->append(x, qreal(it));
        ++x;
    }
}

void MainWindow::updateMemChart() {
    char buf[MAXLINE];
    sprintf(buf, "(MiB) %7.1f total, %7.1f free, %7.1f shared, %7.1f buffer",
            double(currentSysinfo.totalram>>10)/1024,
            double(currentSysinfo.freeram>>10)/1024,
            double(currentSysinfo.sharedram>>10)/1024,
            double(currentSysinfo.bufferram>>10)/1024);
    ui->memDetailLabel->setText(buf);
    memHistory->erase(memHistory->begin());
    memHistory->push_back(1 - float(currentSysinfo.freeram) / currentSysinfo.totalram);
    memSeries->clear();
    int x = 1;
    for (auto &it: *memHistory) {
        memSeries->append(x, qreal(it));
        ++x;
    }
}

void MainWindow::updateSwapChart() {
    char buf[MAXLINE];
    sprintf(buf, "(MiB) %7.1f total, %7.1f free",
            double(currentSysinfo.totalswap>>10)/1024,
            double(currentSysinfo.freeswap>>10)/1024);
    ui->swapDetailLabel->setText(buf);
    swapHistory->erase(swapHistory->begin());
    swapHistory->push_back(1 - float(currentSysinfo.freeswap) / currentSysinfo.totalswap);
    swapSeries->clear();
    int x = 1;
    for (auto &it: *swapHistory) {
        swapSeries->append(x, qreal(it));
        ++x;
    }
}
