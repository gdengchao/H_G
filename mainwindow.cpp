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

    // Recommend maf and  ms
    ui->mafDoubleSpinBox->setValue(0.05);
    ui->mindDoubleSpinBox->setValue(0.2);
    ui->genoDoubleSpinBox->setValue(0);
//    ui->mafSlider->setValue(5);
//    ui->msSlider->setValue(10);

    // Initiate variables.
    fileReader = new FileReader;
    workDirectory = new WorkDirectory;
    phenoSelector = new PhenoSelector;
    runningMsgWidget = new RunningMsgWidget;
    gemmaParamWidget = new GemmaParamWidget;
    emmaxParamWidget = new EmmaxParamWidget;
    process = new QProcess;

    // connect QProcess->start(tool) and runningMsgWidget.
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(on_readoutput()));
    connect(process, SIGNAL(readyReadStandardError()), this, SLOT(on_readerror()));
    connect(runningMsgWidget, SIGNAL(closeSignal()), this, SLOT(on_closeRunningWidget()));
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

    gemmaParamWidget->close();
    delete gemmaParamWidget;
    emmaxParamWidget->close();
    delete emmaxParamWidget;

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

    QFile fptr(fileNames[0]);
    fptr.open(QIODevice::ReadOnly|QIODevice::Text);
    QString phenoFirstLine = fptr.readLine();
    phenoFirstLine.replace("\r\n", "");         // Strip "\n"
    phenoFirstLine.replace("\n", "");
    QStringList phenoList = phenoFirstLine.split(QRegExp("\\s+"), QString::SkipEmptyParts);;

    if (phenoList.length() < 3)
    {
        QMessageBox::information(nullptr, "Error", "Phenotype file format error!    ");
        ui->selectedPhenoListWidget->clear();
        ui->excludedPhenoListWidget->clear();
        return;
    }

    ui->pheFileToolButton->setIcon(QIcon(":/new/icon/images/file.png"));    // Set file Icon.
    this->fileReader->setPhenotypeFile(fileNames[0]);

    QFileInfo  pheFileInfo(fileNames[0]);
    QString fileBaseName = pheFileInfo.baseName();
    QString fileName = pheFileInfo.fileName(); // Get the file name from a path.
    ui->pheFileLabel->setText(fileName);

    // Get types of phenotype, and write to list widget.
    QString fileSuffix = pheFileInfo.suffix();
    if (fileSuffix != "phe")
    {   // FID IID PHE1 PHE2 PHE3 ... (With header)

        phenoList.removeFirst();    // Remove first two columns
        phenoList.removeFirst();
        phenoSelector->setSelectedPheno(phenoList);
    }
    else
    {   // FID IID PHE (No header)
        QStringList phenoList;
        phenoList.append(fileBaseName);
        phenoSelector->setSelectedPheno(phenoList);
    }

    QStringList list;
    this->phenoSelector->setExcludedPheno(list);

    ui->selectedPhenoListWidget->clear();
    ui->excludedPhenoListWidget->clear();
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

    QFileInfo  genoFileInfo(fileNames[0]);
    QString fileName = genoFileInfo.fileName(); // Get the file name from a path.
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

    QFileInfo  mapFileInfo(fileNames[0]);
    QString fileName = mapFileInfo.fileName(); // Get the file name from a path.
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

    QFileInfo  covarFileInfo(fileNames[0]);
    QString fileName = covarFileInfo.fileName(); // Get the file name from a path.
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

    QFileInfo  kinFileInfo(fileNames[0]);
    QString fileName = kinFileInfo.fileName(); // Get the file name from a path.
    ui->kinFileLabel->setText(fileName);
}

void MainWindow::on_browButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Choose directory");
    if (!dir.isEmpty())
    {
        this->workDirectory->setOutputDirectory(dir);
        ui->outdirLineEdit->setText(dir+"/"+this->workDirectory->getProjectName());
    }
}


