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
    graphViewer = new GraphViewer;

    // Default output directory setting
    workDirectory->setProjectName("pro1");
    workDirectory->setOutputDirectory("/home/chao/Desktop/test");
    ui->projectNameLineEdit->setText(workDirectory->getProjectName());
    ui->outdirLineEdit->setText(workDirectory->getOutputDirectory()+"/"+workDirectory->getProjectName());

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

    delete graphViewer;
}


void MainWindow::on_pheFileToolButton_clicked()
{
    // Basic display
    QFileDialog *fileDialog = new QFileDialog(this, "Open phenotype file", "", "pheno(*.phe *.txt);;all(*)");
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
    QFileDialog *fileDialog = new QFileDialog(this, "Open genotype file", "", "geno(*.vcf *.ped *.tped *.bed);;all(*)");
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
    QFileDialog *fileDialog = new QFileDialog(this, "Open map file", "", "map(*.map *.tfam);;all(*)");
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
    QFileDialog *fileDialog = new QFileDialog(this, "Open covariate file", "", "covar(*.cov *.covar *.txt);;all(*)");
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
    QFileDialog *fileDialog = new QFileDialog(this, "Open kinship file", "", "pheno(*.kin *.kinf *.txt);;all(*)");
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

void MainWindow::on_outdirBrowButton_clicked()
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
    QString tool = ui->toolComboBox->currentText();
    QString phenotype = this->fileReader->getPhenotypeFile();
    QString genotype = this->fileReader->getGenotypeFile();
    QString map = this->fileReader->getMapFile();
    QString covar = this->fileReader->getCovariateFile();
    QString kinship = this->fileReader->getKinshipFile();
    QString out = this->workDirectory->getOutputDirectory();  // Include project name.
    QString name = this->workDirectory->getProjectName();

    if (genotype.isNull())
    {
        QMessageBox::information(nullptr, "Error", "Plese select a genotype file!  ");
        return;
    }
    if (out.isNull() || name.isNull())
    {
        QMessageBox::information(nullptr, "Error", "Plese select a  correct work directory!  ");
        return;
    }

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
            if (!this->callEmmaxGwas(phenotype, genotype, map, covar, kinship, out, name))
            {
                this->resetWindow();
                return;
            }
        }

        if (tool == "gemma")
        {
            if (!this->callGemmaGwas(phenotype, genotype, map, covar, kinship, out, name))
            {
                this->resetWindow();
                return;
            }
        }

        if (tool == "plink")  // plink GWAS
        {
            if (!this->callPlinkGwas(phenotype, genotype, map, covar, kinship, out, name))
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
                if (!this->callEmmaxGwas(madedPheFile, genotype, map, covar, kinship, out, name))
                {
                    this->resetWindow();

                    return;
                }
            }

            if (tool == "gemma")
            {
                if (!this->callGemmaGwas(madedPheFile, genotype, map, covar, kinship, out, name))
                {
                    this->resetWindow();
                    return;
                }
            }

            if (tool == "plink")  // plink GWAS
            {
                if (!this->callPlinkGwas(madedPheFile, genotype, map, covar, kinship, out, name))
                {
                    this->resetWindow();
                    return;
                }
            }
        }
    }

    this->resetWindow();
}

bool MainWindow::callGemmaGwas(QString phenotype, QString genotype, QString map,
                               QString covar, QString kinship, QString out, QString name)
{
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
        this->process->start(this->toolpath+"plink", plink.getParamList());
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


    if (gemmaParamWidget->isFamCompletedAuto()
            && !gemma.phe_fam_Preparation(phenotype, genoFileAbPath+"/"+genoFileBaseName+"_tmp"+".fam"))
    {   // Replace "NA" to "-9", then complete .fam
        // .fam: FID IID PID MID Sex 1 Phe  (phenotype data to the 7th column of .fam)
        this->resetWindow();
        return false;
    }


    if (kinship.isNull() && this->gemmaParamWidget->isMakeRelatedMatAuto())
    {
         if (!gemma.makeKinship(genoFileAbPath+"/"+genoFileBaseName+"_tmp", genoFileBaseName+"_tmp", moreParam))
         {
             this->resetWindow();
             return false;  // Make kinship failed.
         }
         this->process->start(this->toolpath + "gemma", gemma.getParamList());
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

         if (moreParam["kinmatrix"] == "1")
         {
             kinship = QDir::currentPath() + "/output/" + genoFileBaseName+"_tmp" + ".cXX.txt";
         }
         else
         {
             kinship = QDir::currentPath() + "/output/" + genoFileBaseName+"_tmp" + ".sXX.txt";
         }
    }

    if (gemma.runGWAS(genoFileAbPath+"/"+genoFileBaseName+"_tmp", phenotype, covar, kinship,
                      name+"_"+pheFileBaseName, model, moreParam))
    {
        this->process->start(this->toolpath+"gemma", gemma.getParamList());
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
        if (moreParam["kinmatrix"] == "1")
        {
            file.remove(QDir::currentPath() + "/output/"+genoFileBaseName+"_tmp.cXX.txt");
        }
        else
        {
            file.remove(QDir::currentPath() + "/output/"+genoFileBaseName+"_tmp.sXX.txt");
        }
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

//        ui->gwasResultLineEdit->setText(out+"/output"+(i==0?"":QString::number(i))+"/"+name+"_"+pheFileBaseName);


        if (model == "LMM")
        {
            ui->gwasResultLineEdit->setText(out+"/output"+(i==0?"":QString::number(i))
                                            +"/"+name+"_"+pheFileBaseName+".assoc.txt");
        }

        this->runningMsgWidget->setTitle("Gemma("+pheFileBaseName+"): "+name+" is finished");
    }
    else
    {
        this->resetWindow();
        return false;
    }

    return true;
}

