#include <QDebug>
#include <QFileDialog>
#include <QClipboard>
#include <QFontDatabase>
#include <QSettings>
#include <QActionGroup>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include "z80system.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_localEcho(false)
{
    ui->setupUi(this);
    m_console = new ANSIConsoleView(this);

    ui->mainLayout->addWidget(m_console);

    m_registerView = new RegisterView(this);
    ui->debugLayout->addWidget(m_registerView);

    m_disasmView = new DisasmView(this);
    ui->debugLayout->addWidget(m_disasmView);

    m_brkLayout = new QHBoxLayout();
    m_brkEnabled = new QCheckBox("Enable");
    m_brkEnabled->setFocusPolicy(Qt::NoFocus);
    m_brkLayout->addWidget(m_brkEnabled, 0);
    m_brkAddressEdit = new QLineEdit();
    m_brkLayout->addWidget(m_brkAddressEdit,1);
    ui->debugLayout->addLayout(m_brkLayout);

    connect(m_brkEnabled, SIGNAL(toggled(bool)), this, SLOT(breakpointEnableTriggered(bool)));

    // setup the status bar
    m_filenameLabel = new QLabel();
    m_runStateLabel = new QLabel();
    ui->statusBar->addPermanentWidget(m_runStateLabel, 1);
    ui->statusBar->addPermanentWidget(m_filenameLabel, 4);

    // setup Z80 system and connect
    // the signals and slots
    m_sys = new Z80SystemThread(m_console, this);
    m_sys->start();

    m_debugTimer = new QTimer(this);
    connect(m_debugTimer, SIGNAL(timeout()), this, SLOT(onDebugTimer()));
    m_debugTimer->start(500);

    // setup a mutually exclusive group for
    // system clock speed

    QActionGroup *clockspeedGroup = new QActionGroup(this);
    clockspeedGroup->addAction(ui->actionClock_at_7_MHz);
    clockspeedGroup->addAction(ui->actionClock_at_20_MHz);
    ui->actionClock_at_7_MHz->setChecked(true);

    // setup a mutually exclusive group for
    // memory expansion type

    QActionGroup *sysconfigGroup = new QActionGroup(this);
    sysconfigGroup->addAction(ui->action32K_RAM_32K_ROM);
    sysconfigGroup->addAction(ui->action64K_RAM);
    ui->action32K_RAM_32K_ROM->setChecked(true);

    ui->actionLocal_echo->setCheckable(true);
    ui->actionLocal_echo->setActionGroup(NULL);

    m_disasmView->setModel(m_sys->getSystemPtr());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    const char ANSI_BS = 8;

    // handle paste events
    if (event->matches(QKeySequence::Paste))
    {
        const QClipboard *clipboard = QApplication::clipboard();
        if (clipboard != 0)
        {
            QString txt = clipboard->text();
            uint8_t prev = 0xff;
            if (!txt.isEmpty())
            {
                QByteArray array = txt.toLocal8Bit();
                for(int32_t i=0; i<array.size(); i++)
                {
                    if ((prev == 10) && ((uint16_t)array[i] != 13))
                    {
                        m_sys->putSerialData(13);
                    }
                    m_sys->putSerialData(array[i]);
                    prev = array[i];
                }
            }
        }
        return;
    }

    // handle regular key events but skip
    if (m_console)
    {
        switch(event->key())
        {
        case Qt::Key_Backspace:
            m_sys->putSerialData(ANSI_BS);
            if (m_localEcho)
            {
                m_console->submitByte(ANSI_BS);
            }
            break;
        case Qt::Key_Return:
            m_sys->putSerialData(13);
            m_sys->putSerialData(10);
            if (m_localEcho)
            {
                m_console->submitByte(13);
                m_console->submitByte(10);
            }
            break;
        default:
            QByteArray array = event->text().toLocal8Bit();
            if (array.size() != 0)
            {
                m_sys->putSerialData(array[0]);
                if (m_localEcho)
                    m_console->submitByte(array[0]);
            }
            break;
        }
    }
}

/** set CPU to run or halt */
void MainWindow::updateRunState(bool state)
{
    if (state == true)
    {
        m_runStateLabel->setText("System: Running");
    }
    else
    {
        m_runStateLabel->setText("System: Halted");
    }
}