void MainWindow::on_excludeAllPhenoButton_clicked()
{
    if (ui->selectedPhenoListWidget->count() <= 1)
    {
        return;
    }
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
    if (ui->selectedPhenoListWidget->count() <= 1)
    {
        return;
    }
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

    QString toolpath = "//home//chao//Documents//code//H_G/tools//"; // Laptop Linux. Attention
    QString tool = ui->toolComboBox->currentText();

    QString phenotype = this->fileReader->getPhenotypeFile();
    QString genotype = this->fileReader->getGenotypeFile();
    QString map = this->fileReader->getMapFile();
    QString covar = this->fileReader->getCovariateFile();
    QString kinship = this->fileReader->getKinshipFile();
    QString out = this->workDirectory->getOutputDirectory();  // Include project name.
    QString name = this->workDirectory->getProjectName();

    ui->rungwasButton->setText("Running");
    ui->rungwasButton->setDisabled(true);
    this->runningMsgWidget->clearText();
    this->runningMsgWidget->show();

    QFileInfo pheFileInfo(phenotype);
    QString pheFileBaseName = pheFileInfo.baseName();
    QString pheFileAbPath = pheFileInfo.absolutePath();
    QString pheFileSuffix = pheFileInfo.suffix();

    if (pheFileSuffix == "phe")
    {
        if (tool == "emmax")
        {
            if (!this->callEmmaxGwas(toolpath, phenotype, genotype, map, covar, kinship, out, name))
            {
                this->resetWindow();
                return;
            }
        }

        if (tool == "gemma")
        {
            if (!this->callGemmaGwas(toolpath, phenotype, genotype, map, covar, kinship, out, name))
            {
                this->resetWindow();
                return;
            }
        }

        if (tool == "plink")  // plink GWAS
        {
            if (!this->callPlinkGwas(toolpath, phenotype, genotype, map, covar, kinship, out, name))
            {
                this->resetWindow();
                return;
            }
        }
    }
    else
    {
        for (int i = 0; i < ui->selectedPhenoListWidget->count(); i++)
        {
            QListWidgetItem *item = ui->selectedPhenoListWidget->item(i);

            if (!this->makePheFile(phenotype, item->text()))
            {
                this->resetWindow();
                return;
            }
            QString madedPheFile = pheFileAbPath + "/" + item->text() + ".phe";
            if (tool == "emmax")
            {
                if (!this->callEmmaxGwas(toolpath, madedPheFile, genotype, map, covar, kinship, out, name))
                {
                    this->resetWindow();

                    return;
                }
            }

            if (tool == "gemma")
            {
                if (!this->callGemmaGwas(toolpath, madedPheFile, genotype, map, covar, kinship, out, name))
                {
                    this->resetWindow();
                    return;
                }
            }

            if (tool == "plink")  // plink GWAS
            {
                if (!this->callPlinkGwas(toolpath, madedPheFile, genotype, map, covar, kinship, out, name))
                {
                    this->resetWindow();
                    return;
                }
            }
        }
    }

    this->resetWindow();
}

