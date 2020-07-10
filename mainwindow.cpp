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
    qualityControl = new QualityCtrlWidget;
    process = new QProcess;
    graphViewer = new GraphViewer;
    ldByFamGroupButton = new QButtonGroup;

    // Default output directory setting
    workDirectory->setProjectName("pro1");
    workDirectory->setOutputDirectory("/home/dengchao/Desktop/test");
    ui->projectNameLineEdit->setText(workDirectory->getProjectName());
    ui->outdirLineEdit->setText(workDirectory->getOutputDirectory()+"/"+workDirectory->getProjectName());

    // LD by family.
    ldByFamGroupButton->addButton(ui->yesLDByFamRadioButton);
    ldByFamGroupButton->addButton(ui->noLDByFamRadioButton);
    ldByFamGroupButton->setExclusive(true);

    // connect QProcess->start(tool) and runningMsgWidget.
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(on_readoutput()));
    connect(process, SIGNAL(readyReadStandardError()), this, SLOT(on_readerror()));
    connect(runningMsgWidget, SIGNAL(closeSignal()), this, SLOT(on_closeRunningWidget()));
    // connect MToolButton->rightClick
    connect(ui->pheFileToolButton, SIGNAL(closeFileSig()), this, SLOT(on_pheFileToolButton_closeFileSig()));
    connect(ui->genoFileToolButton, SIGNAL(closeFileSig()), this, SLOT(on_genoFileToolButton_closeFileSig()));
    connect(ui->mapFileToolButton, SIGNAL(closeFileSig()), this, SLOT(on_mapFileToolButton_closeFileSig()));
    connect(ui->kinFileToolButton, SIGNAL(closeFileSig()), this, SLOT(on_kinFileToolButton_closeFileSig()));
    connect(ui->covarFileToolButton, SIGNAL(closeFileSig()), this, SLOT(on_covarFileToolButton_closeFileSig()));
    // Graph Viewer
    connect(graphViewer, SIGNAL(doubleClicked()), this, SLOT(graphViewer_clicked_slot()));
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
    qualityControl->close();
    delete qualityControl;
    delete ldByFamGroupButton;

    if (process)    // QProcess
    {
        process->terminate();
        process->waitForFinished(-1);
    }
    delete process;

    delete graphViewer;
}

/**
 * @brief MainWindow::on_pheFileToolButton_clicked
 *          Open phenotype file
 */
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
    {   // Basic cotent: FID IIF PHE.
        QMessageBox::information(nullptr, "Error", "Phenotype file format error!    ");
        ui->selectedPhenoListWidget->clear();
        ui->excludedPhenoListWidget->clear();
        return;
    }

    ui->pheFileToolButton->setShowMenuFlag(true);
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

/**
 * @brief MainWindow::on_pheFileToolButton_closeFileSig
 *          Close phenotype file
 */
void MainWindow::on_pheFileToolButton_closeFileSig()
{
    ui->pheFileToolButton->setShowMenuFlag(false);
    ui->pheFileToolButton->setIcon(QIcon(":/new/icon/images/plus.png"));    // Set plus Icon.
    ui->pheFileLabel->setText("empty");
    ui->selectedPhenoListWidget->clear();
    ui->excludedPhenoListWidget->clear();
    this->fileReader->setPhenotypeFile("");
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
    ui->genoFileToolButton->setShowMenuFlag(true);
    ui->genoFileToolButton->setIcon(QIcon(":/new/icon/images/file.png"));
    this->fileReader->setGenotypeFile(fileNames[0]);

    QFileInfo  genoFileInfo(fileNames[0]);
    QString fileName = genoFileInfo.fileName(); // Get the file name from a path.
    ui->genoFileLabel->setText(fileName);
}

void MainWindow::on_genoFileToolButton_closeFileSig()
{
    ui->genoFileToolButton->setShowMenuFlag(false);
    ui->genoFileToolButton->setIcon(QIcon(":/new/icon/images/plus.png"));    // Set plus Icon.
    ui->genoFileLabel->setText("empty");
    this->fileReader->setGenotypeFile("");
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
    ui->mapFileToolButton->setShowMenuFlag(true);
    ui->mapFileToolButton->setIcon(QIcon(":/new/icon/images/file.png"));
    this->fileReader->setMapFile(fileNames[0]);

    QFileInfo  mapFileInfo(fileNames[0]);
    QString fileName = mapFileInfo.fileName(); // Get the file name from a path.
    ui->mapFileLabel->setText(fileName);
}

void MainWindow::on_mapFileToolButton_closeFileSig()
{
    ui->mapFileToolButton->setShowMenuFlag(false);
    ui->mapFileToolButton->setIcon(QIcon(":/new/icon/images/plus.png"));    // Set plus Icon.
    ui->mapFileLabel->setText("empty");
    this->fileReader->setMapFile("");
}

void MainWindow::on_covarFileToolButton_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this, "Open covariate file", "", "covar(*.eigenvec *eigenval *.cov *.covar *.txt);;all(*)");
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
    ui->covarFileToolButton->setShowMenuFlag(true);
    ui->covarFileToolButton->setIcon(QIcon(":/new/icon/images/file.png"));
    this->fileReader->setCovariateFile(fileNames[0]);

    QFileInfo  covarFileInfo(fileNames[0]);
    QString fileName = covarFileInfo.fileName(); // Get the file name from a path.
    ui->covarFileLabel->setText(fileName);
}

void MainWindow::on_covarFileToolButton_closeFileSig()
{
    ui->covarFileToolButton->setShowMenuFlag(false);
    ui->covarFileToolButton->setIcon(QIcon(":/new/icon/images/plus.png"));    // Set plus Icon.
    ui->covarFileLabel->setText("empty");
    this->fileReader->setCovariateFile("");
}

void MainWindow::on_kinFileToolButton_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this, "Open kinship file", "", "kin(*.kin *.kinf *.txt);;all(*)");
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
    ui->kinFileToolButton->setShowMenuFlag(true);
    ui->kinFileToolButton->setIcon(QIcon(":/new/icon/images/file.png"));
    this->fileReader->setKinshipFile(fileNames[0]);

    QFileInfo  kinFileInfo(fileNames[0]);
    QString fileName = kinFileInfo.fileName(); // Get the file name from a path.
    ui->kinFileLabel->setText(fileName);
}

void MainWindow::on_kinFileToolButton_closeFileSig()
{
    ui->kinFileToolButton->setShowMenuFlag(false);
    ui->kinFileToolButton->setIcon(QIcon(":/new/icon/images/plus.png"));    // Set plus Icon.
    ui->kinFileLabel->setText("empty");
    this->fileReader->setKinshipFile("");
}

/**
 * @brief MainWindow::on_outdirBrowButton_clicked
 *          Choose output directory.
 */
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

/**
 * @brief MainWindow::on_rungwasButton_clicked
 *          Run GWAS
 */
