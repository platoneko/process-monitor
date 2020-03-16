#include "searchdialog.h"
#include "ui_searchdialog.h"


SearchDialog::SearchDialog(int pid_max, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchDialog)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose,true);
    this->setWindowTitle("Search");
    ui->pidBox->setRange(1, pid_max);
    ui->pidBox->setValue(1);
}

SearchDialog::~SearchDialog()
{
    delete ui;
}

void SearchDialog::on_pidCancelButton_clicked()
{
    this->close();
}

void SearchDialog::on_pidConfirmButton_clicked()
{
    emit sendPid(ui->pidBox->value());
    this->close();
}

void SearchDialog::on_commandCancelButton_clicked()
{
    this->close();
}

void SearchDialog::on_commandConfirmButton_clicked()
{
    std::string comm = Utils::trim(ui->commandEdit->text().toStdString());
    if (!comm.empty()) {
        emit sendCommand(comm);
        this->close();
    } else {
        ui->commandEdit->clear();
    }
}
