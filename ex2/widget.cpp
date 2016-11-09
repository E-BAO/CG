#include "widget.h"
#include "ui_widget.h"
#include <QMouseEvent>
#include <QPoint>
#include <QPixmap>
#include <QDebug>
#include <QPainter>
#include <QString>
#include <QMessageBox>
#include <QTimer>
#include <QTimerEvent>
#include <QImage>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    x=130;
    y=80;
    width=600;
    height=400;
    grain=5;
    tension=0.3;
    start_pts=new QPoint[MAX_PTS_NUM];
    end_pts=new QPoint[MAX_PTS_NUM];
    cur_pts=new QPoint[MAX_PTS_NUM];
    end_polar=new struct polar[MAX_PTS_NUM];
    start_polar=new struct polar[MAX_PTS_NUM];
    cur_polar=new struct polar[MAX_PTS_NUM];
    cur_spline=new QPoint[MAX_PTS_NUM*10+10];
    spline1=NULL;
    spline2=NULL;
    spline3=NULL;
    timer=new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timerUpDate()));
    init();
}

Widget::~Widget()
{
    delete ui;
    delete[] start_pts;
    delete[] end_pts;
    delete[] cur_pts;
    delete[] start_polar;
    delete[] end_polar;
    delete[] cur_polar;
    delete timer;
}

void Widget::init()
{
    n1=0;
    n2=0;
    flag=1;
    iscurve=false;
    if(spline1)
    {
        delete spline1;
        spline1=NULL;
    }
    if(spline2)
    {
        delete spline2;
        spline2=NULL;
    }
    if(spline3)
    {
        delete spline3;
        spline3=NULL;
    }
    timer->stop();
    ui->cspline_btn->setEnabled(false);
}

void Widget::timerUpDate()
{
    if(i_type==0)
    {
        this->Linear_Interpolygon();

    }else
    {
        this->Vector_Linear_Interpolygon();
    }
    if(iscurve)
    {
        spline3=new CSpline(cur_pts,n1,grain,tension);
        memcpy(cur_spline,spline3->Spline,allpt*sizeof(QPoint));
    }else
    {
        allpt=n1;
        memcpy(cur_spline,cur_pts,allpt*sizeof(QPoint));
    }
    frame_i++;
    if(frame_i==frame+1)
    {
        timer->stop();
        flag=3;
    }
    this->update();
}

void Widget::on_clean_btn_clicked()
{
    ui->notice_lb->setText(tr("请绘制第一幅图"));
    init();
    ui->lbl_pts_1->setText(QString::number(n1));
    ui->lbl_pts_2->setText(QString::number(n2));
    imgmap->load("");
    this->update();
}

void Widget::on_move_btn_clicked()
{
    if(n1!=n2)
    {
        QMessageBox::information(this,tr("错误"),tr("控制点数目不一致!"));
        return;
    }
    flag=4;
    v_type=ui->v_Box->currentIndex(); // 1= 2+ 3-
    i_type=ui->inter_Box->currentIndex();//1 linear  2 vector
    frame=ui->frame_spinBox->value();
    frame_i=0;
    if(i_type==1)calPolar();
    this->timer->start(50);
}

void Widget::calPolar()
{
    for(int i=0;i<n1-1;i++)
    {
        xy2po(start_pts[i+1],start_pts[i],&start_polar[i]);
        xy2po(end_pts[i+1],end_pts[i],&end_polar[i]);
    }
    qDebug()<<"end "<<endl;
}

void xy2po(QPoint p2,QPoint p1,Polar *po)
{
    float dx=p2.x()-p1.x();
    float dy=p2.y()-p1.y();
    po->r=sqrt(pow(dx,2)+pow(dy,2));
    po->theta=atan(dy/dx);
    qDebug()<<po->theta<<"  "<<dx<<","<<dy<<endl;
    if(dx<0&&dy>0)po->theta+=PI;
    else if(dx<0&&dy<0)po->theta+=PI;
    else if(dx>0&&dy<0)po->theta+=PI*2.0;
    //qDebug()<<po->theta<<"  "<<po->r<<endl;
}

void Widget::Linear_Interpolygon()
{
    float t=(float)frame_i/(float)frame;
    float f=law(v_type,t);
    if(frame_i==frame)f=1.0;
    if(frame_i==0)f=0.0;
    for(int j=0;j<n1;j++)
    {
        cur_pts[j].setX((1-f)*start_pts[j].x()+f*end_pts[j].x());
        cur_pts[j].setY((1-f)*start_pts[j].y()+f*end_pts[j].y());
    }
}

float law(int op,float t)
{
    float f;
    switch(op)
    {
    case 0:
        f=t;
        break;
    case 1:
        f=1-cos(PI*t/2.0);
        break;
    case 2:
        f=sin(PI*t/2.0);
        break;
    default:
        break;
    }
    return f;
}

void Widget::Vector_Linear_Interpolygon()
{
    float t=(float)frame_i/(float)frame;
    float f=law(v_type,t);
    if(frame_i==frame)f=1.0;
    if(frame_i==0)f=0.0;
    cur_pts[0].setX((1-f)*start_pts[0].x()+f*end_pts[0].x());
    cur_pts[0].setY((1-f)*start_pts[0].y()+f*end_pts[0].y());
    qDebug()<<"end"<<endl;
    for(int j=0;j<n1-1;j++)
    {
        float s=end_polar[j].theta-start_polar[j].theta;
        if(fabs(s)>PI)
        {
            if(s>0) end_polar[j].theta-=2*PI;
            else start_polar[j].theta-=2*PI;
        }
        qDebug()<<start_polar[j].theta<<"  "<<end_polar[j].theta<<endl;
        float r=(1-f)*start_polar[j].r+f*end_polar[j].r;
        float theta=(1-f)*start_polar[j].theta+f*end_polar[j].theta;
        //qDebug()<<theta<<endl;
        cur_pts[j+1].setX(cur_pts[j].x()+r*cos(theta));
        cur_pts[j+1].setY(cur_pts[j].y()+r*sin(theta));
    }
}

