#include <QCoreApplication>

#include "click.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Click click;
    // click.start_request();

    return a.exec();
}
