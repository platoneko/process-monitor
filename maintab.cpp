#include "mainwindow.h"
#include "./ui_mainwindow.h"


void MainWindow::initMainTab() {
    ui->taskTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->taskTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->taskTable->setModel(model);
    initTableModel();
    ui->searchModeLabel->setVisible(false);

    connect(ui->taskTable->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(on_sectionClicked(int)));
    connect(ui->taskTable->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::on_selectionChanged);
}

void MainWindow::initTableModel() {
    model->setColumnCount(COLUMN_NUM);
    model->setHeaderData(PID_COLUMN, Qt::Horizontal, "PID");
    model->setHeaderData(PPID_COLUMN, Qt::Horizontal, "PPID");
    model->setHeaderData(USER_COLUMN, Qt::Horizontal, "USER");
    model->setHeaderData(COMMAND_COLUMN, Qt::Horizontal, "NAME");
    model->setHeaderData(PRI_COLUMN, Qt::Horizontal, "PRI");
    model->setHeaderData(NI_COLUMN, Qt::Horizontal, "NI");
    model->setHeaderData(VIRT_COLUMN, Qt::Horizontal, "VIRT");
    model->setHeaderData(RES_COLUMN, Qt::Horizontal, "RES");
    model->setHeaderData(SHR_COLUMN, Qt::Horizontal, "SHR");
    model->setHeaderData(S_COLUMN, Qt::Horizontal, "S");
    model->setHeaderData(CPU_COLUMN, Qt::Horizontal, "CPU%");
    model->setHeaderData(MEM_COLUMN, Qt::Horizontal, "MEM%");
    model->setHeaderData(TIME_COLUMN, Qt::Horizontal, "TIME+");

    ui->taskTable->verticalHeader()->hide();
    ui->taskTable->horizontalHeader()->setVisible(true);
    ui->taskTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->taskTable->setSortingEnabled(false);

    ui->taskTable->setColumnWidth(PID_COLUMN,60);
    ui->taskTable->setColumnWidth(PPID_COLUMN,60);
    ui->taskTable->setColumnWidth(USER_COLUMN,80);
    ui->taskTable->setColumnWidth(COMMAND_COLUMN,160);
    ui->taskTable->setColumnWidth(PRI_COLUMN,60);
    ui->taskTable->setColumnWidth(NI_COLUMN,60);
    ui->taskTable->setColumnWidth(VIRT_COLUMN,60);
    ui->taskTable->setColumnWidth(RES_COLUMN,60);
    ui->taskTable->setColumnWidth(SHR_COLUMN,60);
    ui->taskTable->setColumnWidth(S_COLUMN,30);
    ui->taskTable->setColumnWidth(CPU_COLUMN,60);
    ui->taskTable->setColumnWidth(MEM_COLUMN,60);
    ui->taskTable->setColumnWidth(TIME_COLUMN,120);
}

void MainWindow::updateLoadAverageLabel() {
    char buff[MAXLINE];
    sprintf(buff, "Load average: %.2f %.2f %.2f", currentSysinfo.loads[0]/65536.0, currentSysinfo.loads[1]/65536.0, currentSysinfo.loads[2]/65536.0);
    ui->loadAverage->setText(buff);
}

void MainWindow::updateUptimeLabel() {
    char buff[MAXLINE];
    int hs, ms, ss;
    hs = currentSysinfo.uptime/3600;
    ms = (currentSysinfo.uptime%3600)/60;
    ss = currentSysinfo.uptime%60;
    sprintf(buff, "Uptime: %2d:%02d:%02d", hs, ms, ss);
    ui->uptime->setText(buff);
}

void MainWindow::updateMemBar() {
    ui->memBar->setValue((currentSysinfo.totalram - currentSysinfo.freeram)*100 / currentSysinfo.totalram);
    ui->swpBar->setValue((currentSysinfo.totalswap - currentSysinfo.freeswap)*100 / currentSysinfo.totalswap);
}

void MainWindow::updateTasksLabel() {
    char buff[MAXLINE];
    sprintf(buff, "Tasks: %4d total, %4d running, %4d sleeping, %4d stopped, %4d zombie",
            taskTotal, taskRunning, taskSleeping, taskStopped, taskZombie);
    ui->tasks->setText(buff);
}

