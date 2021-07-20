#include "wavplayer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WavPlayer w;
    w.show();

    return a.exec();
}
