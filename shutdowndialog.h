#ifndef SHUTDOWNDIALOG_H
#define SHUTDOWNDIALOG_H

#include <QDialog>
#include <QMessageBox>

#include <unistd.h>
#include <sys/reboot.h>
#include <linux/reboot.h>

namespace Ui {
class ShutdownDialog;
}

class ShutdownDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShutdownDialog(bool reboot, QWidget *parent = nullptr);
    ~ShutdownDialog();

private slots:
    void on_confirmButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::ShutdownDialog *ui;
    bool reboot;
};

#endif // SHUTDOWNDIALOG_H
