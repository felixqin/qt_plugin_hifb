#include "mainwindow.h"
#include <stdio.h>
#include <QApplication>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Load the embedded font.
    QFontDatabase::removeAllApplicationFonts();
    QString fontPath = "/mnt/data/Fonts/simsun/simsun.ttf";
    int fontId = QFontDatabase::addApplicationFont(fontPath);
    printf("fontPath(%s) fontId(%d)\n", fontPath.toStdString().c_str(), fontId);

    if (fontId != -1)
    {
        QFont font("simsun");
        a.setFont(font);
    }

    printf("hello window!\n");
    MainWindow w;
    w.show();

    printf("hello qt!\n");
    return a.exec();
}
