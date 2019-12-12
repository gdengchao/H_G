#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
