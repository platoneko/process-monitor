#ifndef NICEDIALOG_H
#define NICEDIALOG_H

#include "global.h"

#include <QDialog>
#include <QMessageBox>

#include <sys/resource.h>

namespace Ui {
class NiceDialog;
}

class NiceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NiceDialog(int pid, const std::string comm, int prevNice, QWidget *parent = nullptr);
    ~NiceDialog();

private slots:
    void on_applyButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::NiceDialog *ui;
    const int pid;
    const std::string comm;
    const int prevNice;
    char title[MAXLINE];
};

#endif // NICEDIALOG_H
