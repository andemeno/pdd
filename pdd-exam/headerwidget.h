#ifndef HEADERWIDGET_H
#define HEADERWIDGET_H

#include <QWidget>

class QLabel;

namespace pdd {

class HeaderWidget : public QWidget {
    Q_OBJECT
public:
    explicit HeaderWidget(QWidget *parent = 0);

signals:
public slots:

    void setMainText(const QString& text);
    void setArmNumber(const uint n);
    void setTimerText(const QString& text);

private slots:
    void onSuccesTask();
    void onFailTask();
private:
    QLabel* mainLabel;
    QLabel* armLabel;
    QLabel* timerLabel;
    QLabel* resultLabel;
};

}

#endif // HEADERWIDGET_H
