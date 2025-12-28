//#include "AppointmentEditView.h"
//#include "ui_AppointmentEditView.h"
//#include "idatabase.h"
//#include<QMessageBox>

//AppointmentEditView::AppointmentEditView(QWidget *parent, int rowNo)
//    : QWidget(parent)
//    , ui(new Ui::AppointmentEditView)
//    , m_rowNo(rowNo)
//{
//    ui->setupUi(this);
//    this->setWindowTitle("编辑预约"); // 匹配MasterView的标题显示

//    // 初始化UI
//    initUI();
//    // 加载下拉框数据
//    loadComboBoxData();
//}

//AppointmentEditView::~AppointmentEditView()
//{
//    delete ui;
//    delete m_mapper;
//}

//void AppointmentEditView::initUI()
//{
//    // 初始化预约模型
//    if (!IDatabase::getInstance().initAppointmentModel()) {
//        QMessageBox::critical(this, "错误", "预约模型初始化失败！");
//        return;
//    }

//    auto appointmentModel = IDatabase::getInstance().appointmentTabModel;
//    m_mapper = new QDataWidgetMapper(this);
//    m_mapper->setModel(appointmentModel);
//    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

//    // ========== 校验并绑定字段（关键！） ==========
//    // 1. 绑定PATIENT_ID
//    int patientIdCol = appointmentModel->fieldIndex("PATIENT_ID");
//    if (patientIdCol == -1) {
//        QMessageBox::warning(this, "错误", "Appointment表中不存在PATIENT_ID字段！");
//        return;
//    }
//    m_mapper->addMapping(ui->comboPatient, patientIdCol);

//    // 2. 绑定DOCTOR_ID
//    int doctorIdCol = appointmentModel->fieldIndex("DOCTOR_ID");
//    if (doctorIdCol == -1) {
//        QMessageBox::warning(this, "错误", "Appointment表中不存在DOCTOR_ID字段！");
//        return;
//    }
//    m_mapper->addMapping(ui->comboDoctor, doctorIdCol);

//    // 3. 绑定APPOINT_DATE
//    int dateCol = appointmentModel->fieldIndex("APPOINT_DATE");
//    if (dateCol == -1) {
//        QMessageBox::warning(this, "错误", "Appointment表中不存在APPOINT_DATE字段！");
//        return;
//    }
//    m_mapper->addMapping(ui->dateEdit, dateCol);

//    // 4. 绑定STATUS
//    int statusCol = appointmentModel->fieldIndex("STATUS");
//    if (statusCol == -1) {
//        QMessageBox::warning(this, "错误", "Appointment表中不存在STATUS字段！");
//        return;
//    }
//    m_mapper->addMapping(ui->comboStatus, statusCol);

//    // 设置当前行
//    if (m_rowNo >= 0) {
//        m_mapper->setCurrentIndex(m_rowNo);
//        // ... 原有匹配下拉框的逻辑 ...
//    }
//}


//void AppointmentEditView::loadComboBoxData()
//{
//    // ========== 加载患者下拉框（表名是Patient，大写P） ==========
//    QString patientSql = "SELECT ID, NAME FROM Patient"; // 表名和数据库一致（大写P）
//    QSqlQuery patientQuery = IDatabase::getInstance().querySql(patientSql);

//    // 打印错误（如果有）
//    if (patientQuery.lastError().isValid()) {
//        qDebug() << "患者查询错误：" << patientQuery.lastError().text();
//    } else {
//        qDebug() << "患者查询成功，记录数：" << patientQuery.size();
//    }

//    ui->comboPatient->clear();
//    // 遍历患者数据
//    while (patientQuery.next()) {
//        QString id = patientQuery.value(0).toString();
//        QString name = patientQuery.value(1).toString();
//        ui->comboPatient->addItem(name, id); // 下拉框显示“姓名”，存“ID”
//        qDebug() << "加载患者：" << name << "(ID:" << id << ")";
//    }


//    // ========== 加载医生下拉框（表名是Doctor，大写D） ==========
//    QString doctorSql = "SELECT ID, NAME FROM Doctor"; // 表名和数据库一致（大写D）
//    QSqlQuery doctorQuery = IDatabase::getInstance().querySql(doctorSql);

