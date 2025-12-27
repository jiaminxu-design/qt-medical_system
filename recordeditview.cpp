#include "recordeditview.h"
#include "ui_recordeditview.h"
#include <QMessageBox>
#include <QDebug>
#include <QUuid>
#include <QDate>

RecordEditView::RecordEditView(QWidget *parent, int editRow) :
    QWidget(parent),
    ui(new Ui::RecordEditView),
    db(IDatabase::getInstance()),  // 引用绑定单例
    dataMapper(new QDataWidgetMapper(this)),
    currentRow(editRow),
    isNewRecord(false)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);

    // 1. 校验就诊记录模型是否初始化（引用用 . 访问）
    if (!db.recordTabModel) {
        QMessageBox::critical(this, "错误", "就诊记录模型未初始化！");
        return;
    }

    // 2. 正确判断新增状态：行号 >= 模型总行数 → 新增
    isNewRecord = (currentRow >= db.recordTabModel->rowCount());

    // 3. 加载患者/医生到下拉框
    loadPatientAndDoctor();

    // 4. 初始化数据映射器（仅编辑模式生效）
    initMapper();

    // 5. UI优化 + 数据加载
    if (isNewRecord) {
        this->setWindowTitle("新增就诊记录");
        ui->comboPatient->setFocus();                // 默认聚焦患者选择框
        ui->dateVisit->setDate(QDate::currentDate());// 默认当前日期
    } else {
        this->setWindowTitle("编辑就诊记录");
        // 手动加载选中记录数据到控件（核心修复）
        loadSelectedRecordData();
    }
}

RecordEditView::~RecordEditView()
{
    delete dataMapper;
    delete ui;
}

// 初始化数据映射器（仅编辑模式绑定）
void RecordEditView::initMapper()
{
    dataMapper->setModel(db.recordTabModel);  // 引用用 . 访问
    dataMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    // 仅编辑模式绑定映射器（新增模式无需绑定）
    if (!isNewRecord) {
        int patientCol = db.recordTabModel->fieldIndex("PATIENT_ID");
        int doctorCol = db.recordTabModel->fieldIndex("DOCTOR_ID");
        int visitDateCol = db.recordTabModel->fieldIndex("VISIT_DATE");
        int visitTypeCol = db.recordTabModel->fieldIndex("VISIT_TYPE");
        int diagnosisCol = db.recordTabModel->fieldIndex("DIAGNOSIS");
        int symptomsCol = db.recordTabModel->fieldIndex("SYMPTOMS");

        if (patientCol != -1) dataMapper->addMapping(ui->comboPatient, patientCol);
        if (doctorCol != -1) dataMapper->addMapping(ui->comboDoctor, doctorCol);
        if (visitDateCol != -1) dataMapper->addMapping(ui->dateVisit, visitDateCol);
        if (visitTypeCol != -1) dataMapper->addMapping(ui->comboVisitType, visitTypeCol);
        if (diagnosisCol != -1) dataMapper->addMapping(ui->editDiagnosis, diagnosisCol);
        if (symptomsCol != -1) dataMapper->addMapping(ui->editSymptoms, symptomsCol);

        dataMapper->setCurrentIndex(currentRow);
    }
}

// 加载患者/医生到下拉框（ID+姓名，值为ID）
void RecordEditView::loadPatientAndDoctor()
{
    // 加载患者（显示：姓名(ID)，值：ID）
    if (db.patientTabModel) {  // 引用用 . 访问
        ui->comboPatient->clear();
        for (int i = 0; i < db.patientTabModel->rowCount(); i++) {
            QString patientId = db.patientTabModel->data(
                                    db.patientTabModel->index(i, db.patientTabModel->fieldIndex("ID"))
                                ).toString();
            QString patientName = db.patientTabModel->data(
                                      db.patientTabModel->index(i, db.patientTabModel->fieldIndex("NAME"))
                                  ).toString();
            ui->comboPatient->addItem(patientName + "(" + patientId + ")", patientId);
        }
    }

    // 加载医生（显示：姓名(ID)，值：ID）
    if (db.doctorTabModel) {  // 引用用 . 访问
        ui->comboDoctor->clear();
        for (int i = 0; i < db.doctorTabModel->rowCount(); i++) {
            QString doctorId = db.doctorTabModel->data(
                                   db.doctorTabModel->index(i, db.doctorTabModel->fieldIndex("ID"))
                               ).toString();
            QString doctorName = db.doctorTabModel->data(
                                     db.doctorTabModel->index(i, db.doctorTabModel->fieldIndex("NAME"))
                                 ).toString();
            ui->comboDoctor->addItem(doctorName + "(" + doctorId + ")", doctorId);
        }
    }
}

