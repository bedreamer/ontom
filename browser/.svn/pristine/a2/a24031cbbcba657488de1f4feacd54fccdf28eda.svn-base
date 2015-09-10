#include <QtGui>
#include <iostream>
using namespace std;
#include "browserwindow.h"

int main(int argc, char *argv[])
{
    QApplication *thiz = new QApplication(argc, argv);
    QUrl url;
    browserwindow *browser;

    if ( thiz == NULL ) return 1;
    if ( argc > 1 ) {
        url = QUrl(argv[1]);
    } else {
        url = QUrl("http://127.0.0.1:8080/");
    }

    browser = new browserwindow(url);
    if ( browser == NULL ) return 1;

    browser->getup(url);
    browser->showFullScreen();

    return thiz->exec();
}
