#ifndef WIDGET_H
#define WIDGET_H

#include "cspline.h"
#include <QWidget>
#include <QPoint>
#include <QPaintEvent>
#include <QTimerEvent>
#include <QTimer>
#include <QPixmap>

#define MAX_PTS_NUM 500
#define PT_CURRENCY 10
#define PI 3.14159

namespace Ui {
class Widget;
}

typedef struct polar{
    float theta;
    float r;
}Polar;

class Widget : public QWidget
{
    Q_OBJECT
    QPoint *start_pts,*end_pts,*cur_pts,*start_spline,*end_spline,*cur_spline;
    Polar *start_polar,*end_polar,*cur_polar;
    QPixmap *imgmap;
    int x,y,width,height;
    int flag;
    int n1,n2;
    int v_type,frame,i_type,frame_i;
    float tension;
    int grain;
    int allpt;
    bool iscurve;
    CSpline *spline1,*spline2,*spline3;
    QTimer *timer;

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void paintEvent(QPaintEvent*);
    void init();
    bool inMap(int x,int y);
    bool isOverLap(QPoint p1, QPoint p2);
    void Linear_Interpolygon();
    void Vector_Linear_Interpolygon();
    void calPolar();

private:
    Ui::Widget *ui;

private slots:
    void mousePressEvent(QMouseEvent*);
    void timerUpDate();
    void on_clean_btn_clicked();
    void on_move_btn_clicked();
    void on_cspline_btn_clicked();
    void on_imgButton_clicked();
    void on_tsSlider_actionTriggered(int );
};

float law(int op,float t);
void xy2po(QPoint p1,QPoint p2,Polar *po);

#endif // WIDGET_H
