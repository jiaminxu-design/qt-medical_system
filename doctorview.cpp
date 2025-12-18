#include "doctorview.h"
#include "ui_doctorview.h"
#include "idatabase.h"
#include <QMessageBox>
#include <QHeaderView>

DoctorView::DoctorView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DoctorView)
{
    ui->setupUi(this);
    IDatabase &db = IDatabase::getInstance();

    // 1. 初始化医生模型，绑定到TableView
    if (db.initDoctorModel()) {
        ui->tableView->setModel(db.doctorTabModel);
        ui->tableView->setSelectionModel(db.theDoctorSelection);

        // 隐藏不需要的列（ID）
        ui->tableView->hideColumn(db.doctorTabModel->fieldIndex("ID")); // 隐藏ID列

        // ========== 关键修正：通过模型设置列名 ==========
        db.doctorTabModel->setHeaderData(db.doctorTabModel->fieldIndex("EMPLOYEENO"), Qt::Horizontal,
                                         "工号");
        db.doctorTabModel->setHeaderData(db.doctorTabModel->fieldIndex("NAME"), Qt::Horizontal, "姓名");
        db.doctorTabModel->setHeaderData(db.doctorTabModel->fieldIndex("DEPARTMENT_ID"), Qt::Horizontal,
                                         "科室ID");
        db.doctorTabModel->setHeaderData(db.doctorTabModel->fieldIndex("TITLE"), Qt::Horizontal, "职称");
        db.doctorTabModel->setHeaderData(db.doctorTabModel->fieldIndex("PHONE"), Qt::Horizontal,
                                         "手机号");
        db.doctorTabModel->setHeaderData(db.doctorTabModel->fieldIndex("SCHEDULE"), Qt::Horizontal,
                                         "排班信息");

        // 自适应列宽
        ui->tableView->horizontalHeader()->setStretchLastSection(true);
        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    } else {
        QMessageBox::warning(this, "错误", "初始化医生数据失败！");
    }

    // 2. 初始化科室模型（仅用于编辑医生时的下拉框，无需绑定表格）
    db.initDepartmentModel();
}


DoctorView::~DoctorView()
{
    delete ui;
}

// 添加医生按钮
void DoctorView::on_btAdd_clicked()
{
    int curRow = IDatabase::getInstance().addNewDoctor();
    emit goDoctorEditView(curRow); // 跳转到编辑界面
}

// 编辑医生按钮
void DoctorView::on_btEdit_clicked()
{
    QModelIndex curIndex = IDatabase::getInstance().theDoctorSelection->currentIndex();
    if (!curIndex.isValid()) {
        QMessageBox::warning(this, "提示", "请先选中要编辑的医生！");
        return;
    }
    emit goDoctorEditView(curIndex.row());
}

// 删除医生按钮
void DoctorView::on_btDelete_clicked()
{
    QModelIndex curIndex = IDatabase::getInstance().theDoctorSelection->currentIndex();
    if (!curIndex.isValid()) {
        QMessageBox::warning(this, "提示", "请先选中要删除的医生！");
        return;
    }

    if (QMessageBox::question(this, "确认", "是否删除选中的医生？") != QMessageBox::Yes) {
        return;
    }

    bool ret = IDatabase::getInstance().deleteCurrentDoctor();
    if (ret) {
        QMessageBox::information(this, "成功", "删除医生成功！");
    } else {
        QMessageBox::warning(this, "失败", "删除医生失败！");
    }
}

// 搜索医生按钮（支持姓名/科室名称模糊搜索）
void DoctorView::on_btSearch_clicked()
{
    QString keyword = ui->txtSearch->text().trimmed();
    if (keyword.isEmpty()) {
        IDatabase::getInstance().searchDoctor(""); // 清空筛选，显示全部
        return;
    }

    // 拼接筛选条件：医生姓名 或 科室名称（关联Department表）
    QString filter = QString(
                         "NAME LIKE '%%1%' OR "
                         "DEPARTMENT_ID IN (SELECT ID FROM Department WHERE NAME LIKE '%%1%')"
                     ).arg(keyword);

    IDatabase::getInstance().searchDoctor(filter);
}