//loadSelectedRecordData（确保正确加载选中记录）
void RecordEditView::loadSelectedRecordData()
{
    // 双重校验：行号必须有效
    if (currentRow < 0 || currentRow >= db.recordTabModel->rowCount()) {
        QMessageBox::warning(this, "提示", "选中的就诊记录无效！");
        this->close(); // 直接关闭编辑页
        return;
    }

    // 原有加载逻辑（确保加载的是选中记录的真实数据）
    int patientIdCol = db.recordTabModel->fieldIndex("PATIENT_ID");
    int doctorIdCol = db.recordTabModel->fieldIndex("DOCTOR_ID");
    int visitDateCol = db.recordTabModel->fieldIndex("VISIT_DATE");
    int visitTypeCol = db.recordTabModel->fieldIndex("VISIT_TYPE");
    int diagnosisCol = db.recordTabModel->fieldIndex("DIAGNOSIS");
    int symptomsCol = db.recordTabModel->fieldIndex("SYMPTOMS");

    // 加载选中记录的PATIENT_ID（匹配下拉框）
    QString patientId = db.recordTabModel->data(db.recordTabModel->index(currentRow,
                                                                         patientIdCol)).toString();
    int patientIdx = ui->comboPatient->findData(patientId);
    if (patientIdx >= 0) ui->comboPatient->setCurrentIndex(patientIdx);

    // 加载选中记录的DOCTOR_ID（匹配下拉框）
    QString doctorId = db.recordTabModel->data(db.recordTabModel->index(currentRow,
                                                                        doctorIdCol)).toString();
    int doctorIdx = ui->comboDoctor->findData(doctorId);
    if (doctorIdx >= 0) ui->comboDoctor->setCurrentIndex(doctorIdx);

    // 加载就诊日期、类型、诊断、症状（确保是选中记录的数据）
    QString dateStr = db.recordTabModel->data(db.recordTabModel->index(currentRow,
                                                                       visitDateCol)).toString();
    ui->dateVisit->setDate(QDate::fromString(dateStr, "yyyy-MM-dd"));

    QString visitType = db.recordTabModel->data(db.recordTabModel->index(currentRow,
                                                                         visitTypeCol)).toString();
    int typeIdx = ui->comboVisitType->findText(visitType);
    if (typeIdx >= 0) ui->comboVisitType->setCurrentIndex(typeIdx);

    ui->editDiagnosis->setText(db.recordTabModel->data(db.recordTabModel->index(currentRow,
                                                                                diagnosisCol)).toString());
    ui->editSymptoms->setText(db.recordTabModel->data(db.recordTabModel->index(currentRow,
                                                                               symptomsCol)).toString());
}


// 保存按钮（新增时才插入行，必填项校验）
void RecordEditView::on_btSave_clicked()
{
    // 1. 严格校验必填项（患者/医生必须选）
    QString patientId = ui->comboPatient->currentData().toString().trimmed();
    QString doctorId = ui->comboDoctor->currentData().toString().trimmed();
    if (patientId.isEmpty()) {
        QMessageBox::warning(this, "提示", "请选择患者！");
        return;
    }
    if (doctorId.isEmpty()) {
        QMessageBox::warning(this, "提示", "请选择医生！");
        return;
    }

    // 2. 新增记录：先插入行（仅内存）
    if (isNewRecord) {
        currentRow = db.recordTabModel->rowCount();
        // 插入空行（仅内存，未提交到数据库）
        if (!db.recordTabModel->insertRow(currentRow)) {
            QMessageBox::warning(this, "错误", "插入新记录失败！");
            return;
        }
        // 初始化主键ID
        QString newId = QUuid::createUuid().toString(QUuid::WithoutBraces);
        int idCol = db.recordTabModel->fieldIndex("ID");
        if (idCol != -1) {
            db.recordTabModel->setData(db.recordTabModel->index(currentRow, idCol), newId);
        }
    }

    // 3. 写入控件数据到模型（手动赋值，避免映射器问题）
    int patientIdCol = db.recordTabModel->fieldIndex("PATIENT_ID");
    int doctorIdCol = db.recordTabModel->fieldIndex("DOCTOR_ID");
    int visitDateCol = db.recordTabModel->fieldIndex("VISIT_DATE");
    int visitTypeCol = db.recordTabModel->fieldIndex("VISIT_TYPE");
    int diagnosisCol = db.recordTabModel->fieldIndex("DIAGNOSIS");
    int symptomsCol = db.recordTabModel->fieldIndex("SYMPTOMS");

    if (patientIdCol != -1) {
        db.recordTabModel->setData(db.recordTabModel->index(currentRow, patientIdCol), patientId);
    }
    if (doctorIdCol != -1) {
        db.recordTabModel->setData(db.recordTabModel->index(currentRow, doctorIdCol), doctorId);
    }
    if (visitDateCol != -1) {
        db.recordTabModel->setData(db.recordTabModel->index(currentRow, visitDateCol),
                                   ui->dateVisit->date().toString("yyyy-MM-dd"));
    }
    if (visitTypeCol != -1) {
        db.recordTabModel->setData(db.recordTabModel->index(currentRow, visitTypeCol),
                                   ui->comboVisitType->currentText());
    }
    if (diagnosisCol != -1) {
        db.recordTabModel->setData(db.recordTabModel->index(currentRow, diagnosisCol),
                                   ui->editDiagnosis->toPlainText().trimmed());
    }
    if (symptomsCol != -1) {
        db.recordTabModel->setData(db.recordTabModel->index(currentRow, symptomsCol),
                                   ui->editSymptoms->toPlainText().trimmed());
    }

    // 4. 提交到数据库（最终持久化）
    bool ret = db.submitRecordEdit();  // 引用用 . 访问
    if (ret) {
        QString tip = isNewRecord ? "新增就诊记录成功！" : "编辑就诊记录成功！";
        QMessageBox::information(this, "成功", tip);
        // 新增：发送刷新信号
        emit refreshRecordTable();
        emit goPreviousView();
    } else {
        QSqlError error = db.recordTabModel->lastError();
        QMessageBox::warning(this, "保存失败",
                             QString("错误：%1\n错误代码：%2").arg(error.text()).arg(error.number()));
        // 新增失败时回滚行插入
        if (isNewRecord) {
            db.recordTabModel->removeRow(currentRow);
        }
    }
}

// 取消/返回按钮
void RecordEditView::on_btCancel_clicked()
{
    // 编辑状态：彻底销毁窗口，返回RecordView
    if (!isNewRecord) {
        db.revertRecordEdit(); // 回滚未保存的修改
        this->deleteLater();   // 销毁编辑页（关键：解决界面残留）
        emit goPreviousView(); // 通知RecordView刷新
        return;
    }

    // 新增状态：原有逻辑（保持和添加页一致）
    this->deleteLater();
    emit goPreviousView();
}

