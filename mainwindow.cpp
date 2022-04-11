#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    /* setup toolbar */
    ui->toolBar->setMovable(false);
    ui->toolBar->setIconSize(QSize(30, 30));

    NewAction = new QAction(tr("&新建空白画布"), this);//新建空白文件
    NewAction->setIcon(QIcon(":/pic/icon/file+.svg"));//图标
    ui->toolBar->addAction(NewAction);

    ui->toolBar->addSeparator();

    OpenAction = new QAction(tr("&导入本地图像"), this);
    OpenAction->setIcon(QIcon(":/pic/icon/folder.svg"));
    ui->toolBar->addAction(OpenAction);

    ui->toolBar->addSeparator();

    SaveAction = new QAction(tr("&保存图片"), this);
    SaveAction->setIcon(QIcon(":/pic/icon/save.svg"));
    ui->toolBar->addAction(SaveAction);

    ui->toolBar->addSeparator();

    BackAction = new QAction(tr("&撤销"), this);
    BackAction->setIcon(QIcon(":/pic/icon/arrow-back.svg"));
    ui->toolBar->addAction(BackAction);

    ui->toolBar->addSeparator();

    SizeIcon = new QLabel();
    SizeIcon->setText(tr("画笔粗细："));
    ui->toolBar->addWidget(SizeIcon);

    SizeSlider = new QSlider(Qt::Horizontal);
    SizeSlider->setFixedSize(120, 30);
    SizeSlider->setRange(1, 30);
    SizeSlider->setSliderPosition(1);
    SizeSlider->setTracking(true);
    ui->toolBar->addWidget(SizeSlider);

    SizeLabel = new QLabel();
    SizeLabel->setFixedSize(30, 30);
    SizeLabel->setText(QString("1"));
    ui->toolBar->addWidget(SizeLabel);

    ui->toolBar->addSeparator();

    ColorButton = new QPushButton();
    ColorButton->setText(tr("画笔颜色："));
    ui->toolBar->addWidget(ColorButton);

    ui->toolBar->addSeparator();
    ColorIcon = new QLabel();
    ColorIcon->setFixedSize(30, 30);
    QPixmap pix(30,30);
    pix.fill(Qt::black);
    ColorIcon->setPixmap(pix);
    ui->toolBar->addWidget(ColorIcon);
    ui->actiond_2->setDisabled(true);

    PressLeft = false;
    Canvas = false;
    CanvasPixmap = QPixmap(ui->label->size());
    CanvasPixmap.fill(Qt::white);
    PenColor = Qt::black;
    PenSize = 1;
    dialog = new QColorDialog(this);
    BackAction->setDisabled(true);

    /* connect ToolBar actions to slots */
    connect(NewAction, SIGNAL(triggered()), this, SLOT(NewEmptyCanvas()));
    connect(OpenAction, SIGNAL(triggered()), this, SLOT(OpenNewPic()));
    connect(SaveAction, SIGNAL(triggered()), this, SLOT(SaveCurPic()));
    connect(BackAction, SIGNAL(triggered()), this, SLOT(Backwards()));
    connect(SizeSlider, &QSlider::sliderMoved, [=](int value){
        SizeLabel->setText(QString::number(value));
        PenSize = value;
    });
    connect(ColorButton, SIGNAL(clicked()), this, SLOT(ColorSelect()));

    /* connect MenuBar actions to slots */
    connect(ui->actionx, SIGNAL(triggered()), this, SLOT(NewEmptyCanvas()));
    connect(ui->actiond, SIGNAL(triggered()), this, SLOT(OpenNewPic()));
    connect(ui->actiond_3, SIGNAL(triggered()), this, SLOT(SaveCurPic()));
    connect(ui->actiond_2, SIGNAL(triggered()), this, SLOT(Backwards()));
    connect(ui->action, SIGNAL(triggered()), this, SLOT(SaveDrawPic()));

    //connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(DrawPaint()));
}
/* load pic to ui->label adapted to its size */
void MainWindow::LoadPic(QPixmap pic, QLabel* label)
{
    if(label == nullptr)
        return;
    pic = pic.scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    label->setPixmap(pic.scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
/* transform global pos to pos */
QPoint MainWindow::Position(const QPoint& glob)
{
    QPoint Lglobal = ui->label->mapToGlobal(QPoint(0,0));
    return glob - Lglobal;
}
QPoint MainWindow::PointStretch(const QPoint& a, const double& k1, const double& k2)
{
    return QPoint(a.x()*k1, a.y()*k2);
}
/* paint to a given pixmap */
void MainWindow::DrawPaint(QPixmap &pixmap, bool op = 0)
{
    QPainter painter(&pixmap);
    if(op)
    {
        painter.setRenderHint(QPainter::HighQualityAntialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
    }
    for(int i = 0; i < DrawInfo.size(); i++)
    {
        const QSize& drawsize = DrawInfo[i].DrawSize;
        const QVector<QPoint>& NewLine = DrawInfo[i].Lines;
        painter.setPen(QPen(DrawInfo[i].DrawColor, DrawInfo[i].Width));
        double k1 = (double)PicSize.width()/drawsize.width();
        double k2 = (double)PicSize.height()/drawsize.height();
        for(int j = 0; j < NewLine.size() - 1; j++)
            painter.drawLine(PointStretch(NewLine[j], k1, k2)
                       , PointStretch(NewLine[j + 1], k1, k2));
    }
    painter.end();
}
/* override paintEvent */
void MainWindow::paintEvent(QPaintEvent *)
{
    if(!Canvas)
        return;
    QPixmap TmpPix = CanvasPixmap.copy();
    DrawPaint(TmpPix);
    LoadPic(TmpPix, ui->label);
}
/* override mousePressEvent */
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        QPoint Lglobal = ui->label->mapToGlobal(QPoint(0,0));
        QRect lb(Lglobal, Lglobal+QPoint(ui->label->size().width(), ui->label->size().height()));
        if(!lb.contains(event->globalPos()))
            return;
        PressLeft = true;
        if(!Canvas)
            OpenNewPic();
        else
        {
            Info TmpInfo;
            QSize& drawsize = TmpInfo.DrawSize;
            QVector<QPoint>& NewLine = TmpInfo.Lines;
            NewLine.push_back(Position(event->globalPos()));
            drawsize = ui->label->size();
            TmpInfo.DrawColor = PenColor;
            TmpInfo.Width = PenSize;
            DrawInfo.push_back(TmpInfo);
            BackAction->setEnabled(true);
            ui->actiond_2->setEnabled(true);
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(!Canvas)
        return;
    setCursor(Qt::CrossCursor);
    if(PressLeft && DrawInfo.size())
    {
        QVector<QPoint>& NewLine = DrawInfo.last().Lines;
        NewLine.push_back(Position(event->globalPos()));
        update();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if(PressLeft)
    {
        if(Canvas)
        {
            QVector<QPoint>& NewLine = DrawInfo.last().Lines;
            NewLine.push_back(Position(event->globalPos()));
            update();
            setCursor(Qt::ArrowCursor);
        }
        PressLeft = false;
    }
}

void MainWindow::SaveCurPic()
{
    QString FileName = QFileDialog::getSaveFileName(this,
        tr("保存"), "NewPaint.png", "Image (*.png *.jpg *.bmp)");
    if(FileName.length() > 0)
    {
        QPixmap pixmap = CanvasPixmap.copy();
        DrawPaint(pixmap, 1);
        pixmap.save(FileName);
    }
    else
    {
        QMessageBox::critical(this, tr("错误提示"), tr("保存文件失败！"));
    }
}

void MainWindow::SaveDrawPic()
{
    QString FileName = QFileDialog::getSaveFileName(this,
        tr("保存"), "NewSketch.png", "Image (*.png *.jpg *.bmp)");
    if(FileName.length() > 0)
    {
        QPixmap pixmap(CanvasPixmap.size());
        pixmap.fill(Qt::white);
        DrawPaint(pixmap, 1);
        pixmap.save(FileName);
    }
    else
    {
        QMessageBox::critical(this, tr("错误提示"), tr("保存文件失败！"));
    }
}

void MainWindow::NewEmptyCanvas()
{
    CanvasPixmap = QPixmap(ui->label->size());
    CanvasPixmap.fill(Qt::white);
    PicSize = CanvasPixmap.size();
    Canvas = true;
    DrawInfo.clear();
    BackAction->setDisabled(true);
    ui->actiond_2->setDisabled(true);
    update();
}

void MainWindow::OpenNewPic()
{
    QString NewPicPath = QFileDialog::getOpenFileName(this,
        tr("打开"), "", "Image (*.png *.jpg *.bmp)");
    if(!NewPicPath.isEmpty())
    {
        CanvasPixmap.load(NewPicPath);
        if(CanvasPixmap.isNull())
        {
            return;
        }
        PicSize = CanvasPixmap.size();
        LoadPic(CanvasPixmap, ui->label);
        Canvas = true;
        DrawInfo.clear();
        BackAction->setDisabled(true);
        ui->actiond_2->setDisabled(true);
        update();
    }
    else {
        QMessageBox::critical(this, tr("错误提示"), tr("打开文件失败！"));
    }
}

void MainWindow::Backwards()
{
    if(DrawInfo.size())
        DrawInfo.pop_back();
    if(!DrawInfo.size())
    {
        BackAction->setDisabled(true);
        ui->actiond_2->setDisabled(true);
    }
}

void MainWindow::SetTmpColor(QColor color)
{
    if(color.isValid())
        TmpColor = color;
}

void MainWindow::SetColor()
{
    if(TmpColor.isValid())
        PenColor = TmpColor;
    QPixmap pix(30,30);
    pix.fill(PenColor);
    ColorIcon->setPixmap(pix);
    qDebug()<<PenColor;
}

void MainWindow::ColorSelect()
{
    TmpColor = PenColor;
    dialog->setCurrentColor(PenColor);
    dialog->show();
    connect(dialog, SIGNAL(colorSelected(QColor)), this, SLOT(SetTmpColor(QColor)));
    connect(dialog, SIGNAL(currentColorChanged(QColor)), this, SLOT(SetTmpColor(QColor)));
    connect(dialog, SIGNAL(accepted()), this, SLOT(SetColor()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QPixmap pix = CanvasPixmap.copy();
    pix.fill(Qt::white);
    DrawPaint(pix);
    LoadPic(pix, ui->label_2);
}
