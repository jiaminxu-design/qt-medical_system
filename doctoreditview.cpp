#include "doctoreditview.h"
#include "ui_doctoreditview.h"
#include "idatabase.h"
#include <QMessageBox>
#include <QDebug>
#include <QDateTime>

// 辅助函数：生成医生工号，例如 DOC2025001
QString generateDoctorEmpNo()
{
    // 获取当前年份后四位
    QString year = QString::number(QDateTime::currentDateTime().date().year());
    IDatabase &db = IDatabase::getInstance();
    int totalDoctors = db.doctorTabModel->rowCount(); // 注意：可能包含未提交的新行
    // 更可靠的方式是从数据库查最大编号，但这里简化处理
    int nextIndex = totalDoctors + 1;
    return QString("DOC%1%2").arg(year).arg(nextIndex, 3, 10, QChar('0')); // DOC2025001
}

DoctorEditView::DoctorEditView(QWidget *parent, int index) :
    QWidget(parent),
    ui(new Ui::DoctorEditView),
    currentRow(index),
    dataMapper(new QDataWidgetMapper(this))
{
    ui->setupUi(this);
    IDatabase &db = IDatabase::getInstance();

    // 1. 绑定模型
    dataMapper->setModel(db.doctorTabModel);
    dataMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    // 2. 【关键】先完成所有 mapping，再设置 currentIndex！
    dataMapper->addMapping(ui->dbEditID,        db.doctorTabModel->fieldIndex("ID"));
    dataMapper->addMapping(ui->dbEditEmpNo,     db.doctorTabModel->fieldIndex("EMPLOYEENO"));
    dataMapper->addMapping(ui->dbEditName,      db.doctorTabModel->fieldIndex("NAME"));
    dataMapper->addMapping(ui->dbComboTitle,    db.doctorTabModel->fieldIndex("TITLE"));
    dataMapper->addMapping(ui->dbEditPhone,     db.doctorTabModel->fieldIndex("PHONE"));

    // 3. 绑定科室下拉框（显示 NAME，值用 ID）
    QSqlTableModel *deptModel = db.departmentTabModel;
    ui->dbComboDepartment->setModel(deptModel);
    ui->dbComboDepartment->setModelColumn(deptModel->fieldIndex("NAME"));

    // 4. 设置只读字段
    ui->dbEditID->setReadOnly(true);
    ui->dbEditEmpNo->setReadOnly(true);

    // 5. 根据 index 判断是【编辑】还是【新增】
    if (index >= 0 && index < db.doctorTabModel->rowCount()) {
        // ===== 编辑模式 =====
        dataMapper->setCurrentIndex(index);
        dataMapper->revert(); // 强制加载数据到控件

        // 手动加载排班（纯文本）
        QString schedule = db.doctorTabModel->data(
                               db.doctorTabModel->index(index, db.doctorTabModel->fieldIndex("SCHEDULE"))
                           ).toString();
        ui->dbTextSchedule->setPlainText(schedule);

        // 初始化科室下拉框
        QString deptId = db.doctorTabModel->data(
                             db.doctorTabModel->index(index, db.doctorTabModel->fieldIndex("DEPARTMENT_ID"))
                         ).toString();
        if (!deptId.isEmpty()) {
            for (int i = 0; i < deptModel->rowCount(); ++i) {
                if (deptModel->data(deptModel->index(i, deptModel->fieldIndex("ID"))).toString() == deptId) {
                    ui->dbComboDepartment->setCurrentIndex(i);
                    break;
                }
            }
        }
    } else {
        // ===== 新增模式 =====
        int newRow = db.doctorTabModel->rowCount();
        if (!db.doctorTabModel->insertRow(newRow)) {
            QMessageBox::warning(this, "错误", "无法创建新医生记录！");
            return;
        }
        currentRow = newRow;

        // 清空界面
        ui->dbEditID->clear();
        ui->dbEditEmpNo->setText(generateDoctorEmpNo()); // 自动生成工号
        ui->dbEditName->clear();
        ui->dbEditPhone->clear();
        ui->dbTextSchedule->setPlainText("");
        ui->dbComboDepartment->setCurrentIndex(-1);

        // 设置 mapper 指向新行（虽然无数据，但为后续 submit 准备）
        dataMapper->setCurrentIndex(newRow);
    }
}

DoctorEditView::~DoctorEditView()
{
    delete dataMapper;
    delete ui;
}

void DoctorEditView::on_btnSave_clicked()
{
    IDatabase &db = IDatabase::getInstance();

    // 1. 提交 mapper 控件 → 模型（除 SCHEDULE 外）
    if (!dataMapper->submit()) {
        qDebug() << "DataWidgetMapper submit failed.";
    }

    // 2. 手动处理排班（纯文本）
    QString scheduleText = ui->dbTextSchedule->toPlainText().trimmed();
    int scheduleCol = db.doctorTabModel->fieldIndex("SCHEDULE");
    if (scheduleCol != -1 && currentRow >= 0) {
        db.doctorTabModel->setData(db.doctorTabModel->index(currentRow, scheduleCol), scheduleText);
    }

    // 3. 手动更新科室 ID
    int deptRow = ui->dbComboDepartment->currentIndex();
    int deptIdCol = db.departmentTabModel->fieldIndex("ID");
    int doctorDeptCol = db.doctorTabModel->fieldIndex("DEPARTMENT_ID");

    if (deptRow >= 0 && deptIdCol != -1 && doctorDeptCol != -1) {
        QString deptId = db.departmentTabModel->data(
                             db.departmentTabModel->index(deptRow, deptIdCol)
                         ).toString();
        db.doctorTabModel->setData(db.doctorTabModel->index(currentRow, doctorDeptCol), deptId);
    }

    // 4. 提交到数据库
    bool ret = db.submitDoctorEdit();
    if (ret) {
        QMessageBox::information(this, "成功", "保存医生信息成功！");
        emit goPreviousView();
    } else {
        QSqlError error = db.doctorTabModel->lastError();
        QMessageBox::warning(this, "保存失败",
                             QString("错误：%1\n代码：%2").arg(error.text()).arg(error.number()));
        qDebug() << "数据库错误：" << error.text() << " | Code:" << error.number();
    }
}

void DoctorEditView::on_btnCancel_clicked()
{
    IDatabase::getInstance().revertDoctorEdit();
    emit goPreviousView();
}
