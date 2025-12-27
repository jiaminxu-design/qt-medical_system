//#include "recordview.h"
//#include "ui_recordview.h"

//RecordView::RecordView(QWidget *parent) :
//    QWidget(parent),
//    ui(new Ui::RecordView)
//{
//    ui->setupUi(this);
//}

//RecordView::~RecordView()
//{
//    delete ui;
//}

#include "recordeditview.h"
#include "recordview.h"
#include "ui_recordview.h"
#include <QMessageBox>
#include <QDebug>

RecordView::RecordView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecordView),
    db(IDatabase::getInstance()) // 初始化数据库单例
{
    ui->setupUi(this);
    this->setWindowTitle("就诊记录管理"); // 设置窗口标题

    // 1. 初始化就诊记录模型
    if (!db.recordTabModel) {
        if (!db.initRecordModel()) {
            QMessageBox::critical(this, "错误", "就诊记录模型初始化失败！");
            return;
        }
    }

    // 2. 初始化患者/医生模型（用于下拉框）
    if (!db.patientTabModel) db.initPatientModel();
    if (!db.doctorTabModel) db.initDoctorModel();

    // 3. 绑定就诊记录表格（和药品模块一致）
    ui->tableView->setModel(db.recordTabModel);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setSelectionModel(db.theRecordSelection);

    // 4. 绑定按钮信号槽
    connect(ui->btSearch, &QPushButton::clicked, this, &RecordView::on_btSearch_clicked);
    connect(ui->btAdd, &QPushButton::clicked, this, &RecordView::on_btAdd_clicked);
    connect(ui->btEdit, &QPushButton::clicked, this, &RecordView::on_btEdit_clicked);
    connect(ui->btDelete, &QPushButton::clicked, this, &RecordView::on_btDelete_clicked);
    connect(this, &RecordView::refreshRecordTable, this, &RecordView::onRefreshRecordTable);
}

RecordView::~RecordView()
{
    delete ui;
}

// 搜索按钮
//void RecordView::on_btSearch_clicked()
//{
//    QString searchText = ui->txtSearch->text().trimmed();
//    QStringList filters;

//    // 模糊搜索患者名称
//    if (!searchText.isEmpty())
//        filters << QString("PATIENT_NAME LIKE '%%1%'").arg(searchText);

//    // 组合筛选条件
//    QString finalFilter = filters.join(" AND ");
//    db.searchRecord(finalFilter);
//}

// RecordView.cpp - on_btSearch_clicked 函数
void RecordView::on_btSearch_clicked()
{
    QString searchText = ui->txtSearch->text().trimmed();
    QStringList filters;

    if (!searchText.isEmpty()) {
        // 错误根源：数据库中无 PATIENT_NAME 字段，需关联 patient 表搜索
        // 正确写法：通过 PATIENT_ID 关联 patient 表的 NAME 字段模糊搜索
        QString filter = QString(
                             "PATIENT_ID IN (SELECT ID FROM patient WHERE NAME LIKE '%%1%') OR "
                             "DOCTOR_ID IN (SELECT ID FROM Doctor WHERE NAME LIKE '%%1%') OR "
                             "DIAGNOSIS LIKE '%%1%' OR SYMPTOMS LIKE '%%1%'"
                         ).arg(searchText);
        filters << filter;
    }

    // 组合筛选条件
    QString finalFilter = filters.join(" AND ");
    db.searchRecord(finalFilter);

    // 搜索后刷新表格（确保数据显示）
    ui->tableView->reset();
}


//// 新增按钮
//void RecordView::on_btAdd_clicked()
//{
//    // 1. 新增就诊记录行
//    int newRow = db.addNewRecord();
//    if (newRow < 0) {
//        QMessageBox::warning(this, "错误", "创建新就诊记录失败！");
//        return;
//    }

//    // 2. 选中新行
//    QModelIndex newIndex = db.recordTabModel->index(newRow, 0);
//    db.theRecordSelection->select(newIndex, QItemSelectionModel::Select | QItemSelectionModel::Rows);

//    // 3. 跳转到编辑页面（和药品模块一致）
//    emit goRecordEditView(newIndex.row());
//}

