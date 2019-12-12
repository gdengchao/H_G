#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include "filereader.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->pheFileToolButton->setIcon(QIcon(":/new/icon/images/plus.png"));
    ui->genoFileToolButton->setIcon(QIcon(":/new/icon/images/plus.png"));
    ui->mapFileToolButton->setIcon(QIcon(":/new/icon/images/plus.png"));
    ui->covarFileToolButton->setIcon(QIcon(":/new/icon/images/plus.png"));
    ui->kinFileToolButton->setIcon(QIcon(":/new/icon/images/plus.png"));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pheFileToolButton_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle("Open phenotype file");
    fileDialog->setViewMode(QFileDialog::Detail);

    QStringList fileNames;
    if (fileDialog->exec())
    {
        fileNames = fileDialog->selectedFiles();
    }
    delete fileDialog;
    if (fileNames.isEmpty())    // If didn't choose any file.
    {
        return;
    }
    ui->pheFileToolButton->setIcon(QIcon(":/new/icon/images/file.png"));

    FileReader fileReader;
    fileReader.setPhenotypeFile(fileNames[0]);

    QStringList  fileDirList = fileNames[0].split("/");
    QString fileName = fileDirList[fileDirList.length()-1];
    ui->pheFileLabel->setText(fileName);
}

void MainWindow::on_genoFileToolButton_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle("Open genotype file");
    fileDialog->setViewMode(QFileDialog::Detail);

    QStringList fileNames;
    if (fileDialog->exec())
    {
      fileNames = fileDialog->selectedFiles();
    }
    delete fileDialog;
    if (fileNames.isEmpty())    // If didn't choose any file.
    {
        return;
    }
    ui->genoFileToolButton->setIcon(QIcon(":/new/icon/images/file.png"));

    FileReader fileReader;
    fileReader.setGenotypeFile(fileNames[0]);

    QStringList  fileDirList = fileNames[0].split("/");
    QString fileName = fileDirList[fileDirList.length()-1];
    ui->genoFileLabel->setText(fileName);
}

void MainWindow::on_mapFileToolButton_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle("Open map file");
    fileDialog->setViewMode(QFileDialog::Detail);

    QStringList fileNames;
    if (fileDialog->exec())
    {
      fileNames = fileDialog->selectedFiles();
    }
    delete fileDialog;
    if (fileNames.isEmpty())    // If didn't choose any file.
    {
        return;
    }
    ui->mapFileToolButton->setIcon(QIcon(":/new/icon/images/file.png"));

    FileReader fileReader;
    fileReader.setMapFile(fileNames[0]);

    QStringList  fileDirList = fileNames[0].split("/");
    QString fileName = fileDirList[fileDirList.length()-1];
    ui->mapFileLabel->setText(fileName);
}

void MainWindow::on_covarFileToolButton_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle("Open covariate file");
    fileDialog->setViewMode(QFileDialog::Detail);

    QStringList fileNames;
    if (fileDialog->exec())
    {
      fileNames = fileDialog->selectedFiles();
    }
    delete fileDialog;
    if (fileNames.isEmpty())    // If didn't choose any file.
    {
        return;
    }
    ui->covarFileToolButton->setIcon(QIcon(":/new/icon/images/file.png"));

    FileReader fileReader;
    fileReader.setCovariateFile(fileNames[0]);

    QStringList  fileDirList = fileNames[0].split("/");
    QString fileName = fileDirList[fileDirList.length()-1];
    ui->covarFileLabel->setText(fileName);
}

void MainWindow::on_kinFileToolButton_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle("Open kinship file");
    fileDialog->setViewMode(QFileDialog::Detail);

    QStringList fileNames;
    if (fileDialog->exec())
    {
      fileNames = fileDialog->selectedFiles();
    }
    delete fileDialog;
    if (fileNames.isEmpty())    // If didn't choose any file.
    {
        return;
    }
    ui->kinFileToolButton->setIcon(QIcon(":/new/icon/images/file.png"));

    FileReader fileReader;
    fileReader.setKinshipFile(fileNames[0]);

    QStringList  fileDirList = fileNames[0].split("/");
    QString fileName = fileDirList[fileDirList.length()-1];
    ui->kinFileLabel->setText(fileName);
}

void MainWindow::on_browButton_clicked()
{
    QString outDirectory = QFileDialog::getExistingDirectory(this, "Choose directory");
    ui->  outdirLineEdit->setText(outDirectory);
}
