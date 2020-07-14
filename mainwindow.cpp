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
//    process = new QProcess;
    graphViewer = new GraphViewer;
    ldByFamGroupButton = new QButtonGroup;

    // Default output directory setting
    workDirectory->setProjectName("pro1");
#ifdef QT_NO_DEBUG
    workDirectory->setOutputDirectory("/home");
#else
    workDirectory->setOutputDirectory("/home/dengchao/Desktop/test");
#endif
    ui->projectNameLineEdit->setText(workDirectory->getProjectName());
    ui->outdirLineEdit->setText(workDirectory->getOutputDirectory()+"/"+workDirectory->getProjectName());

    // LD by family.
    ldByFamGroupButton->addButton(ui->yesLDByFamRadioButton);
    ldByFamGroupButton->addButton(ui->noLDByFamRadioButton);
    ldByFamGroupButton->setExclusive(true);

    connect(runningMsgWidget, SIGNAL(closeSignal()), this, SLOT(on_closeRunningWidget()));
    // Multi thread to modify ui.
    connect(this, SIGNAL(runningMsgWidgetAppendText(QString)),
            this->runningMsgWidget, SLOT(on_appendText(QString)));
    connect(this, SIGNAL(setLineEditTextSig(QLineEdit*, QString)),
            this, SLOT(on_setLineEditText(QLineEdit*, QString)));
    connect(this, SIGNAL(setButtonEnabledSig(QAbstractButton *, bool)),
            this, SLOT(on_setButtonEnabled(QAbstractButton *, bool)));
    connect(this, SIGNAL(setGraphViewerGraphSig(QStringList)),
            this, SLOT(on_setGraphViewerGraph(QStringList)));
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

