#pragma once

#if __has_include(<libpq-fe.h>)
#include <libpq-fe.h>
#elif __has_include(<postgresql/libpq-fe.h>)
#include <postgresql/libpq-fe.h>
#else
#error "PostgreSQL client headers not found. Install libpq-dev."
#endif

#include <optional>
#include <string>
#include <vector>

struct DiseaseRecord {
    int id = 0;
    std::string name;
    std::string icdCode;
    bool chronic = false;
    std::string description;
    std::string treatment;
};

struct PatientRecord {
    int id = 0;
    std::string firstName;
    std::string lastName;
    std::string email;
    std::string doctorName;
    std::vector<DiseaseRecord> diseases;
};

struct DoctorRecord {
    int id = 0;
    std::string fullName;
};

struct LoginRecord {
    int userId = 0;
    std::string displayName;
    std::string role;
};

class Database {
public:
    explicit Database(std::string connectionInfo);
    ~Database();

    bool isConnected() const;
    const std::string& getLastError() const;

    bool initSchema();
    bool seedDemoData(const std::string& doctorPasswordHash, const std::string& patientPasswordHash);

    std::vector<DoctorRecord> getDoctors();
    bool doctorExists(int doctorId);

    std::optional<int> registerDoctor(const std::string& fullName,
                                      const std::string& email,
                                      const std::string& passwordHash,
                                      std::string& errorText);

    std::optional<int> registerPatient(const std::string& firstName,
                                       const std::string& lastName,
                                       const std::string& email,
                                       const std::string& passwordHash,
                                       int doctorId,
                                       std::string& errorText);

    std::optional<LoginRecord> loginDoctor(const std::string& email, const std::string& passwordHash);
    std::optional<LoginRecord> loginPatient(const std::string& email, const std::string& passwordHash);

    std::vector<PatientRecord> getDoctorPatients(int doctorId);
    std::optional<PatientRecord> getPatientById(int patientId);
    std::optional<PatientRecord> getPatientBySurname(const std::string& surname);
    std::vector<DiseaseRecord> getDiseasesLibrary();

    bool doctorOwnsPatient(int doctorId, int patientId);
    bool patientHasDisease(int patientId, int diseaseId);

    std::optional<int> upsertDisease(const std::string& name,
                                     const std::string& icdCode,
                                     bool chronic,
                                     const std::string& description,
                                     const std::string& treatment,
                                     std::string& errorText);

    bool assignDiseaseToPatient(int patientId, int diseaseId, std::string& errorText);
    bool updateDisease(int diseaseId,
                       const std::string& name,
                       const std::string& icdCode,
                       bool chronic,
                       const std::string& description,
                       const std::string& treatment,
                       std::string& errorText);

private:
    bool execute(const std::string& sql);
    PGresult* executeParams(const std::string& sql,
                            const std::vector<std::string>& values,
                            bool expectRows);
    bool resultIsOk(const PGresult* result, bool expectRows) const;
    int parseInt(PGresult* result, int row, int col) const;
    bool parseBool(PGresult* result, int row, int col) const;
    void setResultError(PGresult* result);

    PGconn* connection_ = nullptr;
    std::string lastError_;
};
