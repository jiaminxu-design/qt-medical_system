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
