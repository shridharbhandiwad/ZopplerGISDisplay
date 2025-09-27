#ifndef CMAPMAINWINDOW_H
#define CMAPMAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

namespace Ui {
class CMapMainWindow;
}

class CMapMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CMapMainWindow(QWidget *parent = nullptr);
    ~CMapMainWindow();

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void slotMouseRead(QString mouseRead);
    void on_pushButton_MapHome_clicked();

    void on_pushButton_OpenMaps_clicked();

    void on_pushButton_FLUSH_clicked();

    void on_pushButton_EXIT_clicked();

    void updateTrackTable();
private:
    Ui::CMapMainWindow *ui;
    QTimer _m_updateTimer;
};

#endif // CMAPMAINWINDOW_H
