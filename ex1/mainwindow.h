#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "cspline.h"
#include <QMainWindow>
#include <QPoint>
#include <QPixmap>
#include <QTimerEvent>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void init();


private slots:
    void on_Createbtn_clicked();
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent*);
    void timerUpDate();
    void on_cleanbtn_clicked();

    void on_horizontalSlider_valueChanged(int value);

    void on_linebtn_clicked();

    void on_carbtn_clicked();

    void on_ballbtn_clicked();

    void on_start_clicked();

    void on_potbtn_clicked();

    void on_spinBox_valueChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    CSpline *Curve;
    QPoint *points;
    QPoint *Spline;
    double *Arc,*ArcAll;
    //double arc[1024][2];
    int n;
    int allpt;
    int grain;
    int curi;
    int type;
    double v;
    QPoint curpos;
    double curarc;
    double tension;
    double angle;
    double ballv;
    double A,B,C,D,E;
    QPixmap tempPix;
    QPixmap Pix;
    bool isdrawing;
    bool iscurve;
    bool isline;
    bool isball;
    bool iscar;
    bool ispot;
    QTimer *timer;
};

#endif // MAINWINDOW_H
