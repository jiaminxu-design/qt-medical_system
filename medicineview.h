#ifndef MEDICINEVIEW_H
#define MEDICINEVIEW_H

#include <QWidget>
#include "ui_medicineview.h"
#include "idatabase.h"

class MedicineView : public QWidget
{
    Q_OBJECT

public:
    explicit MedicineView(QWidget *parent = nullptr);
    ~MedicineView();

signals:
    void goMedicineEditView(int rowNo);


private slots:
    void on_btSearch_clicked();
    void on_btAdd_clicked();
    void on_btEdit_clicked();
    void on_btDelete_clicked();
    void on_btStockIn_clicked();
    void on_btStockOut_clicked();

private:
    Ui::MedicineView *ui;
    IDatabase &db;
};

#endif // MEDICINEVIEW_H
