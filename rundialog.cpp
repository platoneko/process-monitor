#include "rundialog.h"
#include "ui_rundialog.h"


RunDialog::RunDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RunDialog)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose,true);
    this->setWindowTitle("Run Command");
}

RunDialog::~RunDialog()
{
    delete ui;
}

void RunDialog::on_cancelButton_clicked()
{
    this->close();
}

void RunDialog::on_confirmButton_clicked()
{
    QString command;
    command = ui->commandEdit->text();
    QStringList tokens = command.split(QRegExp("\\s+"));
    char runComm[MAXLINE];
    strncpy(runComm, tokens[0].toStdString().c_str(), MAXLINE);
    for (int i=0; i < runArgc; ++i) {
        delete []runArgv[i];
    }
    delete []runArgv;
    runArgc = tokens.length();
    runArgv = new char *[runArgc+1];
    for (int i=0; i < runArgc; ++i) {
        runArgv[i] = new char[tokens[i].length()+1];
        strcpy(runArgv[i], tokens[i].toStdString().c_str());
    }
    runArgv[runArgc] = nullptr;

    int errno_shmid = shmget(IPC_PRIVATE, sizeof(int),  IPC_CREAT | 0600);
    int *errno_p = static_cast<int *>(shmat(errno_shmid, nullptr, SHM_R | SHM_W));
    *errno_p = errno = 0;
    if (fork() == 0) {
        execvp(runComm, runArgv);
        *errno_p = errno;
        exit(0);
    }
    usleep(100000);
    if (*errno_p != 0) {
        QMessageBox::warning(this, command, strerror(*errno_p));
    }
    shmctl(errno_shmid, IPC_RMID, nullptr);
    this->close();
}
