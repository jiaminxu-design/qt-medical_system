//#include "AppointmentView.h"
//#include "ui_AppointmentView.h"
//#include "idatabase.h"
//#include <QMessageBox>
//#include <QHeaderView>

//AppointmentView::AppointmentView(QWidget *parent)
//    : QWidget(parent)
//    , ui(new Ui::AppointmentView)
//{
//    ui->setupUi(this);
//    this->setWindowTitle("预约排班管理");

//    // 初始化UI和数据
//    initUI();
//    // 绑定表格选中事件（控制按钮启用/禁用）
//    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
//    this, [ = ](const QItemSelection & selected, const QItemSelection & deselected) {
//        bool hasSelection = !selected.isEmpty();
//        ui->btEdit->setEnabled(hasSelection);
//        ui->btDelete->setEnabled(hasSelection);
//    });
//}

//AppointmentView::~AppointmentView()
//{
//    delete ui;
//}

//void AppointmentView::initUI()
//{
//    // 1. 强制初始化预约模型（确保模型存在）
//    auto &db = IDatabase::getInstance();
//    if (!db.initAppointmentModel()) {
//        QMessageBox::critical(this, "错误", "预约模型初始化失败！");
//        return;
//    }

//    auto model = db.appointmentTabModel;
//    model->select(); // 重新加载表数据+结构

//    // 2. 重新绑定表格（必须覆盖旧模型）
//    ui->tableView->setModel(model);
//    ui->tableView->setSelectionModel(db.theAppointmentSelection);

//    // 3. 重新设置表头（确保列名显示）
//    model->setHeaderData(model->fieldIndex("PATIENT_ID"), Qt::Horizontal, "患者ID");
//    model->setHeaderData(model->fieldIndex("DOCTOR_ID"), Qt::Horizontal, "医生ID");
//    model->setHeaderData(model->fieldIndex("APPOINT_DATE"), Qt::Horizontal, "预约日期");
//    model->setHeaderData(model->fieldIndex("STATUS"), Qt::Horizontal, "预约状态");

//    // 4. 表格样式（隐藏ID列）
//    ui->tableView->hideColumn(model->fieldIndex("ID"));
//    ui->tableView->horizontalHeader()->setStretchLastSection(true);
//    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

//    // 5. 加载医生筛选下拉框
//    loadDoctorList();

//    // 手动绑定按钮信号（修复No matching signal错误）
//    connect(ui->btFilter, &QPushButton::clicked, this, &AppointmentView::on_btFilter_clicked);
//    connect(ui->btAdd, &QPushButton::clicked, this, &AppointmentView::on_btAdd_clicked);
//    connect(ui->btEdit, &QPushButton::clicked, this, &AppointmentView::on_btEdit_clicked);
//    connect(ui->btDelete, &QPushButton::clicked, this, &AppointmentView::on_btDelete_clicked);
//}


//void AppointmentView::loadDoctorList()
//{
//    // 删掉is_deleted条件（数据库中无该字段）
//    QSqlQuery query = IDatabase::getInstance().querySql("SELECT ID, NAME FROM Doctor");
//    ui->comboDoctor->clear();
//    ui->comboDoctor->addItem("全部医生", "");

//    while (query.next()) {
//        QString doctorId = query.value(0).toString();
//        QString doctorName = query.value(1).toString();
//        ui->comboDoctor->addItem(doctorName, doctorId);
//    }
//}


//void AppointmentView::refreshTable()
//{
//    auto &db = IDatabase::getInstance();
//    auto model = db.appointmentTabModel;

//    // 1. 清除筛选条件（避免残留过滤）
//    model->setFilter("");
//    // 2. 强制重新查询数据
//    model->select();
//    // 3. 重置表格（刷新视图）
//    ui->tableView->reset();
//    // 4. 重新设置表头（防止列名丢失）
//    model->setHeaderData(model->fieldIndex("PATIENT_ID"), Qt::Horizontal, "患者ID");
//    model->setHeaderData(model->fieldIndex("DOCTOR_ID"), Qt::Horizontal, "医生ID");
//    model->setHeaderData(model->fieldIndex("APPOINT_DATE"), Qt::Horizontal, "预约日期");
//    model->setHeaderData(model->fieldIndex("STATUS"), Qt::Horizontal, "预约状态");
//}


