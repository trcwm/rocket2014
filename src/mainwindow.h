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
    void on_actionLoad_ROM_triggered();

    void on_actionReset_triggered();

    void on_actionQuit_triggered();

    void on_actionHalt_triggered();

    void on_actionResume_triggered();

    void breakpointEnableTriggered(bool state);

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
};

#endif // MAINWINDOW_H
