#include "selectorwidget.h"
#include <QButtonGroup>
#include <QRadioButton>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QApplication>
#include <QMessageBox>
#include "databox.h"

using namespace pdd;

SelectorWidget::SelectorWidget(QWidget *parent)
    : QWidget(parent)
    , choise( 0 ) {

    makeGUI();

    connect( selector, SIGNAL(buttonClicked(int)), this, SLOT(onSelect(int)) );
    connect( startButton, SIGNAL(clicked(bool)), this, SLOT(onStartClick()) );
    connect( exitButton, SIGNAL(clicked(bool)), this, SLOT(onStopClick()) );
}


void SelectorWidget::onSelect(int id) {
    choise = id;
}


void SelectorWidget::onStartClick() {
    if(choise == 1) emit startTraining(true, abThemesCombo->currentIndex());
    else if(choise == 2) emit startTask(true, 20);
    else if(choise == 3) emit startTask(true, 40);
//    else if(choise == 4) emit startTraining(false, cdThemesCombo->currentIndex());
//    else if(choise == 5) emit startTask(false, 20);
//    else if(choise == 6) emit startTask(false, 40);
    else {
        QMessageBox msgBox;
        msgBox.setText( "Для продолжения нужно выбрать задание!" );
        msgBox.exec();
    }
}


void SelectorWidget::onStopClick() {
    qApp->quit();
}


void SelectorWidget::makeGUI() {

    QVBoxLayout* mainLayout = new QVBoxLayout;

    QString promptText("Выберете задание и нажмите кнопку \'НАЧАТЬ\'");
    QLabel* promptLabel = new QLabel(promptText);
    mainLayout->addWidget( promptLabel, 0, Qt::AlignHCenter );
    mainLayout->addSpacing( 40 );

    selector = new QButtonGroup;

    QRadioButton* btn = new QRadioButton( "Ознакомление с вопросами категории AB. Выберете тему:" );
    selector->addButton( btn, 1 );
    mainLayout->addWidget( btn );
    abThemesCombo = new QComboBox;
    connect(abThemesCombo, SIGNAL(activated(int)), btn, SLOT(click()));
    QStringList abthemes = DataBox::inst().getThemes(true);
    for(int i=0; i<abthemes.size(); i++) {
        abThemesCombo->addItem( abthemes[i] );
    }
    mainLayout->addWidget( abThemesCombo );

    btn = new QRadioButton( "Тест на 20 вопросов (АВ). Время выполнения 20 минут." );
    selector->addButton( btn, 2 );
    mainLayout->addWidget( btn );

    btn = new QRadioButton( "Тест на 40 вопросов (АВ). Время выполнения 30 минут." );
    selector->addButton( btn, 3 );
    mainLayout->addWidget( btn );

//    mainLayout->addSpacing( 30 );

//    btn = new QRadioButton( "Ознакомление с вопросами категории CD. Выберете тему:" );
//    selector->addButton( btn, 4 );
//    mainLayout->addWidget( btn );
//    cdThemesCombo = new QComboBox;
//    connect(cdThemesCombo, SIGNAL(activated(int)), btn, SLOT(click()));
//    QStringList cdthemes = DataBox::inst().getThemes(false);
//    for(int i=0; i<cdthemes.size(); i++) {
//        cdThemesCombo->addItem( cdthemes[i] );
//    }
//    mainLayout->addWidget( cdThemesCombo );

//    btn = new QRadioButton( "Тест на 20 вопросов (CD). Время выполнения 20 минут." );
//    selector->addButton( btn, 5 );
//    mainLayout->addWidget( btn );

//    btn = new QRadioButton( "Тест на 40 вопросов (CD). Время выполнения 30 минут." );
//    selector->addButton( btn, 6 );
//    mainLayout->addWidget( btn );

    mainLayout->addSpacing( 50 );

    exitButton = new QPushButton( tr("ВЫХОД") );
    startButton = new QPushButton( tr("НАЧАТЬ") );
    QHBoxLayout* navLayout = new QHBoxLayout;
    navLayout->addStretch( 1 );
    navLayout->addWidget( exitButton );
    navLayout->addWidget( startButton );
    navLayout->addStretch( 1 );
    mainLayout->addLayout( navLayout );

    QGroupBox* gbox = new QGroupBox(this);
    mainLayout->setSizeConstraint( QLayout::SetFixedSize );
    gbox->setLayout( mainLayout );

    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget( gbox, 0, Qt::AlignCenter );
    setLayout( layout );
}
