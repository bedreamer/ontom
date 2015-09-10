#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <QtGui>

class QWebView;
QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

class browserwindow : public QMainWindow
{
    Q_OBJECT
public:
    browserwindow(const QUrl & url);
    void getup(const QUrl & url);

protected slots:
    void adjustLocation();
    void adjustTitle();
    void setProgress(int p);
    void finishLoading(bool);

private:
    QWebView *view;
    QAction *rotateAction;
    int progress;
};

#endif // BROWSERWINDOW_H