void MainWindow::on_runGwasButton_clicked()
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

    ui->runGwasButton->setDisabled(true);
    this->runningMsgWidget->clearText();
    this->runningMsgWidget->show();

    QFileInfo pheFileInfo(phenotype);
    QString pheFileBaseName = pheFileInfo.baseName();
    QString pheFileAbPath = pheFileInfo.absolutePath();
    QString pheFileSuffix = pheFileInfo.suffix();

    if (pheFileSuffix == "phe")
    {   // Only one phenotype data.
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
    {   // There several phenotype data.
        for (int i = 0; i < ui->selectedPhenoListWidget->count(); i++)
        {   // Make .phe file then run GWAS one by one.
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

/**
 * @brief MainWindow::callGemmaGwas
 *      Call gemma to GWAS(Whole process of gemma are implemeted here)
 * @param phenotype (FID MID PHE)
 * @param genotype
 * @param map       :will to find in the same path(and prefix) of genotype file.
 * @param covar
 * @param kinship
 * @param out
 * @param name
 * @return
 */
bool MainWindow::callGemmaGwas(QString phenotype, QString genotype, QString map,
                               QString covar, QString kinship, QString out, QString name)
{
    // Detail parameters.
    QMap<QString, QString> moreParam = this->gemmaParamWidget->getCurrentParam();

    QString model = ui->modelComboBox->currentText();
    QString maf = ui->mafRadioButton->isChecked()? ui->mafDoubleSpinBox->text():nullptr;
    QString mind = ui->mindRadioButton->isChecked()? ui->mindDoubleSpinBox->text():nullptr;
    QString geno = ui->genoRadioButton->isChecked()? ui->genoDoubleSpinBox->text():nullptr;

    // Genotype file info.
    QFileInfo genoFileInfo = QFileInfo(genotype);
    QString genoFileName = genoFileInfo.fileName();         // demo.vcf.gz
    QString genoFileBaseName = genoFileInfo.baseName();     // demo
    QString genoFileSuffix = genoFileInfo.suffix();         // gz
    QString genoFileAbPath = genoFileInfo.absolutePath();

    QString binaryFile = genoFileAbPath+"/"+genoFileBaseName+"_tmp";

    // Phenotype file info.
    QFileInfo pheFileInfo = QFileInfo(phenotype);
    QString pheFileBaseName = pheFileInfo.baseName();
    // Necessary to transform file ?
    bool transformFileFlag = false;
    bool filterDataFlag = false;


    // Need binary files.  Every temp file and a "_tmp" after baseName, and will be deleted after gwas.
    Plink plink;

    if (qualityControl->isLinkageFilterNeeded())
    {
        QString linkageFilteredFilePrefix = genoFileAbPath + "/" + genoFileBaseName + "_ldfl";
        QString winSize, stepLen, r2Threshold;
        this->qualityControl->getLinkageFilterType(winSize, stepLen, r2Threshold);
        plink.linkageFilter(genotype, map, winSize, stepLen, r2Threshold, linkageFilteredFilePrefix);

        if (!runExTool(*(this->process), this->toolpath+"plink", plink.getParamList()))
        {
            return false;
        }

        QString filteredSnpIDFile = linkageFilteredFilePrefix + ".prune.in";

        if (!this->checkoutExistence(filteredSnpIDFile))
        {
            QMessageBox::information(this, "Error", "Linkage filter error.");
            return false;
        }

        plink.extractBySnpNameFile(genotype, map, filteredSnpIDFile, linkageFilteredFilePrefix);

        if (!runExTool(*(this->process), this->toolpath+"plink", plink.getParamList()))
        {
            return false;
        }

        genotype = linkageFilteredFilePrefix + ".ped";
        map = linkageFilteredFilePrefix + ".map";
        genoFileName = genoFileBaseName + "_ldfl";

        if (!this->checkoutExistence(genotype) ||
            !this->checkoutExistence(map))
        {
            QMessageBox::information(this, "Error", "Extaract snp after linkage filter error.");
            return false;
        }
    }

    if (isVcfFile(genotype)) // Transform "vcf" to "transpose"
    {
        if(!plink.vcf2binary(genotype, binaryFile, maf, mind, geno))
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
        if (!plink.plink2binary(genotype, map, binaryFile, maf, mind, geno))
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
        if (!plink.transpose2binary(genotype, map, binaryFile, maf, mind, geno))
        {
            this->resetWindow();
            return false;
        }
        transformFileFlag = true;
    }

    if (genotype.split(".")[genotype.split(".").length()-1] == "bed")
    {   // When don't set any QC param, it won't execute.
        plink.filterBinaryFile(genoFileAbPath+"/"+genoFileBaseName, maf, mind, geno, binaryFile);
        filterDataFlag = true;
    }

    if (transformFileFlag || filterDataFlag)
    {   // Run plink to transform file or filter data.
        if (!runExTool(*(this->process), this->toolpath+"plink", plink.getParamList()))
        {
            return false;
        }
    }

    Gemma gemma;

    if (gemmaParamWidget->isFamCompletedAuto())
    {
        // Replace "NA" to "-9", then complete .fam
        // .fam: FID IID PID MID Sex 1 Phe  (phenotype data to the 7th column of .fam)
        QFuture<bool> fu =  QtConcurrent::run(&gemma, &Gemma::phe_fam_Preparation, phenotype, binaryFile+".fam");
        while (!fu.isFinished())
        {
            qApp->processEvents(QEventLoop::AllEvents, 100);
        }
        if (!fu.result())
        {
            this->resetWindow();
            return false;
        }
    }

    if (kinship.isNull() && this->gemmaParamWidget->isMakeRelatedMatAuto())
    {
         if (!gemma.makeKinship(binaryFile, genoFileBaseName+"_tmp", moreParam))
         {
             this->resetWindow();
             return false;  // Make kinship failed.
         }
         if (!runExTool(*(this->process), this->toolpath+"gemma", gemma.getParamList()))
         {
             return false;
         }
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

    if (!gemma.runGWAS(genoFileAbPath+"/"+genoFileBaseName+"_tmp", phenotype, covar, kinship,
                      name+"_"+pheFileBaseName, model, moreParam))
    {
        this->resetWindow();
        return false;
    }
    if (!runExTool(*(this->process), this->toolpath+"gemma", gemma.getParamList()))
    {
        return false;
    }

    QFile file;
    // delete intermidiate file.
    file.remove(binaryFile+".bed");
    file.remove(binaryFile+".bim");
    file.remove(binaryFile+".fam");
    file.remove(binaryFile+".log");
    file.remove(binaryFile+".nosex");
    if (moreParam["kinmatrix"] == "1")
    {
        file.remove(QDir::currentPath() + "/output/"+genoFileBaseName+"_tmp.cXX.txt");
    }
    else
    {
        file.remove(QDir::currentPath() + "/output/"+genoFileBaseName+"_tmp.sXX.txt");
    }
    file.remove(QDir::currentPath() + "/output/"+genoFileBaseName+"_tmp.log.txt");

    if (qualityControl->isLinkageFilterNeeded())
    {
        file.remove(genotype);
        file.remove(map);
    }

    if (qualityControl->isLinkageFilterNeeded())
    {
        file.remove(genotype);
        file.remove(map);
    }

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

    // Correct p value
    QString correctionType = this->gemmaParamWidget->getCorrectionType();
    if (model == "LMM" && !correctionType.isNull())
    {
        QString pValFile = out+"/output"
                +(i==0?"":QString::number(i))+"/"+name+"_"+pheFileBaseName+".assoc.txt";
        QString correctedFile = out+"/output"
                +(i==0?"":QString::number(i))+"/"+name+"_"+pheFileBaseName+"_corr.assoc.txt";

        qDebug() << "pValFile: " << pValFile;
        qDebug() << "correctedFile: " << correctedFile;
        this->pValCorrect(pValFile, true, correctionType, correctedFile);

        if (!this->checkoutExistence(correctedFile))
        {
            QMessageBox::information(this, "Error", "Gemma corrected error.");
            return false;
        }

        ui->qqmanGwasResultLineEdit->setText(correctedFile);
    }
    else if (model == "LMM")
    {
        if (!this->checkoutExistence(out+"/output"
            +(i==0?"":QString::number(i))+"/"+name+"_"+pheFileBaseName+".assoc.txt"))
        {
            QMessageBox::information(this, "Error", "Gemma GWAS error.");
            return false;
        }

        ui->qqmanGwasResultLineEdit->setText(out+"/output"
            +(i==0?"":QString::number(i))+"/"+name+"_"+pheFileBaseName+".assoc.txt");
    }

    return true;
}

/**
 * @brief MainWindow::callEmmaxGwas
 *      Call emmax to GWAS(Whole process of emmax are implemeted here)
 * @param phenotype
 * @param genotype
 * @param map
 * @param covar
 * @param kinship
 * @param out
 * @param name
 * @return
 */
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

    QString transposeFile =genoFileAbPath+"/"+genoFileBaseName+"_tmp";

    // Phenotype file info.
    QFileInfo pheFileInfo = QFileInfo(phenotype);
    QString pheFileBaseName = pheFileInfo.baseName();

    // Necessary to transform file ?
    bool transformFileFlag = false;
    bool filterDataFlag = false;

    // Need tped/fam files. Add "_tmp", then delete after gwas.
    Plink plink;

    if (qualityControl->isLinkageFilterNeeded())
    {
        QString linkageFilteredFilePrefix = genoFileAbPath + "/" + genoFileBaseName + "_ldfl";
        QString winSize, stepLen, r2Threshold;
        this->qualityControl->getLinkageFilterType(winSize, stepLen, r2Threshold);
        plink.linkageFilter(genotype, map, winSize, stepLen, r2Threshold, linkageFilteredFilePrefix);

        if (!runExTool(*(this->process), this->toolpath+"plink", plink.getParamList()))
        {
            return false;
        }

        QString filteredSnpIDFile = linkageFilteredFilePrefix + ".prune.in";

        plink.extractBySnpNameFile(genotype, map, filteredSnpIDFile, linkageFilteredFilePrefix);

        if (!runExTool(*(this->process), this->toolpath+"plink", plink.getParamList()))
        {
            return false;
        }

        genotype = linkageFilteredFilePrefix + ".ped";
        map = linkageFilteredFilePrefix + ".map";
        genoFileName = genoFileBaseName + "_ldfl";
    }

    if (isVcfFile(genotype)) // Transform "vcf" to "transpose"
    {
        if(!plink.vcf2transpose(genotype, transposeFile, maf, mind, geno))
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

        if (!plink.plink2transpose(genotype, map, transposeFile, maf, mind, geno))
        {
            return false;
        }
        transformFileFlag = true;
    }
    if (genotype.split(".")[genotype.split(".").length()-1] == "bed")  // Transform "binary" to "transpose"
    {
        if (!plink.binary2transpose(genoFileAbPath+"/"+genoFileBaseName,
                                    transposeFile, maf, mind, geno))
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
        plink.filterTransposeFile(genotype, map, maf, mind, geno, transposeFile);
        filterDataFlag = true;
    }

    if (transformFileFlag || filterDataFlag)
    {
        if (!runExTool(*(this->process), this->toolpath+"plink", plink.getParamList()))
        {
                return false;
        }
    }

    Emmax emmax;
    if (kinship.isNull() && emmaxParamWidget->isMakeKinAuto())
    {
         if (!emmax.makeKinship(transposeFile, moreParam))
         {
             this->resetWindow();
             return false;  // Make kinship failed.
         }
         if (!runExTool(*(this->process), this->toolpath+"emmax-kin", emmax.getParamList()))
         {
             return false;
         }

         if (emmaxParamWidget->isBNkinMatrix())
         {
             kinship = genoFileAbPath + "/" + genoFileBaseName+"_tmp" + ".hBN.kinf";
         }
         else
         {
            kinship = genoFileAbPath + "/" + genoFileBaseName+"_tmp" + ".hIBS.kinf";
         }
    }

    if (!emmax.runGWAS(transposeFile, phenotype, covar, kinship,
                      out+"/"+name+"_"+pheFileBaseName, moreParam))
    {
        this->resetWindow();
        return false;
    }
    if (!runExTool(*(this->process), this->toolpath+"emmax", emmax.getParamList()))
    {
        return false;
    }

    // Correct p value
    QString correctionType = this->emmaxParamWidget->getCorrectionType();

    if (!correctionType.isNull())
    {
        QString pValFile = out+"/"+name+"_"+pheFileBaseName+".ps";
        QString correctedFile = out+"/"+name+"_"+pheFileBaseName+"_corr.ps";

        qDebug() << "pValFile: " << pValFile;
        qDebug() << "correctedFile: " << correctedFile;
        // There no header of emmax result file.
        this->pValCorrect(pValFile, false, correctionType, correctedFile);

        if (!this->checkoutExistence(correctedFile))
        {
            QMessageBox::information(this, "Error", "Gemma corrected error.");
            return false;
        }

        ui->qqmanGwasResultLineEdit->setText(correctedFile);
    }
    else
    {
        ui->qqmanGwasResultLineEdit->setText(out+"/"+name+"_"+pheFileBaseName+".ps");
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

    if (qualityControl->isLinkageFilterNeeded())
    {
        file.remove(genotype);
        file.remove(map);
    }

    return true;
}

/**
 * @brief MainWindow::callPlinkGwas
 *      Call gemma to GWAS(Whole process of gemma are implemeted here)
 * @param phenotype
 * @param genotype
 * @param map
 * @param covar
 * @param kinship
 * @param out
 * @param name
 * @return
 */
bool MainWindow::callPlinkGwas(QString phenotype, QString genotype, QString map,
                               QString covar, QString kinship, QString out, QString name)
{
    QString model = ui->modelComboBox->currentText();
    QString maf = ui->mafRadioButton->isChecked()? ui->mafDoubleSpinBox->text():nullptr;
    QString mind = ui->mindRadioButton->isChecked()? ui->mindDoubleSpinBox->text():nullptr;
    QString geno = ui->genoRadioButton->isChecked()? ui->genoDoubleSpinBox->text():nullptr;

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

    // Linkage filter
    if (qualityControl->isLinkageFilterNeeded())
    {
        QString linkageFilteredFilePrefix = genoFileAbPath + "/" + genoFileBaseName + "_ldfl";
        QString winSize, stepLen, r2Threshold;
        this->qualityControl->getLinkageFilterType(winSize, stepLen, r2Threshold);

        this->runningMsgWidget->appendText("LinkageFilter");
        plink.linkageFilter(genotype, map, winSize, stepLen, r2Threshold, linkageFilteredFilePrefix);
        if (!runExTool(*(this->process), this->toolpath+"plink", plink.getParamList()))
        {
            return false;
        }

        QString filteredSnpIDFile = linkageFilteredFilePrefix + ".prune.in";

        if (!this->checkoutExistence(filteredSnpIDFile))
        {
            QMessageBox::information(this, "Error", "Linkage filter error.");
            return false;
        }

        plink.extractBySnpNameFile(genotype, map, filteredSnpIDFile, linkageFilteredFilePrefix);

       this->runningMsgWidget->appendText("extractBySnpName");
        if (!runExTool(*(this->process), this->toolpath+"plink", plink.getParamList()))
        {
            return false;
        }

        this->runningMsgWidget->appendText("OK");

        genotype = linkageFilteredFilePrefix + ".ped";
        map = linkageFilteredFilePrefix + ".map";

        if (!this->checkoutExistence(genotype) || !this->checkoutExistence(map))
        {
            QMessageBox::information(this, "Error", "Extract snp after linkage filter error.");
            return false;
        }
    }

    // Run GWAS(Set parameters)
    if(!plink.runGWAS(phenotype, genotype, map, covar, kinship, model,
                     maf, mind, geno, out+"/"+name+"_"+pheFileBaseName))
    {
        return false;
    }
    if (!runExTool(*(this->process), this->toolpath+"plink", plink.getParamList()))
    {
        return false;
    }

    if (!this->checkoutExistence(out+"/"+name+"_"+pheFileBaseName+".assoc."+model.toLower()))
    {
        QMessageBox::information(this, "Error", "Plink GWAS error.");
        return false;
    }

    ui->qqmanGwasResultLineEdit->setText(out+"/"+name+"_"+pheFileBaseName+".assoc."+model.toLower());

    return true;
}

/**
 * @brief MainWindow::on_readoutput
 *      Read data from standard ouput stream, generated from tool which is running, to show in RunningMsgWidget
 */
void MainWindow::on_readoutput()
{
    QString text = QString::fromLocal8Bit(this->process->readAllStandardOutput().data());
    this->runningMsgWidget->setText(this->refreshMessage(this->runningMsgWidget->getText(), text));
    this->runningMsgWidget->repaint();
    qApp->processEvents();
    QThread::msleep(10);
}

/**
 * @brief MainWindow::on_readerror
 *      Read data from standard error stream, generated from tool which is running, to show in RunningMsgWidget
 */
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
    QThread::msleep(10);
}

/**
 * @brief MainWindow::on_closeRunningWidget
 */
void MainWindow::on_closeRunningWidget()
{
    if (!this->runningMsgWidget->isVisible())
    {
        return;
    }

    if (this->process->isOpen())
    {   // Juage there are any tools running now.
        QMessageBox::StandardButton ret = QMessageBox::information(this, "Notice",
           "The association will be terminated if close the widget!   ",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if (ret == QMessageBox::Yes)
        {
            this->process->close();
            this->runningMsgWidget->clearText();
            this->runningMsgWidget->hide();
            this->resetWindow();
        }
    }
    else
    {   // Close widget directly while no tool running.
        this->runningMsgWidget->clearText();
        this->runningMsgWidget->hide();
    }
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

/**
 * @brief MainWindow::isVcfFile
 *      Judge the file whether a VCF file from file name.
 * @param file
 * @return
 */
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

/**
 * @brief MainWindow::resetWindow
 *      Reset MainWindow.
 */
void MainWindow::resetWindow()
{
    if (this->process->isOpen())
    {
        this->process->close();
    }
    if (this->process)
    {
        this->process->terminate();
        this->process->waitForFinished(-1);
    }
    ui->runGwasButton->setEnabled(true);
    ui->ldRunPushButton->setEnabled(true);
    ui->ldPlotPushButton->setEnabled(true);
    ui->pcaRunPushButton->setEnabled(true);
    ui->pcaPlotPushButton->setEnabled(true);
    ui->drawManPushButton->setEnabled(true);
    ui->drawQQPushButton->setEnabled(true);
    ui->strucAnnoRunPushButton->setEnabled(true);
    ui->funcAnnoRunPushButton->setEnabled(true);
    ui->funcAnnoStepPushButton->setEnabled(true);
}

void MainWindow::on_projectNameLineEdit_textChanged(const QString &text)
{   // When edit finished and the current text is empty, set a default name.
    this->workDirectory->setProjectName(text);
    if (!ui->outdirLineEdit->text().isEmpty())
    {   // If a out directory is selected, display the out directory + the project name.
        ui->outdirLineEdit->setText(this->workDirectory->getOutputDirectory()+"/"+text);
    }
}

/**
 * @brief MainWindow::on_toolComboBox_currentTextChanged
 *      Select a new tool and get supported model of tool.
 * @param tool
 */
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

/**
 * @brief MainWindow::makePheFile
 *      Make phenotype file according selected phenotype.
 * @param phenotype     phenotype file path
 * @param selectedPheno
 * @return
 */
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

/**
 * @brief MainWindow::on_detailPushButton_clicked
 *      Set and show paramWidget
 */
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

/**
 * @brief MainWindow::refreshMessage
 *      To mananage running message.
 * @param curText
 * @param newText
 * @return
 */
QString MainWindow::refreshMessage(QString curText, QString newText)
{   // Remain lots of problems, little validance now.
    if (newText.isEmpty())
    {   // newMsg don't have valid message.
        return curText;
    }
    if (curText.isEmpty())
    {   // The first message.
        return newText;
    }

    QRegExp regRxp("\\s*((100|[1-9]?\\d)%\\s*)");

    // For current text: replace pecent number to null string.
    curText.replace(QRegExp("\\s*(100|[1-9]?\\d)%\\s*"), "");
    // For current text: repalce multiple line break to only one.
    curText.replace(QRegExp("\n\n+"), "\n");
    // For current text: replace multiple '=' to null string.
    //    (only matching gemma output message)
    curText.replace(QRegExp("=+"), "");

    // Only consider gemma here (gemma msg: ===========   12%)
    QRegExp gemmaMsg("(=+\\s*(100|[1-9]?\\d)%\\s*)+");
    int pos = gemmaMsg.indexIn(newText);
    if (pos > -1)
    {   // Will cause multiple '\n' in current text.
        // So it's necessary to repalce multiple line break to only one in current text.
        newText = "\n"+gemmaMsg.cap(1);
    }

//    if (curText[curText.size()-1] == '%' && newText[newText.size()-1] == '%')
//    {   // Refresh percent.
//        QString::iterator iter = newText.end(); // After the last char.
//        QString newPercent = "%";
//        iter--;iter--;  // Pointer to the last number.
//        // Get the last percent in newText when there multi percent in the same line.
//        while ((*iter).isNumber())
//        {
//            newPercent = *iter + newPercent;
//            iter--;
//        }

//        // Remove the percent in curText.

//        while(curText[curText.size()-1] == '%')
//        {
//            curText.remove(curText.size()-1, 1);
//            iter = curText.end();
//            iter--; iter--;
//            while ((*iter).isNumber())
//            {
//                curText.remove(curText.size()-1, 1);
//            }
//        }
//    }

    return curText + newText;
}

void MainWindow::on_projectNameLineEdit_editingFinished()
{   // Edit is finised but current text is empty.
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
        QString gwasResulFile = ui->qqmanGwasResultLineEdit->text();
        if (gwasResulFile.isEmpty())
        {   // Gwas result file is necessary.
            this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
            this->runningMsgWidget->appendText("A GWAS result file is necessary.");
            qApp->processEvents();
            throw -1;
        }

        this->runningMsgWidget->show();
        this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
        this->runningMsgWidget->appendText("Make qqman input file, ");
        qApp->processEvents();

        // Transform gwas result file type to input file type of qqman.
        QStringList qqmanFile = makeQQManInputFile(gwasResulFile); //   path/name.gemma_wald
        QStringList outList;

        if (qqmanFile.isEmpty())
        {   // makeQQManInputFile error.
            this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
            this->runningMsgWidget->appendText("Make qqman input file ERROR. ");
            qApp->processEvents();
            throw -1;
        }

        this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
        this->runningMsgWidget->appendText("Make qqman input file OK. ");
        qApp->processEvents();

        for (auto item:qqmanFile)
        {   // Multiple result, multiple output plot, append to list.
            outList.append(this->workDirectory->getOutputDirectory()+"/"+this->workDirectory->getProjectName()
                           +"_"+item.split(".")[item.split(".").length()-1]+"_man.png");
        }

        this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
        this->runningMsgWidget->appendText("Draw manhattan plot, ");
        qApp->processEvents();
        if (!this->drawManhattan(qqmanFile, outList))
        {   // drawManhattan error
            this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
            this->runningMsgWidget->appendText("Draw manhattan plot ERROR. ");
            qApp->processEvents();
            throw -1;
        }
        this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
        this->runningMsgWidget->appendText("Draw manhattan plot OK.");
        this->runningMsgWidget->appendText("\nmanhattan plot: \n" + outList.join("\n"));
        qApp->processEvents();

        QFile file;
        for (auto item:qqmanFile)
        {   // Remove intermediate file.
            file.remove(item);
        }
    } catch (...) {
        this->resetWindow();    // reset MainWindow
    }
    this->resetWindow();
}

void MainWindow::on_drawQQPushButton_clicked()
{
    ui->drawQQPushButton->setEnabled(false);
    qApp->processEvents();
    try {
        QString gwasResulFile = ui->qqmanGwasResultLineEdit->text();
        if (gwasResulFile.isEmpty())
        {   // Gwas result file is necessary.
            this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
            this->runningMsgWidget->appendText("A GWAS result file is necessary.");
            qApp->processEvents();
            throw -1;
        }

        this->runningMsgWidget->show();
        this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
        this->runningMsgWidget->appendText("Make qqman input file, ");
        qApp->processEvents();
        // Transform gwasResultFile to input file type of qqman.
        QStringList qqmanFile = makeQQManInputFile(gwasResulFile); //   path/name.gemma_wald
        QStringList outList;

        if (qqmanFile.isEmpty())
        {
            this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
            this->runningMsgWidget->appendText("Make qqman input file ERROR. ");
            qApp->processEvents();
            throw -1;
        }

        this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
        this->runningMsgWidget->appendText("Make qqman input file OK. ");
        qApp->processEvents();

        for (auto item:qqmanFile)
        {   // Multiple result, multiple output plot.
            outList.append(this->workDirectory->getOutputDirectory()+"/"+this->workDirectory->getProjectName()
                           +"_"+item.split(".")[item.split(".").length()-1]+"_qq.png");
        }

        this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
        this->runningMsgWidget->appendText("Draw QQ plot, ");
        qApp->processEvents();
        if (!this->drawQQplot(qqmanFile, outList))
        {
            this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
            this->runningMsgWidget->appendText("Draw QQ plot ERROR. ");
            qApp->processEvents();
            throw -1;
        }
        this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
        this->runningMsgWidget->appendText("Draw QQ OK. ");
        this->runningMsgWidget->appendText("\nQQ plot: \n" + outList.join("\n"));
        qApp->processEvents();

        QFile file;
        for (auto item:qqmanFile)
        {   // Remove intermediate file.
            file.remove(item);
        }
    } catch (int) {
       this->resetWindow();
    }

    this->resetWindow();
}

/**
 * @brief MainWindow::drawManhattan
 *          Set parameter and call plot.R to draw manhattan plot.
 * @param data  input data file
 * @param out   output file
 * @return
 */
bool MainWindow::drawManhattan(QStringList data, QStringList out)
{
    if (data.isEmpty() || out.isEmpty() || data.size() != out.size())
    {
        return false;;
    }

    // Threshold value
    int gwBase =  ui->gwBaseLineEdit->text().toInt();
    int gwExpo = ui->gwExpoLineEdit->text().toInt();
    int sgBase = ui->sgBaseLineEdit->text().toInt();
    int sgExpo = ui->sgExpoLineEdit->text().toInt();

    QStringList param;

    for (int i = 0; i < data.size(); i++)
    {
        param.clear();
        param.append(this->scriptpath+"qqman/plot.R");
        param.append("manhattan");
        param.append(data[i]);
        param.append(out[i]);
        param.append(QString::number(gwBase)+'e'+QString::number(gwExpo));
        param.append(QString::number(sgBase)+'e'+QString::number(sgExpo));

        // R in environment path is necessary.
        if (!runExTool(*(this->process), "Rscript", param))
        {
            return false;
        }
    }
    // Show plot
    this->graphViewer->setGraph(out);
    this->graphViewer->show();
    return true;
}

/**
 * @brief MainWindow::drawQQplot
 *      Set parameter and call plot.R to draw QQ plot.
 * @param data  input data file
 * @param out   out file
 * @return
 */
bool MainWindow::drawQQplot(QStringList data, QStringList out)
{
    if (data.isEmpty() || out.isEmpty() || data.size() != out.size())
    {
        return false;;
    }

    QStringList param;

    for (int i = 0; i < data.size(); i++)
    {
        param.clear();
        param.append(this->scriptpath+"qqman/plot.R");
        param.append("qqplot");
        param.append(data[i]);
        param.append(out[i]);
        // R in environment path is necessary.
        if (!runExTool(*(this->process), "Rscript", param))
        {
            return false;
        }

    }
    // Show plot
    this->graphViewer->setGraph(out);
    this->graphViewer->show();
    return true;
}

/**
 * @brief MainWindow::on_gwasReultBrowButton_clicked
 *      To select gwas result file.
 */
void MainWindow::on_qqmanGwasReultBrowButton_clicked()
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
    ui->qqmanGwasResultLineEdit->setText(fileNames[0]);
}

/**
 * @brief MainWindow::makeQQmanFile
 * @param pvalueFile(generated by association tool)
 * @return  A file will be a input of manhattan.(header: SNP CHR BP P)
 */
QStringList MainWindow::makeQQManInputFile(QString pvalueFile)
{
    QStringList qqmanInFileList;
    if (pvalueFile.isNull() || pvalueFile.isEmpty())
    {
        return qqmanInFileList;
    }

    QFileInfo pvalueFileInfo(pvalueFile);
    QString pvalueFileAbPath = pvalueFileInfo.absolutePath();
    QString pvalueFileBaseName = pvalueFileInfo.baseName();
    QString pvalueFileSuffix = pvalueFileInfo.suffix();
    QString pvalueFileComSuffix = pvalueFileInfo.completeSuffix();

    if ( pvalueFileComSuffix == "assoc.linear" || pvalueFileComSuffix == "assoc.logistic")
    {   // Plink output file don't need to be transformed.
        qqmanInFileList.append(pvalueFile);
        return qqmanInFileList;
    }

    QFile gwasResultFile(pvalueFile);
    QTextStream gwasResultFileStream(&gwasResultFile);
    if (!gwasResultFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(nullptr, "Error", "Open gwasResultFile error.  ");
        return qqmanInFileList;
    }

    /* Script(one line):
     *  sed 's/:/\t/g; s/chr//g' 222_filter1_phe1_BN.ps
     *  |perl -alne '{if(/^0/){print "19\tchr0:$F[1]\t$F[0]\t$F[1]\t$F[3]"}
     *  else{print "$F[0]\tchr$F[0]:$F[1]\t$F[0]\t$F[1]\t$F[3]"}}'
     *  |sort -k 1,1n -k4,4n |perl -alne '{$num=shift @F;
     *  $line=join "\t",@F; print $line}'
     *  |sed '1 i\SNP\tCHR\tBP\tP' > phe1_emmax_table
     */
    if (pvalueFileSuffix == "ps")
    {   // Emmax output file.
        /* From: .ps:
         *      [SNP ID(CHR:BP)], [beta], [p-value] (header, don't exist in file)
         *      chr0:39616  0.7571908167    0.2146455451
         * To:
         *      SNP CHR BP  P (Header is necessary)
         */
        QFile qqmanInputFile(pvalueFileAbPath+"/"+pvalueFileBaseName+"."+"emmax");
        QTextStream qqmanInputFileStream(&qqmanInputFile);
        qqmanInputFile.open(QIODevice::ReadWrite);
        qqmanInputFileStream << "SNP\tCHR\tBP\tP" << endl; // Write header
        while (!gwasResultFileStream.atEnd())
        {   // Read data line by line.
            QStringList curLine = gwasResultFileStream.readLine().split(QRegExp("\\s+"), QString::SkipEmptyParts);
            QString SNP = curLine[0];

            if (SNP.split(":").length() < 2)
            {
                QMessageBox::information(nullptr, "Error", ".ps file format error(maybe without chr).   ");
                return qqmanInFileList;
            }
            QString CHR = SNP.split(":")[0].remove(0, 3); // e.g. remove "chr" in "chr12", to get "12"
            QString BP = SNP.split(":")[1];
            QString P = curLine[curLine.length()-1];
            qqmanInputFileStream << SNP << "\t" << CHR << "\t" << BP << "\t" << P << endl;
        }
        qqmanInFileList.append(qqmanInputFile.fileName());
        qqmanInputFile.close();
        gwasResultFile.close();
        return qqmanInFileList;
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
         *  chr	rs	ps	n_miss	allele1	allele0	af	beta	se	logl_H1	l_remle	l_mle	p_wald	p_lrt	p_score
         * To:
         *  SNP CHR BP  P (Header is necessary)
         */
        QStringList header = gwasResultFileStream.readLine().split(QRegExp("\\s+"), QString::SkipEmptyParts);    // Read header, we don't need it in .qqman file.
        int num = (header.length() != 12)  ? (header.length()-13+1) : 1;  // Multi(3) p-values in file when chosed all tested.
        for (int i = 1; i <= num; i++)
        {
            gwasResultFileStream.seek(0);       // Back to begin of the file.
            gwasResultFileStream.readLine();    // Read the header in the first line and strip it.
            QFile qqmanInputFile(pvalueFileAbPath+"/"+pvalueFileBaseName+"."+"gemma"
                                 +"_"+header[header.length()-i].split("_")[1]);    // out_name.gemma_wald
            QTextStream qqmanInputFileStream(&qqmanInputFile);
            qqmanInputFile.open(QIODevice::ReadWrite);
            qqmanInputFileStream << "SNP\tCHR\tBP\tP" << endl; // Write header
            while (!gwasResultFileStream.atEnd())
            {
                QStringList curLine = gwasResultFileStream.readLine().split(QRegExp("\\s+"), QString::SkipEmptyParts);
                QString SNP = curLine[1];
                QString CHR = curLine[0];
                QString BP = curLine[2];
                QString P = curLine[curLine.length()-i];
                qqmanInputFileStream << SNP << "\t" << CHR << "\t" << BP << "\t" << P << endl;
            }
            qqmanInFileList.append(qqmanInputFile.fileName());
            qqmanInputFile.close();
        }
        gwasResultFile.close();
        return qqmanInFileList;
    }

    return qqmanInFileList;
}

void MainWindow::on_pcaRunPushButton_clicked()
{
    if (this->fileReader->getGenotypeFile().isNull() || this->fileReader->getGenotypeFile().isEmpty())
    {
        QMessageBox::information(nullptr, "Error", "A genotype file is necessary!   ");
        return;
    }

    ui->pcaRunPushButton->setEnabled(false);
    qApp->processEvents();

    this->runningMsgWidget->show();

    try {
        QString genotype = this->fileReader->getGenotypeFile();
        QFileInfo genoFileInfo(genotype);
        QString genoFileAbPath = genoFileInfo.absolutePath();
        QString genoFileBaseName = genoFileInfo.baseName();
        QString map = this->fileReader->getMapFile();
        QString out = this->workDirectory->getOutputDirectory();
        QString name = this->workDirectory->getProjectName();
        //  binaryFile: Set a default path. Binary geno file with paht without suffix.
        QString binaryFile = genoFileAbPath+"/"+genoFileBaseName+"_tmp";

        bool transformFileFlag = false;

        // Need binary files.  Every temp file and a "_tmp" after baseName, and will be deleted after gwas.
        Plink plink;
        if (isVcfFile(genotype)) // Transform "vcf" to "transpose"
        {
            if(!plink.vcf2binary(genotype, binaryFile, nullptr, nullptr, nullptr))
            {
                throw -1;
            }

            transformFileFlag = true;
        }
        if (genotype.split(".")[genotype.split(".").length()-1] == "ped")  // Transform "plink" to "binary"
        {
            if (map.isNull())
            {
                map = genoFileAbPath+"/"+genoFileBaseName+".map";
            }
            if (!plink.plink2binary(genotype, map, binaryFile, nullptr, nullptr, nullptr))
            {
                throw -1;
            }

            transformFileFlag = true;
        }

        if (genotype.split(".")[genotype.split(".").length()-1] == "tped")  // Transform "transpose" to "binary"
        {
            if (map.isNull())
            {
                map = genoFileAbPath+"/"+genoFileBaseName+".tfam";
            }
            if (!plink.transpose2binary(genotype, map, binaryFile, nullptr, nullptr, nullptr))
            {
                throw -1;
            }
            transformFileFlag = true;
        }

        if (transformFileFlag)
        {   // Run plink to transform file or filter data.
            if (!runExTool(*(this->process), this->toolpath+"plink", plink.getParamList()))
            {
                return;
            }
        }
        else
        {
            binaryFile = genoFileAbPath + "/" + genoFileBaseName;
        }

        // Mkae GRM
        Gcta gcta;
        if (gcta.makeGRM(binaryFile, binaryFile))
        {
            if (!runExTool(*(this->process), this->toolpath+"gcta64", gcta.getParamList()))
            {
                return;
            }
        }

        // Run PCA
        if (gcta.runPCA(binaryFile, ui->nPCsLineEdit->text().toInt(),
                        ui->nThreadsLineEdit->text().toInt(), out+"/"+genoFileBaseName))
        {
            if (!runExTool(*(this->process), this->toolpath+"gcta64", gcta.getParamList()))
            {
                return;
            }
        }

        QFile file;
        if (transformFileFlag)
        {
            file.remove(binaryFile+".bed");
            file.remove(binaryFile+".bim");
            file.remove(binaryFile+".fam");
            file.remove(binaryFile+".nosex");
        }
        file.remove(binaryFile+".grm.bin");
        file.remove(binaryFile+".grm.id");
        file.remove(binaryFile+".grm.N.bin");

        ui->eigenvalueLineEdit->setText(out+"/"+genoFileBaseName+".eigenval");
        ui->eigenvectorLineEdit->setText(out+"/"+genoFileBaseName+".eigenvec");

    } catch (...) {
        this->resetWindow();
    }

    this->resetWindow();
}

void MainWindow::on_ldRunPushButton_clicked()
{
    if (this->fileReader->getGenotypeFile().isNull() || this->fileReader->getGenotypeFile().isEmpty())
    {
        QMessageBox::information(nullptr, "Error", "A genotype file is necessary!   ");
        return;
    }

    ui->ldRunPushButton->setEnabled(false);
    qApp->processEvents();

    if (ui->yesLDByFamRadioButton->isChecked())
    {
        this->runPopLDdecaybyFamily();
    }
    else
    {
        this->runPopLDdecaySingle();
    }
    this->resetWindow();
}

void MainWindow::runPopLDdecaybyFamily(void)
{
    try {
        QString out(this->workDirectory->getOutputDirectory());
        QString name(this->workDirectory->getProjectName());
        QString genotype(this->fileReader->getGenotypeFile());
        QString map(this->fileReader->getMapFile());
        QFileInfo genoFileInfo(genotype);
        QString genoFileSuffix = genoFileInfo.suffix();
        QString genoFileBaseName = genoFileInfo.baseName();
        QString genoFileAbPath = genoFileInfo.absolutePath();
        QStringList keepFileList;
        this->runningMsgWidget->show();

        Plink plink;
        PopLDdecay popLDdecay;
        if (isVcfFile(genotype)){} // Transform "vcf" to "transpose"

        // Make .keep file.
        this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
        this->runningMsgWidget->appendText("Make .keep file, \n");
        qApp->processEvents();

        if (genoFileSuffix == "ped")
        {
            keepFileList = popLDdecay.makeKeepFile(genotype);
        }
        if (genoFileSuffix == "tped")
        {
            map = map.isNull() ? genoFileAbPath+"/"+genoFileBaseName+".tfam" : map;
            keepFileList = popLDdecay.makeKeepFile(map);
        }
        if (genoFileSuffix == "bed")
        {
            map = map.isNull() ? genoFileAbPath+"/"+genoFileBaseName+".fam" : map;
            keepFileList = popLDdecay.makeKeepFile(map);
        }

        this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
        this->runningMsgWidget->appendText(".keep file OK.\n");
        qApp->processEvents();

        bool isLD_OK = true;
        for (QString keepFile:keepFileList)
        {
            QFileInfo keepFileInfo(keepFile);
            QString keepFileBaseName = keepFileInfo.baseName();
            QString keepFileAbPath = keepFileInfo.absolutePath();

            this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
            this->runningMsgWidget->appendText("Make "+keepFileBaseName+".ped"+" and "+keepFileBaseName+".map, \n");
            qApp->processEvents();

            // Split ped and map file.
            if (genoFileSuffix == "ped")
            {
                map = map.isNull() ? genoFileAbPath+"/"+genoFileBaseName+".map" : map;
                plink.splitPlinkFile(genotype, map, keepFile,
                        genoFileAbPath+"/"+keepFileBaseName);
            }
            if (genoFileSuffix == "tped")
            {
                map = map.isNull() ? genoFileAbPath+"/"+genoFileBaseName+".tfam" : map;
                plink.splitTransposeFile(genotype, map, keepFile,
                        genoFileAbPath+"/"+keepFileBaseName);
            }
            if (genoFileSuffix == "bed")
            {
                plink.splitBinaryFile(genoFileAbPath+"/"+genoFileBaseName, keepFile, genoFileAbPath+"/"+keepFileBaseName);
            }

            this->process->start(this->toolpath+"plink", plink.getParamList());
            if (!this->process->waitForStarted())
            {
                isLD_OK = false;
                throw -1;
            }
            if (!this->process->waitForFinished(-1))
            {
                this->resetWindow();
                isLD_OK = false;
                throw -1;
            }
            this->process->close();
            this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
            this->runningMsgWidget->appendText(keepFileBaseName+".ped and "+keepFileBaseName+".map OK.\n");
            qApp->processEvents();

            QFile file;
//            file.remove(keepFile);

            this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
            this->runningMsgWidget->appendText("Make "+keepFileBaseName+".genotype.\n");
            qApp->processEvents();
            // Make .genotype file.
            if (popLDdecay.makeGenotype(genoFileAbPath+"/"+keepFileBaseName+".ped",
                                    genoFileAbPath+"/"+keepFileBaseName+".map",
                                    genoFileAbPath+"/"+keepFileBaseName+".genotype"))
            {
                this->process->start(this->scriptpath+"poplddecay/plink2genotype", popLDdecay.getParamList());
                if (!this->process->waitForStarted())
                {
                    isLD_OK = false;
                    throw -1;
                }
                qDebug() << this->process->pid();
                while (!this->process->waitForFinished(-1))
                {
                    qApp->processEvents(QEventLoop::ExcludeUserInputEvents, 200);
                    QThread::msleep(10);
                }
                this->process->close();

                this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
                this->runningMsgWidget->appendText(keepFileBaseName+".genotype OK.\n");
                qApp->processEvents();
            }
            else
            {
                this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
                this->runningMsgWidget->appendText("Make "+keepFileBaseName+".genotype ERROR.\n");
                qApp->processEvents();
                isLD_OK = false;
                throw -1;
            }

//            file.remove(genoFileAbPath+"/"+keepFileBaseName+".ped");
//            file.remove(genoFileAbPath+"/"+keepFileBaseName+".map");
            file.remove(genoFileAbPath+"/"+keepFileBaseName+".log");
            file.remove(genoFileAbPath+"/"+keepFileBaseName+".nosex");

            // Run LD.
            // keepFileName:fileAbPath+"/"+fileBaseName+"_"+fid+".keep"
            if (popLDdecay.runLD(genoFileAbPath+"/"+keepFileBaseName+".genotype",
                                 out+"/"+name+"_"+keepFileBaseName.split("_")[keepFileBaseName.split("_").length()-1]))
            {
                this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
                this->runningMsgWidget->appendText("Run LD,\n");
                qApp->processEvents();
                this->process->start(this->toolpath+"PopLDdecay", popLDdecay.getParamList());
                if (!this->process->waitForStarted())
                {
//                    QMessageBox::information(nullptr, "Error", "Can't find perl in system path. ");
                    isLD_OK = false;
                    throw -1;
                }
                while (!this->process->waitForFinished(-1))
                {
                    qApp->processEvents(QEventLoop::ExcludeUserInputEvents, 200);
                    QThread::msleep(10);
                }
                this->process->close();
                ui->ldResultLineEdit->setText(out+"/"+name+"_"+keepFileBaseName.split("_")[keepFileBaseName.split("_").length()-1]+".stat.gz");
                this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
                this->runningMsgWidget->appendText("LD OK. (FID: " + keepFileBaseName.split("_")[keepFileBaseName.split("_").length() - 1] + ")\n");
                qApp->processEvents();
            }
            else
            {
                this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
                this->runningMsgWidget->appendText("LD ERROR. (FID: " + keepFileBaseName.split("_")[keepFileBaseName.split("_").length() - 1] + ")\n");
                qApp->processEvents();
                isLD_OK = false;
                throw -1;
            }
//            file.remove(genoFileAbPath+"/"+keepFileBaseName+".genotype");
        }

        if (isLD_OK)
        {
            this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
            this->runningMsgWidget->appendText("LD by family done. \n");
            qApp->processEvents();
        }
    } catch (...) {
        ;
    }
}

void MainWindow::runPopLDdecaySingle(void)
{
    try {
        QString genotype = this->fileReader->getGenotypeFile();
        QFileInfo genoFileInfo(genotype);
        QString genoFileAbPath = genoFileInfo.absolutePath();
        QString genoFileBaseName = genoFileInfo.baseName();
        QString map = this->fileReader->getMapFile();
        QString out = this->workDirectory->getOutputDirectory();
        QString name = this->workDirectory->getProjectName();
        //  binaryFile: Set a default path. Binary geno file with paht without suffix.
        QString plinkFile = genoFileAbPath+"/"+genoFileBaseName+"_tmp";

        this->runningMsgWidget->show();
        bool transformFileFlag = false;
        this->runningMsgWidget->show();

        // Need binary files.  Every temp file and a "_tmp" after baseName, and will be deleted after gwas.
        Plink plink;
        if (isVcfFile(genotype)) // Transform "vcf" to "transpose"
        {
            if(!plink.vcf2plink(genotype, plinkFile, nullptr, nullptr, nullptr))
            {
                throw -1;
            }

            transformFileFlag = true;
        }
        if (genotype.split(".")[genotype.split(".").length()-1] == "bed")  // Transform "plink" to "binary"
        {
            if (!plink.binary2plink(genoFileAbPath+"/"+genoFileBaseName, plinkFile, nullptr, nullptr, nullptr))
            {
                throw -1;
            }

            transformFileFlag = true;
        }

        if (genotype.split(".")[genotype.split(".").length()-1] == "tped")  // Transform "transpose" to "binary"
        {
            if (map.isNull())
            {
                map = genoFileAbPath+"/"+genoFileBaseName+".tfam";
            }
            if (!plink.transpose2plink(genotype, map, plinkFile, nullptr, nullptr, nullptr))
            {
                throw -1;
            }
            transformFileFlag = true;
        }

        if (transformFileFlag)
        {   // Run plink to transform file or filter data.
            if (!runExTool(*(this->process), this->toolpath+"plink", plink.getParamList()))
            {
                return;
            }
        }
        else
        {
            plinkFile = genoFileAbPath + "/" + genoFileBaseName;
        }

        this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
        this->runningMsgWidget->appendText("Make" + plinkFile + ".genotype.\n");
        qApp->processEvents();
        PopLDdecay popLDdecay;
        if (popLDdecay.makeGenotype(plinkFile+".ped", plinkFile+".map", plinkFile+".genotype"))
        {
            if (!runExTool(*(this->process), this->scriptpath+"poplddecay/plink2genotype",
                           popLDdecay.getParamList()))
            {
                return;
            }

            this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
            this->runningMsgWidget->appendText(plinkFile + ".genotype OK.\n");
            qApp->processEvents();
        }
        else
        {
            this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
            this->runningMsgWidget->appendText(plinkFile + ".genotype ERROR.\n");
            qApp->processEvents();
            throw -1;
        }

        QFile file;
        file.remove(plinkFile+".ped");
        file.remove(plinkFile+".map");
        file.remove(plinkFile+".nosex");
        file.remove(plinkFile+".log");

        this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
        this->runningMsgWidget->appendText("Run LD, \n");
        qApp->processEvents();
        if (popLDdecay.runLD(plinkFile+".genotype", out+"/"+name))
        {
            if (!runExTool(*(this->process), this->toolpath+"PopLDdecay",
                           popLDdecay.getParamList()))
            {
                return;
            };
            ui->ldResultLineEdit->setText(out+"/"+name+".stat.gz");
            this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
            this->runningMsgWidget->appendText("LD OK.\n"+out+"/"+name+".stat.gz" + "\n");
            qApp->processEvents();
        }
        else
        {
            this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
            this->runningMsgWidget->appendText("LD ERROR.\n");
            qApp->processEvents();
            throw -1;
        }
        file.remove(plinkFile+".genotype");
    } catch (...) {
        ;
    }
}

void MainWindow::on_ldPlotPushButton_clicked()
{

    QString ldResultFile = ui->ldResultLineEdit->text();
    if (ldResultFile.isNull() || ldResultFile.isEmpty())
    {
        return;
    }
    try {
        ui->ldPlotPushButton->setEnabled(false);
        qApp->processEvents();
        QString out = this->workDirectory->getOutputDirectory();
        QString name = this->workDirectory->getProjectName();
        PopLDdecay popLDdecay;
        if (popLDdecay.plotLD(ldResultFile, out+"/"+name+"_ld"))
        {
            if (!runExTool(*(this->process), this->scriptpath+"poplddecay/Plot_OnePop",
                           popLDdecay.getParamList()))
            {
                return;
            };
            QStringList graphList(out+"/"+name+"_ld.png");
            this->graphViewer->setGraph(graphList);
            this->graphViewer->show();
        }
    } catch (...) {
        this->resetWindow();
    }

    this->resetWindow();
}

void MainWindow::on_ldReultBrowButton_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this, "Open LD result file", this->workDirectory->getOutputDirectory(),
                                              "result(*.stat *stat.gz);;all(*)");
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
    ui->ldResultLineEdit->setText(fileNames[0]);
}

void MainWindow::graphViewer_clicked_slot()
{
    qDebug() << "Graph viewer clicked" << endl;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "MainWindow::closeEvent";
    this->resetWindow();
    if (this->runningMsgWidget->isVisible())
    {
        this->runningMsgWidget->close();
    }
    if (this->graphViewer->isVisible())
    {
        this->graphViewer->close();
    }
    if (this->gemmaParamWidget->isVisible())
    {
        this->gemmaParamWidget->close();
    }
    if (this->emmaxParamWidget->isVisible())
    {
        this->emmaxParamWidget->close();
    }
    if (this->qualityControl->isVisible())
    {
        this->qualityControl->close();
    }
    if (this->isVisible() && !runningMsgWidget->isVisible())
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::on_gffFileBrowButton_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this, "Open GFF file file", "",
                                              "gff(*.gff *.gff2 *.gff3 *gff.txt);;all(*)");
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
    ui->gffFileLineEdit->setText(fileNames[0]);
}

