#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cspline.h"
#include <QPainter>
#include <QMouseEvent>
#include <QString>
#include <QTimerEvent>
#include <QTimer>
#include <QDebug>
#include <QtMath>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    points=new QPoint[100];
    Spline=NULL;
    Curve=NULL;
    Pix=QPixmap(1000,800);
    tempPix=QPixmap(1000,800);
    timer=new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timerUpDate()));
    this->init();
}

MainWindow::~MainWindow()
{
    delete[] points;
    delete ui;
}

void MainWindow::init()
{
    n=0;
    grain=0;
    tension=0.0;
    allpt=0;
    curi=0;
    isdrawing=false;
    iscurve=false;
    isline=true;
    iscar=false;
    isball=false;
    ispot=true;
    ballv=0.0;
    Pix.fill(Qt::white);
    tempPix.fill(Qt::white);
    ui->carbtn->setEnabled(false);
    ui->ballbtn->setEnabled(false);
    ui->start->setEnabled(false);
}

void MainWindow::on_Createbtn_clicked()
{
    if(n<=1)return;
    iscurve=true;
    grain=ui->spinBox->value();
    tension=(double)ui->horizontalSlider->value()/100.0;
    //CSpline line(points,n,grain,tension);
    allpt=(grain+1)*(n-1)+1;
    if(Curve)
    {
        //qDebug()<<"delete"<<endl;
        delete Curve;
        Curve=NULL;
    }
    Curve=new CSpline(points,n,grain,tension);
    Spline=Curve->Spline;
    Arc=Curve->Arc;
    ArcAll=Curve->ArcAll;
    //memcpy(Spline,line.Spline,sizeof(QPoint)*allpt);
    Pix.fill(Qt::white);
    ui->carbtn->setEnabled(true);
    ui->ballbtn->setEnabled(true);
    this->update();
}



void MainWindow::paintEvent(QPaintEvent*)
{
    QPainterPath path,path2;

    if(n>100)return;

    QPainter pp(&Pix);
    //pp.setBrush(Qt::black);
    pp.drawRect(100,50,850,600);
    if(n>0)path.moveTo(points[0].x(),points[0].y());
    for(int i=0;i<n;i++)
    {
        if(isline)path.lineTo(points[i].x(),points[i].y());
        path.addEllipse(points[i],3,3);
        if(isline)path.moveTo(points[i].x(),points[i].y());
    }
    pp.drawPath(path);


    path2.moveTo(points[0].x(),points[0].y());

   // arc[0][0]=0;
  //  arc[0][1]=0;
    for(int i=1;i<allpt;i++)
    {
        path2.lineTo(Spline[i].x(),Spline[i].y());
        if(ispot)path2.addEllipse(Spline[i],2,2);
        //pp.setBrush(Qt::NoBrush);
        path2.moveTo(Spline[i].x(),Spline[i].y());
       // arc[i][0]=sqrt(pow(Spline[i].x()-Spline[i-1].x(),2)+pow(Spline[i].y()-Spline[i-1].y(),2));
       // arc[i][1]=arc[i-1][1]+arc[i][0];
       // qDebug()<<i<<"  "<<arc[i][0]<<"  "<<arc[i][1]<<endl;
    }
    pp.setBrush(Qt::red);
    pp.drawPath(path2);
    if(isball)
    {
        double d=50;
        qDebug()<<"i="<<curi<<endl;
        if(curi>0);
//            if(curi<allpt)angle=(qAtan2(Spline[curi].y()-Spline[curi-1].y(),Spline[curi].x()-Spline[curi-1].x())
//                +qAtan2(Spline[curi+1].y()-Spline[curi].y(),Spline[curi+1].x()-Spline[curi].x()))*0.5;
//            else
//                angle=qAtan2(Spline[curi].y()-Spline[curi-1].y(),Spline[curi].x()-Spline[curi-1].x());
        else angle=qAtan2(Spline[2].y()-Spline[1].y(),Spline[2].x()-Spline[1].x());
        pp.save();
        pp.translate(curpos.x(),curpos.y());
        pp.rotate(angle/3.14*180.0);
        pp.translate(-d/2.0,-d);
        QPixmap carpix;
        carpix.load("E:\\2016_2\\Animation\\ex1\\ball.png");

        ballv=(ballv+v/d/1.57*180.0);
        //while(ballv>360.0)ballv-=360.0;
        pp.translate(d*0.5,d*0.5);
        pp.rotate(ballv);//ballv/3.14*180.0
        pp.translate(-d*0.5,-d*0.5);
        pp.drawPixmap(0,0,d,d,carpix);
        //pp.drawEllipse(0,0,r,r);
        pp.restore();
    }
    if(iscar)
    {
        double w=60.0,h=40.0;
        qDebug()<<"i="<<curi<<endl;
        //int curi*(grain);
        if(curi>0);
//            if(curi<allpt)angle=(qAtan2(Spline[curi].y()-Spline[curi-1].y(),Spline[curi].x()-Spline[curi-1].x())
//                +qAtan2(Spline[curi+1].y()-Spline[curi].y(),Spline[curi+1].x()-Spline[curi].x()))*0.5;
//            else
//                angle=qAtan2(Spline[curi].y()-Spline[curi-1].y(),Spline[curi].x()-Spline[curi-1].x());
        else angle=qAtan2(Spline[2].y()-Spline[1].y(),Spline[2].x()-Spline[1].x());
        //qDebug()<<angle<<endl;
        pp.save();
        pp.translate(curpos.x(),curpos.y());
       // pp.drawEllipse(0,0,2,2);
        pp.rotate(angle/3.14*180);
        pp.translate(-w/2.0,-h+2.0);//to be real
        QPixmap carpix;
        carpix.load("E:\\2016_2\\Animation\\ex1\\car.png");
        pp.drawPixmap(0,0,w,h,carpix);
        //pp.drawRect(0,0,w,h);
        pp.restore();
    }
    QPainter painter(this);
    painter.drawPixmap(0,0,Pix);
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    if(e->button()!=Qt::LeftButton)return;
    if(e->x()<100||e->x()>950||e->y()<50||e->y()>650)return;

    points[n].setX(e->x());
    points[n].setY(e->y());

    this->n++;
    this->update();
}