bool MainWindow::callGemmaGwas(QString toolpath, QString phenotype, QString genotype, QString map,
                               QString covar, QString kinship, QString out, QString name)
{
    if (out.isNull() || name.isNull())
    {
        QMessageBox::information(nullptr, "Error", "Plese select a  correct work directory!  ");
        return false;
    }

    // Detail parameters.
    QMap<QString, QString> moreParam = this->gemmaParamWidget->getCurrentParam();

    QString model = ui->modelComboBox->currentText();
    QString maf = ui->mafRadioButton->isChecked()? ui->mafDoubleSpinBox->text():nullptr;
    QString mind = ui->mindRadioButton->isChecked()? ui->mindDoubleSpinBox->text():nullptr;
    QString geno = ui->genoRadioButton->isChecked()? ui->genoDoubleSpinBox->text():nullptr;
    //UserOS *userOS = new UserOS;

    // Genotype file info.
    QFileInfo genoFileInfo = QFileInfo(genotype);
    QString genoFileName = genoFileInfo.fileName();         // demo.vcf.gz
    QString genoFileBaseName = genoFileInfo.baseName();     // geno
    QString genoFileSuffix = genoFileInfo.suffix();         //
    QString genoFileAbPath = genoFileInfo.absolutePath();

    // Phenotype file info.
    QFileInfo pheFileInfo = QFileInfo(phenotype);
    QString pheFileBaseName = pheFileInfo.baseName();
\
    // Necessary to transform file ?
    bool transformFileFlag = false;
    bool filterDataFlag = false;

    // Need binary files.  Every temp file and a "_tmp" after baseName, and will be deleted after gwas.
    Plink plink;
    if (isVcfFile(genotype)) // Transform "vcf" to "transpose"
    {
        if(!plink.vcf2binary(genotype, genoFileAbPath+"/"+genoFileBaseName+"_tmp", maf, mind, geno))
        {
            this->resetWindow();
            return false;
        }

        transformFileFlag = true;
    }
    if (genotype.split(".")[genotype.split(".").length()-1] == "ped")  // Transform "plink" to "binary"
    {
        if (map.isNull())
        {
            map = genoFileAbPath+"/"+genoFileBaseName+".map";
        }
        if (!plink.plink2binary(genotype, map, genoFileAbPath+"/"+genoFileBaseName+"_tmp", maf, mind, geno))
        {
            this->resetWindow();
            return false;
        }

        transformFileFlag = true;
    }

    if (genotype.split(".")[genotype.split(".").length()-1] == "tped")  // Transform "transpose" to "binary"
    {
        if (map.isNull())
        {
            map = genoFileAbPath+"/"+genoFileBaseName+".tfam";
        }
        if (!plink.transpose2binary(genotype, map, genoFileAbPath+"/"+genoFileBaseName+"_tmp", maf, mind, geno))
        {
            this->resetWindow();
            return false;
        }
        transformFileFlag = true;
    }

    if (genotype.split(".")[genotype.split(".").length()-1] == "bed")
    {   // When don't set any QC param, it won't execute.
        plink.filterBinaryFile(genoFileAbPath+"/"+genoFileBaseName, maf, mind, geno, genoFileAbPath+"/"+genoFileBaseName+"_tmp");
        filterDataFlag = true;
    }

    if (transformFileFlag || filterDataFlag)
    {   // Run plink to transform file or filter data.
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
    }

    Gemma gemma;

    if (!gemma.phe_fam_Preparation(phenotype, genoFileAbPath+"/"+genoFileBaseName+"_tmp"+".fam"))
    {   // Replace "NA" to "-9", then complete .fam
        // .fam: FID IID PID MID Sex 1 Phe  (phenotype data to the 7th column of .fam)
        this->resetWindow();
        return false;
    }

    if (kinship.isNull())
    {
         if (!gemma.makeKinship(genoFileAbPath+"/"+genoFileBaseName+"_tmp", genoFileBaseName+"_tmp", moreParam))
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
         //this->runningMsgWidget->setTitle("Making " + genoFileBaseName+"_tmp" + ".cXX.txt");
         this->runningMsgWidget->setTitle("Making kinship");
         if (!this->process->waitForFinished(-1))
         {
             this->resetWindow();
             QMessageBox::information(nullptr, "Error", "Exit gemma with error when  make kinship!   ");
             return false;
         }
         //this->runningMsgWidget->setTitle(genoFileBaseName+"_tmp" + ".cXX.txt is made");
         this->runningMsgWidget->setTitle("Kinship is made");
         //kinship = genoFileAbPath + "/output/" + genoFileBaseName + ".cXX.txt";    // Attention
         kinship = QDir::currentPath() + "/output/" + genoFileBaseName+"_tmp" + ".cXX.txt";
    }

    if (gemma.runGWAS(genoFileAbPath+"/"+genoFileBaseName+"_tmp", phenotype, covar, kinship,
                      name+"_"+pheFileBaseName, model, moreParam))
    {
        this->process->start(toolpath+"gemma", gemma.getParamList());
        // Running message to display message.
        if (!this->process->waitForStarted())
        {
            this->resetWindow();
            return false;
        }
        this->runningMsgWidget->setTitle("Gemma(" + pheFileBaseName+ "): " + name+" is running...");
        if (!this->process->waitForFinished(-1))
        {
            this->resetWindow();
            QMessageBox::information(nullptr, "Error", "Exit gemma with error when run GWAS! ");
            return false;
        }

        QFile file;
        // delete intermidiate file.
        file.remove(genoFileAbPath+"/"+genoFileBaseName+"_tmp.bed");
        file.remove(genoFileAbPath+"/"+genoFileBaseName+"_tmp.bim");
        file.remove(genoFileAbPath+"/"+genoFileBaseName+"_tmp.fam");
        file.remove(genoFileAbPath+"/"+genoFileBaseName+"_tmp.log");
        file.remove(genoFileAbPath+"/"+genoFileBaseName+"_tmp.nosex");
        file.remove(QDir::currentPath() + "/output/"+genoFileBaseName+"_tmp.cXX.txt");
        file.remove(QDir::currentPath() + "/output/"+genoFileBaseName+"_tmp.log.txt");

        QDir dir;   // gemma output in the execution file dir by default.
        QDir objDir(out+"/output");
        // We move it to the work dir.
        int i = 0;
        while(objDir.exists())
        {   // It will be wrong when object dir existed.
            i++;
            objDir.setPath(out+"/output"+QString::number(i));
        }
        // It will be wrong when "/output" change to "/output/"
        dir.rename(QDir::currentPath() + "/output", out+"/output"+(i==0?"":QString::number(i)));

        this->runningMsgWidget->setTitle("Gemma("+pheFileBaseName+"): "+name+" is finished");
    }
    else
    {
        this->resetWindow();
        return false;
    }

    return true;
}

