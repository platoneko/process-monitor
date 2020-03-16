#ifndef RUNDIALOG_H
#define RUNDIALOG_H

#include "global.h"

#include <QDialog>
#include <QMessageBox>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>

namespace Ui {
class RunDialog;
}

class RunDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RunDialog(QWidget *parent = nullptr);
    ~RunDialog();

private slots:
    void on_cancelButton_clicked();

    void on_confirmButton_clicked();

private:
    Ui::RunDialog *ui;
};

#endif // RUNDIALOG_H
