//#include "recordeditview.h"
//#include "ui_recordeditview.h"

//RecordEditView::RecordEditView(QWidget *parent) :
//    QWidget(parent),
//    ui(new Ui::RecordEditView)
//{
//    ui->setupUi(this);
//}

//RecordEditView::~RecordEditView()
//{
//    delete ui;
//}

//#include "recordeditview.h"
//#include "ui_recordeditview.h"
//#include <QMessageBox>
//#include <QDebug>
//#include <QUuid>

//RecordEditView::RecordEditView(QWidget *parent, int editRow) :
//    QWidget(parent),
//    ui(new Ui::RecordEditView),
//    db(IDatabase::getInstance()),
//    dataMapper(new QDataWidgetMapper(this)),
//    currentRow(editRow),
//    isNewRecord(false)
//{
//    ui->setupUi(this);

//    // 1. 校验模型
//    if (!db.recordTabModel) {
//        QMessageBox::critical(this, "错误", "就诊记录模型未初始化！");
//        return;
//    }

//    // 2. 判断是否为新增记录
//    isNewRecord = (currentRow == db.recordTabModel->rowCount() - 1) &&
//                  (db.recordTabModel->data(db.recordTabModel->index(currentRow,
//                                                                    db.recordTabModel->fieldIndex("PATIENT_NAME"))).toString().isEmpty());

//    // 3. 加载患者/医生到下拉框
//    loadPatientAndDoctor();

//    // 4. 初始化数据映射器（和药品模块一致）
//    initMapper();

//    // 5. UI优化
//    if (isNewRecord) {
//        this->setWindowTitle("新增就诊记录");
//        ui->comboPatient->setFocus(); // 默认聚焦患者选择框
//        ui->dateVisit->setDate(QDate::currentDate()); // 默认当前日期
//    } else {
//        this->setWindowTitle("编辑就诊记录");
//    }
//}

//RecordEditView::~RecordEditView()
//{
//    delete dataMapper;
//    delete ui;
//}

//void RecordEditView::initMapper()
//{
//    dataMapper->setModel(db.recordTabModel);
//    dataMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

//    // 绑定实际字段名：PATIENT_ID、DOCTOR_ID
//    int patientCol = db.recordTabModel->fieldIndex("PATIENT_ID");
//    int doctorCol = db.recordTabModel->fieldIndex("DOCTOR_ID");
//    int visitDateCol = db.recordTabModel->fieldIndex("VISIT_DATE");
//    int visitTypeCol = db.recordTabModel->fieldIndex("VISIT_TYPE");
//    int diagnosisCol = db.recordTabModel->fieldIndex("DIAGNOSIS");
//    int symptomsCol = db.recordTabModel->fieldIndex("SYMPTOMS");

//    if (patientCol != -1) dataMapper->addMapping(ui->comboPatient, patientCol);
//    if (doctorCol != -1) dataMapper->addMapping(ui->comboDoctor, doctorCol);
//    if (visitDateCol != -1) dataMapper->addMapping(ui->dateVisit, visitDateCol);
//    if (visitTypeCol != -1) dataMapper->addMapping(ui->comboVisitType, visitTypeCol);
//    if (diagnosisCol != -1) dataMapper->addMapping(ui->editDiagnosis, diagnosisCol);
//    if (symptomsCol != -1) dataMapper->addMapping(ui->editSymptoms, symptomsCol);

//    if (currentRow >= 0 && currentRow < db.recordTabModel->rowCount()) {
//        dataMapper->setCurrentIndex(currentRow);
//    }
//}


//void RecordEditView::loadPatientAndDoctor()
//{
//    // 加载患者（ID+姓名）
//    if (db.patientTabModel) {
//        ui->comboPatient->clear();
//        for (int i = 0; i < db.patientTabModel->rowCount(); i++) {
//            QString patientId = db.patientTabModel->data(
//                                    db.patientTabModel->index(i, db.patientTabModel->fieldIndex("ID"))
//                                ).toString();
//            QString patientName = db.patientTabModel->data(
//                                      db.patientTabModel->index(i, db.patientTabModel->fieldIndex("NAME"))
//                                  ).toString();
//            // 下拉框显示“姓名(ID)”，方便识别
//            ui->comboPatient->addItem(patientName + "(" + patientId + ")", patientId);
//        }
//    }

