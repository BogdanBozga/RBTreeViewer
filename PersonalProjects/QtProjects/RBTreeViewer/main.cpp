#include <QApplication>
#include <QLabel>
#include <QScrollArea>
#include <QLayout>
#include <QPainter>
#include <QTimer>
#include <QFont>
#include <math.h>
#include <iostream>
#include <sstream>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QIntValidator>
#include <QThread>
#include <QPixmap>


struct RBNode
{
    int key;       // key
    RBNode *p;     // pointer to parent
    RBNode *left;  // pointer to left child
    RBNode *right; // pointer to right child
    enum color { RED, BLACK };
    color col;

    static RBNode* Nil; // sentinel leaf node

    RBNode(int k, RBNode *l = Nil, RBNode *r = Nil, RBNode *parent = Nil, color c = RBNode::BLACK) :
        key(k), p(parent), left(l), right(r), col(c) { }

    std::string toString()
    {
        std::ostringstream os;
//        os << key << ((col == RBNode::RED) ? ":r" : ":b");
        os <<((col == RBNode::RED) ? "RED" : "BLACK");
        return os.str();
    }

    ~RBNode()
    {
        if (this != Nil)
        {
            if(left != Nil)
                delete left;
            if(right != Nil)
                delete right;
        }
    }

};

 RBNode* RBNode::Nil = new RBNode(0);

struct RBTree
{
    RBNode* root;

    void LeftRotate(RBNode* x)
    {
        // Set y
        RBNode* y = x->right;
        // Turn y's left subtree into x's right subtree
        x->right = y->left;
        if (y->left != RBNode::Nil)
            y->left->p = x;
        // link x's parent to y
        y->p = x->p;
        if (x->p == RBNode::Nil)
            root = y;
        else if (x == x->p->left)
            x->p->left = y;
        else
            x->p->right = y;
        // Put x on y's left
        y->left = x;
        x->p = y;
    }

    /**
     * RightRotate(y) also assumes that y is the right child of
     * an RBNode x
     */

    void RightRotate(RBNode* y)
    {
        // Set x
        RBNode* x = y->left;
        // Turn x's right subtree into y's left subtree
        y->left = x->right;
        if (x->right != RBNode::Nil)
            x->right->p = y;
        // link y's parent to x
        x->p = y->p;
        if (y->p == RBNode::Nil)
            root = x;
        else if (y == y->p->left)
            y->p->left = x;
        else
            y->p->right = x;
        // Put y on x's right
        x->right = y;
        y->p = x;
    }

    RBTree()
    {
        root = RBNode::Nil;
    }
    ~RBTree()
    {
        delete root;
    }

    RBNode* createNode(int key)
    {
        return new RBNode(key);
    }

    bool isNil(RBNode* n)
    {
        return (n == RBNode::Nil);
    }

    RBNode* search(RBNode* w, int key)
    {
        if (isNil(w) || w->key == key)
            return w;
        return search( (key < w->key) ? w->left : w->right, key);
    }

    /**
     * maximum(w) returns the node with maximum key in the
     * RB-tree with root node w
     */
    RBNode* maximum(RBNode* w)
    {
        RBNode* x = w;
        while (!isNil(x->right))
            x = x->right;
        return x;
    }

    /**
     * minimum(w) returns the node with minimum key in the
     * RB-tree with root node w
     */
    RBNode* minimum(RBNode* w)
    {
        RBNode* x = w;
        while (!isNil(x->left))
            x = x->left;
        return x;
    }

    /**
     * successor(w) returns the successor of node w in the
     * RB-tree to which node w belongs
     */
    RBNode* successor(RBNode* w)
    {
        if(isNil(w)) return w;
        RBNode* x = w;
        if (!isNil(x->right))
            return minimum(x->right);
        RBNode* y = x->p;
        while (!isNil(y) && x == y->right)
        {
            x = y;
            y = x->p;
        }
        return y;
    }

    RBNode* predecessor(RBNode* w)
    {
        if(isNil(w)) return w;
        RBNode* x = w;
        if (!isNil(x->left))
            return maximum(x->left);
        RBNode* y = x->p;
        while (!isNil(y) && x == y->left)
        {
            x = y;
            y = x->p;
        }
        return y;
    }


    /**
     * RBInsert(z) inserts node z in this RB-tree
     */
    void RBInsert(RBNode* z)
    {
        // insert like in a binary search tree
        RBNode *y = RBNode::Nil;
        RBNode *x = root;
        while (!isNil(x))
        {
            y = x;
            x = (z->key < x->key) ? x->left : x->right;
        }
        z->p = y;
        if (isNil(y))
            root = z;
        else if (z->key < y->key)
            y->left = z;
        else
            y->right = z;
        z->left = z->right = RBNode::Nil;
        z->col = RBNode::RED;
        RBInsertFixup(z);
    }

