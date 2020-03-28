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
    }
}
