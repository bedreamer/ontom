#include <QtGui>
#include <QtNetwork>
#include <QtWebKit>
#include "browserwindow.h"

browserwindow::browserwindow(const QUrl &url)
{
    view = new QWebView(this);

    if ( view ) {
        //view->load(url);
        connect(view, SIGNAL(loadFinished(bool)), SLOT(adjustLocation()));
        connect(view, SIGNAL(titleChanged(QString)), SLOT(adjustTitle()));
        connect(view, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
        connect(view, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));

        setCentralWidget(view);
    }
}

void browserwindow::getup(const QUrl &url)
{
    if ( view  ) {
        view->load(url);
    }
}

// load finish
void browserwindow::adjustLocation()
{
}

void browserwindow::adjustTitle()
{
    if (progress <= 0 || progress >= 100)
        setWindowTitle(view->title());
    else
        setWindowTitle(QString("%1 (%2%)").arg(view->title()).arg(progress));
}

void browserwindow::setProgress(int p)
{
    progress = p;
    adjustTitle();
}

void browserwindow::finishLoading(bool)
{
/*    progress = 100;
    adjustTitle();
    view->page()->mainFrame()->evaluateJavaScript(jQuery);

    rotateImages(rotateAction->isChecked());
*/
}
