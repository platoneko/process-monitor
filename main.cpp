#include "mainwindow.h"
#include "global.h"

#include <QApplication>
#include <QThread>

#include <unistd.h>
#include <sys/wait.h>


char **runArgv;
int runArgc;

void catch_child(int sig_num)
{
    /* when we get here, we know there's a zombie child waiting */
    int child_status;
    wait(&child_status);
}


int main(int argc, char *argv[])
{
    runArgv = new char *[1];
    runArgv[0] = nullptr;
    runArgc = 0;

    signal(SIGCHLD, catch_child);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
