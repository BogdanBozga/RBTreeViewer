#include "mainwindow.h"
//#include "ui_mainwindow.h"
#include <QLabel>
#include <QScrollArea>
#include <QLayout>
#include <QPainter>
#include <QTimer>
#include <QFont>
#include <math.h>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
//    QTimer *timer = new QTimer(this);
////    connect(timer, &QTimer::timeout, this, QOverload<>::of(&MainWindow::update));
//    timer->start(1000);
//    setWindowTitle(tr("Analog Clock"));
//    resize(200, 200);

    this->redColor = QColor(255, 0, 0);
    this->blackColor = QColor(0,0,0);
    this->whiteColor = QColor(255,255,255);

    RBTree* RB = new RBTree();
    RB->RBInsert(RB->createNode(5));

    drawTree(RB);
//    setData(613,"BLACK",10,10);

}
void MainWindow::crossingFunction(RBNode *node_, QPoint coordonatePoint){
    if(node_ == RBNode::Nil){

    }else{
        int currentDepth = tree->depth(node_);
        QPoint leftPoint(coordonatePoint.x()-pow(2,currentDepth)*unitarUnit, coordonatePoint.y()+unitarUnit); // this only look like easy mathematic formula to create, it's not
        QPoint rightPoint(coordonatePoint.x()+pow(2,currentDepth)*unitarUnit, coordonatePoint.y()+unitarUnit);
        crossingFunction(node_->left,leftPoint);
        crossingFunction(node_->right,rightPoint);
        drawNode(node_,coordonatePoint);
    }

}

void MainWindow::drawNode(RBNode *node_,QPoint coodronate){
//    this->nodeHeight = tree->depth(node_);


    //QString::fromUtf8( node_->toString().data(), node_->toString().size() ); //********** conversion from std::string to QString, userd in next functin
    setData(node_->key,QString::fromUtf8( node_->toString().data(), node_->toString().size() ),coodronate.x(),coodronate.y());
    update();
}



void MainWindow::drawTree(RBTree *tree_){
    this->tree = tree_;
    this->treeHeight = tree_->depth(tree_->root);
    crossingFunction(tree_->root,QPoint(1920/2,10));
}


void MainWindow::setData(int valueNumber,QString color, int xCoordonate,int yCoordonate){

    QString numberQString = QString::number(valueNumber);
    std::string fname = numberQString.toStdString();
    this->number = new char [fname.size()+1];
    std::strcpy( this->number, fname.c_str());
    this->color = color;
    this->x = xCoordonate;
    this->y = yCoordonate;
//    update();
}

void MainWindow::paintEvent(QPaintEvent *)
{
    x = currentDrawingPoint.x();
    y = currentDrawingPoint.y();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QFont font = painter.font();
    font.setPixelSize(20);
    painter.setFont(font);



    QRect rectangle(x, y, width, height); // the big one for the circle (arc)
    QRect rectangleText((x+width/2-textWidth/2), (y+height/2-textHeight/2), textWidth, textHeight); // don't touch this formula, trust my

    if(color=="BLACK"){
        painter.setPen(blackColor);
    }else{
        painter.setPen(redColor);
    }
    painter.drawArc(rectangle, startAngle, spanAngle);


    painter.setPen(whiteColor);
    painter.drawText(rectangleText, Qt::AlignCenter, tr(number));
}



MainWindow::~MainWindow()
{
//    delete ui;
}

