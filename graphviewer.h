#ifndef GRAPHVIEWER_H
#define GRAPHVIEWER_H

#include <QWidget>
#include <QPixmap>
#include <QGraphicsScene>

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

private:
    Ui::GraphViewer *ui;
    QImage *image;
    QGraphicsScene *scene;
};

#endif // GRAPHVIEWER_H
