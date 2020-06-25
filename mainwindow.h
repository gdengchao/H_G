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
#include <QDateTime>
#include <QThread>
#include <QTimer>
#include <QtConcurrent>
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
#include "annovar.h"
#include "runningmsgwidget.h"
#include "gemmaparamwidget.h"
#include "emmaxparamwidget.h"
#include "graphviewer.h"
#include "funcannotator.h"

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
    void on_pheFileToolButton_closeFileSig();
    void on_genoFileToolButton_closeFileSig();
    void on_mapFileToolButton_closeFileSig();
    void on_kinFileToolButton_closeFileSig();
    void on_covarFileToolButton_closeFileSig();
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
    void on_runGwasButton_clicked();
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
    void on_qqmanGwasReultBrowButton_clicked();
    void on_pcaRunPushButton_clicked();
    // Linkage disequilibrium
    void on_ldRunPushButton_clicked();
    void on_ldPlotPushButton_clicked();
    void on_ldReultBrowButton_clicked();
    // Graph viewer
    void graphViewer_clicked_slot();    // on_*** is implicit slot function
    // Structural annotation
    void on_gffFileBrowButton_clicked();
    void on_strucAnnoRunPushButton_clicked();
    void on_fastaFileBrowButton_clicked();
    void on_avinFileBrowButton_clicked();
    // Functional annotation
    void on_snpPosBrowButton_clicked();
    void on_baseFileBrowButton_clicked();
    void on_varFuncFileBrowButton_clicked();
    void on_exVarFuncFileBrowButton_clicked();
    void on_funcAnnoRunPushButton_clicked();
    void on_funcAnnoStepPushButton_clicked();
    void on_annoPvalBrowButton_clicked();

    // Timer
    void timer_timeout_slot();

private:
    Ui::MainWindow *ui;
//    QThread workThread;
    void closeEvent(QCloseEvent *event) override;
//    bool eventFilter(QObject *obj, QEvent *ev) override;
    //  Tool and script path;
    QString toolpath = "/home/dengchao/Documents/code/H_G/tools/";
    QString scriptpath = "/home/dengchao/Documents/code/H_G/script/";
//    QString toolpath = "tools/";  // Realease
//    QString scriptpath = "script/";

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
    bool drawManhattan(QStringList data, QStringList out);
    bool drawQQplot(QStringList data, QStringList out);
    QStringList makeQQManInputFile(QString pvalueFile);
    void runPopLDdecaySingle(void);
    void runPopLDdecaybyFamily(void);
};
#endif // MAINWINDOW_H
