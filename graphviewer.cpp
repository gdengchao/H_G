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

void GraphViewer::setGraph(QString graph)
{
    if(image->load(graph))
    {
        scene->addPixmap(QPixmap::fromImage(*image));
        ui->graphicsView->setScene(scene);
        ui->graphicsView->resize(image->width() + 10, image->height() + 10);
        this->adjustSize();

        QFileInfo graphInfo(graph);
        QString baseName = graphInfo.baseName();
        this->setWindowTitle(baseName);
        ui->graphicsView->show();

        this->curImage = graph;
    }
}

void GraphViewer::setGraph(QStringList graph)
{
    if(image->load(graph[0]))
    {
        scene->addPixmap(QPixmap::fromImage(*image));
        ui->graphicsView->setScene(scene);
        ui->graphicsView->resize(image->width() + 10, image->height() + 10);
        this->adjustSize();

        QFileInfo graphInfo(graph[0]);
        QString baseName = graphInfo.baseName();
        this->setWindowTitle(baseName);
        ui->graphicsView->show();

        imgList.clear();
        imgList.append(graph);
        curImage = graph[0];
    }
}

void GraphViewer::showEvent(QShowEvent *)
{
    ui->graphicsView->fitInView(scene->sceneRect(),Qt::KeepAspectRatio);
}

void GraphViewer::mousePressEvent(QMouseEvent *e)
{
    if(e->button()==Qt::LeftButton)
    {
        qDebug() << "Graph clicked" << endl;

        QStringList imgList = this->getImgList();
        QString curImage = this->getCurrentImg();
        int index = imgList.indexOf(curImage);
        if (index == -1)
        {
            return;
        }
        this->setGraph(imgList[(index+1)%imgList.length()]);

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
