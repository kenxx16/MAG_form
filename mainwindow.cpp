#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update (int s)
{
    cout<<"  --  "<<s<<endl;
    if(s!=-1){
        ui->progressBar->setValue(s);
    }
}

void MainWindow::done (int s)
{
    if(s==-1){
        ui->pushButton_3->setEnabled(true);
    }

    if(s==-2){
        ui->pushButton_6->setEnabled(true);
    }
}

void MainWindow::getData (Ltype L)
{
    L1 = L;
}

void MainWindow::getData (SPtype SP, SPtype SPint)
{
    coord = SP;
    coordInt = SPint;
}

//---------------------------------------Выбор-----------------------------------------------------------
//выбор OBS
void MainWindow::on_pushButton_clicked()
{
    QString str = QFileDialog::getOpenFileName(0, "Open Dialog", "", "*.*");
    ui->lineEdit->setText(str);
    ui->pushButton_7->setEnabled(true);
}

//Выбор SP3
void MainWindow::on_pushButton_4_clicked()
{
    QString str = QFileDialog::getOpenFileName(0, "Open Dialog", "", "*.*");
    ui->lineEdit_3->setText(str);
    ui->pushButton_5->setEnabled(true);
}

//---------------------------------------Чтение----------------------------------------------------------
//Чтение OBS

void MainWindow::on_pushButton_7_clicked()
{
    QString str = ui->lineEdit->text();
    QThread *thread= new QThread;
    OBSthread *my = new OBSthread(str, L1);

    my->moveToThread(thread);
    qRegisterMetaType<Ltype>("L") ;
    connect(my, SIGNAL(send(int)), this, SLOT(update(int)));
    connect(my, SIGNAL(finished(int)), this, SLOT(done(int)));
    connect(my, SIGNAL(sendData(Ltype)), this,SLOT(getData(Ltype)));

    connect(thread, SIGNAL(started()), my, SLOT(run()));

    thread->start();
}


//Чтение SP3
void MainWindow::on_pushButton_5_clicked()
{
    QString str = ui->lineEdit_3->text();
    QThread *thread2= new QThread;
    SP3thread *my2 = new SP3thread(str, coord);

    my2->moveToThread(thread2);
    qRegisterMetaType<SPtype>("SP") ;
    connect(my2, SIGNAL(send(int)), this, SLOT(update(int)));
    connect(my2, SIGNAL(finished(int)), this, SLOT(done(int)));
    connect(my2, SIGNAL(sendData(SPtype, SPtype)), this,SLOT(getData(SPtype, SPtype)));

    connect(thread2, SIGNAL(started()), my2, SLOT(run()));

    thread2->start();
}

//---------------------------------------Графики---------------------------------------------------------
//Построение графика (OBS)
void MainWindow::on_pushButton_3_clicked()
{
//    if(ui->lineEdit_2->text().isEmpty()){
//    }
//    int num = ui->lineEdit_2->text().toInt();
//    ui->widget->clearGraphs();
//    ui->widget->addGraph();
//    QMapIterator<QDateTime, QMap<QString, QMap<QString, double>>> i(L1);
//    QVector<double> L, time;

//        int e=0;
//        i.toFront();
//        cout<<num<<"  ";
//        while (i.hasNext()) {
//            e++;
//            i.next();
//            cout << i.value()[num] <<" / ";
//            time.append(e);
//            L.append(i.value()[num]);
//        }
//        cout<<endl;
//        cout<<endl;


//    ui->widget->addGraph();
//    ui->widget->graph(0)->setData(time,L);
//    ui->widget->graph(0)->setLineStyle(QCPGraph::lsNone);
//    ui->widget->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 2));
//    ui->widget->xAxis->setRange(time[0],time[time.size()-1]);//Для оси Ox

//        //вычислить минимальное и максимальное значение в векторах
//        double minY = 999999999999, maxY = L[0];
//        for (int i=0; i<time.size(); i++)
//        {
//            if (L[i]<minY and L[i]!=0) minY = L[i];
//            if (L[i]>maxY and L[i]!=0) maxY = L[i];
//        }
//        ui->widget->yAxis->setRange(minY, maxY);//Для оси Oy
//        ui->widget->replot();
}

