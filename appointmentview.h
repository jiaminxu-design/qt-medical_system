//#ifndef APPOINTMENTVIEW_H
//#define APPOINTMENTVIEW_H

//#include <QWidget>

//namespace Ui {
//class AppointmentView;
//}

//class AppointmentView : public QWidget
//{
//    Q_OBJECT

//public:
//    explicit AppointmentView(QWidget *parent = nullptr);
//    ~AppointmentView();

//private:
//    Ui::AppointmentView *ui;
//};

//#endif // APPOINTMENTVIEW_H

//#ifndef APPOINTMENTEDITVIEW_H
//#define APPOINTMENTEDITVIEW_H

//#include <QWidget>
//#include <QDataWidgetMapper>

//namespace Ui {
//class AppointmentEditView;
//}

//class AppointmentEditView : public QWidget
//{
//    Q_OBJECT

//public:
//    explicit AppointmentEditView(QWidget *parent = nullptr, int rowNo = -1);
//    ~AppointmentEditView() override;

//signals:
//    void appointmentSaved(); // 保存成功信号
//    void goPreviousView();   // 返回信号

//private slots:
//    void on_btSave_clicked();
//    void on_btCancel_clicked();

//private:
//    Ui::AppointmentEditView *ui;
//    int m_rowNo = -1;
//    QDataWidgetMapper *m_mapper = nullptr;

//    void initUI();
//    void loadComboBoxData();
//    bool validateForm();
//};

//#endif // APPOINTMENTEDITVIEW_H

//#ifndef APPOINTMENTVIEW_H
//#define APPOINTMENTVIEW_H

//#include <QWidget>
//#include <QSqlTableModel>
//#include <QItemSelectionModel>

//namespace Ui {
//class AppointmentView;
//}

//class AppointmentView : public QWidget
//{
//    Q_OBJECT

//public:
//    explicit AppointmentView(QWidget *parent = nullptr);
//    ~AppointmentView() override;

//    void initUI();
//public slots:
//    void refreshTable();

//signals:
//    void goAppointmentEditView(int rowNo);

//private slots:
//    void on_btAdd_clicked();
//    void on_btEdit_clicked();
//    void on_btDelete_clicked();
//    void on_btFilter_clicked();
//    void on_btConfirm_clicked();
//    void on_btCancel_clicked();

//private:
//    Ui::AppointmentView *ui;
//    void loadDoctorList();
//};

//#endif // APPOINTMENTVIEW_H


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
    void goAppointmentEditView(int rowNo); // 跳转到编辑页

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

