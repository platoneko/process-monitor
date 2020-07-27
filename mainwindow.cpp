#include "mainwindow.h"
#include "./ui_mainwindow.h"

using namespace std;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    timer(new QTimer),
    model(new QStandardItemModel),
    cpuHistory(new deque<float>),
    cpuSeries(new QLineSeries),
    cpuChart(new QChart),
    memHistory(new deque<float>),
    memSeries(new QLineSeries),
    memChart(new QChart) ,
    swapHistory(new deque<float>),
    swapSeries(new QLineSeries),
    swapChart(new QChart)
{
    dir_ptr = opendir("/proc");
    chdir("/proc");

    ifstream infile("/proc/sys/kernel/pid_max");
    infile >> pid_max;
    infile.close();

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
}

void MainWindow::updateTaskInfo() {
    struct dirent *dir_entry;
    ifstream infile;

    string buff;

    unsigned short uid, pid, ppid;
    int priority, nice;
    int  pgrp, session, tty_nr, tpgid;
    unsigned long minflt, cminflt, majflt, cmajflt, utime, stime;
    long cutime, cstime, num_threads;
    unsigned int flags;
    char state;
    string name;

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
            infile.open("status");
            infile >> buff >> name;
            for (int j = 0; j < 8; ++j) {
                getline(infile, buff);
            }
            infile >> buff >> uid;
            infile.close();

            infile.open("stat");
            infile >> pid >> buff;
            infile.get();
            infile >> state >> ppid >> pgrp >> session >> tty_nr >> tpgid >> flags;
            infile >> minflt >> cminflt >> majflt >> cmajflt;
            infile >> utime >> stime >> cutime >> cstime;
            infile >> priority >> nice >> num_threads;
            infile.close();

            infile.open("statm");
            infile >> virt >> res >> shr;
            infile.close();
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
                taskInfo->comm = name;
                taskInfo->valid = true;
                cpuUsed += taskInfo->cpu;
                if (searchMode && name.find(searchedCommand) != string::npos) {
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
                                                 name);
                if (searchMode && name.find(searchedCommand) != string::npos) {
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