//    // 加载医生（ID+姓名）
//    if (db.doctorTabModel) {
//        ui->comboDoctor->clear();
//        for (int i = 0; i < db.doctorTabModel->rowCount(); i++) {
//            QString doctorId = db.doctorTabModel->data(
//                                   db.doctorTabModel->index(i, db.doctorTabModel->fieldIndex("ID"))
//                               ).toString();
//            QString doctorName = db.doctorTabModel->data(
//                                     db.doctorTabModel->index(i, db.doctorTabModel->fieldIndex("NAME"))
//                                 ).toString();
//            ui->comboDoctor->addItem(doctorName + "(" + doctorId + ")", doctorId);
//        }
//    }
//}


//// 保存按钮
//void RecordEditView::on_btSave_clicked()
//{
//    // 1. 校验必填项
//    if (ui->comboPatient->currentText().isEmpty()) {
//        QMessageBox::warning(this, "提示", "请选择患者！");
//        return;
//    }
//    if (ui->comboDoctor->currentText().isEmpty()) {
//        QMessageBox::warning(this, "提示", "请选择医生！");
//        return;
//    }

//    // 2. 新增记录：生成ID
//    if (isNewRecord) {
//        QString newId = QUuid::createUuid().toString(QUuid::WithoutBraces);
//        int idCol = db.recordTabModel->fieldIndex("ID");
//        if (idCol != -1) {
//            db.recordTabModel->setData(db.recordTabModel->index(currentRow, idCol), newId);
//        }
//    }

//    // 3. 提交映射器数据
//    if (!dataMapper->submit()) {
//        QSqlError error = db.recordTabModel->lastError();
//        QMessageBox::warning(this, "错误", "提交数据失败：" + error.text());
//        return;
//    }

//    // 4. 处理就诊日期（手动格式化）
//    int visitDateCol = db.recordTabModel->fieldIndex("VISIT_DATE");
//    if (visitDateCol != -1) {
//        QString dateStr = ui->dateVisit->date().toString("yyyy-MM-dd");
//        db.recordTabModel->setData(db.recordTabModel->index(currentRow, visitDateCol), dateStr);
//    }

//    // 5. 提交到数据库
//    bool ret = db.submitRecordEdit();
//    if (ret) {
//        QString tip = isNewRecord ? "新增就诊记录成功！" : "编辑就诊记录成功！";
//        QMessageBox::information(this, "成功", tip);
//        emit goPreviousView(); // 返回上一级
//    } else {
//        QSqlError error = db.recordTabModel->lastError();
//        QMessageBox::warning(this, "保存失败",
//                             QString("错误：%1\n代码：%2").arg(error.text()).arg(error.number()));
//    }
//}

//void RecordEditView::on_btCancel_clicked()
//{
//    // 撤销未提交的修改
//    db.revertRecordEdit();

//    // 新增记录且未填写内容 → 删除空行
//    if (isNewRecord) {
//        // 校验是否为空记录（患者/医生都未选）
//        bool isEmpty = (ui->comboPatient->currentText().isEmpty()) &&
//                       (ui->comboDoctor->currentText().isEmpty()) &&
//                       (ui->editDiagnosis->toPlainText().isEmpty());

//        if (isEmpty && currentRow >= 0 && currentRow < db.recordTabModel->rowCount()) {
//            db.recordTabModel->removeRow(currentRow); // 删除空行
//            db.submitRecordEdit(); // 提交删除操作
//        }
//    }

//    emit goPreviousView(); // 返回上一级
//}



// RecordEditView.cpp
//RecordEditView::RecordEditView(QWidget *parent, int editRow) :
//    QWidget(parent),
//    ui(new Ui::RecordEditView),
//    db(IDatabase::getInstance()),
//    dataMapper(new QDataWidgetMapper(this)),
//    currentRow(editRow),
//    isNewRecord(false)
//{
//    ui->setupUi(this);

//    if (!db.recordTabModel) {
//        QMessageBox::critical(this, "错误", "就诊记录模型未初始化！");
//        return;
//    }

//    // 正确判断新增状态：行号 >= 模型总行数 → 新增
//    isNewRecord = (currentRow >= db.recordTabModel->rowCount());
//    loadPatientAndDoctor();
//    initMapper();

//    if (isNewRecord) {
//        this->setWindowTitle("新增就诊记录");
//        ui->comboPatient->setFocus();
//        ui->dateVisit->setDate(QDate::currentDate());
//    } else {
//        this->setWindowTitle("编辑就诊记录");
//        // 手动加载选中记录数据（核心：解决映射器失效问题）
//        loadSelectedRecordData();
//    }
//}

//// 新增：手动加载选中记录数据
//void RecordEditView::loadSelectedRecordData()
//{
//    if (currentRow < 0 || currentRow >= db.recordTabModel->rowCount()) return;