void MainWindow::on_strucAnnoRunPushButton_clicked()
{
    QString gffFile = ui->gffFileLineEdit->text();
    QString fastaFile = ui->fastaFileLineEdit->text();
    if (gffFile.isNull() || gffFile.isEmpty() || fastaFile.isNull() || fastaFile.isEmpty())
    {
        return;
    }

    try
    {
        this->runningMsgWidget->show();
        ui->strucAnnoRunPushButton->setEnabled(false);
        qApp->processEvents();

        QFileInfo gffFileInfo(gffFile);
        QString gffFileBaseName = gffFileInfo.baseName();
        QString gffFileCompBaseName = gffFileInfo.completeBaseName();
        QString gffFileSuffix = gffFileInfo.suffix();
        QString gffFileAbPath = gffFileInfo.absolutePath();

        QString gtfFile =gffFileAbPath+"/"+gffFileCompBaseName+".gtf";

        // gffTogtf
        Annovar annovar;
        if (!annovar.gffTogtf(gffFile, gtfFile))
        {
            throw -1;
        }
        this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
        this->runningMsgWidget->appendText("Gff to gtf, \n");
        if (!runExTool(*(this->process), this->toolpath+"gffread", annovar.getParamList()))
        {
            throw -1;
        }
        this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
        this->runningMsgWidget->appendText("Gff to gtf OK.\n");
        qApp->processEvents();

        // gtfToGenePred
        QString refGeneFile = gffFileAbPath+"/"+gffFileBaseName+"_refGene.txt";
        if (!annovar.gtfToGenePred(gtfFile, refGeneFile))
        {
            throw -1;
        }
        this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
        this->runningMsgWidget->appendText("Gtf to genePred, \n");
        qApp->processEvents();
        if (!runExTool(*(this->process), this->toolpath+"gtfToGenePred", annovar.getParamList()))
        {
            throw -1;
        }
        this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
        this->runningMsgWidget->appendText("Gtf to genePred OK.\n");
        qApp->processEvents();

        // retrieve_seq_from_fasta
        QString outFastaFile = gffFileAbPath + "/" + gffFileBaseName + "_refGeneMrna.fa";
        if (!annovar.retrieveSeqFromFasta(refGeneFile, fastaFile, outFastaFile))
        {
            throw -1;
        }

        this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
        this->runningMsgWidget->appendText("Retrieve seq from fasta,\n");
        qApp->processEvents();

        if (!runExTool(*(this->process), this->scriptpath+"annovar/retrieve_seq_from_fasta",
                       annovar.getParamList()))
        {
            throw -1;
        }
        this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
        this->runningMsgWidget->appendText("Retrieve seq from fasta OK.\n");
        qApp->processEvents();

        // annotation
        QString out = this->workDirectory->getOutputDirectory();
        QString name = this->workDirectory->getProjectName();
        QString outFile = out + "/" + name + "_" + gffFileBaseName;
        QString avinput = ui->avinFileLineEdit->text();
        QString refGeneDir = gffFileAbPath;
        QString refGenePrefix = gffFileBaseName;

        // table_annovar
//        if (!annovar.tableAnnovar(avinput, refGeneDir, refGenePrefix, outFile))
//        {
//            throw -1;
//        }
//        this->process->start(this->scriptpath+"annovar/table_annovar", annovar.getParamList());

        // annotate_variation
        if (!annovar.annotateVariation(avinput, refGeneDir, refGenePrefix, outFile))
        {
            throw -1;
        }
        this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
        this->runningMsgWidget->appendText("Annotation,\n");
        qApp->processEvents();

        if (!runExTool(*(this->process), this->scriptpath+"annovar/annotate_variation",
                       annovar.getParamList()))
        {
            throw -1;
        }

        this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
        this->runningMsgWidget->appendText("Annotation OK.\n");
        qApp->processEvents();

        ui->varFuncFileLineEdit->setText(outFile+".variant_functino");
        ui->exVarFuncFileLineEdit->setText(outFile+".exonic_variant_function");

        this->process->close();

    } catch(...)
    {
        this->resetWindow();
    }
    this->resetWindow();
}

