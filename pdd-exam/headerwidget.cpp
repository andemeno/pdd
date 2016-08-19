#include "headerwidget.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPixmap>
#include "databox.h"
#include "programconfig.h"

using namespace pdd;

HeaderWidget::HeaderWidget(QWidget *parent)
    : QWidget(parent) {

    QVBoxLayout* layout = new QVBoxLayout;
    QHBoxLayout* topLayout = new QHBoxLayout;

    armLabel = new QLabel;
    armLabel->setAlignment(Qt::AlignHCenter);
    mainLabel = new QLabel;
    mainLabel->setAlignment(Qt::AlignHCenter);
    timerLabel = new QLabel;
    timerLabel->setAlignment(Qt::AlignHCenter);
    resultLabel = new QLabel;
    resultLabel->setAlignment(Qt::AlignHCenter);
    resultLabel->hide();

    topLayout->addWidget(armLabel);
    topLayout->addWidget(mainLabel, 1);
    topLayout->addWidget(timerLabel);
    layout->addLayout(topLayout);
    layout->addWidget(resultLabel);

    setLayout(layout);

    DataBox& box = DataBox::inst();
    connect(&box, SIGNAL(successTask()), this, SLOT(onSuccesTask()));
    connect(&box, SIGNAL(failTask()), this, SLOT(onFailTask()));

}

void HeaderWidget::setMainText(const QString& text) {
    mainLabel->setText(text.toUpper());
}

void HeaderWidget::setArmNumber(const uint n) {
    if(n==0)
        armLabel->setText("");
    else
        armLabel->setText(QString("АРМ\n%1").arg(n));
}

void HeaderWidget::setTimerText(const QString& text) {
    timerLabel->setText(text.toUpper());
    if(text.isEmpty())
        resultLabel->hide();
}

void HeaderWidget::onSuccesTask() {
    QPixmap pixmap(Config::inst().succesedTaskImage.c_str());
    resultLabel->setPixmap(pixmap);
    resultLabel->show();
}

void HeaderWidget::onFailTask() {
    QPixmap pixmap(Config::inst().failedTaskImage.c_str());
    resultLabel->setPixmap(pixmap);
    resultLabel->show();
}