void MainWindow::updateCpuBar(){
    ui->cpuBar->setValue(int(cpuUsed));
}

void MainWindow::updateTaskTable() {
    if (searchMode && matchedTaskTotal == 0) {
        searchMode = false;
        ui->searchModeLabel->setVisible(false);
    }

    int rowCount = model->rowCount();
    int total;
    if (searchMode) {
        total = matchedTaskTotal;
    } else {
        total = taskTotal;
    }
    if (rowCount < total) {
        for (int i = rowCount; i < total; ++i) {
            for (int j = 0; j < COLUMN_NUM; ++j) {
                model->setItem(i, j, new TableItem);
                model->item(i, j)->setEditable(false);
            }
        }
    }
    model->setRowCount(total);
    int row = 0;
    char s_virt[MAXLINE], s_res[MAXLINE], s_shr[MAXLINE], s_time[MAXLINE];
    for (auto &taskInfo: taskInfoDict) {
        if (taskInfo.second->valid) {
            if (!searchMode || taskInfo.second->matched) {
                model->item(row, PID_COLUMN)->setData(taskInfo.second->pid, Qt::DisplayRole);
                model->item(row, PPID_COLUMN)->setData(taskInfo.second->ppid, Qt::DisplayRole);
                model->item(row, USER_COLUMN)->setData(getpwuid(uid_t(taskInfo.second->uid))->pw_name, Qt::DisplayRole);
                model->item(row, COMMAND_COLUMN)->setData(taskInfo.second->comm.c_str(), Qt::DisplayRole);
                model->item(row, PRI_COLUMN)->setData(taskInfo.second->pri, Qt::DisplayRole);
                model->item(row, NI_COLUMN)->setData(taskInfo.second->ni, Qt::DisplayRole);
                formatSize(taskInfo.second->virt, s_virt);
                model->item(row, VIRT_COLUMN)->setData(s_virt, Qt::DisplayRole);
                formatSize(taskInfo.second->res, s_res);
                model->item(row, RES_COLUMN)->setData(s_res, Qt::DisplayRole);
                formatSize(taskInfo.second->shr, s_shr);
                model->item(row, SHR_COLUMN)->setData(s_shr, Qt::DisplayRole);
                model->item(row, S_COLUMN)->setData(QChar(taskInfo.second->s), Qt::DisplayRole);
                model->item(row, CPU_COLUMN)->setData(QString::number(taskInfo.second->cpu, 'f', 1), Qt::DisplayRole);
                model->item(row, MEM_COLUMN)->setData(QString::number(taskInfo.second->mem, 'f', 1), Qt::DisplayRole);
                formatTime(taskInfo.second->time, s_time);
                model->item(row, TIME_COLUMN)->setData(s_time, Qt::DisplayRole);
                model->item(row, VIRT_COLUMN)->setData(qlonglong(taskInfo.second->virt), Qt::UserRole);
                model->item(row, RES_COLUMN)->setData(qlonglong(taskInfo.second->res), Qt::UserRole);
                model->item(row, SHR_COLUMN)->setData(qlonglong(taskInfo.second->shr), Qt::UserRole);
                model->item(row, TIME_COLUMN)->setData(qlonglong(taskInfo.second->time), Qt::UserRole);
                model->item(row, CPU_COLUMN)->setData(taskInfo.second->cpu, Qt::UserRole);
                model->item(row, MEM_COLUMN)->setData(taskInfo.second->mem, Qt::UserRole);
                model->item(row, S_COLUMN)->setData(statePriority.at(taskInfo.second->s), Qt::UserRole);
                ++row;
            }
            taskInfo.second->valid = false;
        }
    }
    if (searchMode) {
        ui->searchModeLabel->setVisible(true);
    }

    sortTable();
    currVerticalScrollValue = ui->taskTable->verticalScrollBar()->value();
    currHorizontalScrollValue = ui->taskTable->horizontalScrollBar()->value();
    ui->taskTable->selectRow(currSelectedRow);
    ui->taskTable->verticalScrollBar()->setValue(currVerticalScrollValue);
    ui->taskTable->horizontalScrollBar()->setValue(currHorizontalScrollValue);
}

