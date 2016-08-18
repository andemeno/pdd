#ifndef QUESTION_FORM_H
#define QUESTION_FORM_H

#include <QWidget>
#include <question.h>

class QTableWidget;

namespace pdd {

class pdd_db;

class question_form : public QWidget {
    Q_OBJECT
public:
    explicit question_form(pdd_db* db, const uint th_n, const uint n, QWidget *parent = 0);
signals:
    void question_saved();

private slots:
    void on_cell_dbl_clicked(const int row, const int column);
    void on_save();

private:
    pdd_db* doc;
    question quest;
    QTableWidget* table;
};

}

#endif // QUESTION_FORM_H