//void RecordView::on_btAdd_clicked()
//{
//    if (!db.recordTabModel) {
//        QMessageBox::warning(this, "提示", "就诊记录模型未初始化，无法添加！");
//        return;
//    }

//    // 1. 新增内存中的空行
//    int newRow = db.addNewRecord();
//    if (newRow < 0) {
//        QMessageBox::warning(this, "错误", "创建新就诊记录失败！");
//        return;
//    }

//    // 2. 选中新行
//    QModelIndex newIndex = db.recordTabModel->index(newRow, 0);
//    db.theRecordSelection->select(newIndex, QItemSelectionModel::Select | QItemSelectionModel::Rows);

//    // 3. 跳转到编辑页面（必须填写内容后才能保存，否则取消会删除空行）
//    emit goRecordEditView(newIndex.row());
//}

// RecordView.cpp
//void RecordView::on_btAdd_clicked()
//{
//    if (!db.recordTabModel) {
//        QMessageBox::warning(this, "提示", "就诊记录模型未初始化，无法添加！");
//        return;
//    }
//    // 仅获取待新增行号，不插入行
//    int newRow = db.addNewRecord();
//    if (newRow < 0) {
//        QMessageBox::warning(this, "错误", "创建新就诊记录失败！");
//        return;
//    }
//    // 跳转编辑页，传递“待新增行号”
//    emit goRecordEditView(newRow);
//}
void RecordView::on_btAdd_clicked()
{
    // 1. 先校验模型是否初始化（移到最前面）
    if (!db.recordTabModel) {
        QMessageBox::warning(this, "提示", "就诊记录模型未初始化，无法添加！");
        return;
    }

    // 2. 声明并获取 newRow（此时模型已校验，不会为空）
    int newRow = db.addNewRecord();
    if (newRow < 0) {
        QMessageBox::warning(this, "错误", "创建新就诊记录失败！");
        return;
    }

//    // 3. 创建编辑页对象
//    RecordEditView *editView = new RecordEditView(this, newRow);
//    // 绑定刷新信号
//    connect(editView, &RecordEditView::refreshRecordTable, this, &RecordView::onRefreshRecordTable);

    // 4. 跳转编辑页
    emit goRecordEditView(newRow);
}






// 编辑按钮
//void RecordView::on_btEdit_clicked()
//{
//    // 检查是否选中行
//    QModelIndex curIndex = db.theRecordSelection->currentIndex();
//    if (!curIndex.isValid()) {
//        QMessageBox::warning(this, "提示", "请先选中要编辑的就诊记录！");
//        return;
//    }

//    // 跳转到编辑页面
//    emit goRecordEditView(curIndex.row());
//}

// RecordView.cpp
void RecordView::on_btEdit_clicked()
{
    // 1. 先声明并获取选中行（移到最前面）
    QModelIndex curIndex = db.theRecordSelection->currentIndex();

    // 2. 严格校验：必须选中有效行
    if (!curIndex.isValid() || curIndex.row() >= db.recordTabModel->rowCount()) {
        QMessageBox::warning(this, "提示", "请先选中一条有效的就诊记录再编辑！");
        return;
    }

    // 3. 再创建编辑页对象（此时 curIndex 已定义）
    RecordEditView *editView = new RecordEditView(this, curIndex.row());
    // 绑定刷新信号
    connect(editView, &RecordEditView::refreshRecordTable, this, &RecordView::onRefreshRecordTable);

    // 4. 跳转编辑页
    emit goRecordEditView(curIndex.row());
}



// 删除按钮
void RecordView::on_btDelete_clicked()
{
    if (!db.theRecordSelection->hasSelection()) {
        QMessageBox::warning(this, "警告", "请先选择要删除的就诊记录！");
        return;
    }

    if (QMessageBox::question(this, "确认", "确定删除该就诊记录？") == QMessageBox::Yes) {
        if (db.deleteCurrentRecord()) {
            QMessageBox::information(this, "成功", "删除成功！");
        } else {
            QMessageBox::warning(this, "失败", "删除失败！");
        }
    }
}

void RecordView::onRefreshRecordTable()
{
    // 刷新模型数据
    db.recordTabModel->select();
    // 重置表格显示
    ui->tableView->reset();
    // 清空搜索框（避免过滤条件残留）
    ui->txtSearch->clear();
}