void MainWindow::on_fastaFileBrowButton_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this, "Open fasta file file", "",
                                              "fasta(*.fa *.Fa *.fasta *.Fasta);;all(*)");
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
    ui->fastaFileLineEdit->setText(fileNames[0]);
}

void MainWindow::on_avinFileBrowButton_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this, "Open annovar input file file", "",
                                              "avinput(*.avinput *.txt *.);;all(*)");
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
    ui->avinFileLineEdit->setText(fileNames[0]);
}

/**
 * @brief MainWindow::on_snpPosBrowButton_clicked
 *      To open postion of SNP file.
 */
void MainWindow::on_snpPosBrowButton_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this, "Open SNP postion file", this->workDirectory->getOutputDirectory(),
                                              "all(*)");
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
    ui->snpPosLineEdit->setText(fileNames[0]);
}

/**
 * @brief MainWindow::on_baseFileBrowButton_clicked
 *          To open database file of annotation.
 */
void MainWindow::on_baseFileBrowButton_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this, "Open GWAS result file", this->workDirectory->getOutputDirectory(),
                                              "base(all(*)");
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
    ui->baseFileLineEdit->setText(fileNames[0]);
}

/**
 * @brief MainWindow::on_varFuncFileBrowButton_clicked
 *          To open .variant_function file.
 */
