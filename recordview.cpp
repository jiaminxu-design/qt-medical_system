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

//on_btSearch_clicked 函数
void RecordView::on_btSearch_clicked()
{
    QString searchText = ui->txtSearch->text().trimmed();
    QStringList filters;

    if (!searchText.isEmpty()) {
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


// 新增按钮
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
    // 3. 跳转编辑页
    emit goRecordEditView(newRow);
}


//编辑按钮
void RecordView::on_btEdit_clicked()
{
    // 1. 强化校验：必须选中有效行（且行号在合法范围内）
    QModelIndex curIndex = db.theRecordSelection->currentIndex();
    if (!curIndex.isValid()) {
        QMessageBox::warning(this, "提示", "请先选中一条就诊记录再编辑！");
        return;
    }
    int row = curIndex.row();
    if (row < 0 || row >= db.recordTabModel->rowCount()) {
        QMessageBox::warning(this, "提示", "选中的记录无效，请重新选择！");
        return;
    }

    // 2. 复用添加页的独立窗口逻辑（解决界面显示问题）
    // 找到MasterView作为父对象，避免嵌入RecordView
    QWidget *masterView = this->parentWidget();
    RecordEditView *editView = new RecordEditView(masterView, row);

    // 3. 绑定返回信号：关闭编辑页时彻底销毁，刷新列表
    connect(editView, &RecordEditView::goPreviousView, this, [ = ]() {
        editView->deleteLater(); // 彻底销毁编辑页，避免残留
        this->onRefreshRecordTable(); // 刷新RecordView列表
    });

    // 4. 强制编辑页为独立窗口（和添加页一致）
    editView->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    editView->setWindowTitle("编辑就诊记录"); // 区分添加页标题
    editView->show(); // 独立窗口显示，不会嵌入RecordView
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