//    if (process)    // QProcess
//    {
//        process->terminate();
//        process->waitForFinished(-1);
//    }
//    delete process;

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
    if (this->runningFlag)
    {
        QMessageBox::information(nullptr, "Error", "A project is running now.");
        return;
    }
    this->runningFlag = true;

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
//    this->runningMsgWidget->clearText();

    QFileInfo pheFileInfo(phenotype);
    QString pheFileBaseName = pheFileInfo.baseName();
    QString pheFileAbPath = pheFileInfo.absolutePath();
    QString pheFileSuffix = pheFileInfo.suffix();

    QFuture<void> fu = QtConcurrent::run(QThreadPool::globalInstance(), [&]()
    {
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
    });
    while (!fu.isFinished())
    {
        qApp->processEvents(QEventLoop::AllEvents, 200);
    }

    this->resetWindow();
    this->runningFlag = false;
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

        if (!runExTool(this->toolpath+"plink", plink.getParamList()))
        {
            return false;
        }

        QString filteredSnpIDFile = linkageFilteredFilePrefix + ".prune.in";

        if (!this->checkoutExistence(filteredSnpIDFile))
        {
            QMessageBox::information(nullptr, "Error", "Linkage filter error.");
            return false;
        }

        plink.extractBySnpNameFile(genotype, map, filteredSnpIDFile, linkageFilteredFilePrefix);

        if (!runExTool(this->toolpath+"plink", plink.getParamList()))
        {
            return false;
        }

        genotype = linkageFilteredFilePrefix + ".ped";
        map = linkageFilteredFilePrefix + ".map";
        genoFileName = genoFileBaseName + "_ldfl";

        if (!this->checkoutExistence(genotype) ||
            !this->checkoutExistence(map))
        {
            QMessageBox::information(nullptr, "Error", "Extaract snp after linkage filter error.");
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
        if (!runExTool(this->toolpath+"plink", plink.getParamList()))
        {
            return false;
        }
    }

    Gemma gemma;

    if (gemmaParamWidget->isFamCompletedAuto())
    {
        // Replace "NA" to "-9", then complete .fam
        // .fam: FID IID PID MID Sex 1 Phe  (phenotype data to the 7th column of .fam)
        gemma.phe_fam_Preparation(phenotype, binaryFile+".fam");
    }

    if (kinship.isNull() && this->gemmaParamWidget->isMakeRelatedMatAuto())
    {
         if (!gemma.makeKinship(binaryFile, genoFileBaseName+"_tmp", moreParam))
         {
             this->resetWindow();
             return false;  // Make kinship failed.
         }
         if (!runExTool(this->toolpath+"gemma", gemma.getParamList()))
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
    if (!runExTool(this->toolpath+"gemma", gemma.getParamList()))
    {
        return false;
    }

    QFile file;
    // delete intermidiate file.
    if (binaryFile+".bed" != fileReader->getGenotypeFile())
    {
        file.remove(binaryFile+".bed");
        file.remove(binaryFile+".bim");
        file.remove(binaryFile+".fam");
    }
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

    if (qualityControl->isLinkageFilterNeeded() && genotype != fileReader->getGenotypeFile())
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
            QMessageBox::information(nullptr, "Error", "Gemma corrected error.");
            return false;
        }

        if (checkoutExistence(correctedFile))
        {
            emit setLineEditTextSig(ui->qqmanGwasResultLineEdit, correctedFile);
            QThread::msleep(10);
        }
    }
    else if (model == "LMM")
    {
        if (this->runningFlag && !this->checkoutExistence(out+"/output"
            +(i==0?"":QString::number(i))+"/"+name+"_"+pheFileBaseName+".assoc.txt"))
        {
            QMessageBox::information(nullptr, "Error", "Gemma GWAS error.");
            return false;
        }

//        ui->qqmanGwasResultLineEdit->setText(out+"/output"
//            +(i==0?"":QString::number(i))+"/"+name+"_"+pheFileBaseName+".assoc.txt");
        emit setLineEditTextSig(ui->qqmanGwasResultLineEdit, out+"/output"
                 +(i==0?"":QString::number(i))+"/"+name+"_"+pheFileBaseName+".assoc.txt");
        QThread::msleep(10);
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

        if (!runExTool(this->toolpath+"plink", plink.getParamList()))
        {
            return false;
        }

        QString filteredSnpIDFile = linkageFilteredFilePrefix + ".prune.in";

        plink.extractBySnpNameFile(genotype, map, filteredSnpIDFile, linkageFilteredFilePrefix);

        if (!runExTool(this->toolpath+"plink", plink.getParamList()))
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
        if (!runExTool(this->toolpath+"plink", plink.getParamList()))
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
         if (!runExTool(this->toolpath+"emmax-kin", emmax.getParamList()))
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
    if (!runExTool(this->toolpath+"emmax", emmax.getParamList()))
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
            QMessageBox::information(nullptr, "Error", "Gemma corrected error.");
            return false;
        }

        if (checkoutExistence(correctedFile))
        {
//            ui->qqmanGwasResultLineEdit->setText(correctedFile);
            emit setLineEditTextSig(ui->qqmanGwasResultLineEdit, correctedFile);
            QThread::msleep(10);
        }
    }
    else
    {
        if (this->runningFlag && !checkoutExistence(out+"/"+name+"_"+pheFileBaseName+".ps"))
        {
            QMessageBox::information(nullptr, "Error", "Emmax GWAS error.");
            return false;
        }
//            ui->qqmanGwasResultLineEdit->setText(out+"/"+name+"_"+pheFileBaseName+".ps");
        emit setLineEditTextSig(ui->qqmanGwasResultLineEdit, out+"/"+name+"_"+pheFileBaseName+".ps");
        QThread::msleep(10);
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

    if (qualityControl->isLinkageFilterNeeded() && genotype != fileReader->getGenotypeFile())
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
        emit runningMsgWidgetAppendText("Linkage filter,\n");

        plink.linkageFilter(genotype, map, winSize, stepLen, r2Threshold, linkageFilteredFilePrefix);
        if (!runExTool(this->toolpath+"plink", plink.getParamList()))
        {
            return false;
        }

        QString filteredSnpIDFile = linkageFilteredFilePrefix + ".prune.in";

        if (!this->checkoutExistence(filteredSnpIDFile))
        {
            QMessageBox::information(nullptr, "Error", "Linkage filter error.");
            return false;
        }

        plink.extractBySnpNameFile(genotype, map, filteredSnpIDFile, linkageFilteredFilePrefix);

        emit runningMsgWidgetAppendText("Extract by SNP name\n");

        if (!runExTool(this->toolpath+"plink", plink.getParamList()))
        {
            return false;
        }

        emit runningMsgWidgetAppendText("OK\n");

        genotype = linkageFilteredFilePrefix + ".ped";
        map = linkageFilteredFilePrefix + ".map";
        genoFileName = genoFileName + "_ldfl";

        if (!this->checkoutExistence(genotype) || !this->checkoutExistence(map))
        {
            QMessageBox::information(nullptr, "Error", "Extract snp after linkage filter error.");
            return false;
        }
    }

    // Run GWAS(Set parameters)
    if(!plink.runGWAS(phenotype, genotype, map, covar, kinship, model,
                     maf, mind, geno, out+"/"+name+"_"+pheFileBaseName))
    {
        return false;
    }
    if (!runExTool(this->toolpath+"plink", plink.getParamList()))
    {
        return false;
    }

    if (this->runningFlag && !this->checkoutExistence(out+"/"+name+"_"+pheFileBaseName+".assoc."+model.toLower()))
    {
        QMessageBox::information(nullptr, "Error", "Plink GWAS error.");
        return false;
    }

    if (runningFlag && checkoutExistence(out+"/"+name+"_"+pheFileBaseName+".assoc."+model.toLower()))
    {
//        ui->qqmanGwasResultLineEdit->setText(out+"/"+name+"_"+pheFileBaseName+".assoc."+model.toLower());
        emit setLineEditTextSig(ui->qqmanGwasResultLineEdit,
                                out+"/"+name+"_"+pheFileBaseName+".assoc."+model.toLower());
    }


    QFile file;
    file.remove(out+"/"+name+"_"+pheFileBaseName+".nosex");
    file.remove(out+"/"+name+"_"+pheFileBaseName+".log");

    return true;
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

    if (this->runningFlag)
    {
        // Juage there are any tools running now.
        QMessageBox::StandardButton ret = QMessageBox::information(this, "WARNING",
           "The project may be terminated if close the widget!   ",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

        if (ret == QMessageBox::Yes)
        {
            emit terminateProcess();
            this->runningFlag = false;
            this->runningMsgWidget->clearText();
            this->runningMsgWidget->hide();
            this->resetWindow();
        }
        return;
    }
    else
    {   // Close widget directly while no tool running.
        this->runningMsgWidget->clearText();
        this->runningMsgWidget->hide();
    }
}

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
//    if (this->process->isOpen())
//    {
//        this->process->close();
//    }
//    if (this->process)
//    {
//        this->process->terminate();
//        this->process->waitForFinished(-1);
//    }
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

    return curText + newText;
}


