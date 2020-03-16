#include "killdialog.h"
#include "ui_killdialog.h"

#include <QMessageBox>

#include <sys/wait.h>
#include <sys/stat.h>

KillDialog::KillDialog(const int pid, const std::string comm, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KillDialog),
    pid(pid),
    comm(comm)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose,true);
    sprintf(title, "PID %d %s", pid, comm.c_str());
    this->setWindowTitle(title);
    char line[MAXLINE];
    sprintf(line, "Sure to kill task with pid %d ?", pid);
    ui->promptLabel->setText(line);
    ui->commLabel->setText(comm.c_str());
}

KillDialog::~KillDialog()
{
    delete ui;
}

void KillDialog::on_cancelButton_clicked()
{
    this->close();
}

void KillDialog::on_confirmButton_clicked()
{
    errno = 0;
    kill(pid, SIGKILL);
    printf("%s\n", strerror(errno));
    fflush(stdout);
    if (errno != 0) {
        QMessageBox::warning(this, title, strerror(errno));
        errno = 0;
    }
    this->close();
}