void MainWindow::on_cleanbtn_clicked()
{
    n=0;
    allpt=0;
    Pix.fill(Qt::white);
    iscurve=false;
    isline=true;
    iscar=false;
    isball=false;
    if(Curve)
    {
        delete Curve;
        Curve=NULL;
    }
    this->init();
    this->update();
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    double v=(double)value/100.0;
    ui->label_3->setText(QString::number(v,'f',2));
    on_Createbtn_clicked();
}

void MainWindow::on_linebtn_clicked()
{
    isline=!isline;
    if(!isline)ui->linebtn->setText("显示折线");
    else ui->linebtn->setText("隐藏折线");
    Pix.fill(Qt::white);
    this->update();

}

void MainWindow::on_carbtn_clicked()
{
    Pix.fill(Qt::white);
    iscar=!iscar;
    if(iscar)
    {
        isball=false;
        ui->start->setEnabled(true);
    }
    curi=0;
    curpos.setX(Spline[0].x());
    curpos.setY(Spline[0].y());
    this->update();
}

void MainWindow::on_ballbtn_clicked()
{
    Pix.fill(Qt::white);
    isball=!isball;
    if(isball)
    {
        iscar=false;
        ui->start->setEnabled(true);
    }
    curi=0;
    ballv=0.0;
    curpos.setX(Spline[0].x());
    curpos.setY(Spline[0].y());
    this->update();
}

void MainWindow::on_start_clicked()
{
    type=ui->comboBox->currentIndex();
    v=ui->vEdit->text().toDouble();
    curpos.setX(Spline[0].x());
    curpos.setY(Spline[0].y());
    curarc=0;
    curi=1;
    if(iscar||isball)timer->start(100);
}

void MainWindow::timerUpDate()
{
    switch(type)
    {
    case 0:
        v=ui->vEdit->text().toDouble();
        break;
    case 1:
        v+=2;
        break;
    case 2:
        if(v>5)v-=2;
        break;
    }
    curarc+=v;
    while(ArcAll[curi]<curarc&&curi<=n)curi++;
    if(curi>0)curi--;
    if(curi==n)
    {
        timer->stop();
        curpos.setX(points[curi-1].x());
        curpos.setY(points[curi-1].y());
    }else
    {
        double di=curarc-ArcAll[curi];
        //double f=di/arc[curi+1][0];
        //double f=Curve->findArc(curi,di);
        angle=Curve->FindArc(curi,di,&curpos);
       // curpos.setX(Curve->FindArc(curi,di).x());
       // curpos.setY(Curve->FindArc(curi,di).y());
       // angle=Curve->calAng(i,u);
        //qDebug()<<curi<<" "<<di<<"  "<<Arc[curi+1]<<endl;
        qDebug()<<curi<<curpos.x()<<","<<curpos.y()<<endl;
        //curpos.setX((1-f)*Spline[curi].x()+f*Spline[curi+1].x());
        //curpos.setY((1-f)*Spline[curi].y()+f*Spline[curi+1].y());
    }
    Pix.fill(Qt::white);
    this->update();
}

void MainWindow::on_potbtn_clicked()
{
    ispot=!ispot;
    if(ispot)ui->potbtn->setText("隐藏插值点");
    else ui->potbtn->setText("显示插值点");
    Pix.fill(Qt::white);
    this->update();
}

void MainWindow::on_spinBox_valueChanged(const QString &)
{
    on_Createbtn_clicked();
}