void MainWindow::on_projectNameLineEdit_textChanged(const QString &text)
{   // When edit finished and the current text is empty, set a default name.
    this->workDirectory->setProjectName(text);
    if (!ui->outdirLineEdit->text().isEmpty())
    {   // If a out directory is selected, display the out directory + the project name.
        ui->outdirLineEdit->setText(this->workDirectory->getOutputDirectory()+"/"+text);
    }
}

void MainWindow::on_projectNameLineEdit_editingFinished()
{   // Edit is finised but current text is empty.
    if (ui->projectNameLineEdit->text().isEmpty())
    {
        ui->projectNameLineEdit->setText("pro1");
    }
}

void MainWindow::on_outdirLineEdit_editingFinished()
{
    if (ui->outdirLineEdit->text().isEmpty())
    {
        ui->outdirLineEdit->setText(this->workDirectory->getOutputDirectory() + "/" +
                                    this->workDirectory->getProjectName());
    }
}

void MainWindow::on_drawManPushButton_clicked()
{
    if (this->runningFlag)
    {
        QMessageBox::information(nullptr, "Error", "A project is running now.");
        return;
    }

    this->runningFlag = true;
    ui->drawManPushButton->setEnabled(false);
    qApp->processEvents();

    QFuture<void> fu = QtConcurrent::run(QThreadPool::globalInstance(), [&]()
    {
        try {
            QString gwasResulFile = ui->qqmanGwasResultLineEdit->text();
            if (gwasResulFile.isEmpty())
            {   // Gwas result file is necessary.
                emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                                "\nA GWAS result file is necessary.\n");
                throw -1;
            }

            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                      "\nMake qqman input file, \n");
            QThread::msleep(10);
            // Transform gwas result file type to input file type of qqman.
            QStringList qqmanFile = makeQQManInputFile(gwasResulFile); //   path/name.gemma_wald
            QStringList outList;
            if (qqmanFile.isEmpty())
            {   // makeQQManInputFile error.
                emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                          "\nMake qqman input file ERROR.\n");
                QThread::msleep(10);
                throw -1;
            }
            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                      "\nMake qqman input file OK.\n");
            QThread::msleep(10);

            for (auto item:qqmanFile)
            {   // Multiple result, multiple output plot, append to list.
                outList.append(this->workDirectory->getOutputDirectory()+"/"+this->workDirectory->getProjectName()
                               +"_"+item.split(".")[item.split(".").length()-1]+"_man.png");
            }

            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                      "\nDraw manhattan plot, \n");
            QThread::msleep(10);
            if (!this->drawManhattan(qqmanFile, outList))
            {   // drawManhattan error
                emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                          "\nDraw manhattan plot ERROR.\n");
                QThread::msleep(10);
                throw -1;
            }

            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                      "\nDraw manhattan plot OK." +
                                      "\nmanhattan plot: \n" +
                                      outList.join("\n")+"\n");
            QThread::msleep(10);

            QFile file;
            for (auto item:qqmanFile)
            {   // Remove intermediate file.
                if (item == gwasResulFile)
                {
                    continue;
                }
                file.remove(item);
            }
        } catch (...) {
            this->resetWindow();    // reset MainWindow
        }
    });
    while (!fu.isFinished())
    {
        qApp->processEvents(QEventLoop::AllEvents, 200);
    }
    this->resetWindow();
    this->runningFlag = false;
}