//void AppointmentView::on_btAdd_clicked()
//{
//    // 直接传递-1表示新增（不再调用addNewAppointment，避免重复插行）
//    emit goAppointmentEditView(-1);
//}


//void AppointmentView::on_btEdit_clicked()
//{
//    // 获取选中行
//    QModelIndex curIndex = ui->tableView->currentIndex();
//    if (!curIndex.isValid()) {
//        QMessageBox::warning(this, "提示", "请选择要编辑的预约！");
//        return;
//    }
//    emit goAppointmentEditView(curIndex.row());
//}

//void AppointmentView::on_btDelete_clicked()
//{
//    if (QMessageBox::question(this, "确认", "确定要删除该预约吗？") != QMessageBox::Yes) {
//        return;
//    }

//    if (IDatabase::getInstance().deleteCurrentAppointment()) {
//        QMessageBox::information(this, "成功", "预约删除成功！");
//        refreshTable();
//    } else {
//        QMessageBox::warning(this, "失败", "预约删除失败！");
//    }
//}


//void AppointmentView::on_btFilter_clicked()
//{
//    QStringList filters;
//    // 1. 医生筛选
//    QString doctorId = ui->comboDoctor->currentData().toString();
//    if (!doctorId.isEmpty()) {
//        filters << QString("DOCTOR_ID = '%1'").arg(doctorId);
//    }
//    // 2. 状态筛选
//    QString status = ui->comboStatusFilter->currentText();
//    if (status != "全部状态") {
//        filters << QString("STATUS = '%1'").arg(status);
//    }
//    // 3. 注释掉日期筛选（或改为可选）
//    // QDate selectedDate = ui->dateEditSelected->date();
//    // filters << QString("APPOINT_DATE = '%1'").arg(selectedDate.toString("yyyy-MM-dd"));
//    auto &db = IDatabase::getInstance();
//    db.searchAppointment(filters.join(" AND "));
//}


//void AppointmentView::on_btConfirm_clicked()
//{
//    // 确认预约：修改STATUS为"已确认"
//    QModelIndex curIndex = ui->tableView->currentIndex();
//    if (!curIndex.isValid()) {
//        QMessageBox::warning(this, "提示", "请选择要确认的预约！");
//        return;
//    }

//    auto model = IDatabase::getInstance().appointmentTabModel;
//    int statusCol = model->fieldIndex("STATUS");
//    model->setData(model->index(curIndex.row(), statusCol), "已确认");

//    if (IDatabase::getInstance().submitAppointmentEdit()) {
//        QMessageBox::information(this, "成功", "预约已确认！");
//        refreshTable();
//    } else {
//        QMessageBox::warning(this, "失败", "确认预约失败！");
//    }
//}

//void AppointmentView::on_btCancel_clicked()
//{
//    // 取消预约：修改STATUS为"已取消"
//    QModelIndex curIndex = ui->tableView->currentIndex();
//    if (!curIndex.isValid()) {
//        QMessageBox::warning(this, "提示", "请选择要取消的预约！");
//        return;
//    }

//    auto model = IDatabase::getInstance().appointmentTabModel;
//    int statusCol = model->fieldIndex("STATUS");
//    model->setData(model->index(curIndex.row(), statusCol), "已取消");

//    if (IDatabase::getInstance().submitAppointmentEdit()) {
//        QMessageBox::information(this, "成功", "预约已取消！");
//        refreshTable();
//    } else {
//        QMessageBox::warning(this, "失败", "取消预约失败！");
//    }
//}


//#include "AppointmentView.h"
//#include "ui_AppointmentView.h"
//#include "idatabase.h"
//#include <QMessageBox>
//#include <QHeaderView>

//AppointmentView::AppointmentView(QWidget *parent)
//    : QWidget(parent), ui(new Ui::AppointmentView)
//{
//    ui->setupUi(this);
//    this->setWindowTitle("预约排班管理");
//    initUI();