    RBNode* del(RBNode* z)
    {
        RBNode *y = (isNil(z->left) || isNil(z->right)) ? z : successor(z);
        RBNode *x = !isNil(y->left) ? y->left : y->right;
        x->p = y->p;
        if(isNil(y->p))
        {
            root = x;
        }
        else
        {
            if (y == y->p->left)
                y->p->left = x;
            else
                y->p->right = x;
        }
        if (y != z)
        {
            z->key = y->key;
            // copy y's satellite data into z
        }
        if (y->col == RBNode::BLACK)
            RBDeleteFixup(x);
        return y;
    }

    void RBDeleteFixup(RBNode* x)
    {
        RBNode* w;
        while ((x != root) && (x->col == RBNode::BLACK))
        {
            if (x == x->p->left)
            {
                w = x->p->right;
                if(w->col == RBNode::RED)
                {
                    w->col = RBNode::BLACK;
                    x->p->col = RBNode::RED;
                    LeftRotate(x->p);
                    w = x->p->right;
                }
                if((w->left->col == RBNode::BLACK) && (w->right->col == RBNode::BLACK))
                {
                    w->col = RBNode::RED;
                    x = x->p;
                }
                else
                {
                    if(w->right->col == RBNode::BLACK)
                    {
                        w->left->col = RBNode::BLACK;
                        w->col = RBNode::RED;
                        RightRotate(w);
                        w = x->p->right;
                    }
                    w->col = x->p->col;
                    x->p->col = RBNode::BLACK;
                    w->right->col = RBNode::BLACK;
                    LeftRotate(x->p);
                    x = root;
                }
            }
            else
            {
                // same as 'then' clause with 'right' and 'left' exchanged
                w = x->p->left;
                if(w->col == RBNode::RED)
                {
                    w->col = RBNode::BLACK;
                    x->p->col = RBNode::RED;
                    RightRotate(x->p);
                    w = x->p->left;
                }
                if((w->left->col == RBNode::BLACK) && (w->right->col == RBNode::BLACK))
                {
                    w->col = RBNode::RED;
                    x = x->p;
                }
                else
                {
                    if(w->left->col == RBNode::BLACK)
                    {
                        w->right->col = RBNode::BLACK;
                        w->col = RBNode::RED;
                        LeftRotate(w);
                        w = x->p->left;
                    }
                    w->col = x->p->col;
                    x->p->col = RBNode::BLACK;
                    w->left->col = RBNode::BLACK;
                    RightRotate(x->p);
                    x = root;
                }
            }
        }
        x->col = RBNode::BLACK;
    }

    void RBInsertFixup(RBNode* z)
    {
        while(z->p->col == RBNode::RED)
            if (z->p == z->p->p->left)
            {
                RBNode* y = z->p->p->right;
                if (y->col == RBNode::RED)
                {
                    z->p->col = RBNode::BLACK;
                    y->col = RBNode::BLACK;
                    z->p->p->col = RBNode::RED;
                }
                else
                {
                    if (z==z->p->right)
                    {
                        z = z->p;
                        LeftRotate(z);
                    }
                    z->p->col = RBNode::BLACK;
                    z->p->p->col = RBNode::RED;
                    RightRotate(z->p->p);
                }
            }
            else
            {
                /* same as 'then' clause,
                 * with 'left' and 'right' exchanged */
                RBNode* y = z->p->p->left;
                if (y->col == RBNode::RED)
                {
                    z->p->col = RBNode::BLACK;
                    y->col = RBNode::BLACK;
                    z->p->p->col = RBNode::RED;
                }
                else
                {
                    if (z==z->p->left)
                    {
                        z = z->p;
                        RightRotate(z);
                    }
                    z->p->col = RBNode::BLACK;
                    z->p->p->col = RBNode::RED;
                    LeftRotate(z->p->p);
                }
            }
        root->col = RBNode::BLACK;

    }

    void inorder(RBNode* T)
    {
        if (!isNil(T))
        {
            inorder(T->left);
            std::cout << T->toString() << ' ';
            inorder(T->right);
        }
    }

    void inorder()
    {
        if (isNil(root))
        {
            std::cout << "empty";
        }
        else
            inorder(root);
    }

