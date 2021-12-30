#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "rbnode.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setData(int valueNumber, QString color, int xCoordonate, int yCoordonate);
protected:
    void paintEvent(QPaintEvent *event) override;
//    void paintEvent();
    char *number;
    int x;
    int y;
    QString color;
    RBTree *tree;
    RBNode *currentNode;

    QPoint  currentDrawingPoint;

protected:
    void drawNode(RBNode *, QPoint);
    void drawTree(RBTree *);
    void crossingFunction(RBNode *, QPoint);
private:
    int const startAngle = 0 * 16;
    int const spanAngle = 360 * 16;
    QColor redColor;
    QColor blackColor;
    QColor whiteColor;

    double const width = 55.0;
    double const height = 55.0;
    double const textHeight = 25; // specifical size for
    double const textWidth = 50;   // 4 decimal number maximum
    double const unitarUnit = 55.0;
    int treeHeight;
    int nodeHeight;


};
#endif // MAINWINDOW_H

