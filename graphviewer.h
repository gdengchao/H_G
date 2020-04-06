#ifndef GRAPHVIEWER_H
#define GRAPHVIEWER_H

#include <QWidget>
#include <QPixmap>
#include <QGraphicsScene>
#include <QFileInfo>

namespace Ui {
class GraphViewer;
}

class GraphViewer : public QWidget
{
    Q_OBJECT

public:
    explicit GraphViewer(QWidget *parent = nullptr);
    ~GraphViewer();

    void setGraph(QString graph);
    void setGraph(QStringList graph);

private:
    Ui::GraphViewer *ui;
    QImage *image;
    QGraphicsScene *scene;

    void showEvent(QShowEvent *);
};

#endif // GRAPHVIEWER_H
