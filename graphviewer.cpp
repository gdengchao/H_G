#include "graphviewer.h"
#include "ui_graphviewer.h"

GraphViewer::GraphViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphViewer)
{
    ui->setupUi(this);

    image = new QImage;
    scene = new QGraphicsScene;
}

GraphViewer::~GraphViewer()
{
    delete ui;
    delete image;
    delete scene;
}

void GraphViewer::loopChangeGraphInList(void)
{
    int index = imgList.indexOf(this->getCurrentImg());
    if (imgList.length() <= 1 || index == -1)
    {
        return;
    }
    QString graph = imgList[(index+1)%imgList.length()];
    if(image->load(graph))
    {
        scene->clear();
        scene->addPixmap(QPixmap::fromImage(*image));
        ui->graphicsView->resize(image->width() + 10, image->height() + 10);
        ui->graphicsView->setScene(scene);
        //this->resize(image->width() + 15, image->height() + 15);
        updateGeometry();
        this->resize(image->width() + 10, image->height() + 10);
        qApp->processEvents();

        QFileInfo graphInfo(graph);
        QString baseName = graphInfo.baseName();
        this->setWindowTitle(baseName);
        ui->graphicsView->show();

        this->curImage = graph;
    }
}

// graph must in imgList.
void GraphViewer::setGraph(QStringList graph)
{
    if(image->load(graph[0]))
    {
        scene->clear();
        scene->addPixmap(QPixmap::fromImage(*image));
        ui->graphicsView->setScene(scene);
        ui->graphicsView->resize(image->width() + 10, image->height() + 10);
        this->adjustSize();

        QFileInfo graphInfo(graph[0]);
        QString baseName = graphInfo.baseName();
        this->setWindowTitle(baseName);
        ui->graphicsView->show();

        this->imgList.clear();
        imgList.append(graph);
        curImage = graph[0];
    }
}

void GraphViewer::showEvent(QShowEvent *)
{
    ui->graphicsView->fitInView(scene->sceneRect(),Qt::KeepAspectRatio);
}

void GraphViewer::mouseDoubleClickEvent(QMouseEvent *e)
{
    if(e->button()==Qt::LeftButton)
    {
        qDebug() << "Graph clicked" << endl;

        this->loopChangeGraphInList();

        qDebug() << "imgList: " << imgList << endl;
        qDebug() << "curImage: " << curImage << endl;

        emit clicked();
    }
}

QStringList GraphViewer::getImgList(void)
{
    return this->imgList;
}

QString GraphViewer::getCurrentImg(void)
{
    return curImage;
}

void GraphViewer::setImgList(QStringList imgList)
{
    this->imgList.clear();
    this->imgList.append(imgList);
}
