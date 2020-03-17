#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    timer(new QTimer),
    model(new QStandardItemModel),
    cpuHistory(new std::list<float>(POINT_NUM, 0.0)),
    cpuSeries(new QLineSeries),
    cpuChart(new QChart),
    memHistory(new std::list<float>(POINT_NUM, 0.0)),
    memSeries(new QLineSeries),
    memChart(new QChart) ,
    swapHistory(new std::list<float>(POINT_NUM, 0.0)),
    swapSeries(new QLineSeries),
    swapChart(new QChart)
{
    dir_ptr = opendir("/proc");
    chdir("/proc");

    FILE *fp;
    char pid_max[MAXLINE];
    fp = fopen("sys/kernel/pid_max", "r");
    fgets(pid_max, MAXLINE, fp);
    fclose(fp);
    this->pid_max = atoi(pid_max);

    ui->setupUi(this);
    this->setWindowTitle("Process Monitor");

    initMainTab();
    initSystemTab();
    initPerformanceTab();
    update();

    connect(timer, &QTimer::timeout, this, &MainWindow::update);
    timer->start(3000);
}

MainWindow::~MainWindow()
{
    closedir(dir_ptr);
    delete ui;
    delete timer;
    delete model;
    delete cpuHistory;
    delete cpuSeries;
    delete cpuChart;
}

void MainWindow::update() {
    updateFree.acquire();
    updateSysinfo();
    updateTaskInfo();
    updateMemBar();
    updateCpuBar();
    updateLoadAverageLabel();
    updateUptimeLabel();
    updateTasksLabel();
    updateTaskTable();
    updateCurrentDatetimeLabel();
    updateCpuChart();
    updateMemChart();
    updateSwapChart();
    updateFree.release();
}

void MainWindow::updateTaskInfo() {
    FILE *fp;
    struct dirent *dir_entry;
    char buf[MAXLINE], line[MAXLINE];

    unsigned short uid, pid, ppid;
    char priority, nice;
    int  pgrp, session, tty_nr, tpgid;
    unsigned long minflt, cminflt, majflt, cmajflt, utime, stime;
    long cutime, cstime, num_threads;
    unsigned int flags;
    char state, comm[MAXLINE];

    // char command[MAXLINE];

    unsigned long virt, res, shr;

    struct timeval tv;
    gettimeofday(&tv, nullptr);
    time_t currDatetime = tv.tv_sec * 1000000 + tv.tv_usec;
    timeInterval = currDatetime - lastDatetime;
    lastDatetime = currDatetime;

    cpuUsed = 0.0;
    taskTotal = taskRunning = taskSleeping = taskStopped = taskZombie = 0;
    matchedTaskTotal = 0;
    while ((dir_entry = readdir(dir_ptr))) {
        if (isNumeric(dir_entry->d_name)) {
            ++taskTotal;
            chdir(dir_entry->d_name);
            fp = fopen("status", "r");
            for (int j = 0; j < 9; ++j) {
                fgets(buf, MAXLINE, fp);
            }
            fclose(fp);
            sscanf(buf, "%s%hu", line, &uid);

            fp = fopen("stat", "r");
            fgets(buf, MAXLINE, fp);
            sscanf(buf, "%hu%s %c%hu%d%d%d%d%u%lu%lu%lu%lu%lu%lu%ld%ld%c%c%ld",
            &pid, comm, &state,
            &ppid, &pgrp, &session, &tty_nr, &tpgid, &flags,
            &minflt, &cminflt, &majflt, &cmajflt,
            &utime, &stime, &cutime, &cstime,
            &priority, &nice,
            &num_threads);
            fclose(fp);

            fp = fopen("statm", "r");
            fgets(buf, MAXLINE, fp);
            sscanf(buf, "%lu%lu%lu", &virt, &res, &shr);
            fclose(fp);
            virt <<= 2;
            res <<= 2;
            shr <<= 2;

            /*
            fp = fopen("cmdline", "r");
            memset(buf, 0, MAXLINE);
            fgets(buf, MAXLINE, fp);
            fclose(fp);
            formatCommand(buf, command);
            */

            switch (state) {
            case 'R':
                ++taskRunning;
                break;
            case 'S':
                ++taskSleeping;
                break;
            case 'T':
                ++taskStopped;
                break;
            case 'Z':
                ++taskZombie;
                break;
            case 'D':
                ++taskSleeping;
                break;
            case 'I':
                ++taskSleeping;
                break;
            default:
                break;
            }

            if (taskInfoDict.count(pid)) {
                TaskInfo *taskInfo = taskInfoDict[pid];
                taskInfo->ppid = ppid;
                taskInfo->uid = uid;
                taskInfo->pri = priority;
                taskInfo->ni = nice;
                taskInfo->virt = virt;
                taskInfo->res = res;
                taskInfo->shr = shr;
                taskInfo->s = state;
                taskInfo->cpu = float(utime+stime-taskInfo->time)*1000000/timeInterval;
                taskInfo->mem = float(res<<10)/currentSysinfo.totalram*100;
                taskInfo->time = utime+stime;
                taskInfo->comm = std::string(comm, 1, strlen(comm)-2);
                taskInfo->valid = true;
                cpuUsed += taskInfo->cpu;
                if (searchMode && strstr(comm, searchedCommand.c_str())) {
                    taskInfo->matched = true;
                    ++matchedTaskTotal;
                } else {
                    taskInfo->matched = false;
                }
            } else {
                taskInfoDict[pid] = new TaskInfo(pid, ppid, uid, priority, nice,
                                                 virt, res, shr,
                                                 state, float(res<<10)/currentSysinfo.totalram*100,
                                                 utime+stime,
                                                 std::string(comm, 1, strlen(comm)-2));
                if (searchMode && strstr(comm, searchedCommand.c_str())) {
                    taskInfoDict[pid]->matched = true;
                    ++matchedTaskTotal;
                } else {
                    taskInfoDict[pid]->matched = false;
                }
            }
            chdir("..");
        }
    }
    rewinddir(dir_ptr);

    ++gcEra;
    /* garbage colletion */
    if (gcEra == gcInterval) {
        auto it = taskInfoDict.begin();
        while (it != taskInfoDict.end()) {
            if (!it->second->valid) {
                delete it->second;
                it = taskInfoDict.erase(it);
            } else {
                it++;
            }
        }
        gcEra = 0;
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Escape) {
        if (searchMode) {
            timer->stop();
            ui->searchModeLabel->setVisible(false);
            searchMode = false;
            update();
            timer->start(3000);
        }
    }

}