void MainWindow::on_varFuncFileBrowButton_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this, "Open variant function file", this->workDirectory->getOutputDirectory(),
                                              "var_func(*.variant_function);;(all(*)");
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
    ui->varFuncFileLineEdit->setText(fileNames[0]);
}

/**
 * @brief MainWindow::on_exVarFuncFileBrowButton_clicked
 *          To open .exonic_variant_function file.
 */
void MainWindow::on_exVarFuncFileBrowButton_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this, "Open exonic variant function file", this->workDirectory->getOutputDirectory(),
                                              "ex_var_func(*.exonic_variant_function);;(all(*)");
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
    ui->exVarFuncFileLineEdit->setText(fileNames[0]);
}

/**
 * @brief MainWindow::on_funcAnnoRunPushButton_clicked
 *          To run functional anotation.
 *          Start from snpPosFile made in annoFuncStep or input.
 */
void MainWindow::on_funcAnnoRunPushButton_clicked()
{
    ui->funcAnnoRunPushButton->setEnabled(false);
    qApp->processEvents();

    try {
        QString snpPosFile = ui->snpPosLineEdit->text();            // SNP position file: SNP P_VAL CHR BP
        QString baseFile = ui->baseFileLineEdit->text();            // data base file
        QString exVarFuncFile = ui->exVarFuncFileLineEdit->text();  // .exonic_variant_function
        QString varFuncFile = ui->varFuncFileLineEdit->text();      // .variant_function
        QString out = this->workDirectory->getOutputDirectory();
        QString name = this->workDirectory->getProjectName();

        if (snpPosFile.isEmpty())
        {
            QMessageBox::information(nullptr, "Error", "A position of SNP file is necessary.");
            throw -1;
        }
        if (baseFile.isEmpty())
        {
            QMessageBox::information(nullptr, "Error", "A annotation base file is necessary.");
            throw -1;
        }
        if (varFuncFile.isEmpty())
        {
            QMessageBox::information(nullptr, "Error", "A .variant_function file is necessary.");
            throw -1;
        }
        if (exVarFuncFile.isEmpty())
        {
            QMessageBox::information(nullptr, "Error", "A .exonic_variant_function file is necessary.");
            throw -1;
        }

        QFileInfo snpPosFileInfo(snpPosFile);
        QString snpPosFileAbPath = snpPosFileInfo.absolutePath();
        QString snpPosFileBaseName = snpPosFileInfo.baseName();

        QString exonicPosFile = snpPosFileAbPath + "/exonic_pos";
        QString nonExonicPosFile = snpPosFileAbPath + "/non_exonic_pos";
        QString funcAnnoResult = out + "/" + name +"_func_anno";
        FuncAnnotator funcAnnotator;

        this->runningMsgWidget->show();
        QFuture<void> fu = QtConcurrent::run([&]()
        {   // Run functional annotation in another thread;
            this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
            this->runningMsgWidget->appendText("Complete exonic SNP infomation,");
            qApp->processEvents();
            if (!funcAnnotator.complExoSnpInfo(snpPosFile, exVarFuncFile, exonicPosFile))
            {
                throw -1;
            }
            this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
            this->runningMsgWidget->appendText("OK\n");
            qApp->processEvents();

            this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
            this->runningMsgWidget->appendText("Complete non-exonic SNP infomation,");
            qApp->processEvents();
            if (!funcAnnotator.complNonExoSnpInfo(exonicPosFile, snpPosFile, varFuncFile, nonExonicPosFile))
            {
                throw -1;
            }
            this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
            this->runningMsgWidget->appendText("OK\n");
            qApp->processEvents();

            this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
            this->runningMsgWidget->appendText("Complete functional annotation infomation,");
            qApp->processEvents();
            if (!funcAnnotator.complFuncAnnoInfo(exonicPosFile, nonExonicPosFile, baseFile, funcAnnoResult))
            {
                throw -1;
            }
            this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
            this->runningMsgWidget->appendText("Functional annotation OK\n");
            qApp->processEvents();
        });
        while (!fu.isFinished())
        {
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents, 200);
            QThread::msleep(10);
        }
    } catch (...) {
        this->resetWindow();
    }

    ui->funcAnnoRunPushButton->setEnabled(true);
    qApp->processEvents();
}

