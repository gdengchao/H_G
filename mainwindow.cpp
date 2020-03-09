#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "workdirectory.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Intiate Icon.(cross icon)
    ui->pheFileToolButton->setIcon(QIcon(":/new/icon/images/plus.png"));
    ui->genoFileToolButton->setIcon(QIcon(":/new/icon/images/plus.png"));
    ui->mapFileToolButton->setIcon(QIcon(":/new/icon/images/plus.png"));
    ui->covarFileToolButton->setIcon(QIcon(":/new/icon/images/plus.png"));
    ui->kinFileToolButton->setIcon(QIcon(":/new/icon/images/plus.png"));

    // Press Ctrl to select more.
    ui->selectedPhenoListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->excludedPhenoListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

    // Initiate variables.
    fileReader = new FileReader;
    workDirectory = new WorkDirectory;
    phenoSelector = new PhenoSelector;
    runningMsgWidget = new RunningMsgWidget;
    process = new QProcess;

    // connect QProcess->start(tool) and runningMsgWidget.
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(on_readoutput()));
    connect(process, SIGNAL(readyReadStandardError()), this, SLOT(on_readerror()));
    connect(runningMsgWidget, SIGNAL(close()), this->process, SLOT(on_closeRunningWidget()));
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

    if (process)    // QProcess
    {
        process->terminate();
        process->waitForFinished(-1);
    }
    delete process;
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
    ui->pheFileToolButton->setIcon(QIcon(":/new/icon/images/file.png"));    // Set file Icon.

    this->fileReader->setPhenotypeFile(fileNames[0]);

    QStringList  fileDirList = fileNames[0].split("/");
    QString fileName = fileDirList[fileDirList.length()-1]; // Get the file name from a path.
    ui->pheFileLabel->setText(fileName);

    // Get types of phenotype, and write to list widget. ------- Just every coloumn(second begin) is phenotype is OK.
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
    {   // If a out directory is selected, display the out directory + the module name.
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
    //QString toolpath = "F://Code//Qt/H_G//tools//";  // Lab418. For Debug, start from debug/a.exe fold.
    //QString toolpath = "G:\\GitHub\\H_G\\tools\\";  // Laptop Win10
    // if (userOS->currentOS() == "linux") {}

    QString tool = ui->toolComboBox->currentText();
    ui->rungwasButton->setText("Running");
    ui->rungwasButton->setDisabled(true);
    this->runningMsgWidget->clearText();
    this->runningMsgWidget->show();

    if (tool == "emmax")
    {
        if (!this->callEmmaxGwas())
        {
            this->resetWindow();
            return;
        }
    }

    if (tool == "gemma")
    {
        if (!this->callGemmaGwas())
        {
            this->resetWindow();
            return;
        }
    }

    if (tool == "plink")  // plink GWAS
    {
        if (!this->callPlinkGwas())
        {
            this->resetWindow();
            this->runningMsgWidget->close();
            return;
        }
    }

    this->resetWindow();
}