void MainWindow::formatCommand(char *src, char *dest) {
    int cnt = 0, i = 0;
    while (cnt != 2 && i < MAXLINE) {
        if (*src == 0) {
            ++cnt;
            *dest++ = ' ';
        } else {
            cnt = 0;
            *dest++ = *src++;
        }
        ++i;
    }
    *(dest-2) = 0;
}

void MainWindow::formatSize(unsigned long l_size, char *s_size) {
    if (l_size < 10000) {
        if (l_size == 0) {
            strcpy(s_size, "0");
        } else {
            sprintf(s_size, "%luK", l_size);
        }
    } else {
        l_size >>= 10;
        if (l_size < 10000) {
            sprintf(s_size, "%luM", l_size);
        } else {
            l_size >>= 10;
            sprintf(s_size, "%luG", l_size);
        }
    }
}

void MainWindow::formatTime(unsigned long l_time, char *s_time) {
    unsigned long m;
    float s;
    m = l_time/6000;
    s = float(l_time)/100 - m*60;
    if (m >= 60) {
        unsigned long h = m/60;
        m = m%60;
        sprintf(s_time, "%ldh%02ld:%d", h, m, int(s));
    } else {
        sprintf(s_time, "%ld:%05.2f", m, s);
    }
}

void MainWindow::on_sectionClicked(int index) {
    switch (index) {
    case 0:
        if (sortMethod == PID_ASC) {
            sortMethod = PID_DES;
        } else {
            sortMethod = PID_ASC;
        }
        break;
    case 1:
        if (sortMethod == PPID_ASC) {
            sortMethod = PPID_DES;
        } else {
            sortMethod = PPID_ASC;
        }
        break;
    case 2:
        if (sortMethod == USER_ASC) {
            sortMethod = USER_DES;
        } else {
            sortMethod = USER_ASC;
        }
        break;
    case 3:
        if (sortMethod == COMMAND_ASC) {
            sortMethod = COMMAND_DES;
        } else {
            sortMethod = COMMAND_ASC;
        }
        break;
    case 4:
        if (sortMethod == PRI_ASC) {
            sortMethod = PRI_DES;
        } else {
            sortMethod = PRI_ASC;
        }
        break;
    case 5:
        if (sortMethod == NI_ASC) {
            sortMethod = NI_DES;
        } else {
            sortMethod = NI_ASC;
        }
        break;
    case 6:
        if (sortMethod == VIRT_DES) {
            sortMethod = VIRT_ASC;
        } else {
            sortMethod = VIRT_DES;
        }
        break;
    case 7:
        if (sortMethod == RES_DES) {
            sortMethod = RES_ASC;
        } else {
            sortMethod = RES_DES;
        }
        break;
    case 8:
        if (sortMethod == SHR_DES) {
            sortMethod = SHR_ASC;
        } else {
            sortMethod = SHR_DES;
        }
        break;
    case 9:
        if (sortMethod == S_ASC) {
            sortMethod = S_DES;
        } else {
            sortMethod = S_ASC;
        }
        break;
    case 10:
        if (sortMethod == CPU_DES) {
            sortMethod = CPU_ASC;
        } else {
            sortMethod = CPU_DES;
        }
        break;
    case 11:
        if (sortMethod == MEM_DES) {
            sortMethod = MEM_ASC;
        } else {
            sortMethod = MEM_DES;
        }
        break;
    case 12:
        if (sortMethod == TIME_DES) {
            sortMethod = TIME_ASC;
        } else {
            sortMethod = TIME_DES;
        }
        break;
    default:
        break;
    }
    sortTable();
    currVerticalScrollValue = ui->taskTable->verticalScrollBar()->value();
    currHorizontalScrollValue = ui->taskTable->horizontalScrollBar()->value();
    ui->taskTable->selectRow(currSelectedRow);
    ui->taskTable->horizontalScrollBar()->setValue(currHorizontalScrollValue);
    ui->taskTable->verticalScrollBar()->setValue(currVerticalScrollValue);
}

