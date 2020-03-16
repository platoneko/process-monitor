#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>

#include "utils.h"

namespace Ui {
class SearchDialog;
}

class SearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchDialog(int pid_max, QWidget *parent = nullptr);
    ~SearchDialog();

private slots:
    void on_pidConfirmButton_clicked();

    void on_pidCancelButton_clicked();

    void on_commandConfirmButton_clicked();

    void on_commandCancelButton_clicked();


signals:
    void sendPid(int pid);
    void sendCommand(const std::string command);
    void closeSignal();

private:
    Ui::SearchDialog *ui;
    void closeEvent(QCloseEvent *event) {emit closeSignal();}
};

#endif // SEARCHDIALOG_H
