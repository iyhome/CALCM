#include "CALCM.h"
#include <QStringConverter>
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CALCM w;
    w.show();
    return a.exec();
}
