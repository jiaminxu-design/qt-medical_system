#ifndef MEDICINEVIEW_H
#define MEDICINEVIEW_H

#include <QWidget>
#include "ui_medicineview.h"
#include "idatabase.h"
#include "StatThread.h"

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
    void on_btSyncRemote_clicked();

    void on_btStat_clicked(); // 新增：统计按钮槽函数
    void onStatFinished(const QVariantMap &statData); // 新增：接收统计结果

private:
    Ui::MedicineView *ui;
    IDatabase &db;

    StatThread *statThread; // 新增：统计线程对象
};

#endif // MEDICINEVIEW_H
