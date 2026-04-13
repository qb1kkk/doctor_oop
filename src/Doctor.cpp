#include "Doctor.h"

#include <cctype>
#include <utility>

namespace {
bool isAscii(const std::string& value) {
    for (unsigned char ch : value) {
        if (ch > 127) {
            return false;
        }
    }
    return true;
}

std::string toLowerAscii(std::string value) {
    for (char& ch : value) {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return value;
}
} // namespace

Doctor::Doctor(std::string fullName) : fullName_(std::move(fullName)) {
}

void Doctor::addPatient(const Patient& patient) {
    patients_.push_back(patient);
}

std::optional<Patient> Doctor::findPatientByLastName(const std::string& lastName) const {
    const bool asciiQuery = isAscii(lastName);
    const std::string queryLower = asciiQuery ? toLowerAscii(lastName) : "";
    for (const auto& patient : patients_) {
        if (patient.getLastName() == lastName) {
            return patient;
        }
        if (asciiQuery && isAscii(patient.getLastName()) && toLowerAscii(patient.getLastName()) == queryLower) {
            return patient;
        }
    }
    return std::nullopt;
}

const std::string& Doctor::getFullName() const {
    return fullName_;
}
