#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QVector>
#include <QPoint>
#include <QToolBar>
#include <QAction>
#include <QPalette>
#include <QColor>
#include <QFile>
#include <QString>
#include <QFileDialog>
#include <QDebug>
#include <QSlider>
#include <QColorDialog>
#include <QPushButton>
#include <QPalette>
#include <QLabel>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected: // 重写事件
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);/*
    void keyPressEvent(QKeyEvent *e);*/

private:
struct Info
{
    QVector<QPoint> Lines;
    QSize DrawSize;
    QColor DrawColor;
    int Width;
};
    Ui::MainWindow *ui;
    bool PressLeft; // LeftButton pressed
    bool Canvas; // Canvas exists
    QPixmap CanvasPixmap; // Canvas Picture
    QVector<Info> DrawInfo;
    QSize PicSize;
    void LoadPic(QPixmap pic, QLabel* label); // load pic to label
    QPoint Position(const QPoint& glob);
    QPoint PointStretch(const QPoint& a, const double& k1, const double& k2);
    QColor PenColor;
    QColor TmpColor;
    int PenSize;
    QColorDialog *dialog;
    QLabel *ColorIcon, *SizeIcon, *SizeLabel;
    QAction *BackAction, *NewAction, *OpenAction, *SaveAction;
    QSlider *SizeSlider;
    QPushButton *ColorButton;
    void DrawPaint(QPixmap &pixmap, bool op);

public slots:
    void SaveCurPic(); // save Picture to local
    void OpenNewPic(); // load Picture from local
    void NewEmptyCanvas(); // Create a blank canvas
    void Backwards(); // back one step
    void ColorSelect();
    void SetTmpColor(QColor color);
    void SetColor();
    void SaveDrawPic();

private slots:
    void on_pushButton_clicked();
};
#endif // MAINWINDOW_H