bool MainWindow::callEmmaxGwas(QString toolpath, QString phenotype, QString genotype, QString map,
                               QString covar, QString kinship, QString out, QString name)
{
    if (out.isNull() || name.isNull())
    {
        QMessageBox::information(nullptr, "Error", "Plese select a  correct work directory!  ");
        return false;
    }

    // Detail parameters.
    QMap<QString, QString> moreParam = this->emmaxParamWidget->getCurrentParam();

    QString model = ui->modelComboBox->currentText();
    QString maf = ui->mafRadioButton->isChecked()? ui->mafDoubleSpinBox->text():nullptr;
    QString mind = ui->mindRadioButton->isChecked()? ui->mindDoubleSpinBox->text():nullptr;
    QString geno = ui->genoRadioButton->isChecked()? ui->genoDoubleSpinBox->text():nullptr;
    //UserOS *userOS = new UserOS;

    // Genotype file info.
    QFileInfo genoFileInfo = QFileInfo(genotype);
    QString genoFileName = genoFileInfo.fileName();
    QString genoFileBaseName = genoFileInfo.baseName();
    QString genoFileSuffix = genoFileInfo.suffix();
    QString genoFileAbPath = genoFileInfo.absolutePath();

    // Phenotype file info.
    QFileInfo pheFileInfo = QFileInfo(phenotype);
    QString pheFileBaseName = pheFileInfo.baseName();

    // Necessary to transform file ?
    bool transformFileFlag = false;
    bool filterDataFlag = false;

    // Need tped/fam files. Add "_tmp", then delete after gwas.
    Plink plink;
    if (isVcfFile(genotype)) // Transform "vcf" to "transpose"
    {
        if(!plink.vcf2transpose(genotype, genoFileAbPath+"/"+genoFileBaseName+"_tmp", maf, mind, geno))
        {
            return false;
        }
        transformFileFlag = true;
    }
    if (genotype.split(".")[genotype.split(".").length()-1] == "ped")  // Transform "plink" to "transpose"
    {
        if (map.isNull())
        {
            map = genoFileAbPath+"/"+genoFileBaseName+".map";
        }

        if (!plink.plink2transpose(genotype, map, genoFileAbPath+"/"+genoFileBaseName+"_tmp", maf, mind, geno))
        {
            return false;
        }
        transformFileFlag = true;
    }
    if (genotype.split(".")[genotype.split(".").length()-1] == "bed")  // Transform "binary" to "transpose"
    {
        if (!plink.binary2transpose(genoFileAbPath+"/"+genoFileBaseName,
                                    genoFileAbPath+"/"+genoFileBaseName+"_tmp", maf, mind, geno))
        {
            return false;
        }
        transformFileFlag = true;
    }

    if (genotype.split(".")[genotype.split(".").length()-1] == "tped")  // Transform "binary" to "transpose"
    {
        if (map.isNull())
        {
            map = genoFileAbPath + "/" + genoFileBaseName + ".tfam";
        }
        plink.filterTransposeFile(genotype, map, maf, mind, geno, genoFileAbPath+"/"+genoFileBaseName+"_tmp");
        filterDataFlag = true;
    }

    if (transformFileFlag || filterDataFlag)
    {
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
    }

    Emmax emmax;
    if (kinship.isNull() && emmaxParamWidget->isMakeKinAuto())
    {
         if (!emmax.makeKinship(genoFileAbPath+"/"+genoFileBaseName+"_tmp", moreParam))
         {
             return false;  // Make kinship failed.
         }
         this->process->start(toolpath+"emmax-kin", emmax.getParamList());
         if (!this->process->waitForStarted())
         {
             this->resetWindow();
             return false;
         }
         //this->runningMsgWidget->setTitle("Making " + genoFileBaseName + ".hBN.kinf");
         this->runningMsgWidget->setTitle("Making kinship");
         if (!this->process->waitForFinished(-1))
         {
             this->resetWindow();
             QMessageBox::information(nullptr, "Error", "Exit emmax-kin with error when  make kinship    !");
             return false;
         }
         //this->runningMsgWidget->setTitle(genoFileBaseName + ".hBN.kinf is made");
         this->runningMsgWidget->setTitle("Kinship is made");

         if (emmaxParamWidget->isBNkinMatrix())
         {
             kinship = genoFileAbPath + "/" + genoFileBaseName+"_tmp" + ".hBN.kinf";
         }
         else
         {
            kinship = genoFileAbPath + "/" + genoFileBaseName+"_tmp" + ".hIBS.kinf";
         }
    }
//    else if (emmaxParamWidget->isMakeKinAuto())
//    {
//        QMessageBox::information(nullptr, "Error", "Making kinship ERROR!   ");
//        return false;
//    }
    if (emmax.runGWAS(genoFileAbPath+"/"+genoFileBaseName+"_tmp", phenotype, covar, kinship,
                      out+"/"+name+"_"+pheFileBaseName, moreParam))
    {
        this->process->start(toolpath+"emmax", emmax.getParamList());
        // Running message to display message.
        if (!this->process->waitForStarted())
        {
            this->resetWindow();
            return false;
        }
        this->runningMsgWidget->setTitle("Emmax(" + pheFileBaseName+"): " + name + " is running...");
        if (!this->process->waitForFinished(-1))
        {
            this->resetWindow();
            QMessageBox::information(nullptr, "Error", "Exit emmax with error when run GWAS    !");
            return false;
        }


        QFile file;
        // delete intermidiate file.
        file.remove(genoFileAbPath+"/"+genoFileBaseName+"_tmp.map");
        file.remove(genoFileAbPath+"/"+genoFileBaseName+"_tmp.tped");
        file.remove(genoFileAbPath+"/"+genoFileBaseName+"_tmp.tfam");
        file.remove(genoFileAbPath+"/"+genoFileBaseName+"_tmp.log");
        file.remove(genoFileAbPath+"/"+genoFileBaseName+"_tmp.nosex");
        if (emmaxParamWidget->isBNkinMatrix())
        {
            file.remove(genoFileAbPath+"/"+genoFileBaseName+"_tmp.hBN.kinf");
        }
        else
        {
            file.remove(genoFileAbPath+"/"+genoFileBaseName+"_tmp.hIBS.kinf");
        }

        this->runningMsgWidget->setTitle("Emmax(" + pheFileBaseName+"): " + name + " is finished");
    }
    else
    {
        return false;
    }

    return true;
}

