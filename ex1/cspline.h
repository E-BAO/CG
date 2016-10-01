#ifndef CSPLINE_H
#define CSPLINE_H


#include <QPoint>

void CSpline(QPoint *Spline,QPoint points[],int grain,double tension,int n);

void CubicSpline(QPoint *Spline,int n, QPoint *ctlpoints, int grain, double tension);

void GetCardinalMatrix(double a1);

void Matrix(double p[],double a, double b, double c, double d);

double Cal(double alpha,double p[]);

#endif // CSPLINE_H
