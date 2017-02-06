#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTextEdit>
#include <QLabel>
#include <QSettings>

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

    void on_actionQuit_triggered();

    void on_actionHalt_triggered();

    void on_actionResume_triggered();

private:
    /** set CPU to run or halt */
    void setRunState(bool state);

    Ui::MainWindow  *ui;
    ConsoleView     *m_console;
    RegisterView    *m_registerView;
    DisasmView      *m_disasmView;
    Z80SystemThread *m_sys;

    //permanent controls for the status bar
    QLabel          *m_filenameLabel;
    QLabel          *m_runStateLabel;

    // timer for updating the debug displays
    QTimer          *m_debugTimer;
};

#endif // MAINWINDOW_H