//    // 打印错误（如果有）
//    if (doctorQuery.lastError().isValid()) {
//        qDebug() << "医生查询错误：" << doctorQuery.lastError().text();
//    } else {
//        qDebug() << "医生查询成功，记录数：" << doctorQuery.size();
//    }

//    ui->comboDoctor->clear();
//    // 遍历医生数据（注意：这里用doctorQuery，不是patientQuery）
//    while (doctorQuery.next()) {
//        QString id = doctorQuery.value(0).toString();
//        QString name = doctorQuery.value(1).toString();
//        ui->comboDoctor->addItem(name, id); // 下拉框显示“姓名”，存“ID”
//        qDebug() << "加载医生：" << name << "(ID:" << id << ")";
//    }


//    // ========== 加载状态下拉框（固定选项） ==========
//    ui->comboStatus->clear();
//    ui->comboStatus->addItems({"未确认", "已确认", "已取消"});
//}


//bool AppointmentEditView::validateForm()
//{
//    // 校验患者
//    if (ui->comboPatient->currentData().toString().isEmpty()) {
//        QMessageBox::warning(this, "提示", "请选择患者！");
//        return false;
//    }
//    // 校验医生
//    if (ui->comboDoctor->currentData().toString().isEmpty()) {
//        QMessageBox::warning(this, "提示", "请选择医生！");
//        return false;
//    }
//    // 校验日期
//    if (ui->dateEdit->date() < QDate::currentDate()) {
//        QMessageBox::warning(this, "提示", "预约日期不能早于当前日期！");
//        return false;
//    }
//    return true;
//}

//void AppointmentEditView::on_btSave_clicked()
//{
//    if (!validateForm()) {
//        return;
//    }

//    auto &db = IDatabase::getInstance();
//    auto model = db.appointmentTabModel;
//    // 新增时插入行+生成ID
//    if (m_rowNo == -1) {
//        m_rowNo = model->rowCount();
//        if (!model->insertRow(m_rowNo)) {
//            QMessageBox::warning(this, "失败", "插入行失败：" + model->lastError().text());
//            return;
//        }
//        // 强制生成ID（必须，否则ID为空）
//        QString uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
//        int idCol = model->fieldIndex("ID");
//        model->setData(model->index(m_rowNo, idCol), uuid);
//    }

//    // 手动赋值（将QDate转为字符串，适配数据库类型）
//    int patientCol = model->fieldIndex("PATIENT_ID");
//    int doctorCol = model->fieldIndex("DOCTOR_ID");
//    int dateCol = model->fieldIndex("APPOINT_DATE");
//    int statusCol = model->fieldIndex("STATUS");

//    model->setData(model->index(m_rowNo, patientCol), ui->comboPatient->currentData());
//    model->setData(model->index(m_rowNo, doctorCol), ui->comboDoctor->currentData());
//    // 核心修复：QDate转字符串
//    model->setData(model->index(m_rowNo, dateCol), ui->dateEdit->date().toString("yyyy-MM-dd"));
//    model->setData(model->index(m_rowNo, statusCol), ui->comboStatus->currentText());

//    // 提交到数据库
//    if (db.submitAppointmentEdit()) {
//        QMessageBox::information(this, "成功", "预约保存成功！");
//        model->select();
//        emit appointmentSaved();
//        emit goPreviousView();
//    } else {
//        QMessageBox::warning(this, "失败", "保存失败：" + model->lastError().text());
//        model->revertAll();
//    }
//}


//void AppointmentEditView::on_btCancel_clicked()
//{
//    // 撤销修改并返回
//    IDatabase::getInstance().revertAppointmentEdit();
//    emit goPreviousView();
//}





//#include "AppointmentEditView.h"
//#include "ui_AppointmentEditView.h"
//#include "idatabase.h"
//#include <QMessageBox>
//#include <QUuid>
//#include <QDebug>

