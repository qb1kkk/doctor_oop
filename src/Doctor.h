#pragma once

#include "Patient.h"

#include <optional>
#include <string>
#include <vector>

class Doctor {
public:
    explicit Doctor(std::string fullName);

    void addPatient(const Patient& patient);
    std::optional<Patient> findPatientByLastName(const std::string& lastName) const;
    const std::string& getFullName() const;

private:
    std::string fullName_;
    std::vector<Patient> patients_;
};
