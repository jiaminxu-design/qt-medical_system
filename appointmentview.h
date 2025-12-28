#ifndef APPOINTMENTVIEW_H
#define APPOINTMENTVIEW_H

#include <QWidget>
#include <QSqlTableModel>
#include <QItemSelectionModel>

namespace Ui {
class AppointmentView;
}

class AppointmentView : public QWidget
{
    Q_OBJECT

public:
    explicit AppointmentView(QWidget *parent = nullptr);
    ~AppointmentView() override;

    void initUI();
public slots:
    void refreshTable(); // 刷新表格

signals:
    void goAppointmentEditView(int rowNo, const QDate &date);
private slots:
    void on_btAdd_clicked();      // 新增预约
    void on_btEdit_clicked();     // 编辑预约
    void on_btDelete_clicked();   // 删除预约
    void on_btFilter_clicked();   // 筛选预约

private:
    Ui::AppointmentView *ui;
    void loadDoctorList();        // 加载医生筛选下拉框
};

#endif // APPOINTMENTVIEW_H