bool MainWindow::callGemmaGwas(void)
{
    QString toolpath = "//home//chao//Documents//code//H_G/tools//"; // Laptop Linux. Attention
    QString tool = ui->toolComboBox->currentText();

    QString phenotype = this->fileReader->getPhenotypeFile();
    QString genotype = this->fileReader->getGenotypeFile();
    QString map = this->fileReader->getMapFile();
    QString covar = this->fileReader->getCovariateFile();
    QString kinship = this->fileReader->getKinshipFile();
    QString out = this->workDirectory->getOutputDirectory();  // Include module name.
    QString name = this->workDirectory->getModuleName();
    if (out.isNull() || name.isNull())
    {
        QMessageBox::information(nullptr, "Error", "Plese select a  correct work directory!  ");
        return false;
    }

    QString model = ui->modelComboBox->currentText();
    QString ms = ui->msRadioButton->isChecked()? ui->msDoubleSpinBox->text():nullptr;
    QString maf = ui->mafRadioButton->isChecked()? ui->mafDoubleSpinBox->text():nullptr;
    //UserOS *userOS = new UserOS;

    // Genotype file info.
    QFileInfo genoFileInfo = QFileInfo(genotype);
    QString genoFileName = genoFileInfo.fileName();         // demo.vcf.gz
    QString genoFileBaseName = genoFileInfo.baseName();     // geno
    QString genoFileSuffix = genoFileInfo.suffix();         //
    QString genoFileAbPath = genoFileInfo.absolutePath();

    // Need binary files.
    Plink plink;
    if (isVcfFile(genotype)) // Transform "vcf" to "transpose"
    {
        if(!plink.vcf2binary(genotype, genoFileAbPath+"/"+genoFileBaseName, maf, ms))
        {
            this->resetWindow();
            return false;
        }
    }
    if (genotype.split(".")[genotype.split(".").length()-1] == "ped"
            && map.split(".")[map.split(".").length()-1] == "map")  // Transform "plink" to "transpose"
    {
        if (!plink.plink2binary(genotype, map, genoFileAbPath+"/"+genoFileBaseName))
        {
            this->resetWindow();
            return false;
        }
    }
    this->process->start(toolpath+"plink", plink.getParamList());
    if (!this->process->waitForStarted())
    {
        this->resetWindow();
        return false;
    }
    this->runningMsgWidget->setTitle("Making " + genoFileBaseName +".beb/bim/fam");
    if (!this->process->waitForFinished(-1))
    {
        this->resetWindow();
        QMessageBox::information(nullptr, "Error", "Exit plink with error when make binary in callGemmaGwas!  ");
        return false;
    }
    this->runningMsgWidget->setTitle(genoFileBaseName +".beb/bim/fam is made");

    Gemma gemma;

    if (!gemma.phe_fam_Preparation(phenotype, genoFileAbPath+"/"+genoFileBaseName+".fam"))
    {   // Replace "NA" to "-9", then complete .fam
        // .fam: FID IID PID MID Sex 1 Phe  (phenotype data to the 7th column of .fam)
        this->resetWindow();
        return false;
    }

    if (kinship.isNull())
    {
         if (!gemma.makeKinship(genoFileAbPath+"/"+genoFileBaseName, genoFileBaseName))
         {
             this->resetWindow();
             return false;  // Make kinship failed.
         }
         this->process->start(toolpath + "gemma", gemma.getParamList());
         if (!this->process->waitForStarted())
         {
             this->resetWindow();
             return false;
         }
         this->runningMsgWidget->setTitle("Making " + genoFileBaseName + ".cXX.txt");
         if (!this->process->waitForFinished(-1))
         {
             this->resetWindow();
             QMessageBox::information(nullptr, "Error", "Exit gemma with error when  make kinship!   ");
             return false;
         }
         this->runningMsgWidget->setTitle(genoFileBaseName + ".cXX.txt is made");
         //kinship = genoFileAbPath + "/output/" + genoFileBaseName + ".cXX.txt";    // Attention
         kinship = QDir::currentPath() + "/output/" + genoFileBaseName + ".cXX.txt";
    }

    if (gemma.runGWAS(genoFileAbPath+"/"+genoFileBaseName, phenotype, covar, kinship, name))
    {
        this->process->start(toolpath+"gemma", gemma.getParamList());
        // Running message to display message.
        if (!this->process->waitForStarted())
        {
            this->resetWindow();
            return false;
        }
        this->runningMsgWidget->setTitle("Gemma: " + name+" is running...");
        if (!this->process->waitForFinished(-1))
        {
            this->resetWindow();
            QMessageBox::information(nullptr, "Error", "Exit gemma with error when run GWAS! ");
            return false;
        }

        QDir gemmaOut(QDir::currentPath() + "/output/");
        gemmaOut.rename(QDir::currentPath() + "/output/", out+"/output/");

        this->runningMsgWidget->setTitle("Gemma: " + name+" is finished");
    }
    else
    {
        this->resetWindow();
        return false;
    }

    return true;
}

