#ifndef RECORDVIEW_H
#define RECORDVIEW_H

#include <QWidget>
#include "idatabase.h"

namespace Ui {
class RecordView;
}

class RecordView : public QWidget
{
    Q_OBJECT

public:
    explicit RecordView(QWidget *parent = nullptr);
    ~RecordView();

signals:
    // 跳转编辑页面信号（和药品模块一致）
    void goRecordEditView(int rowNo);
    void refreshRecordTable(); // 新增这行：声明刷新信号

private slots:
    // 按钮槽函数
    void on_btSearch_clicked();    // 搜索
    void on_btAdd_clicked();       // 新增
    void on_btEdit_clicked();      // 编辑
    void on_btDelete_clicked();    // 删除
    void onRefreshRecordTable(); // 新增刷新槽函数

private:
    Ui::RecordView *ui;
    IDatabase &db; // 数据库单例引用
};

#endif // RECORDVIEW_H

