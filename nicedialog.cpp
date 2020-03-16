#include "nicedialog.h"
#include "ui_nicedialog.h"


NiceDialog::NiceDialog(int pid, const std::string comm, int prevNice, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NiceDialog),
    pid(pid),
    comm(comm),
    prevNice(prevNice)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose,true);
    sprintf(title, "PID %d %s", pid, comm.c_str());
    this->setWindowTitle(title);
    ui->niceBox->setRange(-20, 19);
    ui->niceBox->setValue(prevNice);
}

NiceDialog::~NiceDialog()
{
    delete ui;
}

void NiceDialog::on_applyButton_clicked()
{
    int nice = ui->niceBox->value();
    errno = 0;
    setpriority(PRIO_PROCESS, id_t(pid), nice);
    if (errno != 0) {
        QMessageBox::warning(this, title, strerror(errno));
        errno = 0;
    }
    this->close();
}

void NiceDialog::on_cancelButton_clicked()
{
    this->close();
}
