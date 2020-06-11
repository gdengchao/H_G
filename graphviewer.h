#ifndef GRAPHVIEWER_H
#define GRAPHVIEWER_H

#include <QWidget>
#include <QPixmap>
#include <QGraphicsScene>
#include <QFileInfo>
#include <QMouseEvent>
#include <QDebug>

namespace Ui {
class GraphViewer;
}

class GraphViewer : public QWidget
{
    Q_OBJECT

public:
    explicit GraphViewer(QWidget *parent = nullptr);
    ~GraphViewer();

    void setGraph(QStringList graph);
    void setImgList(QStringList imgList);
    QStringList getImgList(void);
    QString getCurrentImg(void);

private:
    void loopChangeGraphInList(void);
//    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
//    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
//    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
//    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
//    void zoom(float scaleFactor);
//    void zoomIn();
//    void zoomOut();
//    void translate(QPointF delta);
//    void setTranslateSpeed(qreal speed);
//    void setZoomDelta(qreal delta);

//    Qt::MouseButton translateButton;
//    qreal translateSpeed;
//    bool isMouseTranslate;
//    QPoint lastMousePos;
//    qreal zoomDelta;
//    qreal scaleData;

protected slots:
    void mouseDoubleClickEvent(QMouseEvent *e) override;

signals:
    void doubleClicked();

private:
    Ui::GraphViewer *ui;
    QImage *image;
    QGraphicsScene *scene;
    QGraphicsView* view;
    QStringList imgList;
    QString curImage;
    void showEvent(QShowEvent *) override;
};

#endif // GRAPHVIEWER_H
