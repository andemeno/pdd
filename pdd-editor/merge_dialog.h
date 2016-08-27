#ifndef MERGE_DIALOG_H
#define MERGE_DIALOG_H

#include <QDialog>
#include <QSharedPointer>

namespace pdd {
class pdd_db;
}

namespace Ui {
class merge_dialog;
}

class QCheckBox;

class merge_dialog : public QDialog {
    Q_OBJECT
public:
    explicit merge_dialog(const QSharedPointer<pdd::pdd_db> doc, QWidget *parent = 0);
    ~merge_dialog();

    std::map<uint, std::pair<uint, uint> > get_data_to_merge();

private slots:

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    std::map<uint, std::pair<uint, uint> > to_merge;
    Ui::merge_dialog *ui;
    std::vector<QCheckBox*> themes;
};

#endif // MERGE_DIALOG_H
