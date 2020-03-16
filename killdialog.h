#ifndef KILLDIALOG_H
#define KILLDIALOG_H

#include "global.h"
#include <QDialog>


namespace Ui {
class KillDialog;
}

class KillDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KillDialog(const int pid, const std::string comm, QWidget *parent = nullptr);
    ~KillDialog();

private slots:
    void on_cancelButton_clicked();

    void on_confirmButton_clicked();

private:
    Ui::KillDialog *ui;
    const int pid;
    const std::string comm;
    char title[MAXLINE];
};

#endif // KILLDIALOG_H