/**
 * @brief MainWindow::on_funcAnnoStepPushButton_clicked
 *          Read map file and p-value file, filter SNP and exstract postion of SNP.
 */
void MainWindow::on_funcAnnoStepPushButton_clicked()
{
    try {
        ui->funcAnnoStepPushButton->setEnabled(false);
        qApp->processEvents();

        QString mapFile = this->fileReader->getMapFile();
        if (mapFile.isNull())
        {
            QMessageBox::information(nullptr, "Error", "A map file is necessary.");
            throw -1;
        }

        QString pvalFile = ui->annoPvalLineEdit->text();    // p-value file(the first column is SNP_ID and the last column is p-value)
        if (pvalFile.isNull() || pvalFile.isEmpty())
        {
            QMessageBox::information(nullptr, "Error", "A p-value file is necessary.");
            throw -1;
        }

        QFileInfo pvalFileInfo(pvalFile);
        QString pvalFileAbPath = pvalFileInfo.absolutePath();
        QString pvalFileBaseName = pvalFileInfo.baseName();

        QString thBase = ui->annoThBaseLineEdit->text();    // Threshold base number.
        QString thExpo = ui->annoThExpoLineEdit->text();    // Threshold exponent.

        if (thBase.isEmpty() || thExpo.isEmpty())
        {
            QMessageBox::information(nullptr, "Error", "Please set the threshold.");
            throw -1;
        }

        QString sigSnpFile = pvalFileAbPath + "/" + pvalFileBaseName + "_sig";   // to save SNP after filter.
        QString sigSnpPosFile = sigSnpFile + "_pos";        // to save extracted position of SNP after filter.

        FuncAnnotator funcAnnotator;

        this->runningMsgWidget->show();

        QFuture<void> fu = QtConcurrent::run([&]()
        {
            this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
            this->runningMsgWidget->appendText("Filter SNP above threshold,");
            if (!funcAnnotator.filterSNP(pvalFile, thBase, thExpo, sigSnpFile))
            {
                throw -1;
            }
            this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
            this->runningMsgWidget->appendText("OK\n");
            qApp->processEvents();
            this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
            this->runningMsgWidget->appendText("Extract position of SNP,");
            qApp->processEvents();
            if (!funcAnnotator.extractPos(sigSnpFile, mapFile, sigSnpPosFile))
            {
                throw -1;
            }
            this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
            this->runningMsgWidget->appendText("OK\n");
            qApp->processEvents();

            ui->snpPosLineEdit->setText(sigSnpPosFile);
            ui->funcAnnoRunPushButton->setEnabled(true);
            qApp->processEvents();
        });
        while (!fu.isFinished())
        {
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents, 200);
        }

    } catch (...) {
        this->resetWindow();
    }

    this->resetWindow();
}

