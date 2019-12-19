#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "filereader.h"
#include "workdirectory.h"
#include "phenoselector.h"

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
    void on_pheFileToolButton_clicked();
    void on_genoFileToolButton_clicked();
    void on_mapFileToolButton_clicked();
    void on_covarFileToolButton_clicked();
    void on_kinFileToolButton_clicked();
    void on_browButton_clicked();
    void on_modulenameLineEdit_textChanged(const QString &text);
    void on_excludeAllPhenoButton_clicked();

    void on_selectAllPhenoButton_clicked();

private:
    Ui::MainWindow *ui;
    FileReader *fileReader;             // Read file, and save the directory.
    WorkDirectory *workDirectory;       // Set and save the module name and the output directory.
    PhenoSelector *phenoSelector;
};
#endif // MAINWINDOW_H