//AppointmentEditView::AppointmentEditView(QWidget *parent, int rowNo)
//    : QWidget(parent), ui(new Ui::AppointmentEditView), m_rowNo(rowNo)
//{
//    ui->setupUi(this);
//    this->setWindowTitle("编辑预约");
//    initUI();
//    loadComboBoxData();
//}

//AppointmentEditView::~AppointmentEditView()
//{
//    delete ui;
//    delete m_mapper;
//}

//void AppointmentEditView::initUI()
//{
//    auto &db = IDatabase::getInstance();
//    if (!db.initAppointmentModel()) {
//        QMessageBox::critical(this, "错误", "预约模型初始化失败！");
//        return;
//    }

//    auto model = db.appointmentTabModel;
//    m_mapper = new QDataWidgetMapper(this);
//    m_mapper->setModel(model);
//    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

//    // 绑定字段
//    int pCol = model->fieldIndex("PATIENT_ID");
//    int dCol = model->fieldIndex("DOCTOR_ID");
//    int dateCol = model->fieldIndex("APPOINT_DATE");
//    int sCol = model->fieldIndex("STATUS");
//    if (pCol != -1) m_mapper->addMapping(ui->comboPatient, pCol);
//    if (dCol != -1) m_mapper->addMapping(ui->comboDoctor, dCol);
//    if (dateCol != -1) m_mapper->addMapping(ui->dateEdit, dateCol);
//    if (sCol != -1) m_mapper->addMapping(ui->comboStatus, sCol);

//    if (m_rowNo >= 0) m_mapper->setCurrentIndex(m_rowNo);
//}

//void AppointmentEditView::loadComboBoxData()
//{
//    auto &db = IDatabase::getInstance();

//    // 加载患者
//    QSqlQuery pQuery = db.querySql("SELECT ID, NAME FROM Patient");
//    ui->comboPatient->clear();
//    while (pQuery.next()) {
//        ui->comboPatient->addItem(pQuery.value(1).toString(), pQuery.value(0).toString());
//    }

//    // 加载医生
//    QSqlQuery dQuery = db.querySql("SELECT ID, NAME FROM Doctor");
//    ui->comboDoctor->clear();
//    while (dQuery.next()) {
//        ui->comboDoctor->addItem(dQuery.value(1).toString(), dQuery.value(0).toString());
//    }

//    // 加载状态
//    ui->comboStatus->clear();
//    ui->comboStatus->addItems({"未确认", "已确认", "已取消"});
//}

//bool AppointmentEditView::validateForm()
//{
//    if (ui->comboPatient->currentData().toString().isEmpty()) {
//        QMessageBox::warning(this, "提示", "请选择患者！");
//        return false;
//    }
//    if (ui->comboDoctor->currentData().toString().isEmpty()) {
//        QMessageBox::warning(this, "提示", "请选择医生！");
//        return false;
//    }
//    if (ui->dateEdit->date() < QDate::currentDate()) {
//        QMessageBox::warning(this, "提示", "预约日期不能早于当前日期！");
//        return false;
//    }
//    return true;
//}

//void AppointmentEditView::on_btSave_clicked()
//{
//    if (!validateForm()) return;

//    auto &db = IDatabase::getInstance();
//    auto model = db.appointmentTabModel;
//    int oldRow = m_rowNo;

//    // 新增行+生成ID
//    if (m_rowNo == -1) {
//        m_rowNo = model->rowCount();
//        if (!model->insertRow(m_rowNo)) {
//            QMessageBox::warning(this, "失败", "插入行失败：" + model->lastError().text());
//            m_rowNo = oldRow;
//            return;
//        }
//        QString uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
//        model->setData(model->index(m_rowNo, model->fieldIndex("ID")), uuid);
//    }

//    // 赋值字段
//    model->setData(model->index(m_rowNo, model->fieldIndex("PATIENT_ID")),
//                   ui->comboPatient->currentData());
//    model->setData(model->index(m_rowNo, model->fieldIndex("DOCTOR_ID")),
//                   ui->comboDoctor->currentData());
//    model->setData(model->index(m_rowNo, model->fieldIndex("APPOINT_DATE")),
//                   ui->dateEdit->date().toString("yyyy-MM-dd"));
//    model->setData(model->index(m_rowNo, model->fieldIndex("STATUS")), ui->comboStatus->currentText());

