#ifndef GENERALPATIENT_H
#define GENERALPATIENT_H

#include "PatientRecord.h"
#include "Appointable.h"

namespace scapms {

class GeneralPatient : public PatientRecord, public Appointable {
private:
    std::string department;

public:
    GeneralPatient(const std::string& id, const std::string& name, int age, const std::string& contact, const std::string& dept);

    std::string getDepartment() const;
    void setDepartment(const std::string& dept);

    std::string getPatientType() const override;
    std::string getDetails() const override;
    double calculateConsultationFee() const override;

    bool scheduleAppointment(const std::string& apptId) override;
    bool cancelAppointment(const std::string& apptId) override;
};

}

#endif