//    // 逐个字段加载数据到控件
//    int patientIdCol = db.recordTabModel->fieldIndex("PATIENT_ID");
//    int doctorIdCol = db.recordTabModel->fieldIndex("DOCTOR_ID");
//    int visitDateCol = db.recordTabModel->fieldIndex("VISIT_DATE");
//    int visitTypeCol = db.recordTabModel->fieldIndex("VISIT_TYPE");
//    int diagnosisCol = db.recordTabModel->fieldIndex("DIAGNOSIS");
//    int symptomsCol = db.recordTabModel->fieldIndex("SYMPTOMS");

//    // 加载患者ID（匹配下拉框）
//    QString patientId = db.recordTabModel->data(db.recordTabModel->index(currentRow, patientIdCol)).toString();
//    int patientIdx = ui->comboPatient->findData(patientId);
//    if (patientIdx >= 0) ui->comboPatient->setCurrentIndex(patientIdx);

//    // 加载医生ID（匹配下拉框）
//    QString doctorId = db.recordTabModel->data(db.recordTabModel->index(currentRow, doctorIdCol)).toString();
//    int doctorIdx = ui->comboDoctor->findData(doctorId);
//    if (doctorIdx >= 0) ui->comboDoctor->setCurrentIndex(doctorIdx);

//    // 加载就诊日期
//    QString dateStr = db.recordTabModel->data(db.recordTabModel->index(currentRow, visitDateCol)).toString();
//    ui->dateVisit->setDate(QDate::fromString(dateStr, "yyyy-MM-dd"));

//    // 加载就诊类型
//    QString visitType = db.recordTabModel->data(db.recordTabModel->index(currentRow, visitTypeCol)).toString();
//    int typeIdx = ui->comboVisitType->findText(visitType);
//    if (typeIdx >= 0) ui->comboVisitType->setCurrentIndex(typeIdx);

//    // 加载诊断和症状
//    ui->editDiagnosis->setText(db.recordTabModel->data(db.recordTabModel->index(currentRow, diagnosisCol)).toString());
//    ui->editSymptoms->setText(db.recordTabModel->data(db.recordTabModel->index(currentRow, symptomsCol)).toString());
//}

//// 修改保存按钮逻辑：新增时才插入行
//void RecordEditView::on_btSave_clicked()
//{
//    // 1. 校验必填项
//    if (ui->comboPatient->currentData().toString().isEmpty()) {
//        QMessageBox::warning(this, "提示", "请选择患者！");
//        return;
//    }
//    if (ui->comboDoctor->currentData().toString().isEmpty()) {
//        QMessageBox::warning(this, "提示", "请选择医生！");
//        return;
//    }

//    // 2. 新增记录：先插入行
//    if (isNewRecord) {
//        currentRow = db.recordTabModel->rowCount();
//        // 插入行（仅内存）
//        if (!db.recordTabModel->insertRow(currentRow)) {
//            QMessageBox::warning(this, "错误", "插入新记录失败！");
//            return;
//        }
//        // 初始化ID
//        QString newId = QUuid::createUuid().toString(QUuid::WithoutBraces);
//        int idCol = db.recordTabModel->fieldIndex("ID");
//        if (idCol != -1) {
//            db.recordTabModel->setData(db.recordTabModel->index(currentRow, idCol), newId);
//        }
//    }

//    // 3. 写入控件数据到模型
//    int patientIdCol = db.recordTabModel->fieldIndex("PATIENT_ID");
//    int doctorIdCol = db.recordTabModel->fieldIndex("DOCTOR_ID");
//    int visitDateCol = db.recordTabModel->fieldIndex("VISIT_DATE");
//    int visitTypeCol = db.recordTabModel->fieldIndex("VISIT_TYPE");
//    int diagnosisCol = db.recordTabModel->fieldIndex("DIAGNOSIS");
//    int symptomsCol = db.recordTabModel->fieldIndex("SYMPTOMS");

