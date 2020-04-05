#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QFileDialog>
#include <QAbstractItemView>
#include <QProcess>
#include <QDebug>
#include <QList>
#include <QThread>
#include <QMap>
#include <QButtonGroup>
#include <RInside.h>
#include <iostream>
#include <cmath>
#include "mtoolbutton.h"
#include "filereader.h"
#include "workdirectory.h"
#include "phenoselector.h"
#include "useros.h"
#include "plink.h"
#include "emmax.h"
#include "gemma.h"
#include "gcta.h"
#include "poplddecay.h"
#include "runningmsgwidget.h"
#include "gemmaparamwidget.h"
#include "emmaxparamwidget.h"
#include "graphviewer.h"

using namespace Rcpp;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // for FileReader
    void on_pheFileToolButton_clicked();
    void on_genoFileToolButton_clicked();
    void on_mapFileToolButton_clicked();
    void on_covarFileToolButton_clicked();
    void on_kinFileToolButton_clicked();
    //  right click
    void on_pheFileToolButton_rightClicked();
    void on_genoFileToolButton_rightClicked();
    void on_mapFileToolButton_rightClicked();
    void on_kinFileToolButton_rightClicked();
    void on_covarFileToolButton_rightClicked();
    // for WorkDirectory
    void on_outdirBrowButton_clicked();
    void on_projectNameLineEdit_textChanged(const QString &text);
    void on_projectNameLineEdit_editingFinished();
    // for PhenoSelector
    void on_excludeAllPhenoButton_clicked();
    void on_selectAllPhenoButton_clicked();
    void on_selectPhenoButton_clicked();
    void on_excludePhenoButton_clicked();
    // for run pushButton
    void on_rungwasButton_clicked();
    // for running Message
    void on_readoutput();
    void on_readerror();
    void on_closeRunningWidget();
    // For tool and model selector
    void on_toolComboBox_currentTextChanged(const QString &tool);
    // Detail params.
    void on_detailPushButton_clicked();
    // Plot QQ_Manhattan
    void on_drawManPushButton_clicked();
    void on_drawQQPushButton_clicked();
    void on_gwasReultBrowButton_clicked();
    void on_pcaRunPushButton_clicked();

    void on_ldRunPushButton_clicked();

    void on_ldPlotPushButton_clicked();

    void on_ldReultBrowButton_clicked();

private:
    Ui::MainWindow *ui;

    //  Tool and script path;
    QString toolpath = "/home/chao/Documents/code/H_G/tools/";
    QString scriptpath = "/home/chao/Documents/code/H_G/script/";
    //QString toolpath = "tools/";  // Realease
    //QString scriptpath = "script/";

    // Basic association parameters.
    FileReader *fileReader;             // Read file, and save the directory.
    WorkDirectory *workDirectory;       // Set and save the module name and the output directory.
    PhenoSelector *phenoSelector;       // Select objective phenotype to analysis.
    // for display running information.
    RunningMsgWidget *runningMsgWidget;
    QProcess *process;
    // To set detail paramters
    GemmaParamWidget *gemmaParamWidget;
    EmmaxParamWidget *emmaxParamWidget;
    // Graph Viewer
    GraphViewer * graphViewer;
    // LD by family.
    QButtonGroup *ldByFamGroupButton;

    // Usual functions.
    bool isVcfFile(QString const file);
    void resetWindow(void);
    bool callPlinkGwas(QString phenotype, QString genotype, QString map,
                       QString covar, QString kinship, QString out, QString name);
    bool callEmmaxGwas(QString phenotype, QString genotype, QString map,
                       QString covar, QString kinship, QString out, QString name);
    bool callGemmaGwas(QString phenotype, QString genotype, QString map,
                       QString covar, QString kinship, QString out, QString name);
    bool makePheFile(QString const phenotype, QString const selectedPheno);
    QString refreshMessage(QString curMsg, QString newMsg);
    bool drawManhattan(QString data, QString out);
    bool drawQQplot(QString data, QString out);
    QString makeQQManInputFile(QString pvalueFile);
};
#endif // MAINWINDOW_H
