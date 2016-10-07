#ifndef CSPLINE_H
#define CSPLINE_H


#include <QPoint>

class CSpline{



public:
    QPoint *Spline;//1024
    QPoint *ctlpoints;//100
    double *px[100];
    double *py[100];
    double *Arc;
    double *ArcAll;
    CSpline(QPoint[], int np, int gr, double t);
    ~CSpline();
    double FindArc(int j, double d,QPoint *p);

private:
    int grain;
    double tension;
    int n;
    int allpt;


    void CubicSpline();

    void GetCardinalMatrix();

    void Matrix(int type,int i,double a, double b, double c, double d);

    double Cal(int type,int i,double alpha);

    double f(int i,double u);

    double Simpson(int i,double u1,double u2);

    double calAng(int i,double u);

    void CalArc(int i, double *p, double *pall);


};





#endif // CSPLINE_H
