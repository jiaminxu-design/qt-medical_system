//#ifndef RECORDEDITVIEW_H
//#define RECORDEDITVIEW_H

//#include <QWidget>

//namespace Ui {
//class RecordEditView;
//}

//class RecordEditView : public QWidget
//{
//    Q_OBJECT

//public:
//    explicit RecordEditView(QWidget *parent = nullptr);
//    ~RecordEditView();

//private:
//    Ui::RecordEditView *ui;
//};

//#endif // RECORDEDITVIEW_H


//#ifndef RECORDEDITVIEW_H
//#define RECORDEDITVIEW_H

//#include <QWidget>
//#include <QDataWidgetMapper>
//#include "idatabase.h"

//namespace Ui {
//class RecordEditView;
//}

//class RecordEditView : public QWidget
//{
//    Q_OBJECT

//public:
//    explicit RecordEditView(QWidget *parent = nullptr, int editRow = -1);
//    ~RecordEditView();

//signals:
//    // 返回上一级信号（和药品模块一致）
//    void goPreviousView();

//private slots:
//    void on_btSave_clicked();    // 保存
//    void on_btCancel_clicked();  // 取消

//private:
//    Ui::RecordEditView *ui;
//    IDatabase &db; // 数据库单例引用
//    QDataWidgetMapper *dataMapper; // 数据映射器（和药品模块一致）
//    int currentRow; // 当前编辑行
//    bool isNewRecord; // 是否为新增记录

//    // 初始化数据映射器
//    void initMapper();
//    // 加载患者/医生到下拉框
//    void loadPatientAndDoctor();
//};

//#endif // RECORDEDITVIEW_H

#ifndef RECORDEDITVIEW_H
#define RECORDEDITVIEW_H

#include <QWidget>
#include <QDataWidgetMapper>
#include "idatabase.h"

namespace Ui {
class RecordEditView;
}

class RecordEditView : public QWidget
{
    Q_OBJECT

public:
    explicit RecordEditView(QWidget *parent = nullptr, int editRow = -1);
    ~RecordEditView();

private slots:
    void on_btSave_clicked();
    void on_btCancel_clicked();

signals:
    void goPreviousView(); // 返回上一级页面的信号
    void refreshRecordTable(); // 新增刷新信号

private:
    Ui::RecordEditView *ui;
    IDatabase &db;
    QDataWidgetMapper *dataMapper;
    int currentRow;
    bool isNewRecord;              // 是否为新增记录

    // 初始化数据映射器
    void initMapper();
    // 加载患者/医生到下拉框
    void loadPatientAndDoctor();
    // 手动加载选中记录数据（解决映射器失效）
    void loadSelectedRecordData();
};

#endif // RECORDEDITVIEW_H
