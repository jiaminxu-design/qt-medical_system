#include "medicineeditview.h"
#include "ui_medicineeditview.h"
#include <QMessageBox>
#include <QDebug>

MedicineEditView::MedicineEditView(QWidget *parent, int /*editRow*/) : // 忽略传入的editRow
    QWidget(parent),
    ui(new Ui::MedicineEditView),
    dataMapper(new QDataWidgetMapper(this))
{
    ui->setupUi(this);

    // 1. 从选择模型获取当前选中的行
    QModelIndex currentIndex = db.theMedicineSelection->currentIndex();

    // 如果没有选中行，则默认选择最后一行（用于新增）
    if (!currentIndex.isValid()) {
        currentIndex = db.medicineTabModel->index(db.medicineTabModel->rowCount() - 1, 0);
        db.theMedicineSelection->select(currentIndex,
                                        QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }

    currentRow = currentIndex.row();

    // 2. 判断是否为新增模式
    //当前行是最后一行 并且 没有被提交过（即新行）
    isNewMedicine = (currentRow == db.medicineTabModel->rowCount() - 1) &&
                    (db.medicineTabModel->data(db.medicineTabModel->index(currentRow,
                                                                          db.medicineTabModel->fieldIndex("NAME"))).toString().isEmpty());

    // 3. 初始化映射器
    initMapper();
    loadMedicineData();

    // 4. 绑定信号
    connect(ui->medSpinStock,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this,
            &MedicineEditView::on_medSpinStock_valueChanged);

    // 5. UI优化
    if (isNewMedicine) {
        this->setWindowTitle("新增药品信息");
        ui->medEditName->setFocus();
        ui->medEditID->setPlaceholderText("保存后自动生成");
    } else {
        this->setWindowTitle("编辑药品信息");
    }
}


MedicineEditView::~MedicineEditView()
{
    delete dataMapper;
    delete ui;
}

// 初始化数据映射器（绑定控件与数据库字段）
void MedicineEditView::initMapper()
{
    // 1. 校验模型是否有效
    if (!db.medicineTabModel) {
        QMessageBox::critical(this, "错误", "药品模型未初始化！");
        return;
    }

    dataMapper->setModel(db.medicineTabModel);
    dataMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    // 2. 字段名校验（打印所有字段，确认字段名）
    qDebug() << "药品表字段：";
    for (int i = 0; i < db.medicineTabModel->record().count(); i++) {
        qDebug() << db.medicineTabModel->record().fieldName(i);
    }

    // 3. 绑定字段（只绑定存在的字段）
    int idCol = db.medicineTabModel->fieldIndex("ID");
    int nameCol = db.medicineTabModel->fieldIndex("NAME");
    int categoryCol = db.medicineTabModel->fieldIndex("CATEGORY");
    int unitCol = db.medicineTabModel->fieldIndex("UNIT");
    int priceCol = db.medicineTabModel->fieldIndex("PRICE");
    int stockCol = db.medicineTabModel->fieldIndex("STOCK");
    int statusCol = db.medicineTabModel->fieldIndex("STOCK_STATUS");
    int expiryCol = db.medicineTabModel->fieldIndex("EXPIRY_DATE");

    // 4. 安全绑定（只绑定存在的列）
    if (idCol != -1) dataMapper->addMapping(ui->medEditID, idCol);
    if (nameCol != -1) dataMapper->addMapping(ui->medEditName, nameCol);
    if (categoryCol != -1) dataMapper->addMapping(ui->medComboCategory, categoryCol);
    if (unitCol != -1) dataMapper->addMapping(ui->medEditUnit, unitCol);
    if (priceCol != -1) dataMapper->addMapping(ui->medSpinPrice, priceCol);
    if (stockCol != -1) dataMapper->addMapping(ui->medSpinStock, stockCol);
    if (statusCol != -1) dataMapper->addMapping(ui->medComboStatus, statusCol);
    if (expiryCol != -1) dataMapper->addMapping(ui->medDateExpiry, expiryCol);

    // 5. 安全定位行
    if (currentRow >= 0 && currentRow < db.medicineTabModel->rowCount()) {
        dataMapper->setCurrentIndex(currentRow);
    }
}


// 加载药品数据到控件
void MedicineEditView::loadMedicineData()
{
    if (isNewMedicine) {
        // 仅初始化默认值
        ui->medSpinStock->setValue(0);
        ui->medComboStatus->setCurrentText("缺货");
        ui->medDateExpiry->setDate(QDate::currentDate().addYears(1));
        ui->medSpinPrice->setValue(0.00);
        ui->medEditUnit->setText("盒"); // 默认单位
        ui->medEditID->clear(); // 清空ID（由系统生成）
        return;
    }

    // 加载原有数据
    if (currentRow >= 0 && currentRow < db.medicineTabModel->rowCount()) {
        // 使用 dataMapper.revert() 恢复原始数据
        dataMapper->revert();

        // 手动处理有效期（因为 dateEdit 不支持自动映射）
        QString expiryStr = db.medicineTabModel->data(
                                db.medicineTabModel->index(currentRow, db.medicineTabModel->fieldIndex("EXPIRY_DATE"))
                            ).toString();
        if (!expiryStr.isEmpty()) {
            QDate expiryDate = QDate::fromString(expiryStr, "yyyy-MM-dd");
            if (expiryDate.isValid()) {
                ui->medDateExpiry->setDate(expiryDate);
            }
        }

        // 库存状态根据库存自动更新
        int stock = ui->medSpinStock->value();
        ui->medComboStatus->setCurrentText(stock > 0 ? "正常库存" : "缺货");
    }
}

// 库存变化时自动更新状态
void MedicineEditView::on_medSpinStock_valueChanged(int arg1)
{
    ui->medComboStatus->setCurrentText(arg1 > 0 ? "正常库存" : "缺货");
}

// 保存按钮点击事件
void MedicineEditView::on_btnSave_clicked()
{
    // 1. 通用校验
    if (ui->medEditName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "提示", "药品名称不能为空！");
        return;
    }

    if (isNewMedicine) {
        // 生成 ID 并设置
        QString newId = QUuid::createUuid().toString(QUuid::WithoutBraces);
        int idCol = db.medicineTabModel->fieldIndex("ID");
        if (idCol != -1) {
            db.medicineTabModel->setData(db.medicineTabModel->index(currentRow, idCol), newId);
        }
    }

    // 2. 提交数据（已通过选择模型确保currentRow有效）
    if (!dataMapper->submit()) {
        QSqlError error = db.medicineTabModel->lastError();
        QMessageBox::warning(this, "错误", "提交数据失败：" + error.text());
        return;
    }

    // 3. 处理有效期
    int expiryCol = db.medicineTabModel->fieldIndex("EXPIRY_DATE");
    if (expiryCol != -1) {
        QString expiryStr = ui->medDateExpiry->date().toString("yyyy-MM-dd");
        db.medicineTabModel->setData(db.medicineTabModel->index(currentRow, expiryCol), expiryStr);
    }

    // 4. 提交到数据库
    bool ret = db.submitMedicineEdit();
    if (ret) {
        QString tip = isNewMedicine ? "新增药品成功！" : "编辑药品成功！";
        QMessageBox::information(this, "成功", tip);
        emit goPreviousView();
    } else {
        QSqlError error = db.medicineTabModel->lastError();
        QMessageBox::warning(this, "保存失败",
                             QString("错误：%1\n代码：%2").arg(error.text()).arg(error.number()));
    }
}


// 取消按钮点击事件
void MedicineEditView::on_btnCancel_clicked()
{
    // 撤销未提交的修改
    db.revertMedicineEdit();
    emit goPreviousView(); // 触发返回信号
}
