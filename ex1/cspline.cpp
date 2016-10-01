#include "cspline.h"
#include <QPoint>
#include <qmath.h>

double m[16];

void CSpline(QPoint *Spline,QPoint points[],int grain,double tension,int n){
    int np;
    //n0=n;
    np=n;
    QPoint ctlpoints[100];
    memcpy(&ctlpoints[1],points,sizeof(QPoint)*np);
    ctlpoints[0].setX(points[0].x());
    ctlpoints[0].setY(points[0].y());
    ctlpoints[np+1].setX(points[np-1].x());
    ctlpoints[np+1].setY(points[np-1].y());
    np+=2;
    CubicSpline(Spline,np,ctlpoints,grain,tension);
}

void CubicSpline(QPoint *Spline,int n, QPoint *ctlpoints, int grain, double tension) {
    QPoint *s, *k0, *km1, *k1, *k2;
    s=Spline;
    double alpha[30];
    GetCardinalMatrix(tension);
    for(int i=0;i<=grain;i++)alpha[i]=(double)i/(grain+1);
    km1=ctlpoints;
    k0=km1+1;
    k1=k0+1;
    k2=k1+1;
    for(int i=1;i<n-1;i++){
        double px[4],py[4];
        Matrix(px,km1->x(), k0->x(), k1->x(), k2->x());
        Matrix(py,km1->y(), k0->y(), k1->y(), k2->y());
        for(int j=0;j<=grain;j++)
        {
           s->setX( qFloor(Cal(alpha[j],px)));
           s->setY( qFloor(Cal(alpha[j],py)));
           s++;
        }
        k0++;
        km1++;
        k1++;
        k2++;
    }
    s->setX(ctlpoints[n-1].x());
    s->setY(ctlpoints[n-1].y());
}

void GetCardinalMatrix(double a1) {
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

void Matrix(double p[],double a, double b, double c, double d) {
    p[0] = m[0] * a + m[1] * b + m[2] * c + m[3] * d;
    p[1] = m[4] * a + m[5] * b + m[6] * c + m[7] * d;
    p[2] = m[8] * a + m[9] * b + m[10] * c + m[11] * d;
    p[3] = m[12] * a + m[13] * b + m[14] * c + m[15] * d;
}

double Cal(double alpha,double p[]){
    return p[3] + alpha*(p[2] + alpha*(p[1] + alpha*p[0]));
}