    void display(RBNode* w, int indent)
    {
        if (!isNil(w))
        {
            display(w->right, indent + 2);
            for (int i = 0; i < indent; i++) std::cout << ' ';
            std::cout << w->toString() << '\n';
            display(w->left, indent + 2);
        }
    }

    void indentedDisplay()
    {
        if (isNil(root))
            std::cout << "The RB tree is empty" <<std::endl;
        else
        {
            std::cout << "The RB tree is" <<std::endl;
            display(root, 0);
        }
    }

    // auxiliary method
    int bh(RBNode* x)
    {
        return (isNil(x) ? 0 : countBlacks(x->left));
    }

    // auxiliary method
    int countBlacks(RBNode* x)
    {
        if(isNil(x)) return 1;
        int n = countBlacks(x->left);
        return ((x->col == RBNode::RED)? n : n+1);
    }

    /**
     * Compute the black-height of this red-black tree
     */
    int bh()
    {
        int bh = 0;
        RBNode *aux = this->root;
        while ( !isNil(aux)){
            aux = aux ->left;
            if(aux->col == RBNode::BLACK)
                bh++;
        }
        return bh;
    }

    // compute the depth of the tree with root *x
    int depth(RBNode* x)
    {
        if (isNil(x))
        {
            return -1;
        }
        else
        {
            int lDepth = depth(x->left);
            int rDepth = depth(x->right);

            return (lDepth<rDepth ? rDepth : lDepth)+1;
        }
    }

    /**
     * Compute the depth of this red-black tree
     */
    int depth()
    {
        return depth(root);
    }

    /* return the maximum key of black nodes.
     * If the tree is empty, return -1000 */
    int maxBlackKey()
    {
        int maxBlack = -1000;
        RBNode *nod = this->root;
        while (!isNil(nod)){
            if(nod->col == RBNode::BLACK)
                maxBlack = ((nod->key >maxBlack)? nod->key : maxBlack);
            nod= nod->right;
        }
        return maxBlack;
    }

    /* return the maximum key of red nodes.
     * If the tree has no red nodes, return -1000 */
    int maxRedKey()
    {
        int maxRed = -1000;
        RBNode *nod = this->root;
        while (!isNil(nod)){
            if(nod->col == RBNode::RED)
                maxRed = ((nod->key >maxRed)? nod->key : maxRed);
            nod= nod->right;
        }
        return maxRed;
    }
};


class MainWindow : public QWidget
{
//    Q_OBJECT
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
    char *number;
    int x;
    int y;
    QString color;
    RBTree *tree;
    RBNode *currentNode;
    QPoint  currentDrawingPoint;

    QWidget *window;
    QPushButton *insertButton;
    QPushButton *deleteButton;
    QPushButton *searchButton;
    QLineEdit *textEdit;
    QPixmap drawPixmap;
    QVBoxLayout *l;
    QLabel *label;
//    QPainter painter;
public slots:
    void insertNode(){
        tree->RBInsert(tree->createNode(textEdit->text().toInt()));
        drawTree();
    }
    void deleteNode(){
        currentNode = tree->search(tree->root, textEdit->text().toInt());
        if (!tree->isNil(currentNode)) tree->del(currentNode);
        else std::cout << "RB: Node not found" << std::endl;
        drawTree();
    }
    void searchNode(){
        currentNode = tree->search(tree->root, textEdit->text().toInt());
    }


public:
    MainWindow(QWidget *parent = nullptr){
//        QTimer *timer = new QTimer(this);
//    //    connect(timer, &QTimer::timeout, this, QOverload<>::of(&MainWindow::update));
////        timer->start(1000);
////        setWindowTitle(tr("Analog Clock"));
        resize(1000, 1000);
        drawPixmap = QPixmap(1000,1000);
        drawPixmap.fill(Qt::green);
        l = new QVBoxLayout(this);
        label = new QLabel(this);
        label->setStyleSheet("QLabel { background-color : red; color : blue; }");
        label->resize(1000,1000);
        label->setBackgroundRole(QPalette::Light);
        label->setPixmap(drawPixmap);
//        l->add (label);

        tree = new RBTree();
            insertButton = new QPushButton;
            deleteButton = new QPushButton;
            searchButton = new QPushButton;
            textEdit = new QLineEdit;
            textEdit->setValidator( new QIntValidator(0, 9999, this));
            textEdit->setMinimumSize(50,40);
            textEdit->setMaximumSize(50,40);
            insertButton->setText("Insert");
            deleteButton->setText("Delete");
            searchButton->setText("Search");
            QObject::connect(insertButton,&QPushButton::clicked,this,&MainWindow::insertNode);
            QObject::connect(deleteButton,&QPushButton::clicked,this,&MainWindow::deleteNode);
            QObject::connect(searchButton,&QPushButton::clicked,this,&MainWindow::searchNode);

            window = new QWidget;
            window->resize(70, 80);
            window->setWindowTitle("Remote");
            QVBoxLayout *layout = new QVBoxLayout(window);
            layout->addWidget(textEdit);
            layout->addWidget(insertButton);
            layout->addWidget(deleteButton);
            layout->addWidget(searchButton);
            window->show();

        this->redColor = QColor(255, 0, 0);
        this->blackColor = QColor(0,0,0);
        this->whiteColor = QColor(255,255,255);
//        painter = new QPainter();
//        painter.begin(this);
//        QPainter Painter(this);
//        painter.
//        painter.setRenderHint(QPainter::Antialiasing);
//        QFont font = painter.font();
//        font.setPixelSize(20);
//        painter.setFont(font);

    }