void MainWindow::sortTable() {
    switch (sortMethod) {
    case PID_ASC:
        ui->taskTable->sortByColumn(PID_COLUMN, Qt::AscendingOrder);
        break;
    case PID_DES:
        ui->taskTable->sortByColumn(PID_COLUMN, Qt::DescendingOrder);
        break;
    case PPID_ASC:
        ui->taskTable->sortByColumn(PID_COLUMN, Qt::AscendingOrder);
        ui->taskTable->sortByColumn(PPID_COLUMN, Qt::AscendingOrder);
        break;
    case PPID_DES:
        ui->taskTable->sortByColumn(PID_COLUMN, Qt::AscendingOrder);
        ui->taskTable->sortByColumn(PPID_COLUMN, Qt::DescendingOrder);
        break;
    case USER_ASC:
        ui->taskTable->sortByColumn(PID_COLUMN, Qt::AscendingOrder);
        ui->taskTable->sortByColumn(USER_COLUMN, Qt::AscendingOrder);
        break;
    case USER_DES:
        ui->taskTable->sortByColumn(PID_COLUMN, Qt::AscendingOrder);
        ui->taskTable->sortByColumn(USER_COLUMN, Qt::DescendingOrder);
        break;
    case COMMAND_ASC:
        ui->taskTable->sortByColumn(PID_COLUMN, Qt::AscendingOrder);
        ui->taskTable->sortByColumn(COMMAND_COLUMN, Qt::AscendingOrder);
        break;
    case COMMAND_DES:
        ui->taskTable->sortByColumn(PID_COLUMN, Qt::AscendingOrder);
        ui->taskTable->sortByColumn(COMMAND_COLUMN, Qt::DescendingOrder);
        break;
    case PRI_ASC:
        ui->taskTable->sortByColumn(PID_COLUMN, Qt::AscendingOrder);
        ui->taskTable->sortByColumn(PRI_COLUMN, Qt::AscendingOrder);
        break;
    case PRI_DES:
        ui->taskTable->sortByColumn(PID_COLUMN, Qt::AscendingOrder);
        ui->taskTable->sortByColumn(PRI_COLUMN, Qt::DescendingOrder);
        break;
    case NI_ASC:
        ui->taskTable->sortByColumn(PID_COLUMN, Qt::AscendingOrder);
        ui->taskTable->sortByColumn(NI_COLUMN, Qt::AscendingOrder);
        break;
    case NI_DES:
        ui->taskTable->sortByColumn(PID_COLUMN, Qt::AscendingOrder);
        ui->taskTable->sortByColumn(NI_COLUMN, Qt::DescendingOrder);
        break;
    case VIRT_ASC:
        ui->taskTable->sortByColumn(PID_COLUMN, Qt::AscendingOrder);
        ui->taskTable->sortByColumn(VIRT_COLUMN, Qt::AscendingOrder);
        break;
    case VIRT_DES:
        ui->taskTable->sortByColumn(PID_COLUMN, Qt::AscendingOrder);
        ui->taskTable->sortByColumn(VIRT_COLUMN, Qt::DescendingOrder);
        break;
    case RES_ASC:
        ui->taskTable->sortByColumn(PID_COLUMN, Qt::AscendingOrder);
        ui->taskTable->sortByColumn(RES_COLUMN, Qt::AscendingOrder);
        break;
    case RES_DES:
        ui->taskTable->sortByColumn(PID_COLUMN, Qt::AscendingOrder);
        ui->taskTable->sortByColumn(RES_COLUMN, Qt::DescendingOrder);
        break;
    case SHR_ASC:
        ui->taskTable->sortByColumn(PID_COLUMN, Qt::AscendingOrder);
        ui->taskTable->sortByColumn(SHR_COLUMN, Qt::AscendingOrder);
        break;
    case SHR_DES:
        ui->taskTable->sortByColumn(PID_COLUMN, Qt::AscendingOrder);
        ui->taskTable->sortByColumn(SHR_COLUMN, Qt::DescendingOrder);
        break;
    case S_ASC:
        ui->taskTable->sortByColumn(PID_COLUMN, Qt::AscendingOrder);
        ui->taskTable->sortByColumn(S_COLUMN, Qt::AscendingOrder);
        break;
    case S_DES:
        ui->taskTable->sortByColumn(PID_COLUMN, Qt::AscendingOrder);
        ui->taskTable->sortByColumn(S_COLUMN, Qt::DescendingOrder);
        break;
    case CPU_ASC:
        ui->taskTable->sortByColumn(PID_COLUMN, Qt::AscendingOrder);
        ui->taskTable->sortByColumn(CPU_COLUMN, Qt::AscendingOrder);
        break;
    case CPU_DES:
        ui->taskTable->sortByColumn(PID_COLUMN, Qt::AscendingOrder);
        ui->taskTable->sortByColumn(CPU_COLUMN, Qt::DescendingOrder);
        break;
    case MEM_ASC:
        ui->taskTable->sortByColumn(PID_COLUMN, Qt::AscendingOrder);
        ui->taskTable->sortByColumn(MEM_COLUMN, Qt::AscendingOrder);
        break;
    case MEM_DES:
        ui->taskTable->sortByColumn(PID_COLUMN, Qt::AscendingOrder);
        ui->taskTable->sortByColumn(MEM_COLUMN, Qt::DescendingOrder);
        break;
    case TIME_ASC:
        ui->taskTable->sortByColumn(PID_COLUMN, Qt::AscendingOrder);
        ui->taskTable->sortByColumn(TIME_COLUMN, Qt::AscendingOrder);
        break;
    case TIME_DES:
        ui->taskTable->sortByColumn(PID_COLUMN, Qt::AscendingOrder);
        ui->taskTable->sortByColumn(TIME_COLUMN, Qt::DescendingOrder);
        break;
    default:
        ui->taskTable->sortByColumn(PID_COLUMN, Qt::AscendingOrder);
        ui->taskTable->sortByColumn(S_COLUMN, Qt::AscendingOrder);
        break;
    }
}

