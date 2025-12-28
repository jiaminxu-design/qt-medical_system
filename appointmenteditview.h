#ifndef APPOINTMENTEDITVIEW_H
#define APPOINTMENTEDITVIEW_H

#include <QWidget>
#include <QDataWidgetMapper>
#include <QDate>

namespace Ui {
class AppointmentEditView;
}

class AppointmentEditView : public QWidget
{
    Q_OBJECT

public:
    explicit AppointmentEditView(QWidget *parent = nullptr, int rowNo = -1,
                                 const QDate &date = QDate::currentDate());
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
    QDate m_selectedDate; // 新增：存储选中的日期
};

#endif // APPOINTMENTEDITVIEW_H


