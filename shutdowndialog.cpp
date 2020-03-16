#include "shutdowndialog.h"
#include "ui_shutdowndialog.h"

ShutdownDialog::ShutdownDialog(bool reboot, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShutdownDialog),
    reboot(reboot)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose,true);
    if (reboot) {
        this->setWindowTitle("Reboot");
        ui->label->setText("Continue to reboot?");
    } else {
        this->setWindowTitle("Shutdown");
        this->setWindowTitle("Continue to shutdown?");
    }
}

ShutdownDialog::~ShutdownDialog()
{
    delete ui;
}

void ShutdownDialog::on_confirmButton_clicked() {
    sync();
    // reboot(LINUX_REBOOT_CMD_POWER_OFF);
    if (reboot) {
        system("shutdown -r now");
        if (errno != 0) {
            QMessageBox::warning(this, "Reboot", strerror(errno));
            errno = 0;
        }
    } else {
        system("shutdown -P now");
        if (errno != 0) {
            QMessageBox::warning(this, "Shutdown", strerror(errno));
            errno = 0;
        }
    }
    this->close();
}

void ShutdownDialog::on_cancelButton_clicked()
{
    this->close();
}
