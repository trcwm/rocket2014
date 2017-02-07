#include <QDebug>
#include <QFileDialog>
#include <QClipboard>
#include <QFontDatabase>
#include <QSettings>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_console = new ConsoleView(this);
    ui->mainLayout->addWidget(m_console);

    m_registerView = new RegisterView(this);
    ui->debugLayout->addWidget(m_registerView);

    m_disasmView = new DisasmView(this);
    ui->debugLayout->addWidget(m_disasmView);

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

    // handle regular key events
    if (m_console)
    {
        switch(event->key())
        {
        case Qt::Key_Backspace:
            m_sys->putSerialData(ANSI_BS);
            break;
        default:
            QByteArray array = event->text().toLocal8Bit();
            m_sys->putSerialData(array[0]);
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

    uint16_t address = m_sys->getRegister(Z80System::REG_PC);

    m_registerView->setRegisterValue(0, m_sys->getRegister(Z80System::REG_A));
    m_registerView->setRegisterValue(1, m_sys->getRegister(Z80System::REG_B));
    m_registerView->setRegisterValue(2, m_sys->getRegister(Z80System::REG_C));
    m_registerView->setRegisterValue(3, m_sys->getRegister(Z80System::REG_D));
    m_registerView->setRegisterValue(4, m_sys->getRegister(Z80System::REG_E));
    m_registerView->setRegisterValue(5, m_sys->getRegister(Z80System::REG_H));
    m_registerView->setRegisterValue(6, m_sys->getRegister(Z80System::REG_L));
    m_registerView->setRegisterValue(7, m_sys->getRegister(Z80System::REG_BC));
    m_registerView->setRegisterValue(8, m_sys->getRegister(Z80System::REG_DE));
    m_registerView->setRegisterValue(9, m_sys->getRegister(Z80System::REG_HL));
    m_registerView->setRegisterValue(10, m_sys->getRegister(Z80System::REG_IX));
    m_registerView->setRegisterValue(11, m_sys->getRegister(Z80System::REG_IY));
    m_registerView->setRegisterValue(12, m_sys->getRegister(Z80System::REG_SP));
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


void MainWindow::on_actionReset_triggered()
{
    m_sys->reset();
}

void MainWindow::on_actionQuit_triggered()
{
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