void MainWindow::on_annoPvalBrowButton_clicked()
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
    ui->annoPvalLineEdit->setText(fileNames[0]);
}

void MainWindow::on_eigenvalFileBrowButton_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this, "Open eigenval file", this->workDirectory->getOutputDirectory(),
                                              "eigenval(*.eigenval);;all(*)");
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
    ui->eigenvalueLineEdit->setText(fileNames[0]);
}

void MainWindow::on_eigenvecFileBrowButton_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this, "Open eigenvec file", this->workDirectory->getOutputDirectory(),
                                              "eigenvec(*.eigenvec);;all(*)");
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
    ui->eigenvectorLineEdit->setText(fileNames[0]);
}

void MainWindow::on_pcaPlotPushButton_clicked()
{
    QString eigenvalFile = ui->eigenvalueLineEdit->text();
    QString eigenvecFile = ui->eigenvectorLineEdit->text();
    QString outFile = this->workDirectory->getOutputDirectory() + "/" +
                      this->workDirectory->getProjectName() + "_pca.png";

    ui->pcaPlotPushButton->setEnabled(false);
    qApp->processEvents();
    if (eigenvalFile.isEmpty() ||
        eigenvecFile.isEmpty() ||
        outFile.isEmpty())
    {
        this->resetWindow();
        return;
    }

    QStringList param;
    param.clear();
    param.append(this->scriptpath+"pca/pca_plot.R");    // Can choose pca_plot.R or pca_ggplot.R
    param.append(eigenvalFile);
    param.append(eigenvecFile);
    param.append(outFile);

    // R in environment path is necessary.
    if (!runExTool(*(this->process), "Rscript", param))
    {
        return;
    }
    this->runningMsgWidget->show();
    this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
    this->runningMsgWidget->appendText("Plot PCA, \n");


    this->runningMsgWidget->appendText(QDateTime::currentDateTime().toString());
    this->runningMsgWidget->appendText("OK, \n");
    this->runningMsgWidget->appendText(outFile);

    // Show plot
    this->graphViewer->setGraph(QStringList() << outFile);
    this->graphViewer->show();
    ui->pcaPlotPushButton->setEnabled(true);
    qApp->processEvents();
}