void MainWindow::on_drawQQPushButton_clicked()
{
    if (this->runningFlag)
    {
        QMessageBox::information(nullptr, "Error", "A project is running now.");
        return;
    }
    this->runningFlag = true;
    ui->drawQQPushButton->setEnabled(false);
    qApp->processEvents();
    QFuture<void> fu = QtConcurrent::run(QThreadPool::globalInstance(), [&]()
    {
        try {
            QString gwasResulFile = ui->qqmanGwasResultLineEdit->text();
            if (gwasResulFile.isEmpty())
            {   // Gwas result file is necessary.
                emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                          "\nA GWAS result file is necessary.\n");
                QThread::msleep(10);
                throw -1;
            }

            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                            "\nMake qqman input file, \n");
            QThread::msleep(10);
            // Transform gwasResultFile to input file type of qqman.
            QStringList qqmanFile = makeQQManInputFile(gwasResulFile); //   path/name.gemma_wald
            QStringList outList;

            if (qqmanFile.isEmpty())
            {
                emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                                "\nMake qqman input file ERROR. \n");
                QThread::msleep(10);
                throw -1;
            }
            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                            "\nMake qqman input file OK. \n");
            QThread::msleep(10);
            for (auto item:qqmanFile)
            {   // Multiple result, multiple output plot.
                outList.append(this->workDirectory->getOutputDirectory()+"/"+this->workDirectory->getProjectName()
                               +"_"+item.split(".")[item.split(".").length()-1]+"_qq.png");
            }

            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                            "\nDraw QQ plot, \n");
            QThread::msleep(10);
            if (!this->drawQQplot(qqmanFile, outList))
            {
                emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                                "\nDraw QQ plot ERROR. \n");
                QThread::msleep(10);
                throw -1;
            }

            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                            "\nDraw QQ plot OK. " +
                                            "\nQQ plot: \n" + outList.join("\n") + "\n");
            QThread::msleep(10);
            QFile file;
            for (auto item:qqmanFile)
            {   // Remove intermediate file.
                if (item == gwasResulFile)
                {
                    continue;
                }
                file.remove(item);
            }
        } catch (int) {
           this->resetWindow();
        }
    });
    while (!fu.isFinished())
    {
        qApp->processEvents(QEventLoop::AllEvents, 200);
    }

    this->resetWindow();
    this->runningFlag = false;
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

    for (int i = 0; i < data.size() && runningFlag; i++)
    {
        param.clear();
        param.append(this->scriptpath+"qqman/plot.R");
        param.append("manhattan");
        param.append(data[i]);
        param.append(out[i]);
        param.append(QString::number(gwBase)+'e'+QString::number(gwExpo));
        param.append(QString::number(sgBase)+'e'+QString::number(sgExpo));

        // R in environment path is necessary.
        if (!runExTool("Rscript", param))
        {
            return false;
        }
    }
    // Show plot

    if (this->runningFlag && checkoutExistence(out[0]))
    {
        emit setGraphViewerGraphSig(out);
        QThread::msleep(10);
    }

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
        if (!runExTool("Rscript", param))
        {
            return false;
        }

    }
    // Show plot
    if (this->runningFlag && checkoutExistence(out[0]))
    {
        emit setGraphViewerGraphSig(out);
        QThread::msleep(10);
    }
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
    if (this->runningFlag)
    {
        QMessageBox::information(nullptr, "Error", "A project is running now.");
        return;
    }
    this->runningFlag = true;
    if (this->fileReader->getGenotypeFile().isNull() || this->fileReader->getGenotypeFile().isEmpty())
    {
        QMessageBox::information(nullptr, "Error", "A genotype file is necessary!   ");
        return;
    }

    ui->pcaRunPushButton->setEnabled(false);
    qApp->processEvents();

    try {
        QFuture<void> fu = QtConcurrent::run([&]()
        {
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
                if (!runExTool(this->toolpath+"plink", plink.getParamList()))
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
                if (!runExTool(this->toolpath+"gcta64", gcta.getParamList()))
                {
                    return;
                }
            }

            // Run PCA
            if (gcta.runPCA(binaryFile, ui->nPCsLineEdit->text().toInt(),
                            ui->nThreadsLineEdit->text().toInt(), out+"/"+genoFileBaseName))
            {
                if (!runExTool(this->toolpath+"gcta64", gcta.getParamList()))
                {
                    return;
                }
            }

            QFile file;
            if (transformFileFlag && binaryFile+".bed" != fileReader->getGenotypeFile())
            {
                file.remove(binaryFile+".bed");
                file.remove(binaryFile+".bim");
                file.remove(binaryFile+".fam");
                file.remove(binaryFile+".nosex");
            }
            file.remove(binaryFile+".grm.bin");
            file.remove(binaryFile+".grm.id");
            file.remove(binaryFile+".grm.N.bin");

//            ui->eigenvalueLineEdit->setText(out+"/"+genoFileBaseName+".eigenval");
            emit setLineEditTextSig(ui->eigenvalueLineEdit, out+"/"+genoFileBaseName+".eigenval");
//            ui->eigenvectorLineEdit->setText(out+"/"+genoFileBaseName+".eigenvec");
            emit setLineEditTextSig(ui->eigenvectorLineEdit, out+"/"+genoFileBaseName+".eigenvec");
        });
        while (!fu.isFinished())
        {
            qApp->processEvents(QEventLoop::AllEvents, 200);
        }

    } catch (...) {
        this->resetWindow();
    }

    this->resetWindow();
    this->runningFlag = false;
}