//    // 提交
//    model->database().transaction();
//    if (db.submitAppointmentEdit()) {
//        model->database().commit();
//        QMessageBox::information(this, "成功", "预约保存成功！");
//        model->select();
//        emit appointmentSaved();
//        emit goPreviousView();
//    } else {
//        model->database().rollback();
//        QMessageBox::warning(this, "失败", "保存失败：" + model->lastError().text());
//        if (oldRow == -1) model->removeRow(m_rowNo);
//        m_rowNo = oldRow;
//    }
//}

//void AppointmentEditView::on_btCancel_clicked()
//{
//    IDatabase::getInstance().revertAppointmentEdit();
//    emit goPreviousView();
//}


#include "AppointmentEditView.h"
#include "ui_AppointmentEditView.h"
#include "idatabase.h"
#include <QMessageBox>
#include <QUuid>
#include <QDebug>
#include <QDate>

AppointmentEditView::AppointmentEditView(QWidget *parent, int rowNo)
    : QWidget(parent), ui(new Ui::AppointmentEditView), m_rowNo(rowNo)
{
    ui->setupUi(this);
    this->setWindowTitle("编辑预约");
    initUI();
    loadComboBoxData();
}

AppointmentEditView::~AppointmentEditView()
{
    delete ui;
    delete m_mapper;
}

void AppointmentEditView::initUI()
{
    auto &db = IDatabase::getInstance();
    if (!db.initAppointmentModel()) {
        QMessageBox::critical(this, "错误", "预约模型初始化失败！");
        return;
    }

    auto model = db.appointmentTabModel;
    m_mapper = new QDataWidgetMapper(this);
    m_mapper->setModel(model);
    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    // 绑定字段
    int pCol = model->fieldIndex("PATIENT_ID");
    int dCol = model->fieldIndex("DOCTOR_ID");
    int dateCol = model->fieldIndex("APPOINT_DATE");
    int sCol = model->fieldIndex("STATUS");

    if (pCol != -1) m_mapper->addMapping(ui->comboPatient, pCol);
    if (dCol != -1) m_mapper->addMapping(ui->comboDoctor, dCol);
    if (dateCol != -1) m_mapper->addMapping(ui->dateEdit, dateCol);
    if (sCol != -1) m_mapper->addMapping(ui->comboStatus, sCol);

    if (m_rowNo >= 0) m_mapper->setCurrentIndex(m_rowNo);
}

void AppointmentEditView::loadComboBoxData()
{
    auto &db = IDatabase::getInstance();

    // 加载患者
    QSqlQuery pQuery = db.querySql("SELECT ID, NAME FROM Patient");
    ui->comboPatient->clear();
    while (pQuery.next()) {
        ui->comboPatient->addItem(pQuery.value(1).toString(), pQuery.value(0).toString());
    }

    // 加载医生
    QSqlQuery dQuery = db.querySql("SELECT ID, NAME FROM Doctor");
    ui->comboDoctor->clear();
    while (dQuery.next()) {
        ui->comboDoctor->addItem(dQuery.value(1).toString(), dQuery.value(0).toString());
    }

    // 加载状态
    ui->comboStatus->clear();
    ui->comboStatus->addItems({"未确认", "已确认", "已取消"});
}

bool AppointmentEditView::validateForm()
{
    // 校验患者
    if (ui->comboPatient->currentData().toString().isEmpty()) {
        QMessageBox::warning(this, "提示", "请选择患者！");
        return false;
    }

    // 校验医生
    if (ui->comboDoctor->currentData().toString().isEmpty()) {
        QMessageBox::warning(this, "提示", "请选择医生！");
        return false;
    }

    // 校验日期
    if (ui->dateEdit->date() < QDate::currentDate()) {
        QMessageBox::warning(this, "提示", "预约日期不能早于当前日期！");
        return false;
    }

    return true;
}

//void AppointmentEditView::on_btSave_clicked()
//{
//    if (!validateForm()) return;

