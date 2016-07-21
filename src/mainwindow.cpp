#include <QDebug>
#include <QFileDialog>
#include <QClipboard>
#include <QFontDatabase>

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

    m_disasm = new QTextEdit(this);
    ui->debugLayout->addWidget(m_disasm);

    // setup Z80 system and connect
    // the signals and slots
    m_sys = new Z80SystemThread(m_console, this);
    if (!m_sys->loadROM("basic32.rom"))
    {
        qDebug() << "Loading of ROM failed!";
    }

    m_sys->start();    

    m_debugTimer = new QTimer(this);
    connect(m_debugTimer, SIGNAL(timeout()), this, SLOT(onDebugTimer()));
    m_debugTimer->start(500);

    QFont disasmFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    QFontMetrics disasmFontMetrics(disasmFont);
    m_disasm->setFont(disasmFont);
    m_disasm->setMinimumWidth(disasmFontMetrics.width("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"));
    //m_disasm->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::ExpandFlag);
    m_disasm->updateGeometry();
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

void MainWindow::onDebugTimer()
{
    if (m_sys == 0)
        return;

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
    m_registerView->setRegisterValue(13, m_sys->getRegister(Z80SystemBase::REG_PC));
    m_registerView->update();

    char buffer[1024];
    m_sys->getDisassembly(buffer, sizeof(buffer), m_sys->getRegister(Z80SystemBase::REG_PC), 10);
    if (m_disasm != 0)
    {
        m_disasm->setText(buffer);
    }
}

void MainWindow::on_actionLoad_ROM_triggered()
{
    QString romfile = QFileDialog::getOpenFileName(this,
         tr("Open ROM file"), "", tr("ROM Files (*.bin *.rom)"));

    if (!romfile.isEmpty())
    {
        m_sys->loadROM(romfile.toStdString().c_str());
        m_sys->reset();
    }
}

