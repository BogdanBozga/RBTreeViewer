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
//#include <QThread>
#include <QPixmap>
#include <QMainWindow>
#include <QCloseEvent>

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
    QList<QString> specialOperationList;


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

//                    qWarning() << "Left rotation";
                    specialOperationList.append("Left rotation");
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
//                        qWarning() << "Right rotation";
                        specialOperationList.append("Right rotation");
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
//                    qWarning() << "Parent and uncle red";
                    specialOperationList.append("Parent and uncle red");
                    z->p->col = RBNode::BLACK;
                    y->col = RBNode::BLACK;
                    z->p->p->col = RBNode::RED;
                }
                else
                {
                    if (z==z->p->right)
                    {
                        z = z->p;
//                        qWarning() << "Left rotation";
                        specialOperationList.append("Left rotation");
                        LeftRotate(z);
                    }
//                    qWarning() << "Coloring parent in black and grandfather in red";
                    specialOperationList.append("Coloring parent in black and grandfather in red");
                    z->p->col = RBNode::BLACK;
                    z->p->p->col = RBNode::RED;
//                    qWarning() << "Right rotation";
                    specialOperationList.append("Right rotation");
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
//                    qWarning() << "Parent and uncle red";
                    specialOperationList.append("Parent and uncle red");
                    z->p->col = RBNode::BLACK;
                    y->col = RBNode::BLACK;
                    z->p->p->col = RBNode::RED;
                }
                else
                {
                    if (z==z->p->left)
                    {
                        z = z->p;
//                        qWarning() << "Right rotation";
                        specialOperationList.append("Right rotation");
                        RightRotate(z);
                    }
//                    qWarning() << "Recoloreting parent and grandfather";
                    specialOperationList.append("Coloring parent and grandfather");
                    z->p->col = RBNode::BLACK;
                    z->p->p->col = RBNode::RED;
//                    qWarning() << "Left rotation";
                    specialOperationList.append("Left rotation");
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




struct LinkedList{
    char * value;
    int x;
    int y;
    int parent_x;//used for drawing the connecting lines
    int parent_y;
    QString color;
    LinkedList *next;
    LinkedList(){
        next = NULL;
    }

    void push(LinkedList** head, int value_, int x_, int y_, QString color_,int parent_x_, int parent_y_){
//        qWarning() << value_;
        LinkedList *new_node = new LinkedList;
        QString numberQString = QString::number(value_);
        std::string fname = numberQString.toStdString();
        new_node->value = new char [fname.size()+1];
        std::strcpy( new_node->value, fname.c_str());
        new_node->x = x_;
        new_node->y = y_;
        parent_x = parent_x_;
        parent_y = parent_y_;
        new_node->color = color_;
        new_node->next = (*head);
        (*head) = new_node;
    }
};





class MainWindow : public QWidget
{
//    Q_OBJECT
//public:
//    bool pathTree = false;

private:
    int const startAngle = 0 * 16;
    int const spanAngle = 360 * 16;
    QColor redColor;
    QColor blackColor;
    QColor greenColor;
    QColor blueColor;
    QColor specialColor;

    double const width = 55.0;
    double const height = 55.0;
    double const textHeight = 25; // specifical size for
    double const textWidth = 50;   // 4 decimal number maximum
    double const unitarUnit = 55.0;
    bool drawPrepare = false; //
    int treeHeight;

//    QPoint currentNodePoint;

    bool pureTree;
    bool alreadyExist = false;

    QList<QPoint> searchPath;
    QPoint currentNodePoint;

    LinkedList *auxList;
    RBTree *tree;
//    RBNode *currentNode;
    QPoint  currentDrawingPoint;
    QWidget *window;
    QPushButton *insertButton;
    QPushButton *deleteButton;
    QPushButton *searchButton;
    QPushButton *nextButton;
    QLineEdit *textEdit;
    QPixmap drawPixmap;
    QVBoxLayout *l;
    QLabel *label;
    QScrollArea *scrollArea;


    int nodeValue;
    bool specialNode=false;
    int wantedOperation;
        //0 - search
        //1 - insert
        //2 - delete

//    bool specialOperation=false;

private:
    void deleteNodeFuntion(){
        specialOperationList =  QList<QString>();
        RBNode *currentNode = tree->search(tree->root, textEdit->text().toInt());
        if (!tree->isNil(currentNode)) tree->del(currentNode);
        else qWarning() << "RB: Node not found";
        drawOnlyTree();
    }


    void insertNodeFuntion(){
        specialOperationList = QList<QString>();
        if(!alreadyExist)
            tree->RBInsert(tree->createNode(nodeValue));
        drawOnlyTree();
    }


public slots:
    void drawNextSlot(){
            if(!searchPath.isEmpty()){
                createNodeLinkedList();
                currentNodePoint = searchPath[0];
                searchPath.removeFirst();
                if(searchPath.isEmpty()){
                    // this means that we find what we search for
                    //change color for rentangle to anounce that
                    specialNode = true;
                }

                drawPrepare = true;
                update();

            }else{
                switch (wantedOperation) {
                    case 0:  // search
                        // you aready draw the search tree
                        // this is the echivalate with the no action
                        drawOnlyTree();
                        break;
                    case 1:  // insert
                        createNodeLinkedList();
                        insertNodeFuntion();
                        wantedOperation = 0;
                        break;
                    case 2:  // delete
                        createNodeLinkedList();
                        deleteNodeFuntion();
                        wantedOperation = 0;
                        break;
                    default:
                        qWarning() << "what the hell";
                }
            }
    }


    void insertNodeSlot(){
//        specialOperation = new QList<QString>;
//        qWarning() << "--";
        wantedOperation = 1;
        searchNodeSlot();
    }


    void deleteNodeSlot(){
//        qWarning() << "-d-";
        wantedOperation = 2;
        searchNodeSlot();
    }


    void searchNodeSlot(){

        nodeValue = textEdit->text().toInt();
        pureTree = false;
        alreadyExist = false;
        RBNode *auxNode = tree->root;

        searchPath.clear();
        QPoint currentNodePoint = QPoint(drawPixmap.width()/2,10);
        searchPath.append(currentNodePoint);
        if(tree->root != RBNode::Nil){
            while (auxNode != RBNode::Nil) {
                if(nodeValue < auxNode->key){
                    if(auxNode->left == RBNode::Nil){
                       currentNodePoint = QPoint(currentNodePoint.x()-unitarUnit, currentNodePoint.y()+unitarUnit);
                       searchPath.append(currentNodePoint);
                        break;
                    }else{
                        currentNodePoint = QPoint(currentNodePoint.x()-pow(2,tree->depth(auxNode->left))*unitarUnit, currentNodePoint.y()+unitarUnit);
                        searchPath.append(currentNodePoint);
                    }
                    auxNode = auxNode->left;
                }else if(nodeValue > auxNode->key){
                    if(auxNode->right == RBNode::Nil){
                        currentNodePoint = QPoint(currentNodePoint.x()+unitarUnit, currentNodePoint.y()+unitarUnit);
                        searchPath.append(currentNodePoint);
                        break;
                    }else{
                         currentNodePoint = QPoint(currentNodePoint.x()+pow(2,tree->depth(auxNode->right))*unitarUnit, currentNodePoint.y()+unitarUnit);
                         searchPath.append(currentNodePoint);
                    }
                    auxNode = auxNode->right;
                }else{
                    alreadyExist = true;
                    break;
                }
            }
            drawNextSlot();
        }else{
            insertNodeFuntion();
        }
    }

public:
    MainWindow(QWidget *parent = nullptr){
        resize(200,180);
        drawPixmap = QPixmap(2000,1000);
        drawPixmap.fill(Qt::gray);
        l = new QVBoxLayout(this);
        label = new QLabel(this);
        label->setStyleSheet("QLabel { background-color : red; color : blue; }");
        label->resize(drawPixmap.width(),drawPixmap.height());
        label->setBackgroundRole(QPalette::Light);
        label->setPixmap(drawPixmap);


        scrollArea = new QScrollArea;
        scrollArea->resize(drawPixmap.width(), drawPixmap.height());
        scrollArea->setBackgroundRole(QPalette::Dark);
        scrollArea->setWidget(label);
        scrollArea->show();

        tree = new RBTree();

        textEdit = new QLineEdit;
        textEdit->setValidator( new QIntValidator(0, 9999, this));
        textEdit->setAlignment(Qt::AlignCenter);
        textEdit->setMinimumSize(60,40);
        textEdit->setMaximumSize(70,40);

        insertButton = new QPushButton;
        deleteButton = new QPushButton;
        searchButton = new QPushButton;
        nextButton = new QPushButton;


        insertButton->setText("Insert");
        deleteButton->setText("Delete");
        searchButton->setText("Search");
        nextButton->setText("Next");

        QObject::connect(insertButton,&QPushButton::clicked,this,&MainWindow::insertNodeSlot);
        QObject::connect(deleteButton,&QPushButton::clicked,this,&MainWindow::deleteNodeSlot);
        QObject::connect(searchButton,&QPushButton::clicked,this,&MainWindow::searchNodeSlot);
        QObject::connect(nextButton,&QPushButton::clicked,this,&MainWindow::drawNextSlot);

        window = new QWidget;
        window->resize(200, 200);
        window->setWindowTitle("Remote");
        QVBoxLayout *layout = new QVBoxLayout(window);
        layout->addWidget(textEdit);
        layout->addWidget(nextButton);
        layout->addWidget(insertButton);
        layout->addWidget(deleteButton);
        layout->addWidget(searchButton);

        window->setParent(this);


        this->redColor = QColor(255, 0, 0);
        this->blackColor = QColor(0,0,0);
        this->greenColor = QColor(0,100,0);
        this->blueColor = QColor(0,0,255);
        this->specialColor = QColor(200,100,0);
    }



    void setData(int valueNumber, QString color, int xCoordonate, int yCoordonate,  int parent_x, int parent_y){
        auxList->push(&auxList, valueNumber, xCoordonate, yCoordonate, color,parent_x,parent_y);
        drawPrepare = true;
        update();
    }

protected:
    void paintEvent(QPaintEvent *event) override{

        if(drawPrepare){
            label->clear();
            drawPixmap = QPixmap(2000,1000);
            drawPixmap.fill(Qt::gray);
            QPainter painter(&drawPixmap);
            painter.setRenderHint(QPainter::Antialiasing);
            QFont font = painter.font();
            font.setPixelSize(20);
            painter.setFont(font);

            while(auxList->next != NULL){
                int x = auxList->x;
                int y = auxList->y;
                char *number = auxList->value;
                QString color = auxList->color;

                LinkedList *d = auxList;
                free(d);
                auxList = auxList->next;

                QRect rectangle(x, y, width, height); // the big one for the circle (arc)
                QRect rectangleText((x+width/2-textWidth/2), (y+height/2-textHeight/2), textWidth, textHeight); // don't touch this formula, trust my

                if(color=="BLACK"){
                    painter.setPen(blackColor);
                }else if(color == "RED"){
                    painter.setPen(redColor);
                }

                painter.drawArc(rectangle, startAngle, spanAngle);
                painter.setPen(greenColor);

                painter.drawText(rectangleText, Qt::AlignCenter, tr(number));

                if(auxList->parent_x != -1){

                    double parentLineDraw_x = auxList->parent_x+unitarUnit/2;
                    double parentLineDraw_y = auxList->parent_y+unitarUnit;

                    double childLineDraw_x = x+unitarUnit/2;
                    double childLineDraw_y = y;

                    painter.drawLine(parentLineDraw_x,parentLineDraw_y,childLineDraw_x,childLineDraw_y);
                }


            }



            if(!pureTree){
                painter.setPen(blueColor);


                QRect rectangle(drawPixmap.width()/2-5*unitarUnit, 10, width, height); // the big one for the circle (arc)
                painter.drawArc(rectangle, startAngle, spanAngle);
                QRect rectangleText((rectangle.x()+width/2-textWidth/2), (rectangle.y()+height/2-textHeight/2), textWidth, textHeight); // don't touch this formula, trust my
                QString numberQString = QString::number(nodeValue);
                char *nodeValueC;
                std::string fname = numberQString.toStdString();
                nodeValueC = new char [fname.size()+1];
                std::strcpy( nodeValueC, fname.c_str());
                painter.drawText(rectangleText, Qt::AlignCenter, tr(nodeValueC));

                if(specialNode){
                    painter.setPen(specialColor);
                    specialNode = false;
                }

                QRect  rectangleIndicatingCircle = QRect(currentNodePoint.x()-unitarUnit/10,currentNodePoint.y()-unitarUnit/10,width+unitarUnit/5,height+unitarUnit/5);
                painter.drawArc(rectangleIndicatingCircle, startAngle, spanAngle);

            }

            if(!specialOperationList.isEmpty()){
                int numberOperation = specialOperationList.count();
                QRect textOperationRect(drawPixmap.width()/2+2*unitarUnit,10,drawPixmap.width()/2-unitarUnit*2,unitarUnit*numberOperation);
                painter.setPen(redColor);
                char *charComands;
                QString stringComands;
                for(QString st : specialOperationList){
                    stringComands+=st;
                    stringComands+="\n";
                }
                charComands = new char [stringComands.size()+1];
                std::string fname = stringComands.toStdString();
                std::strcpy(charComands, fname.c_str());

                painter.drawText(textOperationRect, Qt::AlignCenter, tr(charComands));
            }
            drawPrepare = false;
            label->setPixmap(drawPixmap);

        }

    }


protected:
    void drawNode(RBNode *node_, QPoint coodronate, QPoint parentPoint){

                setData(node_->key,QString::fromUtf8( node_->toString().data(), node_->toString().size() ),coodronate.x(),coodronate.y(),  parentPoint.x(), parentPoint.y());

        }

public:
    void createNodeLinkedList(){
        auxList = new LinkedList;
        crossingFunction(tree->root, QPoint(drawPixmap.width()/2,10), QPoint(-1,-1));
    }


    void drawOnlyTree(){
        createNodeLinkedList();
        pureTree = true;
        this->treeHeight = tree->depth(tree->root);
        drawPrepare = true;
        update();
    }

    void crossingFunction(RBNode *node_, QPoint coordonatePoint, QPoint parentCoordonatePoint){

        if(node_ == RBNode::Nil){

        }else{
            if(!(node_->left==RBNode::Nil)){
                QPoint leftPoint(coordonatePoint.x()-pow(2,tree->depth(node_->left))*unitarUnit, coordonatePoint.y()+unitarUnit); // this only look like easy mathematic formula to create, it's not
                crossingFunction(node_->left,leftPoint,QPoint(coordonatePoint.x(), coordonatePoint.y()));
            }
            if(!(node_->right==RBNode::Nil)){
                QPoint rightPoint(coordonatePoint.x()+pow(2,tree->depth(node_->right))*unitarUnit, coordonatePoint.y()+unitarUnit);
                crossingFunction(node_->right,rightPoint, QPoint(coordonatePoint.x(), coordonatePoint.y()));
            }
            drawNode(node_,coordonatePoint,parentCoordonatePoint);
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







