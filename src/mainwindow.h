#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTextEdit>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QSettings>

#include "z80systemthread.h"
#include "ansiconsoleview.h"
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
    void breakpointEnableTriggered(bool state);

    void on_actionLoad_ROM_triggered();

    void on_actionReset_triggered();

    void on_actionQuit_triggered();

    void on_actionHalt_triggered();

    void on_actionResume_triggered();

    void on_actionReset_terminal_triggered();

    void on_actionAbout_triggered();

    void on_actionClock_at_7_MHz_triggered();

    void on_actionClock_at_20_MHz_triggered();

    void on_action32K_RAM_32K_ROM_triggered();

    void on_action64K_RAM_triggered();

    void on_actionLocal_echo_triggered();

private:
    /** update the run state display in the GUI */
    void updateRunState(bool state);

    Ui::MainWindow  *ui;
    ANSIConsoleView *m_console;
    RegisterView    *m_registerView;
    DisasmView      *m_disasmView;
    Z80SystemThread *m_sys;

    //permanent controls for the status bar
    QLabel          *m_filenameLabel;
    QLabel          *m_runStateLabel;

    //breakpoint address setting
    QHBoxLayout     *m_brkLayout;
    QCheckBox       *m_brkEnabled;
    QLineEdit       *m_brkAddressEdit;

    // timer for updating the debug displays
    QTimer          *m_debugTimer;

    // local echo for the console
    bool m_localEcho;
};

#endif // MAINWINDOW_H