    void setData(int valueNumber, QString color, int xCoordonate, int yCoordonate){
        QString numberQString = QString::number(valueNumber);
        std::string fname = numberQString.toStdString();
        this->number = new char [fname.size()+1];
        std::strcpy( this->number, fname.c_str());
        this->color = color;
        this->x = xCoordonate;
        this->y = yCoordonate;
        qWarning() <<"##########starting in setdata";
        qWarning() << number;
        qWarning() << color;
        qWarning() << x;
        qWarning() << y;
        qWarning() << "###############end in setdata";





    //    update();
    }
protected:
    void paintEvent(QPaintEvent *event) override{
//        x = currentDrawingPoint.x();
//        y = currentDrawingPoint.y();
        label->clear();
        QPainter painter(&drawPixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        QFont font = painter.font();
        font.setPixelSize(20);
        painter.setFont(font);

        QRect rectangle(x, y, width, height); // the big one for the circle (arc)
        QRect rectangleText((x+width/2-textWidth/2), (y+height/2-textHeight/2), textWidth, textHeight); // don't touch this formula, trust my
//        qWarning()<<color;
        if(color=="BLACK"){
            painter.setPen(blackColor);
        }else if(color == "RED"){
            painter.setPen(redColor);
        }
        painter.drawArc(rectangle, startAngle, spanAngle);
        painter.setPen(whiteColor);

        painter.drawText(rectangleText, Qt::AlignCenter, tr(number));
        label->setPixmap(drawPixmap);
    }



protected:
    void drawNode(RBNode *node_, QPoint coodronate){
        //    this->nodeHeight = tree->depth(node_);
            //QString::fromUtf8( node_->toString().data(), node_->toString().size() ); //********** conversion from std::string to QString, userd in next functin
            setData(node_->key,QString::fromUtf8( node_->toString().data(), node_->toString().size() ),coodronate.x(),coodronate.y());
//            qWarning() <<
            update();
//            QThread::sleep(1);
        }


    void drawTree(){
        qWarning() << "--------------";
//        this->tree = tree_;
        this->treeHeight = tree->depth(tree->root);
        crossingFunction(tree->root,QPoint(QWidget::width()/2,10));
    }


    void crossingFunction(RBNode *node_, QPoint coordonatePoint){

        if(node_ == RBNode::Nil){
            qWarning() << "null ";
        }else{
//            qWarning() << node_->key;
//            int currentDepth = tree->depth(node_);
            if(!(node_->left==RBNode::Nil)){
                QPoint leftPoint(coordonatePoint.x()-pow(2,tree->depth(node_->left))*unitarUnit, coordonatePoint.y()+unitarUnit); // this only look like easy mathematic formula to create, it's not
                crossingFunction(node_->left,leftPoint);
            }

            if(!(node_->right==RBNode::Nil)){
                QPoint rightPoint(coordonatePoint.x()+pow(2,tree->depth(node_->right))*unitarUnit, coordonatePoint.y()+unitarUnit);
                crossingFunction(node_->right,rightPoint);
            }
            //            QPoint leftPoint(coordonatePoint.x()-pow(2,tree->depth(node_->left))*unitarUnit, coordonatePoint.y()+unitarUnit); // this only look like easy mathematic formula to create, it's not
//                        QPoint rightPoint(coordonatePoint.x()+pow(2,currentDepth)*unitarUnit, coordonatePoint.y()+unitarUnit);
            //            crossingFunction(node_->left,leftPoint);
//                        crossingFunction(node_->right,rightPoint);
            qWarning() << node_->key;
            qWarning() << tree->depth(node_);
            qWarning()<< coordonatePoint;
            drawNode(node_,coordonatePoint);
        }
    }
};



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