//    // 选中事件绑定
//    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
//    this, [ = ](const QItemSelection & s) {
//        bool hasSel = !s.isEmpty();
//        ui->btEdit->setEnabled(hasSel);
//        ui->btDelete->setEnabled(hasSel);
//    });
//}

//AppointmentView::~AppointmentView()
//{
//    delete ui;
//}

//void AppointmentView::initUI()
//{
//    auto &db = IDatabase::getInstance();
//    if (!db.initAppointmentModel()) {
//        QMessageBox::critical(this, "错误", "预约模型初始化失败！");
//        return;
//    }

//    auto model = db.appointmentTabModel;
//    model->select();

//    // 绑定表格
//    ui->tableView->setModel(model);
//    ui->tableView->setSelectionModel(db.theAppointmentSelection);

//    // 设置表头
//    model->setHeaderData(model->fieldIndex("PATIENT_ID"), Qt::Horizontal, "患者ID");
//    model->setHeaderData(model->fieldIndex("DOCTOR_ID"), Qt::Horizontal, "医生ID");
//    model->setHeaderData(model->fieldIndex("APPOINT_DATE"), Qt::Horizontal, "预约日期");
//    model->setHeaderData(model->fieldIndex("STATUS"), Qt::Horizontal, "预约状态");

//    // 表格样式
//    ui->tableView->hideColumn(model->fieldIndex("ID"));
//    ui->tableView->horizontalHeader()->setStretchLastSection(true);
//    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

//    loadDoctorList();

//    // 手动绑定按钮
//    connect(ui->btFilter, &QPushButton::clicked, this, &AppointmentView::on_btFilter_clicked);
//    connect(ui->btAdd, &QPushButton::clicked, this, &AppointmentView::on_btAdd_clicked);
//    connect(ui->btEdit, &QPushButton::clicked, this, &AppointmentView::on_btEdit_clicked);
//    connect(ui->btDelete, &QPushButton::clicked, this, &AppointmentView::on_btDelete_clicked);

//}

//void AppointmentView::loadDoctorList()
//{
//    QSqlQuery query = IDatabase::getInstance().querySql("SELECT ID, NAME FROM Doctor");
//    ui->comboDoctor->clear();
//    ui->comboDoctor->addItem("全部医生", "");
//    while (query.next()) {
//        ui->comboDoctor->addItem(query.value(1).toString(), query.value(0).toString());
//    }
//}

//void AppointmentView::refreshTable()
//{
//    auto &db = IDatabase::getInstance();
//    auto model = db.appointmentTabModel;
//    model->setFilter("");
//    model->select();
//    ui->tableView->reset();
//    // 重置表头
//    model->setHeaderData(model->fieldIndex("PATIENT_ID"), Qt::Horizontal, "患者ID");
//}

//void AppointmentView::on_btAdd_clicked()
//{
//    emit goAppointmentEditView(-1);
//}

//void AppointmentView::on_btEdit_clicked()
//{
//    QModelIndex idx = ui->tableView->currentIndex();
//    if (!idx.isValid()) {
//        QMessageBox::warning(this, "提示", "请选择要编辑的预约！");
//        return;
//    }
//    emit goAppointmentEditView(idx.row());
//}

//void AppointmentView::on_btDelete_clicked()
//{
//    if (QMessageBox::question(this, "确认",
//                              "确定删除该预约吗？") != QMessageBox::Yes) return;
//    auto &db = IDatabase::getInstance();
//    if (db.deleteCurrentAppointment()) {
//        if (db.submitAppointmentEdit()) {
//            QMessageBox::information(this, "成功", "预约删除成功！");
//            refreshTable();
//        }
//    } else {
//        QMessageBox::warning(this, "失败", "删除失败！");
//    }
//}

//void AppointmentView::on_btFilter_clicked()
//{
//    QStringList filters;
//    QString docId = ui->comboDoctor->currentData().toString();
//    if (!docId.isEmpty()) filters << QString("DOCTOR_ID = '%1'").arg(docId);
//    QString status = ui->comboStatusFilter->currentText();
//    if (status != "全部状态") filters << QString("STATUS = '%1'").arg(status);
//    IDatabase::getInstance().searchAppointment(filters.join(" AND "));
//}