//Построение графика (SP3)
void MainWindow::on_pushButton_6_clicked()
{
    //    if(ui->lineEdit_4->text().isEmpty()){
    //    }
    int num = ui->lineEdit_4->text().toInt();
    ui->widget->clearGraphs();
    ui->widget->addGraph();
    cout<<coordInt.size()<<endl;
    QMapIterator<QDateTime,QMap<int, vector<double>>> i(coordInt);

    QVector<double> X, Y, Z, L, time;

    int e=0;
    i.toFront();
    cout<<"----------"<<endl;
    //cout<<i.value()[num][0]<<endl;
    while (i.hasNext()) {
        cout<<"+"<<endl;
        e++;
        i.next();
        if(i.value()[num].size()==0) break;
        time.append(e);
        X.append(i.value()[num][0]);
        Y.append(i.value()[num][1]);
        Z.append(i.value()[num][2]);
    }

    ui->widget_2->addGraph();
    ui->widget_3->addGraph();
    ui->widget_4->addGraph();

    ui->widget_2->graph(0)->setData(time,X);
    ui->widget_3->graph(0)->setData(time,Y);
    ui->widget_4->graph(0)->setData(time,Z);

    ui->widget_2->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->widget_3->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->widget_4->graph(0)->setLineStyle(QCPGraph::lsNone);

    ui->widget_2->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 2));
    ui->widget_3->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 2));
    ui->widget_4->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 2));

    ui->widget_2->xAxis->setRange(time[0],time[time.size()-1]);//Для оси Ox
    ui->widget_3->xAxis->setRange(time[0],time[time.size()-1]);//Для оси Ox
    ui->widget_4->xAxis->setRange(time[0],time[time.size()-1]);//Для оси Ox

        //вычислить минимальное и максимальное значение в векторах
        double minY_x = 999999999999, maxY_x = X[0];
        double minY_y = 999999999999, maxY_y = Y[0];
        double minY_z = 999999999999, maxY_z = Z[0];
        for (int i=0; i<time.size(); i++)
        {
            if (X[i]<minY_x and X[i]!=0) minY_x = X[i];
            if (X[i]>maxY_x and X[i]!=0) maxY_x = X[i];

            if (Y[i]<minY_y and X[i]!=0) minY_y = Y[i];
            if (Y[i]>maxY_y and X[i]!=0) maxY_y = Y[i];

            if (Z[i]<minY_z and X[i]!=0) minY_z = Z[i];
            if (Z[i]>maxY_z and X[i]!=0) maxY_z = Z[i];


        }
        ui->widget_2->yAxis->setRange(minY_x, maxY_x);//Для оси Oy
        ui->widget_2->replot();

        ui->widget_3->yAxis->setRange(minY_y, maxY_y);//Для оси Oy
        ui->widget_3->replot();

        ui->widget_4->yAxis->setRange(minY_z, maxY_z);//Для оси Oy
        ui->widget_4->replot();
}


//---

double Po(QMap<int, vector<double>> c,
          boost::numeric::ublas::vector<double> XYZ,
          int num){

    //cout<<c[num][0]<<endl;
    double out = sqrt(pow((c[num][0]-XYZ[0]),2)+
                      pow((c[num][1]-XYZ[1]),2)+
                      pow((c[num][2]-XYZ[2]),2));
    return out;
}

//---


//-------------Решение---------------------------------
void MainWindow::on_pushButton_2_clicked()
{
//    boost::numeric::ublas::vector<double> XYZ0(4);
//    XYZ0[0] = 0; XYZ0[1] = 0; XYZ0[2] = 0; XYZ0[3] = 0;
//    boost::numeric::ublas::vector<double> dX(4);
//    dX[0] = 0; dX[1] = 0; dX[2] = 0; dX[3] = 0;
//    boost::numeric::ublas::vector<double> L;
//    boost::numeric::ublas::vector<double> P0(32);
//    boost::numeric::ublas::vector<double> Ux, Uy, Uz, V;
//    QMapIterator<QDateTime,QMap<int, vector<double>>> c(coordInt);
//    QMapIterator<QDateTime, QMap<QString, QMap<QString, double>>> s(L1);
//    boost::numeric::ublas::matrix<double> A;
//    c.toFront();
//    s.toFront();
//    int time = 0;
//    while (s.hasNext() and c.hasNext()) {
//        time++;
//        s.next();
//        c.next();
//        cout<<"--------  "<<time<<endl;

//        for (int num=1; num<=32; num++) {
//            P0(num-1)=Po(c.value(), XYZ0, num);
//            if(s.value()[num]!=0){
//                Ux[num] = (c.value()[num][0]-XYZ0[0])/P0[num];
//                Uy[num] = (c.value()[num][1]-XYZ0[1])/P0[num];
//                Uz[num] = (c.value()[num][2]-XYZ0[2])/P0[num];

//                A(0, num) = -Ux[num];
//                A(1, num) = -Uy[num];
//                A(2, num) = -Uz[num];
//                A(3, num) = 1;

//                L[num] = P0[num] - s.value()[num];
//            }
//        }

//    }
}
//---

