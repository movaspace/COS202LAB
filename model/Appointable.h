#ifndef APPOINTABLE_H
#define APPOINTABLE_H

#include <string>

namespace scapms {

class Appointable {
public:
    virtual ~Appointable() = default;
    virtual bool scheduleAppointment(const std::string& appointmentId) = 0;
    virtual bool cancelAppointment(const std::string& appointmentId) = 0;
};

}

#endif
