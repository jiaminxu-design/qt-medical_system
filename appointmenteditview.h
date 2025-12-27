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
//    ~AppointmentEditView();

//signals:
//    void goPreviousView(); // 返回上一页
//    void appointmentSaved(); // 新增：保存成功信号

//private slots:
//    void on_btSave_clicked();   // 保存预约
//    void on_btCancel_clicked(); // 取消编辑

//private:
//    Ui::AppointmentEditView *ui;
//    int m_rowNo; // 当前编辑的行号
//    QDataWidgetMapper *m_mapper; // 数据映射器（绑定控件和模型）

//    // 初始化UI
//    void initUI();
//    // 加载下拉框数据（患者/医生）
//    void loadComboBoxData();
//    // 校验表单数据
//    bool validateForm();
//};

//#endif // APPOINTMENTEDITVIEW_H

//#ifndef APPOINTMENTEDITVIEW_H
//#define APPOINTMENTEDITVIEW_H

//#include <QWidget>
//#include <QSqlTableModel>
//#include <QDataWidgetMapper>

//namespace Ui {
//class AppointmentEditView;
//}

//class AppointmentEditView : public QWidget
//{
//    Q_OBJECT

//public:
//    explicit AppointmentEditView(QWidget *parent = nullptr, int rowNo = -1);
//    ~AppointmentEditView();

//signals:
//    void appointmentSaved(); // 保存成功信号（通知列表页刷新）
//    void goPreviousView();   // 返回上一页信号

//private slots:
//    void on_btSave_clicked();    // 保存按钮槽函数
//    void on_btCancel_clicked();  // 取消按钮槽函数（可选）

//private:
//    Ui::AppointmentEditView *ui;
//    int m_rowNo = -1;            // 当前编辑行号（-1表示新增）
//    QDataWidgetMapper *m_mapper = nullptr; // 数据映射器（可选）

//    bool validateForm();         // 表单校验函数
//    void initUI();               // UI初始化函数（你已实现）
//    void loadComboBoxData();     // 加载下拉框数据函数（你已实现）
//};

//#endif // APPOINTMENTEDITVIEW_H

#ifndef APPOINTMENTEDITVIEW_H
#define APPOINTMENTEDITVIEW_H

#include <QWidget>
#include <QDataWidgetMapper>

namespace Ui {
class AppointmentEditView;
}

class AppointmentEditView : public QWidget
{
    Q_OBJECT

public:
    explicit AppointmentEditView(QWidget *parent = nullptr, int rowNo = -1);
    ~AppointmentEditView() override;

signals:
    void appointmentSaved(); // 保存成功信号
    void goPreviousView();   // 返回上一页信号

private slots:
    void on_btSave_clicked();    // 保存按钮
    void on_btCancel_clicked();  // 取消按钮

private:
    Ui::AppointmentEditView *ui;
    int m_rowNo = -1;            // 当前编辑行号（-1=新增）
    QDataWidgetMapper *m_mapper = nullptr; // 数据映射器

    bool validateForm();         // 表单校验
    void initUI();               // UI初始化
    void loadComboBoxData();     // 加载下拉框数据
};

#endif // APPOINTMENTEDITVIEW_H
