//#ifndef DOCTORVIEW_H
//#define DOCTORVIEW_H

//#include <QWidget>

//namespace Ui {
//class DoctorView;
//}

//class DoctorView : public QWidget
//{
//    Q_OBJECT

//public:
//    explicit DoctorView(QWidget *parent = nullptr);
//    ~DoctorView();

//signals:
//    void goDoctorEditView(int idx);

//private slots:
//    void on_btAdd_clicked();    // 添加医生
//    void on_btEdit_clicked();   // 编辑医生
//    void on_btDelete_clicked(); // 删除医生
//    void on_btSearch_clicked(); // 搜索医生

//private:
//    Ui::DoctorView *ui;
//};

//#endif // DOCTORVIEW_H




#ifndef DOCTORVIEW_H
#define DOCTORVIEW_H

#include <QWidget>

namespace Ui {
class DoctorView;
}

class DoctorView : public QWidget
{
    Q_OBJECT

public:
    explicit DoctorView(QWidget *parent = nullptr);
    ~DoctorView();

// 新增：跳转到医生编辑界面的信号
signals:
    void goDoctorEditView(int idx);

// 新增：按钮点击的槽函数
private slots:
    void on_btAdd_clicked();    // 添加医生
    void on_btEdit_clicked();   // 编辑医生
    void on_btDelete_clicked(); // 删除医生
    void on_btSearch_clicked(); // 搜索医生

private:
    Ui::DoctorView *ui;
};

#endif // DOCTORVIEW_H
