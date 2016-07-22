#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTextEdit>
#include "z80systemthread.h"
#include "consoleview.h"
#include "registerwindow.h"
#include "disasmview.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void keyPressEvent(QKeyEvent *event);
    void onDebugTimer();

private slots:
    void on_actionLoad_ROM_triggered();

    void on_actionReset_triggered();

private:
    Ui::MainWindow  *ui;
    ConsoleView     *m_console;
    RegisterView    *m_registerView;
    DisasmView      *m_disasmView;
    Z80SystemThread *m_sys;

    QTimer          *m_debugTimer;
};

#endif // MAINWINDOW_H
