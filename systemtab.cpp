#include "mainwindow.h"
#include "./ui_mainwindow.h"


void MainWindow::initSystemTab() {
    updateOsInfoLabel();
    updateCpuInfoGroup();
    updateHostnameGroup();
    updateLastLoginDatetimeLabel();
}

void MainWindow::updateOsInfoLabel() {
    char buf1[MAXLINE], buf2[MAXLINE];
    FILE *fp;
    fp = fopen("/proc/sys/kernel/osrelease", "r");
    fgets(buf1, MAXLINE, fp);
    fclose(fp);
    buf1[strlen(buf1)-1] = 0;
    fp = fopen("/proc/sys/kernel/ostype", "r");
    fgets(buf2, MAXLINE, fp);
    fclose(fp);
    buf2[strlen(buf2)-1] = 0;
    sprintf(buf1, "%s\t%s", buf1, buf2);
    ui->osInfoLabel->setText(buf1);
}

void MainWindow::updateCpuInfoGroup() {
    char buf1[MAXLINE], buf2[MAXLINE];
    int cores, threads;
    FILE *fp;
    fp = fopen("/proc/cpuinfo", "r");
    fgets(buf1, MAXLINE, fp);  // processor
    fgets(buf1, MAXLINE, fp);  // vendor_id
    fgets(buf1, MAXLINE, fp);  // cpu family
    fgets(buf1, MAXLINE, fp);  // model
    fgets(buf1, MAXLINE, fp);  // model name
    strtok(buf1, "\t");
    strcpy(buf2, strtok(nullptr, "\t")+2);
    buf2[strlen(buf2)-1] = 0;
    ui->cpuInfoLabel->setText(buf2);
    fgets(buf1, MAXLINE, fp);  // stepping
    fgets(buf1, MAXLINE, fp);  // microcode
    fgets(buf1, MAXLINE, fp);  // cpu MHz
    fgets(buf1, MAXLINE, fp);  // cache size
    strtok(buf1, "\t");
    strcpy(buf2, strtok(nullptr, "\t")+2);
    buf2[strlen(buf2)-1] = 0;
    sprintf(buf1, "Cache %s", buf2);
    ui->cpuCacheInfoLabel->setText(buf1);
    fgets(buf1, MAXLINE, fp);  // physical id
    fgets(buf1, MAXLINE, fp);  // siblings
    strtok(buf1, "\t");
    strcpy(buf2, strtok(nullptr, "\t")+2);
    buf2[strlen(buf2)-1] = 0;
    threads = atoi(buf2);
    fgets(buf1, MAXLINE, fp);  // core id
    fgets(buf1, MAXLINE, fp);  // cpu cores
    strtok(buf1, "\t");
    strcpy(buf2, strtok(nullptr, "\t")+1);
    buf2[strlen(buf2)-1] = 0;
    cores = atoi(buf2);
    sprintf(buf1, "%d cores, %d threads", cores, threads);
    ui->cpuCoreInfoLabel->setText(buf1);
    fclose(fp);
}

void MainWindow::updateHostnameGroup() {
    char buf[MAXLINE];
    FILE *fp;

    fp = fopen("/proc/sys/kernel/hostname", "r");
    fgets(buf, MAXLINE, fp);
    fclose(fp);
    buf[strlen(buf)-1] = 0;
    ui->hostnameLabel->setText(buf);

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
