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

    // 必须在 setupUi 之后设置！
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);

    setWindowTitle(rowNo == -1 ? "新增预约" : "编辑预约");

    loadComboBoxData();
    initUI();
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

    if (m_rowNo >= 0) {
        m_mapper->setCurrentIndex(m_rowNo);

        // ========== 新增：编辑模式下匹配下拉框选中项 ==========
        // 1. 匹配患者下拉框（根据PATIENT_ID找对应的名称项）
        QString patientId = model->index(m_rowNo, pCol).data().toString();
        int patientIdx = ui->comboPatient->findData(patientId);
        if (patientIdx != -1) {
            ui->comboPatient->setCurrentIndex(patientIdx);
        }

        // 2. 匹配医生下拉框（根据DOCTOR_ID找对应的名称项）
        QString doctorId = model->index(m_rowNo, dCol).data().toString();
        int doctorIdx = ui->comboDoctor->findData(doctorId);
        if (doctorIdx != -1) {
            ui->comboDoctor->setCurrentIndex(doctorIdx);
        }

        // 3. 匹配状态下拉框（根据STATUS文本匹配）
        QString status = model->index(m_rowNo, sCol).data().toString();
        int statusIdx = ui->comboStatus->findText(status);
        if (statusIdx != -1) {
            ui->comboStatus->setCurrentIndex(statusIdx);
        }

        // 4. 匹配日期（QDateEdit需要字符串转QDate）
        QString dateStr = model->index(m_rowNo, dateCol).data().toString();
        QDate appDate = QDate::fromString(dateStr, "yyyy-MM-dd");
        if (appDate.isValid()) {
            ui->dateEdit->setDate(appDate);
        }
    }
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
