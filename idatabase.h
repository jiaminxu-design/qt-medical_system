#ifndef IDATABASE_H
#define IDATABASE_H

#include <QObject>
#include<QtSql>
#include<QSqlDatabase>
#include<QDataWidgetMapper>

class IDatabase : public QObject
{
    Q_OBJECT
public:

    static IDatabase &getInstance()
    {
        static IDatabase instance;
        return instance;
    }

    QString userLogin(QString userName, QString password);

private:

    explicit IDatabase(QObject *parent = nullptr);
    IDatabase(IDatabase const &) = delete;
    void operator=(IDatabase const &)  = delete;

    QSqlDatabase database;

    void initDatabase();


signals:

public:
    // ---------- Patient表 ----------
    bool initPatientModel();
    int addNewPatient();
    bool searchPatient(QString filter);
    bool deleteCurrentPatient();
    bool submitPatientEdit();
    void revertPatientEdit();

    QSqlTableModel *patientTabModel;
    QItemSelectionModel *thePatientSelection;


    // ---------- Department表 ----------
    bool initDepartmentModel();
    int addNewDepartment();
    bool searchDepartment(QString filter);
    bool deleteCurrentDepartment();
    bool submitDepartmentEdit();
    void revertDepartmentEdit();
    QSqlTableModel *departmentTabModel;
    QItemSelectionModel *theDepartmentSelection;

    // ---------- Doctor表 ----------
    bool initDoctorModel();
    int addNewDoctor();
    bool searchDoctor(QString filter);
    bool deleteCurrentDoctor();
    bool submitDoctorEdit();
    void revertDoctorEdit();
    QSqlTableModel *doctorTabModel;
    QItemSelectionModel *theDoctorSelection;

    // ---------- Medicine表 ----------

    bool initMedicineModel();
    int addNewMedicine();
    bool searchMedicine(QString filter);
    bool deleteCurrentMedicine();
    bool submitMedicineEdit();
    void revertMedicineEdit();
    QSqlTableModel *medicineTabModel = nullptr;
    QItemSelectionModel *theMedicineSelection = nullptr;

};

#endif // IDATABASE_H
