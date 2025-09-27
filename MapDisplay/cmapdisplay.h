#ifndef CMAPDISPLAY_H
#define CMAPDISPLAY_H

#include <QWidget>

namespace Ui {
class CMapDisplay;
}

class CMapDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit CMapDisplay(QWidget *parent = nullptr);
    ~CMapDisplay();

private:
    Ui::CMapDisplay *ui;
};

#endif // CMAPDISPLAY_H