//    auto &db = IDatabase::getInstance();
//    auto model = db.appointmentTabModel;
//    int oldRow = m_rowNo;

//    // 新增行+生成ID
//    if (m_rowNo == -1) {
//        m_rowNo = model->rowCount();
//        if (!model->insertRow(m_rowNo)) {
//            QMessageBox::warning(this, "失败", "插入行失败：" + model->lastError().text());
//            m_rowNo = oldRow;
//            return;
//        }
//        QString uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
//        model->setData(model->index(m_rowNo, model->fieldIndex("ID")), uuid);
//    }

//    // 赋值字段
//    model->setData(model->index(m_rowNo, model->fieldIndex("PATIENT_ID")),
//                   ui->comboPatient->currentData());
//    model->setData(model->index(m_rowNo, model->fieldIndex("DOCTOR_ID")),
//                   ui->comboDoctor->currentData());
//    model->setData(model->index(m_rowNo, model->fieldIndex("APPOINT_DATE")),
//                   ui->dateEdit->date().toString("yyyy-MM-dd"));
//    model->setData(model->index(m_rowNo, model->fieldIndex("STATUS")), ui->comboStatus->currentText());

//    // 事务提交
//    model->database().transaction();
//    if (db.submitAppointmentEdit()) {
//        model->database().commit();
//        QMessageBox::information(this, "成功", "预约保存成功！");
//        model->select();
//        emit appointmentSaved();
//        emit goPreviousView();
//    } else {
//        model->database().rollback();
//        QMessageBox::warning(this, "失败", "保存失败：" + model->lastError().text());
//        if (oldRow == -1) model->removeRow(m_rowNo);
//        m_rowNo = oldRow;
//    }
//    if (db.submitAppointmentEdit()) {
//        model->database().commit();
//        QMessageBox::information(this, "成功", "预约保存成功！");
//        model->select(); // 先刷新模型本身
//        emit appointmentSaved(); // 发送“保存成功”信号
//        emit goPreviousView();
//    }
//}
void AppointmentEditView::on_btSave_clicked()
{
    if (!validateForm()) return;

    auto &db = IDatabase::getInstance();
    auto model = db.appointmentTabModel;
    int oldRow = m_rowNo;

    // 新增行+生成ID
    if (m_rowNo == -1) {
        m_rowNo = model->rowCount();
        if (!model->insertRow(m_rowNo)) {
            QMessageBox::warning(this, "失败", "插入行失败：" + model->lastError().text());
            m_rowNo = oldRow;
            return;
        }
        QString uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
        model->setData(model->index(m_rowNo, model->fieldIndex("ID")), uuid);
    }

    // 赋值字段
    model->setData(model->index(m_rowNo, model->fieldIndex("PATIENT_ID")),
                   ui->comboPatient->currentData());
    model->setData(model->index(m_rowNo, model->fieldIndex("DOCTOR_ID")),
                   ui->comboDoctor->currentData());
    model->setData(model->index(m_rowNo, model->fieldIndex("APPOINT_DATE")),
                   ui->dateEdit->date().toString("yyyy-MM-dd"));
    model->setData(model->index(m_rowNo, model->fieldIndex("STATUS")), ui->comboStatus->currentText());

    // ========== 仅保留一段事务提交逻辑 ==========
    model->database().transaction(); // 开启事务
    bool submitOk = db.submitAppointmentEdit(); // 提交修改
    if (submitOk) {
        model->database().commit(); // 提交事务
        QMessageBox::information(this, "成功", "预约保存成功！");
        model->select(); // 刷新模型数据
        emit appointmentSaved(); // 通知列表页刷新
        emit goPreviousView();
    } else {
        model->database().rollback(); // 回滚事务
        QMessageBox::warning(this, "失败", "保存失败：" + model->lastError().text());
        if (oldRow == -1) model->removeRow(m_rowNo); // 新增失败则删除空行
        m_rowNo = oldRow;
    }
}


void AppointmentEditView::on_btCancel_clicked()
{
    IDatabase::getInstance().revertAppointmentEdit();
    emit goPreviousView();
}