bool MainWindow::callPlinkGwas(QString toolpath, QString phenotype, QString genotype, QString map,
                               QString covar, QString kinship, QString out, QString name)
{
    if (out.isNull() || name.isNull())
    {
        QMessageBox::information(nullptr, "Error", "Plese select a  correct work directory!  ");
        return false;
    }

    QString model = ui->modelComboBox->currentText();
    QString maf = ui->mafRadioButton->isChecked()? ui->mafDoubleSpinBox->text():nullptr;
    QString mind = ui->mindRadioButton->isChecked()? ui->mindDoubleSpinBox->text():nullptr;
    QString geno = ui->genoRadioButton->isChecked()? ui->genoDoubleSpinBox->text():nullptr;
    // UserOS *userOS = new UserOS;

    // Genotype file info.
    QFileInfo genoFileInfo = QFileInfo(genotype);
    QString genoFileName = genoFileInfo.fileName();         // demo.vcf.gz
    QString genoFileBaseName = genoFileInfo.baseName();     // geno
    QString genoFileSuffix = genoFileInfo.suffix();         //
    QString genoFileAbPath = genoFileInfo.absolutePath();

    // Phenotype file info.
    QFileInfo pheFileInfo = QFileInfo(phenotype);
    QString pheFileBaseName = pheFileInfo.baseName();

    Plink plink;

    // Run GWAS
    if(plink.runGWAS(phenotype, genotype, map, covar, kinship, model,
                     maf, mind, geno, out+"/"+name+"_"+pheFileBaseName))
    {
        this->process->start(toolpath+"plink", plink.getParamList());
        if (!this->process->waitForStarted())
        {
            this->resetWindow();
            return false;
        }
        this->runningMsgWidget->setTitle("Plink(" + pheFileBaseName+"): " + name+" is running...");
        if (!this->process->waitForFinished(-1))
        {
            this->resetWindow();
            QMessageBox::information(nullptr, "Error", "Exit plink with error when run GWAS    !");
            return false;
        }
        this->runningMsgWidget->setTitle("Plink(" + pheFileBaseName+"): " + name+" is finished");
    }
    else
    {
        return false;
    }

    return true;
}

