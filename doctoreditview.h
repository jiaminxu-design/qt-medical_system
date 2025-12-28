#ifndef DOCTOREDITVIEW_H
#define DOCTOREDITVIEW_H

#include <QWidget>
#include <QDataWidgetMapper> //数据映射器（绑定控件和数据库）

namespace Ui {
class DoctorEditView;
}

class DoctorEditView : public QWidget
{
    Q_OBJECT

public:
    explicit DoctorEditView(QWidget *parent = nullptr, int index = 0); // 新增index参数
    ~DoctorEditView();

// 返回上一级界面的信号
signals:
    void goPreviousView();

//按钮点击的槽函数
private slots:
    void on_btnSave_clicked();   // 保存按钮
    void on_btnCancel_clicked(); // 取消按钮

private:
    Ui::DoctorEditView *ui;
    QDataWidgetMapper *dataMapper; // 数据映射器（核心：绑定控件和数据库）
    int currentRow; // 当前编辑的医生行号
};

#endif // DOCTOREDITVIEW_H
