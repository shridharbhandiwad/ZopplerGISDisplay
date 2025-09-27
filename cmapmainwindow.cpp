#include "cmapmainwindow.h"
#include "ui_cmapmainwindow.h"
#include "cdatawarehouse.h"
#include <QFileDialog>
#include <QTableWidget>
#include <QHeaderView>

CMapMainWindow::CMapMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CMapMainWindow)
{
    ui->setupUi(this);
    ui->widgetMapCanvas->Initialize();
    setWindowTitle("Radar Display");

    // Create floating/minimizable docks for controls and track table
    m_dockControls = new QDockWidget("Controls", this);
    m_dockControls->setObjectName("dockControls");
    m_dockControls->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    m_dockControls->setWidget(ui->widget_TextualControl);
    addDockWidget(Qt::RightDockWidgetArea, m_dockControls);

    m_dockTracks = new QDockWidget("Tracks", this);
    m_dockTracks->setObjectName("dockTracks");
    m_dockTracks->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    m_dockTracks->setWidget(ui->tableWidget);
    addDockWidget(Qt::RightDockWidgetArea, m_dockTracks);

    // Make docks tabbed for cleaner UI when docked on same edge
    tabifyDockWidget(m_dockControls, m_dockTracks);
    m_dockControls->raise();

    // Extend styles to docks for a cohesive, attractive look
    QString dockStyle =
        "QDockWidget { border: 1px solid #00B8D4; background-color: #050D1A; }"
        "QDockWidget::title { background: #00B8D4; color: white; padding: 6px; }";
    this->setStyleSheet(this->styleSheet() + dockStyle);

    // Initialize the table rows/items
    ui->tableWidget->setRowCount(100);
    for ( int i = 0; i < ui->tableWidget->rowCount(); i++ ) {
        for ( int j = 0; j < ui->tableWidget->columnCount(); j++ ) {
            QTableWidgetItem *item = new QTableWidgetItem("");
            item->setForeground(QBrush(Qt::white));
            item->setFont(QFont("century",12,80,false));
            item->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(i,j,item);
        }
    }

    connect(&_m_updateTimer, &QTimer::timeout, this, [this]() {
           updateTrackTable();  // Trigger repaint
       });
    _m_updateTimer.start(1000);


    m_dockControls->hide();
    connect(ui->widgetMapCanvas, SIGNAL(signalMouseRead(QString)), this, SLOT(slotMouseRead(QString)));
}

CMapMainWindow::~CMapMainWindow()
{
    delete ui;
}

void CMapMainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_F:
        m_dockControls->setVisible(!m_dockControls->isVisible());
        break;
    case Qt::Key_T:
        m_dockTracks->setVisible(!m_dockTracks->isVisible());
        break;
    case Qt::Key_H:
        ui->widgetMapCanvas->mapHome();;;
        break;

   }
    QMainWindow::keyPressEvent(event);
}

void CMapMainWindow::slotMouseRead( QString mouseRead ) {
    ui->statusBar->showMessage(mouseRead);
}

void CMapMainWindow::on_pushButton_MapHome_clicked()
{
    ui->widgetMapCanvas->mapHome();
}

void CMapMainWindow::on_pushButton_OpenMaps_clicked()
{
    QString filter = "Raster Files (*.tif *.tiff *.png *.bmp *.jpg *.TIF *.TIFF *.PNG *.BMP *.JPG)";

    QString rasterPath = QFileDialog::getOpenFileName(
                this,
                "Select Raster or World-Referenced Image",
                QString(),
                filter
                );

    if (rasterPath.isEmpty())
        return;

    //ui->widgetMapCanvas->openRasterMap(rasterPath);
    ui->widgetMapCanvas->importRasterMap(rasterPath);
}

void CMapMainWindow::on_pushButton_FLUSH_clicked()
{

}

void CMapMainWindow::on_pushButton_EXIT_clicked()
{
    exit(0);
}


void CMapMainWindow::updateTrackTable() {
    QList<stTrackDisplayInfo> listTracks = CDataWarehouse::getInstance()->getTrackList();

    //ui->tableWidget->clear();
    int row = 0;
    for (const stTrackDisplayInfo &track : listTracks) {
        ui->tableWidget->item(row, 0)->setText(QString::number(track.nTrkId));
        ui->tableWidget->item(row, 1)->setText(QString::number(track.range, 'f', 0));
        ui->tableWidget->item(row, 2)->setText(QString::number(track.azimuth, 'f', 2));
        ui->tableWidget->item(row, 3)->setText(QString::number(track.elevation, 'f', 1));
        ui->tableWidget->item(row, 4)->setText(QString::number(track.heading, 'f', 1));
        ui->tableWidget->item(row, 5)->setText(QString::number(track.velocity, 'f', 1));
        ui->tableWidget->item(row, 6)->setText(QString::number(track.snr, 'f', 1));
        ++row;
    }
}