void MainWindow::on_readoutput()
{
    QString text = QString::fromLocal8Bit(this->process->readAllStandardOutput().data());
    this->runningMsgWidget->appendText(text);
    this->runningMsgWidget->repaint();
    qApp->processEvents();
    QThread::msleep(10);
}

void MainWindow::on_readerror()
{
    QString tool = ui->toolComboBox->currentText();
//    if (tool == "emmax" || tool == "gemma")
//    {   // Emmax let messages print to stand error.
//        this->runningMsgWidget->appendText(QString::fromLocal8Bit(this->process->readAllStandardError().data()));
//        this->runningMsgWidget->repaint();
//        qApp->processEvents();
//    }
//    else
//    {
//        QMessageBox::information(nullptr, "Error", QString::fromLocal8Bit(this->process->readAllStandardError().data()));
//        this->runningMsgWidget->close();
//    }
    this->runningMsgWidget->appendText(QString::fromLocal8Bit(this->process->readAllStandardError().data()));
    this->runningMsgWidget->repaint();
    qApp->processEvents();
}

void MainWindow::on_closeRunningWidget()
{
    this->resetWindow();
    qApp->processEvents();
}

//void MainWindow::on_mafSlider_valueChanged(int value)
//{
//    ui->mafDoubleSpinBox->setValue(value/100.0);
//}