void Widget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setBrush(Qt::white);
    painter.setPen(Qt::black);
    painter.drawRect(x,y,width,height);
    painter.drawPixmap(x+180,y+10,300,300,*imgmap);
    if(!iscurve){
        painter.setBrush(Qt::red);
        for(int i=0;i<n1;i++)
        {
            //flag==1&&
            if(i>0)painter.drawLine(start_pts[i-1],start_pts[i]);
            painter.drawEllipse(start_pts[i],2,2);
        }
        painter.setBrush(Qt::green);
        for(int i=0;i<n2;i++)
        {
            //flag==2&&
            if(i>0)painter.drawLine(end_pts[i-1],end_pts[i]);
            painter.drawEllipse(end_pts[i],2,2);
        }
    }else
    {
        painter.setBrush(Qt::red);
        painter.setPen(Qt::NoPen);
        for(int i=0;i<n1;i++)
            painter.drawEllipse(start_pts[i],2,2);
        painter.setBrush(Qt::green);
        for(int i=0;i<n2;i++)
            painter.drawEllipse(end_pts[i],2,2);
        painter.setPen(Qt::black);
        for(int i=0;i<allpt;i++)
        {
            if(i>0)painter.drawLine(start_spline[i-1],start_spline[i]);
        }
        for(int i=0;i<allpt;i++)
        {
            if(i>0)painter.drawLine(end_spline[i-1],end_spline[i]);
        }
    }
    if(flag>=4)
    {
        for(int i=0;i<allpt;i++){
            if(i>0)painter.drawLine(cur_spline[i-1],cur_spline[i]);
            //painter.drawEllipse(cur_spline[i],3,3);
        }
    }
}

void Widget::mousePressEvent(QMouseEvent* e)
{
    if(!inMap(e->x(),e->y())||flag==3)return;
    if(e->button()!=Qt::LeftButton&&e->button()!=Qt::RightButton)return;
    if(e->button()==Qt::RightButton)
    {
        if(flag==1||flag==2)flag++;
        if(flag==2)ui->notice_lb->setText(tr("请绘制第二幅"));
        if(flag==3)
        {
            ui->notice_lb->setText(tr("绘制完成"));
            ui->cspline_btn->setEnabled(true);
        }
        return;
    }
    if(flag==1)
    {
        start_pts[n1].setX(e->x());
        start_pts[n1].setY(e->y());
        if(n1>0&&isOverLap(start_pts[n1],start_pts[0]))
        {
            flag=2;
            //n1--;
            start_pts[n1].setX(start_pts[0].x());
            start_pts[n1].setY(start_pts[0].y());
            ui->notice_lb->setText(tr("请绘制第二幅"));
        }
        n1++;
        ui->lbl_pts_1->setText(QString::number(n1));
    }else if(flag==2)
    {
        end_pts[n2].setX(e->x());
        end_pts[n2].setY(e->y());
        if(n2>0&&isOverLap(end_pts[n2],end_pts[0]))
        {
            flag=3;
            end_pts[n2].setX(end_pts[0].x());
            end_pts[n2].setY(end_pts[0].y());
            //n2--;
            ui->notice_lb->setText(tr("绘制完成"));
            ui->cspline_btn->setEnabled(true);
        }
        n2++;
        ui->lbl_pts_2->setText(QString::number(n2));
    }
    this->update();
}


bool Widget::inMap(int x,int y)
{
    return x>this->x&&x<this->x+this->width&&y>this->y&&y<this->y+this->height;
}

bool Widget::isOverLap(QPoint p1,QPoint p2)
{

    return abs(p1.x()-p2.x())<=PT_CURRENCY&&abs(p1.y()-p2.y())<=PT_CURRENCY;
}

void Widget::on_cspline_btn_clicked()
{
    if(n1!=n2)
    {
        QMessageBox::information(this,tr("错误"),tr("控制点数目不相等!"));
        return;
    }
    grain=ui->grainBox->value();
    if(flag>=2)
    {
        iscurve=true;
        spline1=new CSpline(start_pts,n1,grain,(double)tension);
        //delete[] start_pts;
        start_spline=spline1->Spline;
        //n1=(grain+1)*(n1-1)+1;
        spline2=new CSpline(end_pts,n2,grain,(double)tension);
        //delete[] end_pts;
        allpt=(grain+1)*(n1-1)+1;
        end_spline=spline2->Spline;
        //ui->cspline_btn->setEnabled(false);
    }
    this->update();
}

void Widget::on_tsSlider_actionTriggered(int )
{
    tension=(float)ui->tsSlider->value()/100.0;
    ui->ts_lb->setText(QString::number((double)tension,'f',2));
}

void Widget::on_imgButton_clicked()
{
    qDebug()<<QApplication::applicationDirPath();
    QString filename=QApplication::applicationDirPath()+"\\"+ui->pathEdit->text();
    if(filename.isEmpty())
         return;
    else
    {
        QImage* img=new QImage;
        if(! ( img->load(filename) ) )
        {
            QMessageBox::information(this,tr("打开图像失败"),tr("打开图像失败!"));
            delete img;
            return;
        }
        imgmap=new QPixmap();
        imgmap->load(filename);
        this->update();
    }
}
