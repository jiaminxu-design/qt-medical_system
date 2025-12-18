//#include "doctoreditview.h"
//#include "ui_doctoreditview.h"
//#include "idatabase.h"
//#include <QMessageBox>
//#include <QDebug>

//DoctorEditView::DoctorEditView(QWidget *parent, int index) :
//    QWidget(parent),
//    ui(new Ui::DoctorEditView),
//    currentRow(index),
//    dataMapper(new QDataWidgetMapper(this))
//{
//    ui->setupUi(this);
//    IDatabase &db = IDatabase::getInstance();

//    // 1. 绑定医生模型到数据映射器
//    dataMapper->setModel(db.doctorTabModel);
//    dataMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit); // 手动提交

//    // 仅对有效行定位
//    if (index >= 0 && index < db.doctorTabModel->rowCount()) {
//        dataMapper->setCurrentIndex(index);
//    }

//    // 2. 绑定控件到数据库字段（新增工号绑定）
//    dataMapper->addMapping(ui->dbEditID, db.doctorTabModel->fieldIndex("ID"));         // 医生ID
//    dataMapper->addMapping(ui->dbEditEmpNo, db.doctorTabModel->fieldIndex("EMPLOYEENO")); // 工号
//    dataMapper->addMapping(ui->dbEditName, db.doctorTabModel->fieldIndex("NAME"));     // 姓名
//    dataMapper->addMapping(ui->dbComboTitle, db.doctorTabModel->fieldIndex("TITLE"));   // 职称
//    dataMapper->addMapping(ui->dbEditPhone, db.doctorTabModel->fieldIndex("PHONE"));   // 手机号
//    dataMapper->addMapping(ui->dbTextSchedule, db.doctorTabModel->fieldIndex("SCHEDULE")); // 排班

//    // 3. 绑定科室下拉框（显示名称，存储ID）
//    QSqlTableModel *deptModel = db.departmentTabModel;
//    ui->dbComboDepartment->setModel(deptModel);
//    ui->dbComboDepartment->setModelColumn(deptModel->fieldIndex("NAME"));

//    // 4. 初始化科室下拉框选中状态
//    if (index >= 0 && index < db.doctorTabModel->rowCount()) {
//        QString deptId = db.doctorTabModel->data(db.doctorTabModel->index(index,
//                                                                          db.doctorTabModel->fieldIndex("DEPARTMENT_ID"))).toString();
//        if (!deptId.isEmpty()) {
//            for (int i = 0; i < deptModel->rowCount(); i++) {
//                if (deptModel->data(deptModel->index(i, deptModel->fieldIndex("ID"))).toString() == deptId) {
//                    ui->dbComboDepartment->setCurrentIndex(i);
//                    break;
//                }
//            }
//        }
//    }

//    // 5. 设置只读属性
//    ui->dbEditID->setReadOnly(true);
//    ui->dbEditEmpNo->setReadOnly(true); // 工号只读

//    // 6. 触发数据填充到控件（修复toCurrent()不存在的问题）
//    if (index >= 0 && index < db.doctorTabModel->rowCount()) {
//        dataMapper->setCurrentIndex(index);
//        dataMapper->revert(); // 从模型加载数据到控件
//    }
//}

//DoctorEditView::~DoctorEditView()
//{
//    delete dataMapper;
//    delete ui;
//}

//// 保存按钮点击事件
//void DoctorEditView::on_btnSave_clicked()
//{
//    IDatabase &db = IDatabase::getInstance();

//    // 1. 处理排班信息：强制转为纯文本（去除HTML等错误格式）
//    QString scheduleText = ui->dbTextSchedule->toPlainText(); // 仅取纯文本内容
//    int scheduleColIndex = db.doctorTabModel->fieldIndex("SCHEDULE"); // 获取排班字段列索引
//    if (scheduleColIndex != -1) { // 确保字段存在
//        db.doctorTabModel->setData(db.doctorTabModel->index(currentRow, scheduleColIndex), scheduleText);
//    }

//    // 2. 提交数据映射器的修改（控件→模型）
//    dataMapper->submit();

//    // 3. 手动更新科室ID（下拉框选中文本对应科室ID）
//    int deptRow = ui->dbComboDepartment->currentIndex();
//    int deptIdColIndex = db.departmentTabModel->fieldIndex("ID");
//    int doctorDeptColIndex = db.doctorTabModel->fieldIndex("DEPARTMENT_ID");

//    if (deptRow >= 0 && deptIdColIndex != -1 && doctorDeptColIndex != -1) {
//        QString deptId = db.departmentTabModel->data(db.departmentTabModel->index(deptRow,
//                                                                                  deptIdColIndex)).toString();
//        db.doctorTabModel->setData(db.doctorTabModel->index(currentRow, doctorDeptColIndex), deptId);
//    }

//    // 4. 提交所有修改到数据库
//    bool ret = db.submitDoctorEdit();
//    if (ret) {
//        QMessageBox::information(this, "成功", "保存医生信息成功！");
//        emit goPreviousView(); // 触发返回上一级信号
//    } else {
//        // 显示详细错误信息
//        QSqlError error = db.doctorTabModel->lastError();
//        QMessageBox::warning(this, "保存失败",
//                             QString("错误原因：%1\n错误代码：%2")
//                             .arg(error.text())
//                             .arg(error.number()));
//        qDebug() << "数据库保存错误详情：" << error.text() << " | 错误代码：" <<
//                 error.number();
//    }
//}


