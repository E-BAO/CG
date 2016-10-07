#include "cspline.h"
#include <QPoint>
#include <qmath.h>
#include <QDebug>

double m[16];

CSpline::CSpline(QPoint points[], int np, int gr, double t){
    ctlpoints=new QPoint[100];
    Spline=new QPoint[1024];
    Arc=new double[100];
    ArcAll=new double[100];
    for(int i=0;i<100;i++)
    {
        px[i]=new double[4];
        py[i]=new double[4];
    }
    n=np;
    grain=gr;
    tension=t;
    allpt=(grain+1)*(n-1)+1;
    QPoint *pc=ctlpoints;
    pc++;
    memcpy(pc,points,sizeof(QPoint)*n);
    ctlpoints[0].setX(points[0].x());
    ctlpoints[0].setY(points[0].y());
    ctlpoints[n+1].setX(points[n-1].x());
    ctlpoints[n+1].setY(points[n-1].y());
    n+=2;
    //qDebug()<<"n="<<n<<" ten="<<tension<<" grain="<<grain<<endl;
    CubicSpline();
}

CSpline::~CSpline(){
    delete[] ctlpoints;
    delete[] Spline;
    delete[] Arc;
    delete[] ArcAll;
    for(int i=0;i<100;i++)
    {
        delete[] px[i];
        delete[] py[i];
    }
}

void CSpline::CubicSpline() {
    QPoint *s, *k0, *km1, *k1, *k2;
    double *parc=Arc;
    double *pall=ArcAll;
    double *pm1=ArcAll;
    pm1++;
    int m=1;
    s=Spline;
    double alpha[30];
    GetCardinalMatrix();
    for(int i=0;i<=grain+1;i++)alpha[i]=(double)i/(grain+1);
    km1=ctlpoints;
    k0=km1+1;
    k1=k0+1;
    k2=k1+1;
    Arc[1]=Arc[0]=0.0;
    ArcAll[1]=ArcAll[0]=0.0;
   //qDebug()<<ArcAll<<endl;
    parc+=2;
    pall+=2;
    for(int i=1;i<n-2;i++){
        Matrix(1,i,km1->x(), k0->x(), k1->x(), k2->x());//px
        Matrix(2,i,km1->y(), k0->y(), k1->y(), k2->y());//py  add pz
        CalArc(i,parc,pall);
        qDebug()<<i<<" "<<*parc<<" "<<(int)*pall<<endl;
        for(int j=0;j<=grain;j++)
        {
           s->setX( qFloor(Cal(1,i,alpha[j])));
           s->setY( qFloor(Cal(2,i,alpha[j])));
           //qDebug()<<"y"<<s->y()<<"arc "<<*parc<<" "<<*pall<<endl;

           s++;
           //m++;
        }
        parc++;
        pall++;
        k0++;
        km1++;
        k1++;
        k2++;
    }
    s->setX(ctlpoints[n-1].x());
    s->setY(ctlpoints[n-1].y());
}

void CSpline::GetCardinalMatrix() {
    double a1=tension;
    m[0] = -a1;
    m[1] = 2. - a1;
    m[2] = a1 - 2.;
    m[3] = a1;
    m[4] = 2.*a1;
    m[5] = a1 - 3.;
    m[6] = 3. - 2 * a1;
    m[7] = -a1;
    m[8] = -a1;
    m[9] = 0.;
    m[10] = a1;
    m[11] = 0.;
    m[12] = 0.;
    m[13] = 1.;
    m[14] = 0.;
    m[15] = 0.;
}

void CSpline::Matrix(int type, int i, double a, double b, double c, double d) {
    double p[4];
    p[0] = m[0] * a + m[1] * b + m[2] * c + m[3] * d;
    p[1] = m[4] * a + m[5] * b + m[6] * c + m[7] * d;
    p[2] = m[8] * a + m[9] * b + m[10] * c + m[11] * d;
    p[3] = m[12] * a + m[13] * b + m[14] * c + m[15] * d;
    if(type==1)
          memcpy(px[i],&p[0],sizeof(double)*4);
    else
          memcpy(py[i],&p[0],sizeof(double)*4);
}

double CSpline::Cal(int type,int i,double u){
    if(type==1)return px[i][3] + u*(px[i][2] + u*(px[i][1] + u*px[i][0]));
    else return py[i][3] + u*(py[i][2] + u*(py[i][1] + u*py[i][0]));
}

double CSpline::calAng(int i,double u)
{
    double dx=px[i][2]+u*(2.0*px[i][1]+u*3.0*px[i][0]);
    double dy=py[i][2]+u*(2.0*py[i][1]+u*3.0*py[i][0]);
    return qAtan2(dy,dx);
}

double CSpline::f(int i,double u)
{
    double A,B,C,D,E;
    double ax=px[i][0],ay=py[i][0],bx=px[i][1],by=py[i][1],cx=px[i][2],cy=py[i][2];
    A=9*(pow(ax,2)+pow(ay,2));
    B=12*(ax*bx+ay*by);
    C=6*(ax*cx+ay*cy)+4*(pow(bx,2)+pow(by,2));
    D=4*(bx*cx+by*cy);
    E=pow(cx,2)+pow(cy,2);
    return sqrt(E+u*(D+u*(C+u*(B+u*A))));
}

double CSpline::Simpson(int j,double u1,double u2)
{
    const int divid=10;//even
    double delta=(u2-u1)/(double)divid;
    double u[divid+1];
    double sum=0.0;
    u[0]=u1;
    sum=f(j,u[0]);
    for(int i=1;i<divid;i++)
    {
        u[i]=u1+delta*i;
        if(i%2==1)sum+=4*f(j,u[i]);
        else sum+=2*f(j,u[i]);
    }
    u[divid]=u2;
    sum+=f(j,u[divid]);
    return delta/3.0*sum;
}

void CSpline::CalArc(int i, double *p, double *pall)
{
    *p=Simpson(i,0.0,1.0);
   // qDebug()<<"-1"<<*pm1<<endl;
    *pall=*(pall-1)+*p;
}

double CSpline::FindArc(int j, double d, QPoint *p)
{
    //int j=(i-1)/(grain+1)+1;
//    double u1=(i-1)%(grain+1)/(grain+1);
//    double u2;
//    if(i%(grain+1)==0)u2=1.0;
//    else u2=i%(grain+1)/(grain+1);
    double u1=0.0,u2=1.0;
    double u3=u2;
    const double theta=0.01;
    double di;
    while(fabs(Simpson(j,u1,u2))>theta)
    {
        u3=(u1+u2)/2.0;
        di=Simpson(j,0.0,u3);
        if(di<d)u1=u3;
        else u2=u3;
    }
    p->setX( qFloor(Cal(1,j,u3)));
    p->setY( qFloor(Cal(2,j,u3)));

    return calAng(j,u3);
}