bool MainWindow::callEmmaxGwas(QString phenotype, QString genotype, QString map,
                               QString covar, QString kinship, QString out, QString name)
{
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
        this->process->start(this->toolpath+"plink", plink.getParamList());
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
         this->process->start(this->toolpath+"emmax-kin", emmax.getParamList());
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
        this->process->start(this->toolpath+"emmax", emmax.getParamList());
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

        ui->gwasResultLineEdit->setText(out+"/"+name+"_"+pheFileBaseName+".ps");

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

bool MainWindow::callPlinkGwas(QString phenotype, QString genotype, QString map,
                               QString covar, QString kinship, QString out, QString name)
{
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
        this->process->start(this->toolpath+"plink", plink.getParamList());
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

        ui->gwasResultLineEdit->setText(out+"/"+name+"_"+pheFileBaseName+".assoc."+model.toLower());

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
    this->runningMsgWidget->setText(this->refreshMessage(this->runningMsgWidget->getText(), text));
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
{   // When edit finished and the current text is empty, set a default name.
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
        if (ui->modelComboBox->currentText() == "LMM")
        {
            this->gemmaParamWidget->setLmmParamEnabled(true);
            this->gemmaParamWidget->setBslmmParamEnabled(false);
        }
        else
        {
            this->gemmaParamWidget->setLmmParamEnabled(false);
            this->gemmaParamWidget->setBslmmParamEnabled(true);
        }

        this->gemmaParamWidget->show();
    }

    if (ui->toolComboBox->currentText() == "emmax")
    {
        this->emmaxParamWidget->show();
    }
}

QString MainWindow::refreshMessage(QString curText, QString newText)
{
    if (newText.isEmpty())
    {   // newMsg don't have valid message.
        return curText;
    }
    if (curText.isEmpty())
    {   // The first message.
        return newText;
    }

    if (curText[curText.size()-1] == '%' && newText[newText.size()-1] == '%')
    {   // Refresh percent.
        QString::iterator iter = newText.end(); // After the last char.
        QString newPercent = "%";
        iter--;iter--;  // Pointer to the last number.
        // Get the last percent in newText when there multi percent in the same line.
        while ((*iter).isNumber())
        {
            newPercent = *iter + newPercent;
            iter--;
        }

        // Remove the percent in curText.

        while(curText[curText.size()-1] == '%')
        {
            curText.remove(curText.size()-1, 1);
            iter = curText.end();
            iter--; iter--;
            while ((*iter).isNumber())
            {
                curText.remove(curText.size()-1, 1);
            }
        }

        return curText + newText;
    }

    qDebug() << curText[curText.size() - 1] << "\t" << newText[newText.size()-1];
    return curText + newText;
}

void MainWindow::on_projectNameLineEdit_editingFinished()
{
    if (ui->projectNameLineEdit->text().isEmpty())
    {
        ui->projectNameLineEdit->setText("pro1");
    }
}

void MainWindow::on_drawManPushButton_clicked()
{
    ui->drawManPushButton->setEnabled(false);
    qApp->processEvents();

    try {
        QString gwasResulFile = ui->gwasResultLineEdit->text();
        QString qqmanFile = makeQQManInputFile(gwasResulFile);
        if (qqmanFile.isNull() || gwasResulFile.isEmpty())
        {
            throw -1;
        }
        this->drawManhattan(qqmanFile, this->workDirectory->getOutputDirectory()
                         +"/"+this->workDirectory->getProjectName()+"_man.png");

        QFile file;
        file.remove(qqmanFile);
    } catch (int) {
        ui->drawManPushButton->setEnabled(true);
        qApp->processEvents();
    }
    ui->drawManPushButton->setEnabled(true);
    qApp->processEvents();
}

void MainWindow::on_drawQQPushButton_clicked()
{
    try {
        ui->drawQQPushButton->setEnabled(false);
        qApp->processEvents();

        QString gwasResulFile = ui->gwasResultLineEdit->text();
        QString qqmanFile = makeQQManInputFile(gwasResulFile);
        if (qqmanFile.isNull() || gwasResulFile.isEmpty())
        {
            throw -1;
        }
        this->drawQQplot(qqmanFile, this->workDirectory->getOutputDirectory()
                         +"/"+this->workDirectory->getProjectName()+"_qq.png");

        QFile file;
        file.remove(qqmanFile);
    } catch (int) {
        ui->drawQQPushButton->setEnabled(true);
        qApp->processEvents();
    }

    ui->drawQQPushButton->setEnabled(true);
    qApp->processEvents();
}

void MainWindow::drawManhattan(QString data, QString out)
{
    if (data.isNull() || out.isNull())
    {
        return;
    }

    int gwBase =  ui->gwBaseLineEdit->text().toInt();
    int gwExpo = ui->gwExpoLineEdit->text().toInt();
    int sgBase = ui->sgBaseLineEdit->text().toInt();
    int sgExpo = ui->sgExpoLineEdit->text().toInt();

    QProcess proc;
    QStringList param;
    param.append(this->scriptpath+"plot.R");
    param.append("manhattan");
    param.append(data);
    param.append(out);
    param.append(QString::number(gwBase)+'e'+QString::number(gwExpo));
    param.append(QString::number(sgBase)+'e'+QString::number(sgExpo));

    proc.start("Rscript", param);
    proc.waitForStarted();
    proc.waitForFinished(-1);


//    if (!QString::fromLocal8Bit(proc.readAllStandardError().data()).isEmpty())
//    {
//        QMessageBox::information(nullptr, "Error", QString::fromLocal8Bit(proc.readAllStandardError().data()));
//    }

//    if (!QString::fromLocal8Bit(proc.readAllStandardOutput().data()).isEmpty())
//    {
//        QMessageBox::information(nullptr, "Output", QString::fromLocal8Bit(proc.readAllStandardOutput().data()));
//    }

    this->graphViewer->setGraph(out);
    this->graphViewer->show();
}

void MainWindow::drawQQplot(QString data, QString out)
{
    if (data.isNull() || out.isNull())
    {
        return;
    }

    QProcess proc;
    QStringList param;
    param.append(this->scriptpath+"plot.R");
    param.append("qqplot");
    param.append(data);
    param.append(out);

    proc.start("Rscript", param);
    proc.waitForStarted();
    proc.waitForFinished(-1);

//    if (!QString::fromLocal8Bit(proc.readAllStandardError().data()).isEmpty())
//    {
//        QMessageBox::information(nullptr, "Error", QString::fromLocal8Bit(proc.readAllStandardError().data()));
//    }

//    if (!QString::fromLocal8Bit(proc.readAllStandardOutput().data()).isEmpty())
//    {
//        QMessageBox::information(nullptr, "Output", QString::fromLocal8Bit(proc.readAllStandardOutput().data()));
//    }

    this->graphViewer->setGraph(out);
    this->graphViewer->show();
}

void MainWindow::on_gwasReultBrowButton_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this, "Open GWAS result file", this->workDirectory->getOutputDirectory(),
                                              "result(*.linear *.logistic *.ps *.txt);;all(*)");
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
    ui->gwasResultLineEdit->setText(fileNames[0]);

    QFileInfo  genoFileInfo(fileNames[0]);
}