void MainWindow::on_ldRunPushButton_clicked()
{
    if (this->runningFlag)
    {
        QMessageBox::information(nullptr, "Error", "A project is running now.");
        return;
    }
    this->runningFlag = true;

    if (this->fileReader->getGenotypeFile().isNull() || this->fileReader->getGenotypeFile().isEmpty())
    {
        QMessageBox::information(nullptr, "Error", "A genotype file is necessary!   ");
        return;
    }

    ui->ldRunPushButton->setEnabled(false);
    qApp->processEvents();

    QFuture<void> fu = QtConcurrent::run([&]()
    {
        if (ui->yesLDByFamRadioButton->isChecked())
        {
            this->runPopLDdecaybyFamily();
        }
        else
        {
            this->runPopLDdecaySingle();
        }
    });
    while (!fu.isFinished())
    {
        qApp->processEvents(QEventLoop::AllEvents, 200);
    }

    this->resetWindow();
    this->runningFlag = false;
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


        Plink plink;
        PopLDdecay popLDdecay;
        if (isVcfFile(genotype)){} // Transform "vcf" to "transpose"

        // Make .keep file.
        emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                        "\nMake .keep file, \n");
        QThread::msleep(10);
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

        emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                        "\n.keep file OK.\n");
        QThread::msleep(10);
        bool isLD_OK = true;
        for (QString keepFile:keepFileList)
        {
            QFileInfo keepFileInfo(keepFile);
            QString keepFileBaseName = keepFileInfo.baseName();
            QString keepFileAbPath = keepFileInfo.absolutePath();

            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                    "\nMake "+keepFileBaseName+".ped"+" and "+keepFileBaseName+".map, \n");
            QThread::msleep(10);
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

            if (!this->runExTool(this->toolpath+"plink", plink.getParamList()))
            {
                throw -1;
            }
            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                "\n"+ keepFileBaseName + ".ped and " + keepFileBaseName + ".map OK.\n");
            QThread::msleep(10);
            QFile file;
            file.remove(keepFile);

            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                            "\nMake "+keepFileBaseName+".genotype,\n");
            QThread::msleep(10);
            // Make .genotype file.
            if (popLDdecay.makeGenotype(genoFileAbPath+"/"+keepFileBaseName+".ped",
                                    genoFileAbPath+"/"+keepFileBaseName+".map",
                                    genoFileAbPath+"/"+keepFileBaseName+".genotype"))
            {
                if (!this->runExTool(this->scriptpath+"poplddecay/plink2genotype",
                                     popLDdecay.getParamList()))
                {
                    throw -1;
                }

                emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                                + "\n" + keepFileBaseName+".genotype OK.\n");
                QThread::msleep(10);
            }
            else
            {
                emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                            "\nMake "+keepFileBaseName+".genotype ERROR.\n");
                QThread::msleep(10);
                isLD_OK = false;
                throw -1;
            }

            file.remove(genoFileAbPath+"/"+keepFileBaseName+".ped");
            file.remove(genoFileAbPath+"/"+keepFileBaseName+".map");
            file.remove(genoFileAbPath+"/"+keepFileBaseName+".log");
            file.remove(genoFileAbPath+"/"+keepFileBaseName+".nosex");

            // Run LD.
            // keepFileName:fileAbPath+"/"+fileBaseName+"_"+fid+".keep"
            if (popLDdecay.runLD(genoFileAbPath+"/"+keepFileBaseName+".genotype",
                                 out+"/"+name+"_"+keepFileBaseName.split("_")[keepFileBaseName.split("_").length()-1]))
            {
                emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                                "\nRun LD,\n");
                QThread::msleep(10);
                if (!this->runExTool(this->toolpath+"PopLDdecay", popLDdecay.getParamList()))
                {
                    throw -1;
                }

