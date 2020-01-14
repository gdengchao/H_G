#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "workdirectory.h"

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

    ui->selectedPhenoListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection); // Press Ctrl to select more.
    ui->excludedPhenoListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

    // Initiate variables.
    fileReader = new FileReader;
    workDirectory = new WorkDirectory;
    phenoSelector = new PhenoSelector;
    runningMsgWidget = new RunningMsgWidget;
    cmd = new QProcess;

    connect(cmd, SIGNAL(readyReadStandardOutput()), this, SLOT(on_readoutput()));
    connect(cmd, SIGNAL(readyReadStandardError()), this, SLOT(on_readerror()));
}

MainWindow::~MainWindow()
{
    // Free pointer.
    delete ui;
    delete fileReader;
    delete workDirectory;
    delete phenoSelector;

    runningMsgWidget->close();
    delete runningMsgWidget;

    if (cmd)
    {
        cmd->terminate();
        cmd->waitForFinished();
    }
    delete cmd;
}


void MainWindow::on_pheFileToolButton_clicked()
{
    // Basic display
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle("Open phenotype file");
    fileDialog->setViewMode(QFileDialog::Detail);

    QStringList fileNames;  // Absolute directory of file.
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

    this->fileReader->setPhenotypeFile(fileNames[0]);

    QStringList  fileDirList = fileNames[0].split("/");
    QString fileName = fileDirList[fileDirList.length()-1]; // Get the file name from a path.
    ui->pheFileLabel->setText(fileName);

    // Get types of phenotype, and write to list widget.
    QFile fptr(fileNames[0]);
    fptr.open(QIODevice::ReadOnly|QIODevice::Text);
    QString phenoFirstLine = fptr.readLine();
    phenoFirstLine.replace("\r\n", "");         // Strip "\n"
    phenoFirstLine.replace("\n", "");
    QStringList phenoList = phenoFirstLine.split("\t");
    phenoList.removeFirst();
    phenoSelector->setSelectedPheno(phenoList);

    ui->selectedPhenoListWidget->insertItems(0, phenoSelector->getSelectedPheno());
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

    this->fileReader->setGenotypeFile(fileNames[0]);

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

    this->fileReader->setMapFile(fileNames[0]);

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

    this->fileReader->setCovariateFile(fileNames[0]);

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

    this->fileReader->setKinshipFile(fileNames[0]);

    QStringList  fileDirList = fileNames[0].split("/");
    QString fileName = fileDirList[fileDirList.length()-1];
    ui->kinFileLabel->setText(fileName);
}

void MainWindow::on_browButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Choose directory");
    if (!dir.isEmpty())
    {
        this->workDirectory->setOutputDirectory(dir);
        ui->outdirLineEdit->setText(dir+"/"+this->workDirectory->getModuleName());
    }
}

void MainWindow::on_modulenameLineEdit_textChanged(const QString &text)
{
    this->workDirectory->setModuleName(text);
    if (!ui->outdirLineEdit->text().isEmpty())
    {
        ui->outdirLineEdit->setText(this->workDirectory->getOutputDirectory()+"/"+text);
    }
}

void MainWindow::on_excludeAllPhenoButton_clicked()
{
    phenoSelector->excludeAllPheno();
    ui->selectedPhenoListWidget->clear();   // Clear the list widget.
    ui->excludedPhenoListWidget->clear();
    ui->selectedPhenoListWidget->insertItems(0, phenoSelector->getSelectedPheno()); // Display
    ui->excludedPhenoListWidget->insertItems(0, phenoSelector->getExcludedPheno());
}

void MainWindow::on_selectAllPhenoButton_clicked()
{
    phenoSelector->selectAllPheno();
    ui->selectedPhenoListWidget->clear();
    ui->excludedPhenoListWidget->clear();
    ui->selectedPhenoListWidget->insertItems(0, phenoSelector->getSelectedPheno());
    ui->excludedPhenoListWidget->insertItems(0, phenoSelector->getExcludedPheno());
}

void MainWindow::on_selectPhenoButton_clicked()
{
    phenoSelector->selectPheno(ui->excludedPhenoListWidget->selectedItems());
    ui->selectedPhenoListWidget->clear();
    ui->excludedPhenoListWidget->clear();
    ui->selectedPhenoListWidget->insertItems(0, phenoSelector->getSelectedPheno());
    ui->excludedPhenoListWidget->insertItems(0, phenoSelector->getExcludedPheno());
}

void MainWindow::on_excludePhenoButton_clicked()
{
    phenoSelector->excludePheno(ui->selectedPhenoListWidget->selectedItems());
    ui->selectedPhenoListWidget->clear();
    ui->excludedPhenoListWidget->clear();
    ui->selectedPhenoListWidget->insertItems(0, phenoSelector->getSelectedPheno());
    ui->excludedPhenoListWidget->insertItems(0, phenoSelector->getExcludedPheno());
}

void MainWindow::on_rungwasButton_clicked()
{
    //QString toolpath = "/tools/";
    QString toolpath = "F:/Code/Qt/H_G/tools/";  // For Debug, start from debug/a.exe fold.
    QString tool = ui->toolComboBox->currentText();

    QString phenotype = fileReader->getPhenotypeFile();
    QString genotype = fileReader->getGenotypeFile();
    QString map = fileReader->getMapFile();
    QString covar = fileReader->getCovariateFile();
    QString kinship = fileReader->getKinshipFile();
    QString out = workDirectory->getOutputDirectory();  // Include module name.
    QString name = workDirectory->getModuleName();
//    if (out.isNull() || name.isNull())
//    {
//        QMessageBox::information(nullptr, "Error", "Plese select a work directory!  ");
//        return;
//    }

    QString model = ui->modelComboBox->currentText();
    QString missingRate = ui->msDoubleSpinBox->text();
    QString maf = ui->mafDoubleSpinBox->text();
    UserOS *userOS = new UserOS;

    ui->rungwasButton->setText("Running");
    ui->rungwasButton->setDisabled(true);

    if (userOS->currentOS() == "winnt")
    {
        if (tool == "plink")
        {
            //cmd->start(toolpath+tool, QStringList()<<"-h"<<"--noweb");
//            cmd->start("cmd");
//            cmd->waitForStarted();

//            // QString transform to char *.
//            QString cmdline = toolpath+tool+".exe"+" -h --noweb\n";
//            QByteArray cmdArray;
//            cmdArray.append(cmdline);
//            cmd->write(cmdArray.data());

            runningMsgWidget->setTitle(name+" is running...");
            runningMsgWidget->show();
            Plink plink;
            if(plink.runGWAS(phenotype, genotype, map, covar, kinship,
                          out,model, missingRate, maf))
            {
                cmd->start(toolpath+tool, plink.getParamList());
            }
        }
    }

    if (cmd)
    {
        cmd->terminate();
        cmd->waitForFinished();
    }
    ui->rungwasButton->setText("Run");
    ui->rungwasButton->setEnabled(true);
}

void MainWindow::on_readoutput()
{
    runningMsgWidget->appendText(QString::fromLocal8Bit(cmd->readAllStandardOutput().data()));
}

void MainWindow::on_readerror()
{
    QMessageBox::information(nullptr, "Error", QString::fromLocal8Bit(cmd->readAllStandardError().data()));
    runningMsgWidget->close();
}