/**
 * @brief MainWindow::makeQQmanFile
 * @param pvalueFile
 * @return  A file will be a input of manhattan.(header: SNP CHR BP P)
 */
QString MainWindow::makeQQManInputFile(QString pvalueFile)
{
    if (pvalueFile.isNull() || pvalueFile.isEmpty())
    {
        return pvalueFile;
    }

    QFileInfo pvalueFileInfo(pvalueFile);
    QString pvalueFileAbPath = pvalueFileInfo.absolutePath();
    QString pvalueFileBaseName = pvalueFileInfo.baseName();
    QString pvalueFileSuffix = pvalueFileInfo.suffix();
    QString pvalueFileComSuffix = pvalueFileInfo.completeSuffix();

    if ( pvalueFileComSuffix == "assoc.linear" || pvalueFileComSuffix == "assoc.logistic")
    {   // Plink output file don't need to be transformed.
        return pvalueFile;
    }

    /* Script(one line):
     *  sed 's/:/\t/g; s/chr//g' 222_filter1_phe1_BN.ps
     *  |perl -alne '{if(/^0/){print "19\tchr0:$F[1]\t$F[0]\t$F[1]\t$F[3]"}
     *  else{print "$F[0]\tchr$F[0]:$F[1]\t$F[0]\t$F[1]\t$F[3]"}}'
     *  |sort -k 1,1n -k4,4n |perl -alne '{$num=shift @F;
     *  $line=join "\t",@F; print $line}'
     *  |sed '1 i\SNP\tCHR\tBP\tP' > phe1_emmax_table
     */

    QFile gwasResultFile(pvalueFile);
    QFile qqmanInputFile(pvalueFileAbPath+"/"+pvalueFileBaseName+"_"+ui->toolComboBox->currentText()+".qqman");
    QTextStream gwasResultFileStream(&gwasResultFile);
    QTextStream qqmanInputFileStream(&qqmanInputFile);
    if (!gwasResultFile.open(QIODevice::ReadOnly) || !qqmanInputFile.open(QIODevice::ReadWrite))
    {
        QMessageBox::information(nullptr, "Error", "Open gwasResultFile error.  ");
        return nullptr;
    }
    if (pvalueFileSuffix == "ps")
    {   // Emmax output file.
        /* From: .ps:
         *      [SNP ID(CHR:BP)], [beta], [p-value] (header, don't exist in file)
         *      chr0:39616  0.7571908167    0.2146455451
         * To:
         *      SNP CHR BP  P (Header is necessary)
         */
        qqmanInputFileStream << "SNP\tCHR\tBP\tP" << endl; // Write header
        while (!gwasResultFileStream.atEnd())
        {
            QStringList curLine = gwasResultFileStream.readLine().split(QRegExp("\\s+"), QString::SkipEmptyParts);
            QString SNP = curLine[0];

            if (SNP.split(":").length() < 2)
            {
                QMessageBox::information(nullptr, "Error", ".ps file format error(maybe without chr).   ");
                return nullptr;
            }
            QString CHR = SNP.split(":")[0].remove(0, 3); // e.g. remove "chr" in "chr12", to get "12"
            QString BP = SNP.split(":")[1];
            QString P = curLine[curLine.length()-1];
            qqmanInputFileStream << SNP << "\t" << CHR << "\t" << BP << "\t" << P << endl;
        }

        return qqmanInputFile.fileName();
    }

    /* Script (one line):
     *  perl -alne '{print "$F[1]\t$F[-1]"}'
     *  phe2_pc2_lmm.assoc.txt |sed 's/:/\t/g;
     *  s/chr//g' |perl -alne '{if(/^0/){print
     *  "chr0:$F[1]\t19\t$F[1]\t$F[2]"}
     *  else{print "chr$F[0]:$F[1]\t$F[0]\t$F[1]\t$F[2]"}}'
     *  |sort -k 2,2n -k3,3n |sed '1d' |sed '1 i\SNP\tCHR\tBP\tP'
     *  > phe2_pc2_gemma_lmm
     */
    if (pvalueFileComSuffix == "assoc.txt")
    {   // Gemma LMM
        /* From:
         *  chr	rs	ps	n_miss	allele1	allele0	af	beta	se	logl_H1	l_remle	p_wald
         * To:
         *  SNP CHR BP  P (Header is necessary)
         */
        gwasResultFileStream.readLine();    // Read header, we don't need it in .qqman file.
        qqmanInputFileStream << "SNP\tCHR\tBP\tP" << endl; // Write header
        while (!gwasResultFileStream.atEnd())
        {
            QStringList curLine = gwasResultFileStream.readLine().split(QRegExp("\\s+"), QString::SkipEmptyParts);
            QString SNP = curLine[1];
            QString CHR = curLine[0];
            QString BP = curLine[2];
            QString P = curLine[curLine.length()-1];
            qqmanInputFileStream << SNP << "\t" << CHR << "\t" << BP << "\t" << P << endl;
        }

        return qqmanInputFile.fileName();;
    }

    return nullptr;
}