//                ui->ldResultLineEdit->setText(out+"/"+name+"_"+keepFileBaseName.split("_")[keepFileBaseName.split("_").length()-1]+".stat.gz");
                emit setLineEditTextSig(ui->ldResultLineEdit,
                    out+"/"+name+"_"+keepFileBaseName.split("_").back()+".stat.gz");
                QThread::msleep(10);
                emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                    "\nLD OK. (FID: " +
                    keepFileBaseName.split("_")[keepFileBaseName.split("_").length() - 1] + ")\n");
                QThread::msleep(10);
            }
            else
            {
                emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                    "\nLD ERROR. (FID: " +
                    keepFileBaseName.split("_")[keepFileBaseName.split("_").length() - 1] + ")\n");
                QThread::msleep(10);
                isLD_OK = false;
                throw -1;
            }
            file.remove(genoFileAbPath+"/"+keepFileBaseName+".genotype");
        }

        if (isLD_OK)
        {
            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                "\nLD by family done. \n");
            QThread::msleep(10);
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

        bool transformFileFlag = false;

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
            if (!runExTool(this->toolpath+"plink", plink.getParamList()))
            {
                return;
            }
        }
        else
        {
            plinkFile = genoFileAbPath + "/" + genoFileBaseName;
        }

        emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                       "\nMake " + plinkFile + ".genotype,\n");
        QThread::msleep(10);
        PopLDdecay popLDdecay;
        if (popLDdecay.makeGenotype(plinkFile+".ped", plinkFile+".map", plinkFile+".genotype"))
        {
            if (!runExTool(this->scriptpath+"poplddecay/plink2genotype",
                           popLDdecay.getParamList()))
            {
                return;
            }

            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                            + "\n" + plinkFile + ".genotype OK.\n");
            QThread::msleep(10);
        }
        else
        {

            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                            + "\n" + plinkFile + ".genotype ERROR.\n");
            QThread::msleep(10);
            throw -1;
        }

        QFile file;
        if (plinkFile != genoFileAbPath + "/" + genoFileBaseName)
        {
            file.remove(plinkFile+".ped");
            file.remove(plinkFile+".map");
        }
        file.remove(plinkFile+".nosex");
        file.remove(plinkFile+".log");

        emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                        + "\nRun LD,\n");
        QThread::msleep(10);
        if (popLDdecay.runLD(plinkFile+".genotype", out+"/"+name))
        {
            if (!runExTool(this->toolpath+"PopLDdecay",
                           popLDdecay.getParamList()))
            {
                return;
            };

            if (this->runningFlag && checkoutExistence(out+"/"+name+".stat.gz"))
            {
//                ui->ldResultLineEdit->setText(out+"/"+name+".stat.gz");
                emit setLineEditTextSig(ui->ldResultLineEdit, out+"/"+name+".stat.gz");
                QThread::msleep(10);
            }

            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                            + "\nLD OK.\n");
            QThread::msleep(10);
        }
        else
        {
            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                            + "\nLD ERROR.\n");
            QThread::msleep(10);
            throw -1;
        }
        file.remove(plinkFile+".genotype");
    } catch (...) {
        ;
    }
}

