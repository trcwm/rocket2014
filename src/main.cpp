#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("MoseleyInstruments");
    QCoreApplication::setOrganizationDomain("moseleyinstruments.com");
    QCoreApplication::setApplicationName("Rocket2014");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
