#ifndef MEDICINEEDITVIEW_H
#define MEDICINEEDITVIEW_H

#include <QWidget>
#include <QDataWidgetMapper>
#include "idatabase.h"

namespace Ui {
class MedicineEditView;
}

class MedicineEditView : public QWidget
{
    Q_OBJECT

public:
    // 构造函数：接收父窗口 + 编辑行索引
    explicit MedicineEditView(QWidget *parent = nullptr, int editRow = -1);
    ~MedicineEditView();

signals:
    // 返回上一级页面的信号（与MasterView联动）
    void goPreviousView();

private slots:
    // 保存按钮点击事件
    void on_btnSave_clicked();
    // 取消按钮点击事件
    void on_btnCancel_clicked();
    // 库存变化时自动更新状态
    void on_medSpinStock_valueChanged(int arg1);

private:

    Ui::MedicineEditView *ui;
    IDatabase &db = IDatabase::getInstance(); // 数据库单例
    QDataWidgetMapper *dataMapper; // 数据映射器（绑定控件与数据库）
    int currentRow;                // 当前编辑的行索引
    bool isNewMedicine;

    // 初始化数据映射器
    void initMapper();
    // 加载药品数据到控件
    void loadMedicineData();
};

#endif // MEDICINEEDITVIEW_H