bool MainWindow::callEmmaxGwas(void)
{
    QString toolpath = "//home//chao//Documents//code//H_G/tools//"; // Laptop Linux. Attention
    QString tool = ui->toolComboBox->currentText();

    QString phenotype = this->fileReader->getPhenotypeFile();
    QString genotype = this->fileReader->getGenotypeFile();
    QString map = this->fileReader->getMapFile();
    QString covar = this->fileReader->getCovariateFile();
    QString kinship = this->fileReader->getKinshipFile();
    QString out = this->workDirectory->getOutputDirectory();  // Include module name.
    QString name = this->workDirectory->getModuleName();
    if (out.isNull() || name.isNull())
    {
        QMessageBox::information(nullptr, "Error", "Plese select a  correct work directory!  ");
        return false;
    }

    QString model = ui->modelComboBox->currentText();
    QString ms = ui->msRadioButton->isChecked()? ui->msDoubleSpinBox->text():nullptr;
    QString maf = ui->mafRadioButton->isChecked()? ui->mafDoubleSpinBox->text():nullptr;
    //UserOS *userOS = new UserOS;

    // Genotype file info.
    QFileInfo genoFileInfo = QFileInfo(genotype);
    QString genoFileName = genoFileInfo.fileName();         // demo.vcf.gz
    QString genoFileBaseName = genoFileInfo.baseName();     // geno
    QString genoFileSuffix = genoFileInfo.suffix();         //
    QString genoFileAbPath = genoFileInfo.absolutePath();
    // Need tped/fam files.
    Plink plink;
    if (isVcfFile(genotype)) // Transform "vcf" to "transpose"
    {
        if(!plink.vcf2transpose(genotype, genoFileAbPath+"/"+genoFileBaseName, maf, ms))
        {
            return false;
        }
    }
    if (genotype.split(".")[genotype.split(".").length()-1] == "ped"
            && map.split(".")[map.split(".").length()-1] == "map")  // Transform "plink" to "transpose"
    {
        if (!plink.plink2transpose(genotype, map, genoFileAbPath+"/"+genoFileBaseName, maf, ms))
        {
            return false;
        }
    }
    this->process->start(toolpath+"plink", plink.getParamList());
    if (!this->process->waitForStarted())
    {
        this->resetWindow();
        return false;
    }
    this->runningMsgWidget->setTitle("Making " + genoFileBaseName +".tped/tfam");
    if (!this->process->waitForFinished(-1))
    {
        this->resetWindow();
        QMessageBox::information(nullptr, "Error", "Exit plink with error when make transpose in callEmmaxGwas    !");
        return false;
    }
    this->runningMsgWidget->setTitle(genoFileBaseName +".tped/tfam is made");

    Emmax emmax;
    if (kinship.isNull())
    {
         if (!emmax.makeKinship(genoFileAbPath+"/"+genoFileBaseName))
         {
             return false;  // Make kinship failed.
         }
         this->process->start(toolpath+"emmax-kin", emmax.getParamList());
         if (!this->process->waitForStarted())
         {
             this->resetWindow();
             return false;
         }
         this->runningMsgWidget->setTitle("Making " + genoFileAbPath + "/" + genoFileBaseName + ".hBN.kinf");
         if (!this->process->waitForFinished(-1))
         {
             this->resetWindow();
             QMessageBox::information(nullptr, "Error", "Exit emmax-kin with error when  make kinship    !");
             return false;
         }
         this->runningMsgWidget->setTitle(genoFileAbPath + "/" + genoFileBaseName + ".hBN.kinf is made");
         kinship = genoFileAbPath + "/" + genoFileBaseName + ".hBN.kinf";
    }
    else
    {
        QMessageBox::information(nullptr, "Error", "Making " + genoFileAbPath + "/" + genoFileBaseName + ".hBN.kinf ERROR!   ");
        return false;
    }
    if (emmax.runGWAS(genoFileAbPath+"/"+genoFileBaseName, phenotype, covar, kinship, out+"/"+name))
    {
        this->process->start(toolpath+"emmax", emmax.getParamList());
        // Running message to display message.
        if (!this->process->waitForStarted())
        {
            this->resetWindow();
            return false;
        }
        this->runningMsgWidget->setTitle("Emmax: " + name+" is running...");
        if (!this->process->waitForFinished(-1))
        {
            this->resetWindow();
            QMessageBox::information(nullptr, "Error", "Exit emmax with error when run GWAS    !");
            return false;
        }
        this->runningMsgWidget->setTitle("Emmax: " + name+" is finished");
    }
    else
    {

        return false;
    }

    return true;
}

