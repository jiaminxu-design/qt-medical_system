#ifndef MASTERVIEW_H
#define MASTERVIEW_H

#include <QWidget>
#include <loginview.h>
#include<doctorview.h>
#include<patienteditview.h>
#include<patientview.h>
#include<welcomeview.h>
#include<recordview.h>
#include<medicineview.h>
#include<appointmentview.h>
#include<doctoreditview.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MasterView;
}
QT_END_NAMESPACE

class MasterView : public QWidget
{
    Q_OBJECT

public:
    MasterView(QWidget *parent = nullptr);
    ~MasterView();

public slots:
    void goLoginView();
    void goWelcomeView();
    void goDoctorView();
    void goPatientEditView(int rowNo);
    void goPatientView();
    void goPreviousView();
    void goRecordView();
    void goMedicineView();
    void goAppointmentView();



    void goDoctorEditView(int rowNo); // 跳转到医生编辑界面

private slots:
    void on_btBack_clicked();

    void on_stackedWidget_currentChanged(int arg1);

    void on_btLogout_clicked();

private:
    void pushWidgetToStackView(QWidget *widget);

    Ui::MasterView *ui;
    WelcomeView *welcomeView;
    DoctorView *doctorView;
    PatientView *patientView;
    LoginView *loginView;
    PatientEditView *patientEditView;
    RecordView *recordView;
    MedicineView *medicineView;
    AppointmentView *appointmentView;
    DoctorEditView *doctorEditView;

};
#endif // MASTERVIEW_H