/**
 * @brief AssocTool::pValCorrect
 * @param pvalFile  The last column is p-val.
 * @param type      Shep-Down(Holm):holm, FDR(BH):BH, Bonferroni:bonferroni
 * @return
 */
bool MainWindow::pValCorrect(QString pvalFile, bool header, QString correctType, QString outFile)
{
    if (pvalFile.isNull() || correctType.isNull() || outFile.isNull())
    {
        return false;
    }

    QStringList param;

    param.clear();
    param.append(this->scriptpath+"qqman/correction.R");
    param.append(pvalFile);
    param.append(header ? "TRUE" : "FALSE");
    param.append(correctType);
    param.append(outFile);
    // R in environment path is necessary.
    if (!runExTool(*(this->process), "Rscript", param))
    {
        return false;
    }

    return true;
}

void MainWindow::on_qualCtrlDetailPushButton_clicked()
{
    this->qualityControl->show();
}

bool MainWindow::runExTool(QProcess &process, QString tool, QStringList param)
{
    process.start(tool, param);
    if (!process.waitForStarted())
    {
        QMessageBox::information(nullptr, "Error", "Can't open " + tool);
        return false;
    }
    while (!process.waitForFinished(-1))
    {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents, 200);
        QThread::msleep(10);
    }
    process.close();

    return true;
}

bool MainWindow::checkoutExistence(QString filePath)
{
    QFile file(filePath);
    return file.exists();
}