void MainWindow::on_ldPlotPushButton_clicked()
{
    if (this->runningFlag)
    {
        QMessageBox::information(nullptr, "Error", "A project is running now.");
        return;
    }
    this->runningFlag = true;
    ui->ldPlotPushButton->setEnabled(false);
    qApp->processEvents();
    QString ldResultFile = ui->ldResultLineEdit->text();
    if (ldResultFile.isNull() || ldResultFile.isEmpty())
    {
        return;
    }
    try {
        QFuture<void> fu = QtConcurrent::run([&]()
        {
            QString out = this->workDirectory->getOutputDirectory();
            QString name = this->workDirectory->getProjectName();
            PopLDdecay popLDdecay;
            emit runningMsgWidgetAppendText("LD plot, \n" + ldResultFile + "\n");
            QThread::msleep(10);
            if (popLDdecay.plotLD(ldResultFile, out+"/"+name+"_ld"))
            {
                if (!runExTool(this->scriptpath+"poplddecay/Plot_OnePop",
                               popLDdecay.getParamList()))
                {
                    return;
                };
                QStringList graphList(out+"/"+name+"_ld.png");
                if (this->runningFlag && checkoutExistence(graphList[0]))
                {
                    emit runningMsgWidgetAppendText("LD plot OK.\n\n" + out+"/"+name+"_ld.png\n");
                    emit setGraphViewerGraphSig(graphList);
                    QThread::msleep(10);
                }
            }
        });
        while (!fu.isFinished())
        {
            qApp->processEvents(QEventLoop::AllEvents, 200);
        }
    } catch (...) {
        this->resetWindow();
    }

    ui->ldPlotPushButton->setEnabled(true);
    qApp->processEvents();
    this->runningFlag = false;
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
    this->runningFlag = true;
    QString gffFile = ui->gffFileLineEdit->text();
    QString fastaFile = ui->fastaFileLineEdit->text();
    if (gffFile.isNull() || gffFile.isEmpty() || fastaFile.isNull() || fastaFile.isEmpty())
    {
        return;
    }

    try
    {
        QFuture<void> fu = QtConcurrent::run([&]()
        {
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
            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                              "\nGff to gtf, \n");
            QThread::msleep(10);
            if (!runExTool(this->toolpath+"gffread", annovar.getParamList()))
            {
                throw -1;
            }
            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                            "\nsGff to gtf OK.\n");
            QThread::msleep(10);
            // gtfToGenePred
            QString refGeneFile = gffFileAbPath+"/"+gffFileBaseName+"_refGene.txt";
            if (!annovar.gtfToGenePred(gtfFile, refGeneFile))
            {
                throw -1;
            }
            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                            "\nGtf to genePred, \n");
            QThread::msleep(10);
            if (!runExTool(this->toolpath+"gtfToGenePred", annovar.getParamList()))
            {
                throw -1;
            }
            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                            "\nGtf to genePred OK.\n");
            QThread::msleep(10);
            // retrieve_seq_from_fasta
            QString outFastaFile = gffFileAbPath + "/" + gffFileBaseName + "_refGeneMrna.fa";
            if (!annovar.retrieveSeqFromFasta(refGeneFile, fastaFile, outFastaFile))
            {
                throw -1;
            }

            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                            "\nRetrieve seq from fasta,\n");
            QThread::msleep(10);

            if (!runExTool(this->scriptpath+"annovar/retrieve_seq_from_fasta",
                           annovar.getParamList()))
            {
                throw -1;
            }

            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                            "Retrieve seq from fasta OK.\n");
            QThread::msleep(10);
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

            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                            "\nAnnotation,\n");
            QThread::msleep(10);

            if (!runExTool(this->scriptpath+"annovar/annotate_variation",
                           annovar.getParamList()))
            {
                throw -1;
            }

            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                            "\nStructural annotation OK.\n");
            QThread::msleep(10);
            if (runningFlag && checkoutExistence(outFile+".variant_functino"))
            {
//                ui->varFuncFileLineEdit->setText(outFile+".variant_functino");
                emit setLineEditTextSig(ui->varFuncFileLineEdit, outFile+".variant_functino");
            }
        });
        while(!fu.isFinished())
        {
            qApp->processEvents(QEventLoop::AllEvents, 200);
        }

    } catch(...)
    {
        this->resetWindow();
    }

    this->resetWindow();
    this->runningFlag = false;
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
    QFileDialog *fileDialog = new QFileDialog(this, "Open annotation base file", this->workDirectory->getOutputDirectory(),
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
    this->runningFlag = true;
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

        QFuture<void> fu = QtConcurrent::run(QThreadPool::globalInstance(), [&]()
        {   // Run functional annotation in another thread;
            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                            "\nComplete exonic SNP infomation,\n");
            QThread::msleep(10);
            if (!funcAnnotator.complExoSnpInfo(snpPosFile, exVarFuncFile, exonicPosFile))
            {
                throw -1;
            }

            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                            "\nOK\n\n" +
                                            QDateTime::currentDateTime().toString() +
                                            "\nComplete non-exonic SNP infomation,\n");
            QThread::msleep(10);
            if (!funcAnnotator.complNonExoSnpInfo(exonicPosFile, snpPosFile, varFuncFile, nonExonicPosFile))
            {
                throw -1;
            }

            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                            "\nComplete functional annotation infomation,\n");
            QThread::msleep(10);
            if (!funcAnnotator.complFuncAnnoInfo(exonicPosFile, nonExonicPosFile, baseFile, funcAnnoResult))
            {
                throw -1;
            }

            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                            "\nFunctional annotation OK\n"+
                                            "\n" + funcAnnoResult + "\n");
            QThread::msleep(10);
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
    this->runningFlag = false;
}

/**
 * @brief MainWindow::on_funcAnnoStepPushButton_clicked
 *          Read map file and p-value file, filter SNP and exstract postion of SNP.
 */
