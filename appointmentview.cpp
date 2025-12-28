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

    // 最终版：信号绑定窗口创建逻辑（已修正）
    connect(this, &AppointmentView::goAppointmentEditView, this, [ = ](int rowNo) {
        // 1. 关闭所有已打开的编辑窗口（避免重叠）
        QList<QWidget *> allWindows = QApplication::topLevelWidgets();
        for (QWidget *w : allWindows) {
            AppointmentEditView *editWin = qobject_cast<AppointmentEditView *>(w);
            if (editWin) {
                editWin->close();
                editWin->deleteLater();
            }
        }

        // 2. 创建窗口：父窗口必须为 nullptr（关键！）
        AppointmentEditView *editView = new AppointmentEditView(nullptr, rowNo);

        // ========== 新增这1行（核心修复） ==========
        editView->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
        // 区分新增/编辑标题
        editView->setWindowTitle(rowNo == -1 ? "新增预约" : "编辑预约");

        // 3. 绑定信号（保留你的逻辑）
        connect(editView, &AppointmentEditView::appointmentSaved,
                this, &AppointmentView::refreshTable, Qt::QueuedConnection);
        connect(editView, &AppointmentEditView::goPreviousView,
                editView, &QWidget::close, Qt::QueuedConnection);

        // 4. 显示窗口（直接show，不做任何布局添加）
        editView->show();
    });

    // 选中事件绑定：控制编辑/删除按钮启用状态
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
}


void AppointmentView::on_btAdd_clicked()
{
    emit goAppointmentEditView(-1); // 仅触发信号，由统一逻辑创建独立窗口
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