bool MainWindow::callPlinkGwas(void)
{
    QString toolpath = "//home//chao//Documents//code//H_G/tools//"; // Laptop Linux. Attention
    QString tool = ui->toolComboBox->currentText();

    QString phenotype = this->fileReader->getPhenotypeFile();
    QString genotype = this->fileReader->getGenotypeFile();
    QString map = this->fileReader->getMapFile();
    QString covar = this->fileReader->getCovariateFile();
    QString kinship = this->fileReader->getKinshipFile();
    QString out = this->workDirectory->getOutputDirectory();  // Include module name.
    QString name = this->workDirectory->getModuleName();
    if (out.isNull() || name.isNull())
    {
        QMessageBox::information(nullptr, "Error", "Plese select a  correct work directory!  ");
        return false;
    }

    QString model = ui->modelComboBox->currentText();
    QString ms = ui->msRadioButton->isChecked()? ui->msDoubleSpinBox->text():nullptr;
    QString maf = ui->mafRadioButton->isChecked()? ui->mafDoubleSpinBox->text():nullptr;
    // UserOS *userOS = new UserOS;

    // Genotype file info.
    QFileInfo genoFileInfo = QFileInfo(genotype);
    QString genoFileName = genoFileInfo.fileName();         // demo.vcf.gz
    QString genoFileBaseName = genoFileInfo.baseName();     // geno
    QString genoFileSuffix = genoFileInfo.suffix();         //
    QString genoFileAbPath = genoFileInfo.absolutePath();


    // Run GWAS
    Plink plink;
    if(plink.runGWAS(phenotype, genotype, map, covar, kinship,
                  model, ms, maf, out+"/"+name))
    {
        this->process->start(toolpath+"plink", plink.getParamList());
        if (!this->process->waitForStarted())
        {
            this->resetWindow();
            return false;
        }
        this->runningMsgWidget->setTitle("Plink: " + name+" is running...");
        if (!this->process->waitForFinished(-1))
        {
            this->resetWindow();
            QMessageBox::information(nullptr, "Error", "Exit plink with error when run GWAS    !");
            return false;
        }
        this->runningMsgWidget->setTitle("Plink: " + name+" is finished");
    }
    else
    {
        return false;
    }

    return true;
}

void MainWindow::on_readoutput()
{
    //QString curText = this->runningMsgWidget->getText();
    this->runningMsgWidget->appendText(QString::fromLocal8Bit(this->process->readAllStandardOutput().data()));
    this->runningMsgWidget->repaint();
    qApp->processEvents();
}

void MainWindow::on_readerror()
{
    QString tool = ui->toolComboBox->currentText();
    if (tool == "emmax" || tool == "gemma")
    {   // Emmax let messages print to stand error.
        this->runningMsgWidget->appendText(QString::fromLocal8Bit(this->process->readAllStandardError().data()));
        this->runningMsgWidget->repaint();
        qApp->processEvents();
    }
    else
    {
        QMessageBox::information(nullptr, "Error", QString::fromLocal8Bit(this->process->readAllStandardError().data()));
        this->runningMsgWidget->close();
    }
}

void MainWindow::on_closeRunningWidget()
{
    if (this->process)
    {
        this->process->terminate();
        this->process->waitForFinished(-1);
    }
    ui->rungwasButton->setText("Run");
    ui->rungwasButton->setEnabled(true);
    qApp->processEvents();
}

void MainWindow::on_mafSlider_valueChanged(int value)
{
    ui->mafDoubleSpinBox->setValue(value/100.0);
}

void MainWindow::on_mafDoubleSpinBox_editingFinished()
{
    ui->mafSlider->setValue(int(ui->mafDoubleSpinBox->value() * 100));
}

void MainWindow::on_msDoubleSpinBox_editingFinished()
{
    ui->msSlider->setValue(int(ui->msDoubleSpinBox->value() * 100));
}

void MainWindow::on_msSlider_valueChanged(int value)
{
    ui->msDoubleSpinBox->setValue(value/100.0);
}

bool MainWindow::isVcfFile(QString file) // Just consider filename.
{
    if (file.isNull() || file.isEmpty())
    {
        return false;
    }

    QFileInfo fileInfo = QFileInfo(file);
    QStringList fileNameList = fileInfo.fileName().split(".");         // demo.vcf.gz

    for (QString item : fileNameList)
    {
        if (item == "vcf")
        {
            return true;
        }
    }

    return false;
}

void MainWindow::resetWindow()
{
    if (this->process)
    {
        this->process->terminate();
        this->process->waitForFinished(-1);
    }
    ui->rungwasButton->setText("Run");
    ui->rungwasButton->setEnabled(true);
}
