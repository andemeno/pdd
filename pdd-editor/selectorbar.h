#ifndef SELECTORBAR_H
#define SELECTORBAR_H

#include <QWidget>
#include <QSharedPointer>

class QComboBox;
class QSpinBox;

namespace pdd {
class pdd_db;
}

class SelectorBar : public QWidget {
    Q_OBJECT
public:
    explicit SelectorBar(QSharedPointer<pdd::pdd_db> db, QWidget *parent = 0);

signals:
    void show_question(uint theme, uint number);

public slots:
    void init_question_box(const int theme) const;
    void on_show_question();

private slots:
    void on_prev();
    void on_next();
    void on_remove();

private:
    QComboBox* themeBox;
    QSpinBox* numberBox;
    QSharedPointer<pdd::pdd_db> doc;
};

#endif // SELECTORBAR_H
