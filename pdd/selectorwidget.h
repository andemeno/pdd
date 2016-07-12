#ifndef SELECTORWIDGET_H
#define SELECTORWIDGET_H

#include <QWidget>

class QButtonGroup;
class QPushButton;
class QComboBox;

namespace pdd {

class SelectorWidget : public QWidget {
    Q_OBJECT
public:
    explicit SelectorWidget(QWidget *parent = 0);
    uint getThemeNumber() const;

signals:
    void startTask(bool cat_ab, uint count);
    void startTraining(bool cat_ab, uint theme);

private slots:
    void onSelect(int id);
    void onStartClick();
    void onStopClick();

private:

    void makeGUI();

    uint choise;
    QComboBox* abThemesCombo;
    //QComboBox* cdThemesCombo;
    QButtonGroup* selector;
    QPushButton* startButton;
    QPushButton* exitButton;
};

}

#endif // SELECTORWIDGET_H
