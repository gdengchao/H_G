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
#include <RInside.h>
#include <iostream>
#include <cmath>
#include "filereader.h"
#include "workdirectory.h"
#include "phenoselector.h"
#include "useros.h"
#include "plink.h"
#include "emmax.h"
#include "gemma.h"
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

    // for WorkDirectory
    void on_browButton_clicked();
    void on_projectNameLineEdit_textChanged(const QString &text);

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

    void on_drawManPushButton_clicked();

    void on_drawQQPushButton_clicked();

private:
    Ui::MainWindow *ui;

    // Basic values.
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

    // Usual
    bool isVcfFile(QString const file);
    void resetWindow(void);
    bool callPlinkGwas(QString toolpath, QString phenotype, QString genotype, QString map,
                       QString covar, QString kinship, QString out, QString name);
    bool callEmmaxGwas(QString toolpath, QString phenotype, QString genotype, QString map,
                       QString covar, QString kinship, QString out, QString name);
    bool callGemmaGwas(QString toolpath, QString phenotype, QString genotype, QString map,
                       QString covar, QString kinship, QString out, QString name);
    bool makePheFile(QString const phenotype, QString const selectedPheno);
    QString refreshMessage(QString curMsg, QString newMsg);
    void drawManhattan(QString data, QString out);
    void drawQQplot(QString data, QString out);
};
#endif // MAINWINDOW_H