//// 取消按钮点击事件
//void DoctorEditView::on_btnCancel_clicked()
//{
//    IDatabase::getInstance().revertDoctorEdit();
//    emit goPreviousView();
//}




//#include "doctoreditview.h"
//#include "ui_doctoreditview.h"
//#include "idatabase.h"
//#include <QMessageBox>
//#include <QDebug>

//DoctorEditView::DoctorEditView(QWidget *parent, int index) :
//    QWidget(parent),
//    ui(new Ui::DoctorEditView),
//    currentRow(index),
//    dataMapper(new QDataWidgetMapper(this))
//{
//    ui->setupUi(this);
//    IDatabase &db = IDatabase::getInstance();

//    // 1. 绑定医生模型到数据映射器
//    dataMapper->setModel(db.doctorTabModel);
//    dataMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

//    // 仅对有效行定位
//    if (index >= 0 && index < db.doctorTabModel->rowCount()) {
//        dataMapper->setCurrentIndex(index);
//    }

//    // 2. 绑定控件到数据库字段（注意：不绑定 dbTextSchedule！）
//    dataMapper->addMapping(ui->dbEditID, db.doctorTabModel->fieldIndex("ID"));
//    dataMapper->addMapping(ui->dbEditEmpNo, db.doctorTabModel->fieldIndex("EMPLOYEENO"));
//    dataMapper->addMapping(ui->dbEditName, db.doctorTabModel->fieldIndex("NAME"));
//    dataMapper->addMapping(ui->dbComboTitle, db.doctorTabModel->fieldIndex("TITLE"));
//    dataMapper->addMapping(ui->dbEditPhone, db.doctorTabModel->fieldIndex("PHONE"));
//    // ❌ 不再绑定 dbTextSchedule

//    // 3. 绑定科室下拉框
//    QSqlTableModel *deptModel = db.departmentTabModel;
//    ui->dbComboDepartment->setModel(deptModel);
//    ui->dbComboDepartment->setModelColumn(deptModel->fieldIndex("NAME"));

//    // 4. 初始化科室选中状态
//    if (index >= 0 && index < db.doctorTabModel->rowCount()) {
//        QString deptId = db.doctorTabModel->data(
//                             db.doctorTabModel->index(index, db.doctorTabModel->fieldIndex("DEPARTMENT_ID"))
//                         ).toString();
//        if (!deptId.isEmpty()) {
//            for (int i = 0; i < deptModel->rowCount(); ++i) {
//                if (deptModel->data(deptModel->index(i, deptModel->fieldIndex("ID"))).toString() == deptId) {
//                    ui->dbComboDepartment->setCurrentIndex(i);
//                    break;
//                }
//            }
//        }

//        // ✅ 手动加载排班信息（纯文本）
//        QString schedule = db.doctorTabModel->data(
//                               db.doctorTabModel->index(index, db.doctorTabModel->fieldIndex("SCHEDULE"))
//                           ).toString();
//        ui->dbTextSchedule->setPlainText(schedule); // 使用 setPlainText 避免解析 HTML
//    }

//    // 5. 设置只读
//    ui->dbEditID->setReadOnly(true);
//    ui->dbEditEmpNo->setReadOnly(true);
//}

//DoctorEditView::~DoctorEditView()
//{
//    delete dataMapper;
//    delete ui;
//}

//void DoctorEditView::on_btnSave_clicked()
//{
//    IDatabase &db = IDatabase::getInstance();

//    // 1. 提交除 SCHEDULE 外的所有字段
//    dataMapper->submit();

//    // 2. 手动处理排班信息（纯文本）
//    QString scheduleText = ui->dbTextSchedule->toPlainText().trimmed();
//    int scheduleCol = db.doctorTabModel->fieldIndex("SCHEDULE");
//    if (scheduleCol != -1 && currentRow >= 0) {
//        db.doctorTabModel->setData(db.doctorTabModel->index(currentRow, scheduleCol), scheduleText);
//    }

//    // 3. 手动更新科室ID
//    int deptRow = ui->dbComboDepartment->currentIndex();
//    int deptIdCol = db.departmentTabModel->fieldIndex("ID");
//    int doctorDeptCol = db.doctorTabModel->fieldIndex("DEPARTMENT_ID");

//    if (deptRow >= 0 && deptIdCol != -1 && doctorDeptCol != -1) {
//        QString deptId = db.departmentTabModel->data(
//                             db.departmentTabModel->index(deptRow, deptIdCol)
//                         ).toString();
//        db.doctorTabModel->setData(db.doctorTabModel->index(currentRow, doctorDeptCol), deptId);
//    }

//    // 4. 提交到数据库
//    bool ret = db.submitDoctorEdit();
//    if (ret) {
//        QMessageBox::information(this, "成功", "保存医生信息成功！");
//        emit goPreviousView();
//    } else {
//        QSqlError error = db.doctorTabModel->lastError();
//        QMessageBox::warning(this, "保存失败",
//                             QString("错误：%1\n代码：%2").arg(error.text()).arg(error.number()));
//        qDebug() << "数据库错误：" << error.text() << " | Code:" << error.number();
//    }
//}

//void DoctorEditView::on_btnCancel_clicked()
//{
//    IDatabase::getInstance().revertDoctorEdit();
//    emit goPreviousView();
//}



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
    // ❌ 不绑定 dbTextSchedule —— 手动处理

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
