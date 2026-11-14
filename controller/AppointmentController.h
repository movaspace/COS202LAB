#ifndef APPOINTMENTCONTROLLER_H
#define APPOINTMENTCONTROLLER_H

#include "../model/ClinicDatabase.h"
#include <string>
#include <vector>

namespace scapms {

struct DailySummary {
    int total;
    int completed;
    int pending;
    int cancelled;
};

class AppointmentController {
private:
    ClinicDatabase* database;

public:
    AppointmentController();
    explicit AppointmentController(ClinicDatabase* db);
    void setDatabase(ClinicDatabase* db);

    bool scheduleAppointment(const std::string& patientId, const std::string& date, const std::string& time, const std::string& reason, std::string& outApptId, std::string& errorMsg);
    bool cancelAppointment(const std::string& apptId, std::string& errorMsg);
    bool completeAppointment(const std::string& apptId, std::string& errorMsg);

    DailySummary getDailySummary(const std::string& date = "") const;
    std::vector<std::string> getPendingPatientsReport() const;
    std::vector<std::pair<std::string, int>> getMostFrequentlyAttendedPatients() const;
};

}

#endif