//    if (patientIdCol != -1) {
//        db.recordTabModel->setData(db.recordTabModel->index(currentRow, patientIdCol),
//                                  ui->comboPatient->currentData().toString());
//    }
//    if (doctorIdCol != -1) {
//        db.recordTabModel->setData(db.recordTabModel->index(currentRow, doctorIdCol),
//                                  ui->comboDoctor->currentData().toString());
//    }
//    if (visitDateCol != -1) {
//        db.recordTabModel->setData(db.recordTabModel->index(currentRow, visitDateCol),
//                                  ui->dateVisit->date().toString("yyyy-MM-dd"));
//    }
//    if (visitTypeCol != -1) {
//        db.recordTabModel->setData(db.recordTabModel->index(currentRow, visitTypeCol),
//                                  ui->comboVisitType->currentText());
//    }
//    if (diagnosisCol != -1) {
//        db.recordTabModel->setData(db.recordTabModel->index(currentRow, diagnosisCol),
//                                  ui->editDiagnosis->toPlainText());
//    }
//    if (symptomsCol != -1) {
//        db.recordTabModel->setData(db.recordTabModel->index(currentRow, symptomsCol),
//                                  ui->editSymptoms->toPlainText());
//    }

//    // 4. 提交到数据库
//    bool ret = db.submitRecordEdit();
//    if (ret) {
//        QString tip = isNewRecord ? "新增就诊记录成功！" : "编辑就诊记录成功！";
//        QMessageBox::information(this, "成功", tip);
//        emit goPreviousView();
//    } else {
//        QSqlError error = db.recordTabModel->lastError();
//        QMessageBox::warning(this, "保存失败",
//                             QString("错误：%1\n代码：%2").arg(error.text()).arg(error.number()));
//    }
//}

//// 修改取消按钮逻辑：新增时不操作数据库，直接返回
//void RecordEditView::on_btCancel_clicked()
//{
//    // 仅撤销内存中的修改，不提交任何操作
//    if (isNewRecord) {
//        // 新增状态：无任何数据库操作，直接返回
//        emit goPreviousView();
//        return;
//    }
//    // 编辑状态：撤销修改
//    db.revertRecordEdit();
//    emit goPreviousView();
//}


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

// 手动加载选中记录数据到控件（核心：解决映射器失效）
void RecordEditView::loadSelectedRecordData()
{
    // 校验行号有效性
    if (currentRow < 0 || currentRow >= db.recordTabModel->rowCount()) return;

    // 获取字段列号
    int patientIdCol = db.recordTabModel->fieldIndex("PATIENT_ID");
    int doctorIdCol = db.recordTabModel->fieldIndex("DOCTOR_ID");
    int visitDateCol = db.recordTabModel->fieldIndex("VISIT_DATE");
    int visitTypeCol = db.recordTabModel->fieldIndex("VISIT_TYPE");
    int diagnosisCol = db.recordTabModel->fieldIndex("DIAGNOSIS");
    int symptomsCol = db.recordTabModel->fieldIndex("SYMPTOMS");

    // 1. 加载患者ID（匹配下拉框值）
    QString patientId = db.recordTabModel->data(db.recordTabModel->index(currentRow,
                                                                         patientIdCol)).toString();
    int patientIdx = ui->comboPatient->findData(patientId);
    if (patientIdx >= 0) ui->comboPatient->setCurrentIndex(patientIdx);

    // 2. 加载医生ID（匹配下拉框值）
    QString doctorId = db.recordTabModel->data(db.recordTabModel->index(currentRow,
                                                                        doctorIdCol)).toString();
    int doctorIdx = ui->comboDoctor->findData(doctorId);
    if (doctorIdx >= 0) ui->comboDoctor->setCurrentIndex(doctorIdx);

    // 3. 加载就诊日期（格式化）
    QString dateStr = db.recordTabModel->data(db.recordTabModel->index(currentRow,
                                                                       visitDateCol)).toString();
    ui->dateVisit->setDate(QDate::fromString(dateStr, "yyyy-MM-dd"));

    // 4. 加载就诊类型（匹配文本）
    QString visitType = db.recordTabModel->data(db.recordTabModel->index(currentRow,
                                                                         visitTypeCol)).toString();
    int typeIdx = ui->comboVisitType->findText(visitType);
    if (typeIdx >= 0) ui->comboVisitType->setCurrentIndex(typeIdx);

    // 5. 加载诊断和症状
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
    /*if (ret) {
        QString tip = isNewRecord ? "新增就诊记录成功！" : "编辑就诊记录成功！";
        QMessageBox::information(this, "成功", tip);
        emit goPreviousView(); // 返回上一级
    }*/ if (ret) {
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

// 取消按钮（新增时无数据库操作，直接返回）
void RecordEditView::on_btCancel_clicked()
{
    // 新增状态：无任何数据库操作，直接返回（核心：不插入/删除任何行）
    if (isNewRecord) {
        emit goPreviousView();
        return;
    }

    // 编辑状态：仅撤销内存中的修改，不提交
    db.revertRecordEdit();  // 引用用 . 访问
    emit goPreviousView();
}
