#include <QDebug>
#include <QFileDialog>
#include <QClipboard>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_console = new ConsoleView(this);
    ui->mainLayout->addWidget(m_console);

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

    uint16_t pc = m_sys->getPC();

    QString strPC = QString::asprintf("%04X", pc);
    ui->PC_display->setText(strPC);
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

