#include "idatabase.h"
#include<QUuid>

void IDatabase::initDatabase()
{
    database = QSqlDatabase::addDatabase("QSQLITE");
    QString aFile = "E:/qt_code/lab4.db";
    database.setDatabaseName(aFile);

    if (!database.open()) {
        qDebug() << "failed to open database.";
    } else
        qDebug() << "open database is ok";
}

bool IDatabase::initPatientModel()
{
    patientTabModel = new QSqlTableModel(this, database);
    patientTabModel->setTable("patient");
    patientTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    patientTabModel->setSort(patientTabModel->fieldIndex("name"), Qt::AscendingOrder);

    if (!(patientTabModel->select()))
        return false;

    thePatientSelection = new QItemSelectionModel(patientTabModel);
    return true;
}

int IDatabase::addNewPatient()
{
    patientTabModel->insertRow(patientTabModel->rowCount(), QModelIndex());
    QModelIndex curIndex = patientTabModel->index(patientTabModel->rowCount() - 1, 1);

    int curRecNo = curIndex.row();
    QSqlRecord curRec = patientTabModel->record(curRecNo);
    curRec.setValue("CREATEDTIMESTAMP", QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    curRec.setValue("ID", QUuid::createUuid().toString(QUuid::WithoutBraces));

    patientTabModel->setRecord(curRecNo, curRec);

    patientTabModel->setData(patientTabModel->index(curRecNo,
                                                    patientTabModel->fieldIndex("CREATEDTIMESTAMP")),

                             QDateTime::currentDateTime().toString("yyyy-MM-dd"));

    return curIndex.row();
}

bool IDatabase::searchPatient(QString filter)
{
    patientTabModel->setFilter((filter));
    return patientTabModel->select();
}

bool IDatabase::deleteCurrentPatient()
{
    QModelIndex curIndex = thePatientSelection->currentIndex();
    patientTabModel->removeRow(curIndex.row());
    patientTabModel->submitAll();
    patientTabModel->select();
}

bool IDatabase::submitPatientEdit()
{
    return patientTabModel->submitAll();
}

void IDatabase::revertPatientEdit()
{
    return patientTabModel->revertAll();
}



QString IDatabase::userLogin(QString userName, QString password)
{
    // return "loginOK";
    QSqlQuery query;
    query.prepare("select username,password from user where userName=:USER");
    query.bindValue(":USER", userName);
    query.exec();
    if (query.first() && query.value("username").isValid()) {
        QString passwd = query.value("password").toString();
        if (passwd == password) {
            qDebug() << "login Ok";
            return "loginOk";
        } else {
            qDebug() << "wrong Password";
            return "wrongPassword";
        }
    } else {
        qDebug() << "no such user";
        return "wrongUsername";
    }

}

IDatabase::IDatabase(QObject *parent) : QObject(parent)
{
    initDatabase();
}





// ========== Department表实现（仅用于读取科室数据，供下拉框选择） ==========
bool IDatabase::initDepartmentModel()
{
    departmentTabModel = new QSqlTableModel(this, database);
    departmentTabModel->setTable("Department"); // 关联Navicat中的Department表
    departmentTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    departmentTabModel->setSort(departmentTabModel->fieldIndex("NAME"), Qt::AscendingOrder);

    if (!departmentTabModel->select()) {
        qDebug() << "科室模型初始化失败：" << departmentTabModel->lastError().text();
        return false;
    }
    theDepartmentSelection = new QItemSelectionModel(departmentTabModel);
    return true;
}

int IDatabase::addNewDepartment()
{
    return 0;    // 无需新增科室（Navicat手动加）
}
bool IDatabase::searchDepartment(QString filter)
{
    return true;    // 无需搜索科室
}
bool IDatabase::deleteCurrentDepartment()
{
    return false;    // 无需删除科室
}
bool IDatabase::submitDepartmentEdit()
{
    return false;    // 无需编辑科室
}
void IDatabase::revertDepartmentEdit() {} // 无需撤销科室修改

// ========== Doctor表实现（核心：医生的增删改查） ==========
bool IDatabase::initDoctorModel()
{
    doctorTabModel = new QSqlTableModel(this, database);
    doctorTabModel->setTable("Doctor"); // 关联Navicat中的Doctor表
    doctorTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    doctorTabModel->setSort(doctorTabModel->fieldIndex("NAME"), Qt::AscendingOrder);

    if (!doctorTabModel->select()) {
        qDebug() << "医生模型初始化失败：" << doctorTabModel->lastError().text();
        return false;
    }
    theDoctorSelection = new QItemSelectionModel(doctorTabModel);
    return true;
}

int IDatabase::addNewDoctor()
{
    // 插入空行
    doctorTabModel->insertRow(doctorTabModel->rowCount());
    int curRecNo = doctorTabModel->rowCount() - 1;

    // 初始化医生ID和工号（EMPLOYEENO）
    QSqlRecord curRec = doctorTabModel->record(curRecNo);
    curRec.setValue("ID", QUuid::createUuid().toString(QUuid::WithoutBraces)); // 唯一ID
    curRec.setValue("EMPLOYEENO", QString("DOC%1").arg(
                        QDateTime::currentDateTime().toTime_t())); // 工号
    doctorTabModel->setRecord(curRecNo, curRec);

    return curRecNo;
}

bool IDatabase::searchDoctor(QString filter)
{
    doctorTabModel->setFilter(filter);
    bool ret = doctorTabModel->select();
    if (!ret) qDebug() << "医生搜索失败：" << doctorTabModel->lastError().text();
    return ret;
}

bool IDatabase::deleteCurrentDoctor()
{
    QModelIndex curIndex = theDoctorSelection->currentIndex();
    if (!curIndex.isValid()) return false;

    doctorTabModel->removeRow(curIndex.row());
    bool ret = doctorTabModel->submitAll();
    doctorTabModel->select(); // 刷新表格
    return ret;
}

bool IDatabase::submitDoctorEdit()
{
    bool ret = doctorTabModel->submitAll();
    if (!ret) qDebug() << "医生信息保存失败：" << doctorTabModel->lastError().text();
    return ret;
}

void IDatabase::revertDoctorEdit()
{
    doctorTabModel->revertAll();
}