void MainWindow::on_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {
    currSelectedRow = selected.indexes().first().row();
    // QMessageBox::warning(this, "selection", std::to_string(currSelectedRow).c_str());
}

void MainWindow::on_killButton_clicked() {
    int pid = model->item(currSelectedRow, 0)->data(Qt::DisplayRole).toInt();
    killDialog = new KillDialog(pid, taskInfoDict[pid]->comm, this);
    killDialog->setModal(true);
    killDialog->show();
}

void MainWindow::on_niceButton_clicked() {
    int pid = model->item(currSelectedRow, 0)->data(Qt::DisplayRole).toInt();
    niceDialog = new NiceDialog(pid, taskInfoDict[pid]->comm, taskInfoDict[pid]->ni, this);
    niceDialog->setModal(true);
    niceDialog->show();
}

void MainWindow::on_runButton_clicked() {
    runDialog = new RunDialog(this);
    runDialog->setModal(true);
    runDialog->show();
}

void MainWindow::on_searchButton_clicked() {
    searchDialog = new SearchDialog(pid_max, this);
    connect(searchDialog, &SearchDialog::sendPid, this, &MainWindow::searchPid);
    connect(searchDialog, &SearchDialog::sendCommand, this, &MainWindow::searchCommand);
    connect(searchDialog, &SearchDialog::closeSignal, this, &MainWindow::searchDialogClosed);
    searchDialog->setModal(true);
    searchDialog->show();
}

void MainWindow::searchPid(int pid) {
    bool pidExist = false;
    int total;
    if (searchMode) {
        total = matchedTaskTotal;
    } else {
        total = taskTotal;
    }
    for (int i = 0; i < total; ++i) {
        if (model->item(i, 0)->data(Qt::DisplayRole) == pid) {
            ui->taskTable->selectRow(i);
            ui->taskTable->scrollTo(ui->taskTable->selectionModel()->selectedIndexes()[0]);
            pidExist = true;
            break;
        }
    }
    if (!pidExist) {
        char msg[MAXLINE];
        sprintf(msg, "Pid %d doesn't exist!", pid);
        QMessageBox::warning(this, "Search", msg);
    }
}

void MainWindow::searchCommand(const std::string command) {
     searchedCommand = command;
     searchMode = true;
     timer->stop();
     update();
     timer->start(3000);
}

void MainWindow::searchDialogClosed() {
    disconnect(searchDialog, &SearchDialog::sendPid, this, &MainWindow::searchPid);
    disconnect(searchDialog, &SearchDialog::sendCommand, this, &MainWindow::searchCommand);
    disconnect(searchDialog, &SearchDialog::closeSignal, this, &MainWindow::searchDialogClosed);
}