//void AppointmentView::on_btConfirm_clicked()
//{
//    QModelIndex idx = ui->tableView->currentIndex();
//    if (!idx.isValid()) {
//        QMessageBox::warning(this, "提示", "请选择要确认的预约！");
//        return;
//    }
//    auto model = IDatabase::getInstance().appointmentTabModel;
//    model->setData(model->index(idx.row(), model->fieldIndex("STATUS")), "已确认");
//    if (IDatabase::getInstance().submitAppointmentEdit()) {
//        QMessageBox::information(this, "成功", "预约已确认！");
//        refreshTable();
//    }
//}

//void AppointmentView::on_btCancel_clicked()
//{
//    QModelIndex idx = ui->tableView->currentIndex();
//    if (!idx.isValid()) {
//        QMessageBox::warning(this, "提示", "请选择要取消的预约！");
//        return;
//    }
//    auto model = IDatabase::getInstance().appointmentTabModel;
//    model->setData(model->index(idx.row(), model->fieldIndex("STATUS")), "已取消");
//    if (IDatabase::getInstance().submitAppointmentEdit()) {
//        QMessageBox::information(this, "成功", "预约已取消！");
//        refreshTable();
//    }
//}



//#include "AppointmentEditView.h"
//#include "ui_AppointmentEditView.h"
//#include "idatabase.h"
//#include <QMessageBox>
//#include <QUuid>
//#include <QDebug>
//#include <QDate>

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
//    model->setData(model->index(m_rowNo, model->fieldIndex("PATIENT_ID")), ui->comboPatient->currentData());
//    model->setData(model->index(m_rowNo, model->fieldIndex("DOCTOR_ID")), ui->comboDoctor->currentData());
//    model->setData(model->index(m_rowNo, model->fieldIndex("APPOINT_DATE")), ui->dateEdit->date().toString("yyyy-MM-dd"));
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
//}

//void AppointmentEditView::on_btCancel_clicked()
//{
//    IDatabase::getInstance().revertAppointmentEdit();
//    emit goPreviousView();
//}

#include "AppointmentView.h"
#include "ui_AppointmentView.h"
#include "idatabase.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QDebug>

AppointmentView::AppointmentView(QWidget *parent)
    : QWidget(parent), ui(new Ui::AppointmentView)
{
    ui->setupUi(this);
    this->setWindowTitle("预约排班管理");
    initUI();

    // 选中事件绑定
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
    this, [ = ](const QItemSelection & s) {
        bool hasSel = !s.isEmpty();
        ui->btEdit->setEnabled(hasSel);
        ui->btDelete->setEnabled(hasSel);
    });
}

AppointmentView::~AppointmentView()
{
    delete ui;
}

void AppointmentView::initUI()
{
    auto &db = IDatabase::getInstance();
    if (!db.initAppointmentModel()) {
        QMessageBox::critical(this, "错误", "预约模型初始化失败！");
        return;
    }

    auto model = db.appointmentTabModel;
    model->select();

    // 绑定表格
    ui->tableView->setModel(model);
    ui->tableView->setSelectionModel(db.theAppointmentSelection);

    // 设置表头
    model->setHeaderData(model->fieldIndex("PATIENT_ID"), Qt::Horizontal, "患者ID");
    model->setHeaderData(model->fieldIndex("DOCTOR_ID"), Qt::Horizontal, "医生ID");
    model->setHeaderData(model->fieldIndex("APPOINT_DATE"), Qt::Horizontal, "预约日期");
    model->setHeaderData(model->fieldIndex("STATUS"), Qt::Horizontal, "预约状态");

    // 表格样式
    ui->tableView->hideColumn(model->fieldIndex("ID"));
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    loadDoctorList();

    // 手动绑定按钮信号
    connect(ui->btFilter, &QPushButton::clicked, this, &AppointmentView::on_btFilter_clicked);
    connect(ui->btAdd, &QPushButton::clicked, this, &AppointmentView::on_btAdd_clicked);
    connect(ui->btEdit, &QPushButton::clicked, this, &AppointmentView::on_btEdit_clicked);
    connect(ui->btDelete, &QPushButton::clicked, this, &AppointmentView::on_btDelete_clicked);
}