//void MainWindow::on_mafDoubleSpinBox_editingFinished()
//{
//    ui->mafSlider->setValue(int(ui->mafDoubleSpinBox->value() * 100));
//}

//void MainWindow::on_msDoubleSpinBox_editingFinished()
//{
//    ui->msSlider->setValue(int(ui->msDoubleSpinBox->value() * 100));
//}

//void MainWindow::on_msSlider_valueChanged(int value)
//{
//    ui->msDoubleSpinBox->setValue(value/100.0);
//}

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
        this->process->waitForFinished();
    }
    ui->rungwasButton->setText("Run");
    ui->rungwasButton->setEnabled(true);
}

void MainWindow::on_projectNameLineEdit_textChanged(const QString &text)
{
    this->workDirectory->setProjectName(text);
    if (!ui->outdirLineEdit->text().isEmpty())
    {   // If a out directory is selected, display the out directory + the project name.
        ui->outdirLineEdit->setText(this->workDirectory->getOutputDirectory()+"/"+text);
    }
}

void MainWindow::on_toolComboBox_currentTextChanged(const QString &tool)
{
    ui->modelComboBox->clear();
    if (tool == "plink")
    {
        Plink plink;
        ui->modelComboBox->addItems(plink.getSupportedModel());
    }

    if (tool == "gemma")
    {
        Gemma gemma;
        ui->modelComboBox->addItems(gemma.getSupportedModel());
    }
    if (tool == "emmax")
    {
        Emmax emmax;
        ui->modelComboBox->addItems(emmax.getSupportedModel());
    }
}

bool MainWindow::makePheFile(QString const phenotype, QString const selectedPheno)
{
    if (phenotype.isNull() || selectedPheno.isEmpty())
    {
        return false;
    }

    QFileInfo pheFileInfo(phenotype);
    QString pheFileAbPath = pheFileInfo.absolutePath();
    QString pheFileBaseName = pheFileInfo.baseName();

    QFile srcFile(phenotype);
    QFile dstFile(pheFileAbPath+"/"+selectedPheno+".phe");

    if (!srcFile.open(QIODevice::ReadOnly) || !dstFile.open(QIODevice::ReadWrite))
    {
        return false;
    }

    QTextStream srcFileStream(&srcFile);
    QTextStream dstFileStream(&dstFile);

    QStringList header = srcFileStream.readLine().split("\t");   // header

    int selectedPheIndex = header.indexOf(selectedPheno);   // get the columns of selected phenotype.

    if (selectedPheIndex == -1)
    {
        return false;
    }

    while (!srcFileStream.atEnd())
    {
        QString outLine;
        QStringList line = srcFileStream.readLine().split("\t", QString::SkipEmptyParts);
        if (selectedPheIndex >= line.length())
        {   // Any missing rate?
             outLine = line[0] + "\t" + line[1] + "\tNA\n";
        }
        else
        {
            outLine = line[0] + "\t" + line[1] + "\t" + line[selectedPheIndex] + "\n";
        }
        //dstFileStream << line[0] << "\t" << line[1] << "\t" << line[selectedPheIndex] << "\n";
        dstFileStream << outLine;
    }

    srcFile.close();
    dstFile.close();

    return true;
}

void MainWindow::on_detailPushButton_clicked()
{
    if (ui->toolComboBox->currentText() == "gemma")
    {
        this->gemmaParamWidget->show();
    }

    if (ui->toolComboBox->currentText() == "emmax")
    {
        this->emmaxParamWidget->show();
    }

}
