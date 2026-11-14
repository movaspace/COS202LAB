#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

namespace scapms {

class ClinicException : public std::runtime_error {
public:
    explicit ClinicException(const std::string& msg) : std::runtime_error(msg) {}
};

class ValidationException : public ClinicException {
public:
    explicit ValidationException(const std::string& msg) : ClinicException(msg) {}
};

class DuplicateIdException : public ClinicException {
public:
    explicit DuplicateIdException(const std::string& msg) : ClinicException(msg) {}
};

class NotFoundException : public ClinicException {
public:
    explicit NotFoundException(const std::string& msg) : ClinicException(msg) {}
};

class FileIOException : public ClinicException {
public:
    explicit FileIOException(const std::string& msg) : ClinicException(msg) {}
};

}

#endif