void MainWindow::on_funcAnnoStepPushButton_clicked()
{
    this->runningFlag = true;
    ui->funcAnnoStepPushButton->setEnabled(false);
    qApp->processEvents();

    QString mapFile = this->fileReader->getMapFile();
    try {
        if (mapFile.isNull() || mapFile.split(".").back() != "map")
        {
            QMessageBox::information(nullptr, "Error", "A .map file is necessary.");
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
        QFuture<void> fu = QtConcurrent::run(QThreadPool::globalInstance(), [&]()
        {
            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                            "\nFilter SNP above threshold,\n");
            QThread::msleep(10);
            if (!funcAnnotator.filterSNP(pvalFile, thBase, thExpo, sigSnpFile))
            {
                throw -1;
            }

            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                            "\nFilter SNP above threshold OK\n");
            QThread::msleep(10);
            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                            "\nExtract position of SNP,\n");
            QThread::msleep(10);
            if (!funcAnnotator.extractPos(sigSnpFile, mapFile, sigSnpPosFile))
            {
                throw -1;
            }

            emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                            "\nExtract position of SNP OK\n");
            QThread::msleep(10);
            if (runningFlag && checkoutExistence(sigSnpFile))
            {
//                ui->snpPosLineEdit->setText(sigSnpPosFile);
                emit setLineEditTextSig(ui->snpPosLineEdit, sigSnpPosFile);
                QThread::msleep(10);
            }
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
    this->runningFlag = false;
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
    if (this->runningFlag)
    {
        QMessageBox::information(nullptr, "Error", "A project is running now.");
        return;
    }
    this->runningFlag = true;
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

    QFuture<void> fu = QtConcurrent::run([&]()
    {
        emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                        "\nPlot PCA, \n");
        QThread::msleep(10);
        QStringList param;
        param.clear();
        param.append(this->scriptpath+"pca/pca_plot.R");    // Can choose pca_plot.R or pca_ggplot.R
        param.append(eigenvalFile);
        param.append(eigenvecFile);
        param.append(outFile);

        // R in environment path is necessary.
        if (!runExTool("Rscript", param))
        {
            return;
        }
        emit runningMsgWidgetAppendText(QDateTime::currentDateTime().toString() +
                                        "\nOK,\n" + outFile + "\n");
        QThread::msleep(10);
        // Show plot
        if (this->runningFlag && checkoutExistence(outFile))
        {
            emit setGraphViewerGraphSig(QStringList() << outFile);
            QThread::msleep(10);
        }
    });
    while (!fu.isFinished())
    {
        qApp->processEvents(QEventLoop::AllEvents, 200);
    }

    ui->pcaPlotPushButton->setEnabled(true);
    qApp->processEvents();
    this->runningFlag = false;
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
    if (!runExTool("Rscript", param))
    {
        return false;
    }

    return true;
}

void MainWindow::on_qualCtrlDetailPushButton_clicked()
{
    this->qualityControl->show();
}

bool MainWindow::runExTool(QString tool, QStringList param)
{
    Process *proc = new Process;

    // Read message form Process and display in RunningMsgWidget
    connect(proc, SIGNAL(outMessageReady(QString)), this, SLOT(on_outMessageReady(QString)));
    connect(proc, SIGNAL(errMessageReady(QString)), this, SLOT(on_errMessageReady(QString)));
    connect(this, SIGNAL(terminateProcess()), proc, SLOT(on_terminateProcess()), Qt::DirectConnection);

//     proc->execute(tool, param);
    proc->start(tool, param);
    if (!proc->waitForStarted())
    {
        QMessageBox::information(nullptr, "Error", "Can't open " + tool);
        return false;
    }
    proc->waitForFinished(-1);

    bool ret = true;
//    if (proc->exitCode() != Process::NormalExit)
//    {
//        emit runningMsgWidgetAppendText(proc->errorString());
//        ret = false;
//    }
    proc->close();
    delete proc;
    proc = nullptr;

    return ret;
}

bool MainWindow::checkoutExistence(QString filePath)
{
    QFile file(filePath);
    return file.exists();
}

void MainWindow::on_outMessageReady(const QString text)
{
    if (this->runningFlag)
    {
        this->runningMsgWidget->setText(this->refreshMessage(this->runningMsgWidget->getText(), text));
        this->runningMsgWidget->repaint();
        qApp->processEvents();
    }
}

void MainWindow::on_errMessageReady(const QString text)
{
    if (this->runningFlag)
    {
        this->runningMsgWidget->appendText(text);
        this->runningMsgWidget->repaint();
        qApp->processEvents();
    }
}

// Mutilple thread slot function.
void MainWindow::on_setLineEditText(QLineEdit *lineEdit, QString text)
{
    if (this->runningFlag)
    {
        lineEdit->setText(text);
    }
}

// Mutilple thread slot function.
void MainWindow::on_setButtonEnabled(QAbstractButton *button, bool boolean)
{
    button->setEnabled(boolean);
}

// Mutilple thread slot function.
void MainWindow::on_setGraphViewerGraph(QStringList plot)
{
    if (this->runningFlag)
    {
        this->graphViewer->setGraph(plot);
        this->graphViewer->show();
    }
}
