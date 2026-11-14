#include "Appointment.h"

namespace scapms {

Appointment::Appointment(const std::string& apptId, const std::string& patId, const std::string& date, const std::string& time, const std::string& reason, const std::string& status)
    : appointmentId(apptId), patientId(patId), date(date), time(time), reason(reason), status(status) {}

std::string Appointment::getAppointmentId() const {
    return appointmentId;
}

std::string Appointment::getPatientId() const {
    return patientId;
}

std::string Appointment::getDate() const {
    return date;
}

std::string Appointment::getTime() const {
    return time;
}

std::string Appointment::getReason() const {
    return reason;
}

std::string Appointment::getStatus() const {
    return status;
}

void Appointment::setDate(const std::string& newDate) {
    date = newDate;
}

void Appointment::setTime(const std::string& newTime) {
    time = newTime;
}

void Appointment::setReason(const std::string& newReason) {
    reason = newReason;
}

void Appointment::setStatus(const std::string& newStatus) {
    status = newStatus;
}

}
