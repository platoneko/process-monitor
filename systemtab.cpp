#include "mainwindow.h"
#include "./ui_mainwindow.h"

using namespace std;

void MainWindow::initSystemTab() {
    updateOsInfoLabel();
    updateCpuInfoGroup();
    updateHostnameGroup();
    updateLastLoginDatetimeLabel();
}

void MainWindow::updateOsInfoLabel() {
    string osrelease, ostype;
    ifstream infile;
    infile.open("/proc/sys/kernel/osrelease");
    getline(infile, osrelease);
    infile.close();
    infile.open("/proc/sys/kernel/ostype");
    getline(infile, ostype);
    infile.close();
    ostringstream out;
    out << osrelease << "\t" << ostype;
    ui->osInfoLabel->setText(out.str().c_str());
}

void MainWindow::updateCpuInfoGroup() {
    string buff, info;
    int cores, threads;
    ostringstream out;
    ifstream infile("/proc/cpuinfo");
    getline(infile, buff);  // processor
    getline(infile, buff);  // vendor_id
    getline(infile, buff);  // cpu family
    getline(infile, buff);  // model
    getline(infile, buff, ':');  // model name
    infile.get();
    getline(infile, info);
    ui->cpuInfoLabel->setText(info.c_str());
    getline(infile, buff);  // stepping
    getline(infile, buff);  // microcode
    getline(infile, buff);  // cpu MHz
    getline(infile, buff, ':');  // cache size
    infile.get();
    getline(infile, info);
    ui->cpuCacheInfoLabel->setText(("Cache\t" + info).c_str());
    getline(infile, buff);  // physical id
    getline(infile, buff, ':');  // siblings
    infile >> threads;
    getline(infile, buff);  // core id
    getline(infile, buff, ':');  // cpu cores
    infile >> cores;
    out << threads << " threads, " << cores << " cores";
    ui->cpuCoreInfoLabel->setText(out.str().c_str());
}

void MainWindow::updateHostnameGroup() {
    string hostname;
    ifstream infile("/proc/sys/kernel/hostname");
    getline(infile, hostname);
    ui->hostnameLabel->setText(hostname.c_str());
    ui->userLabel->setText(getpwuid(getuid())->pw_name);
    ui->groupLabel->setText(getgrgid(getgid())->gr_name);
}

void MainWindow::updateLastLoginDatetimeLabel() {
    struct utmp _utmp;
    int fd = open("/var/run/utmp", O_RDONLY);
    read(fd, &_utmp, sizeof(_utmp));
    ::close(fd);
    time_t loginDatetime = _utmp.ut_tv.tv_sec;
    char *formatLoginDateTime = ctime(&loginDatetime);
    formatLoginDateTime[strlen(formatLoginDateTime)-1] = 0;
    ui->lastLoginDatetimeLabel->setText(formatLoginDateTime);
}

void MainWindow::updateCurrentDatetimeLabel() {
    time_t currentDatetime = time(nullptr);
    char *formatDatetime = ctime(&currentDatetime);
    formatDatetime[strlen(formatDatetime)-1] = 0;
    ui->currentDatetimeLabel->setText(formatDatetime);
}

void MainWindow::on_shutdownButton_clicked() {
    shutdownDialog = new ShutdownDialog(false, this);
    shutdownDialog->setModal(true);
    shutdownDialog->show();
}

void MainWindow::on_rebootButton_clicked() {
    shutdownDialog = new ShutdownDialog(true, this);
    shutdownDialog->setModal(true);
    shutdownDialog->show();
}
