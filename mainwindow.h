#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QFileDialog>
#include <QAbstractItemView>
#include <QProcess>
#include <QDebug>
#include <QList>
#include "filereader.h"
#include "workdirectory.h"
#include "phenoselector.h"
#include "useros.h"
#include "plink.h"
#include "emmax.h"
#include "gemma.h"
#include "runningmsgwidget.h"

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

    // for maf and ms
    void on_mafSlider_valueChanged(int value);
    void on_msSlider_valueChanged(int value);
    void on_msDoubleSpinBox_editingFinished();
    void on_mafDoubleSpinBox_editingFinished();

    // for running Message
    void on_readoutput();
    void on_readerror();
    void on_closeRunningWidget();

    // For tool and model selector
    void on_toolComboBox_currentTextChanged(const QString &tool);


private:
    Ui::MainWindow *ui;
    FileReader *fileReader;             // Read file, and save the directory.
    WorkDirectory *workDirectory;       // Set and save the module name and the output directory.
    PhenoSelector *phenoSelector;       // Select objective phenotype to analysis.

    // for display running information.
    RunningMsgWidget *runningMsgWidget;
    QProcess *process;

    // Usual
    bool isVcfFile(QString file);
    void resetWindow(void);
    bool callPlinkGwas(void);
    bool callEmmaxGwas(void);
    bool callGemmaGwas(void);
};
#endif // MAINWINDOW_H