void MainWindow::onDebugTimer()
{
    if (m_sys == 0)
        return;

    uint16_t address = m_sys->getRegister(Z80SystemBase::REG_PC);

    m_registerView->setRegisterValue(0, m_sys->getRegister(Z80SystemBase::REG_A));
    m_registerView->setRegisterValue(1, m_sys->getRegister(Z80SystemBase::REG_B));
    m_registerView->setRegisterValue(2, m_sys->getRegister(Z80SystemBase::REG_C));
    m_registerView->setRegisterValue(3, m_sys->getRegister(Z80SystemBase::REG_D));
    m_registerView->setRegisterValue(4, m_sys->getRegister(Z80SystemBase::REG_E));
    m_registerView->setRegisterValue(5, m_sys->getRegister(Z80SystemBase::REG_H));
    m_registerView->setRegisterValue(6, m_sys->getRegister(Z80SystemBase::REG_L));
    m_registerView->setRegisterValue(7, m_sys->getRegister(Z80SystemBase::REG_BC));
    m_registerView->setRegisterValue(8, m_sys->getRegister(Z80SystemBase::REG_DE));
    m_registerView->setRegisterValue(9, m_sys->getRegister(Z80SystemBase::REG_HL));
    m_registerView->setRegisterValue(10, m_sys->getRegister(Z80SystemBase::REG_IX));
    m_registerView->setRegisterValue(11, m_sys->getRegister(Z80SystemBase::REG_IY));
    m_registerView->setRegisterValue(12, m_sys->getRegister(Z80SystemBase::REG_SP));
    m_registerView->setRegisterValue(13, address);
    m_registerView->update();

    m_disasmView->setViewParameters(address, address, 20);

    updateRunState(m_sys->isCPURunning());
}

void MainWindow::on_actionLoad_ROM_triggered()
{
    // load previous ROM loading location from a persitent storage
    // using QSettings class

    QSettings settings;

    QString loadPath = settings.value("paths/prevromlocation","").toString();

    QString romfile = QFileDialog::getOpenFileName(this,
         tr("Open ROM file"), loadPath, tr("ROM Files (*.bin *.rom)"));

    if (!romfile.isEmpty())
    {
        m_sys->loadROM(romfile.toStdString().c_str());
        m_sys->reset();
        m_filenameLabel->setText("Loaded: " + romfile);

        // set the prevromlocation to the location we
        // loaded this ROM from
        QFileInfo info(romfile);
        settings.setValue("paths/prevromlocation", info.path());
    }
}

void MainWindow::breakpointEnableTriggered(bool state)
{
    if (state)
    {
        // check if the validity of the breakpoint address
        QString txt = m_brkAddressEdit->text();
        bool conversionOk = false;
        int address = txt.toInt(&conversionOk, 16);
        if (conversionOk)
        {
            m_sys->setBreakpoint(address); // disable
        }
        else
        {
            m_brkEnabled->setCheckState(Qt::Unchecked);
            m_sys->setBreakpoint(-1); // disable
        }
    }
    else
    {
        m_sys->setBreakpoint(-1); // disable
    }
}

void MainWindow::on_actionReset_triggered()
{
    m_sys->reset();
}

void MainWindow::on_actionQuit_triggered()
{
    m_sys->quit();
    m_sys->wait(5000);  //max 5. second wait
    close();
}

void MainWindow::on_actionHalt_triggered()
{
    m_sys->setCPUState(false);
}


void MainWindow::on_actionResume_triggered()
{
    m_sys->setCPUState(true);
}

void MainWindow::on_actionReset_terminal_triggered()
{
    m_console->reset();
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog *dialog = new AboutDialog(this);
    dialog->exec();
}

void MainWindow::on_actionClock_at_7_MHz_triggered()
{
    m_sys->setClockRate(7000000);
}

void MainWindow::on_actionClock_at_20_MHz_triggered()
{
    m_sys->setClockRate(20000000);
}

void MainWindow::on_action32K_RAM_32K_ROM_triggered()
{
    RC2014System* ptr = (RC2014System*)m_sys->getSystemPtr();
    ptr->setSystemType(RC2014System::SYS_32KRAM);
}

void MainWindow::on_action64K_RAM_triggered()
{
    RC2014System* ptr = (RC2014System*)m_sys->getSystemPtr();
    ptr->setSystemType(RC2014System::SYS_64KRAM);
}


void MainWindow::on_actionLocal_echo_triggered(bool checked)
{
    m_localEcho = checked;
}
