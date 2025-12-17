#ifndef MEDICINEVIEW_H
#define MEDICINEVIEW_H

#include <QWidget>

namespace Ui {
class MedicineView;
}

class MedicineView : public QWidget
{
    Q_OBJECT

public:
    explicit MedicineView(QWidget *parent = nullptr);
    ~MedicineView();

private:
    Ui::MedicineView *ui;
};

#endif // MEDICINEVIEW_H
