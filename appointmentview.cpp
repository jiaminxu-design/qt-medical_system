#include"appointmenteditview.h"
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

    // 提前绑定：只要新增窗口发送appointmentSaved，就刷新表格
    connect(this, &AppointmentView::goAppointmentEditView, this, [ = ](int rowNo) {
        AppointmentEditView *editView = new AppointmentEditView(nullptr, rowNo);
        connect(editView, &AppointmentEditView::appointmentSaved, this, &AppointmentView::refreshTable);
        connect(editView, &AppointmentEditView::goPreviousView, editView, &QWidget::close);
        editView->setWindowFlags(Qt::Dialog);
        editView->show();
    });

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
    // ========== 1. 强制初始化预约模型（确保模型有效） ==========
    auto &db = IDatabase::getInstance();
    if (!db.initAppointmentModel()) {
        QMessageBox::critical(this, "错误", "预约模型初始化失败！");
        return;
    }
    auto appointmentModel = db.appointmentTabModel;

    // ========== 2. 表格与模型深度绑定（解决渲染失效） ==========
    ui->tableView->setModel(appointmentModel);          // 绑定数据模型
    ui->tableView->setSelectionModel(db.theAppointmentSelection); // 绑定选择模型
    ui->tableView->clearSpans();                        // 清空表格跨度（避免布局异常）
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // 整行选择
    ui->tableView->setAlternatingRowColors(true);       // 隔行变色（恢复样式）

    // ========== 3. 强制设置表头（解决表头消失） ==========
    // 先清空原有表头映射
    appointmentModel->setHeaderData(0, Qt::Horizontal, "");
    // 按字段索引绑定表头（必须和数据库字段一致）
    int colID = appointmentModel->fieldIndex("ID");
    int colPatientID = appointmentModel->fieldIndex("PATIENT_ID");
    int colDoctorID = appointmentModel->fieldIndex("DOCTOR_ID");
    int colAppDate = appointmentModel->fieldIndex("APPOINT_DATE");
    int colStatus = appointmentModel->fieldIndex("STATUS");

    appointmentModel->setHeaderData(colPatientID, Qt::Horizontal, "患者ID");
    appointmentModel->setHeaderData(colDoctorID, Qt::Horizontal, "医生ID");
    appointmentModel->setHeaderData(colAppDate, Qt::Horizontal, "预约日期");
    appointmentModel->setHeaderData(colStatus, Qt::Horizontal, "预约状态");

    // ========== 4. 隐藏ID列 + 调整列宽（用户无需看到ID） ==========
    ui->tableView->hideColumn(colID);
    ui->tableView->resizeColumnToContents(colPatientID);
    ui->tableView->resizeColumnToContents(colDoctorID);
    ui->tableView->setColumnWidth(colAppDate, 150); // 预约日期列固定宽度
    ui->tableView->setColumnWidth(colStatus, 100);  // 状态列固定宽度

    // ========== 5. 强制刷新模型数据（解决数据不加载） ==========
    appointmentModel->select();
    qDebug() << "预约模型最终记录数：" << appointmentModel->rowCount(); // 验证数据

    // ========== 6. 修复按钮状态（编辑/删除默认禁用） ==========
    ui->btEdit->setEnabled(false);
    ui->btDelete->setEnabled(false);
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
    if (!db.appointmentTabModel) return;

    // 1. 强制重新查询数据库（拉取最新数据）
    db.appointmentTabModel->select();

    // 2. 轻量刷新视图（替代reset，避免清空模型）
    ui->tableView->viewport()->update();
    ui->tableView->horizontalHeader()->update();

    // 3. 重新设置表头（防止表头丢失）
    auto model = db.appointmentTabModel;
    model->setHeaderData(model->fieldIndex("PATIENT_ID"), Qt::Horizontal, "患者ID");
    model->setHeaderData(model->fieldIndex("DOCTOR_ID"), Qt::Horizontal, "医生ID");
    model->setHeaderData(model->fieldIndex("APPOINT_DATE"), Qt::Horizontal, "预约日期");
    model->setHeaderData(model->fieldIndex("STATUS"), Qt::Horizontal, "预约状态");

    // 4. 隐藏ID列
    ui->tableView->hideColumn(model->fieldIndex("ID"));

    qDebug() << "刷新后预约记录数：" << model->rowCount();
}

void AppointmentView::on_btAdd_clicked()
{
    // ========== 关键：弹窗独立（解决界面重叠） ==========
    AppointmentEditView *editView = new AppointmentEditView(nullptr, -1);
    editView->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    editView->setWindowTitle("新增预约");

    // ========== 绑定保存成功信号（核心：触发表格刷新） ==========
    // 先检查槽函数是否存在（避免 No such slot 警告）
    if (this->metaObject()->indexOfSlot("refreshTable()") != -1) {
        connect(editView, &AppointmentEditView::appointmentSaved,
                this, &AppointmentView::refreshTable, Qt::QueuedConnection); // 队列连接防止界面卡顿
    }

    // ========== 绑定关闭信号 ==========
    connect(editView, &AppointmentEditView::goPreviousView, editView, &QWidget::close);

    // ========== 显示弹窗 ==========
    editView->show();
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