void AppointmentView::loadDoctorList()
{
    QSqlQuery query = IDatabase::getInstance().querySql("SELECT ID, NAME FROM Doctor");
    ui->comboDoctor->clear();
    ui->comboDoctor->addItem("全部医生", "");
    while (query.next()) {
        ui->comboDoctor->addItem(query.value(1).toString(), query.value(0).toString());
    }
}

void AppointmentView::refreshTable()
{
    auto &db = IDatabase::getInstance();
    auto model = db.appointmentTabModel;
    model->setFilter("");
    model->select();
    ui->tableView->reset();

    // 重置表头
    model->setHeaderData(model->fieldIndex("PATIENT_ID"), Qt::Horizontal, "患者ID");
    model->setHeaderData(model->fieldIndex("DOCTOR_ID"), Qt::Horizontal, "医生ID");
    model->setHeaderData(model->fieldIndex("APPOINT_DATE"), Qt::Horizontal, "预约日期");
    model->setHeaderData(model->fieldIndex("STATUS"), Qt::Horizontal, "预约状态");
}

void AppointmentView::on_btAdd_clicked()
{
    emit goAppointmentEditView(-1); // -1表示新增
}

void AppointmentView::on_btEdit_clicked()
{
    QModelIndex idx = ui->tableView->currentIndex();
    if (!idx.isValid()) {
        QMessageBox::warning(this, "提示", "请选择要编辑的预约！");
        return;
    }
    emit goAppointmentEditView(idx.row());
}

void AppointmentView::on_btDelete_clicked()
{
    if (QMessageBox::question(this, "确认",
                              "确定删除该预约吗？") != QMessageBox::Yes) return;

    auto &db = IDatabase::getInstance();
    if (db.deleteCurrentAppointment()) {
        if (db.submitAppointmentEdit()) {
            QMessageBox::information(this, "成功", "预约删除成功！");
            refreshTable();
        }
    } else {
        QMessageBox::warning(this, "失败", "删除失败！");
    }
}

void AppointmentView::on_btFilter_clicked()
{
    QStringList filters;
    // 医生筛选
    QString docId = ui->comboDoctor->currentData().toString();
    if (!docId.isEmpty()) filters << QString("DOCTOR_ID = '%1'").arg(docId);

    // 状态筛选
    QString status = ui->comboStatusFilter->currentText();
    if (status != "全部状态") filters << QString("STATUS = '%1'").arg(status);

    IDatabase::getInstance().searchAppointment(filters.join(" AND "));
}

void AppointmentView::on_btConfirm_clicked()
{
    QModelIndex idx = ui->tableView->currentIndex();
    if (!idx.isValid()) {
        QMessageBox::warning(this, "提示", "请选择要确认的预约！");
        return;
    }

    auto model = IDatabase::getInstance().appointmentTabModel;
    model->setData(model->index(idx.row(), model->fieldIndex("STATUS")), "已确认");

    if (IDatabase::getInstance().submitAppointmentEdit()) {
        QMessageBox::information(this, "成功", "预约已确认！");
        refreshTable();
    } else {
        QMessageBox::warning(this, "失败", "确认预约失败！");
    }
}

void AppointmentView::on_btCancel_clicked()
{
    QModelIndex idx = ui->tableView->currentIndex();
    if (!idx.isValid()) {
        QMessageBox::warning(this, "提示", "请选择要取消的预约！");
        return;
    }

    auto model = IDatabase::getInstance().appointmentTabModel;
    model->setData(model->index(idx.row(), model->fieldIndex("STATUS")), "已取消");

    if (IDatabase::getInstance().submitAppointmentEdit()) {
        QMessageBox::information(this, "成功", "预约已取消！");
        refreshTable();
    } else {
        QMessageBox::warning(this, "失败", "取消预约失败！");
    }
}
