#include "medicineview.h"
#include "ui_medicineview.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>

MedicineView::MedicineView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MedicineView),
    db(IDatabase::getInstance())
{
    ui->setupUi(this);

    if (!db.medicineTabModel) {
        if (!db.initMedicineModel()) {
            QMessageBox::critical(this, "错误", "药品数据库模型初始化失败！");
            delete ui;
            return;
        }
    }

    // 绑定模型
    ui->tableView->setModel(db.medicineTabModel);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setSelectionModel(db.theMedicineSelection);

    // 连接信号槽
    connect(ui->btSearch, &QPushButton::clicked, this, &MedicineView::on_btSearch_clicked);
    connect(ui->btAdd, &QPushButton::clicked, this, &MedicineView::on_btAdd_clicked);
    connect(ui->btEdit, &QPushButton::clicked, this, &MedicineView::on_btEdit_clicked);
    connect(ui->btDelete, &QPushButton::clicked, this, &MedicineView::on_btDelete_clicked);
    connect(ui->btStockIn, &QPushButton::clicked, this, &MedicineView::on_btStockIn_clicked);
    connect(ui->btStockOut, &QPushButton::clicked, this, &MedicineView::on_btStockOut_clicked);

    connect(ui->btSyncRemote, &QPushButton::clicked, this, &MedicineView::on_btSyncRemote_clicked);
}

MedicineView::~MedicineView()
{
    delete ui;
}

// ======== 按钮槽函数========
void MedicineView::on_btSearch_clicked()
{
    QString name = ui->txtName->text().trimmed();
    QString category = ui->comboCategory->currentText();
    QString status = ui->comboStockStatus->currentText();

    QStringList filters;

    if (!name.isEmpty())
        filters << QString("NAME LIKE '%%1%'").arg(name);

    if (category != "全部")
        filters << QString("CATEGORY = '%1'").arg(category);

    if (status != "全部")
        filters << QString("STOCK_STATUS = '%1'").arg(status);

    QString finalFilter = filters.join(" AND ");
    db.searchMedicine(finalFilter);
}


void MedicineView::on_btAdd_clicked()
{
    // 1. 刷新模型
    db.medicineTabModel->select();

    // 2. 创建新行
    int newRow = db.addNewMedicine();
    if (newRow < 0) {
        QMessageBox::warning(this, "错误", "创建新药品失败！");
        return;
    }

    // 3. 选中新行
    QModelIndex newIndex = db.medicineTabModel->index(newRow, 0);
    db.theMedicineSelection->select(newIndex, QItemSelectionModel::Select | QItemSelectionModel::Rows);

    // 4. 发送信号跳转
    emit goMedicineEditView(newIndex.row());
}


void MedicineView::on_btEdit_clicked()
{
    // 检查是否选中行
    QModelIndex curIndex = db.theMedicineSelection->currentIndex();
    if (!curIndex.isValid()) {
        QMessageBox::warning(this, "提示", "请先选中要编辑的药品！");
        return;
    }

    // 触发跳转到MedicineEditView（通过信号通知MasterView）
    emit goMedicineEditView(curIndex.row());
}


void MedicineView::on_btDelete_clicked()
{
    if (!db.theMedicineSelection->hasSelection()) {
        QMessageBox::warning(this, "警告", "请先选择要删除的药品！");
        return;
    }

    if (QMessageBox::question(this, "确认", "确定删除该药品？") == QMessageBox::Yes) {
        if (db.deleteCurrentMedicine()) {
            QMessageBox::information(this, "成功", "删除成功！");
        } else {
            QMessageBox::warning(this, "失败", "删除失败！");
        }
    }
}

void MedicineView::on_btStockIn_clicked()
{
    if (!db.theMedicineSelection->hasSelection()) {
        QMessageBox::warning(this, "警告", "请先选择药品！");
        return;
    }

    bool ok;
    int amount = QInputDialog::getInt(this, "入库", "数量:", 1, 1, 99999, 1, &ok);
    if (!ok) return;

    QModelIndex index = db.theMedicineSelection->currentIndex();
    int stockCol = db.medicineTabModel->fieldIndex("STOCK");
    int statusCol = db.medicineTabModel->fieldIndex("STOCK_STATUS");

    int current = db.medicineTabModel->data(index.sibling(index.row(), stockCol)).toInt();
    int newStock = current + amount;

    db.medicineTabModel->setData(index.sibling(index.row(), stockCol), newStock);
    db.medicineTabModel->setData(index.sibling(index.row(), statusCol),
                                 newStock > 0 ? "正常库存" : "缺货");

    if (db.submitMedicineEdit()) {
        QMessageBox::information(this, "成功", "入库成功！");
    } else {
        QMessageBox::warning(this, "失败", "入库失败！");
    }
}

void MedicineView::on_btStockOut_clicked()
{
    if (!db.theMedicineSelection->hasSelection()) {
        QMessageBox::warning(this, "警告", "请先选择药品！");
        return;
    }

    bool ok;
    int amount = QInputDialog::getInt(this, "出库", "数量:", 1, 1, 99999, 1, &ok);
    if (!ok) return;

    QModelIndex index = db.theMedicineSelection->currentIndex();
    int stockCol = db.medicineTabModel->fieldIndex("STOCK");
    int current = db.medicineTabModel->data(index.sibling(index.row(), stockCol)).toInt();

    if (current < amount) {
        QMessageBox::warning(this, "错误", "库存不足！");
        return;
    }

    int newStock = current - amount;
    int statusCol = db.medicineTabModel->fieldIndex("STOCK_STATUS");

    db.medicineTabModel->setData(index.sibling(index.row(), stockCol), newStock);
    db.medicineTabModel->setData(index.sibling(index.row(), statusCol),
                                 newStock > 0 ? "正常库存" : "缺货");

    if (db.submitMedicineEdit()) {
        QMessageBox::information(this, "成功", "出库成功！");
    } else {
        QMessageBox::warning(this, "失败", "出库失败！");
    }
}




void MedicineView::on_btSyncRemote_clicked()
{
    // 触发远程同步
    bool ret = db.syncMedicineFromRemote();
    // 提示同步结果
    QMessageBox::information(this, "同步结果",
                             ret ? "药品参考信息同步成功！" : "同步失败！");
    // 刷新表格显示同步后的数据
    db.medicineTabModel->select();
    ui->tableView->viewport()->update();
}
