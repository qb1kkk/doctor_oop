#include "Database.h"

#include <unordered_map>

namespace {
constexpr const char* UNIQUE_VIOLATION = "23505";
}

Database::Database(std::string connectionInfo) {
    connection_ = PQconnectdb(connectionInfo.c_str());
    if (PQstatus(connection_) != CONNECTION_OK) {
        lastError_ = PQerrorMessage(connection_);
    }
}

Database::~Database() {
    if (connection_ != nullptr) {
        PQfinish(connection_);
        connection_ = nullptr;
    }
}

bool Database::isConnected() const {
    return connection_ != nullptr && PQstatus(connection_) == CONNECTION_OK;
}

const std::string& Database::getLastError() const {
    return lastError_;
}

bool Database::execute(const std::string& sql) {
    PGresult* result = PQexec(connection_, sql.c_str());
    if (result == nullptr) {
        lastError_ = "Database error: null result.";
        return false;
    }
    const bool ok = resultIsOk(result, false);
    if (!ok) {
        setResultError(result);
    }
    PQclear(result);
    return ok;
}

PGresult* Database::executeParams(const std::string& sql,
                                  const std::vector<std::string>& values,
                                  bool expectRows) {
    std::vector<const char*> rawValues;
    rawValues.reserve(values.size());
    for (const auto& value : values) {
        rawValues.push_back(value.c_str());
    }

    PGresult* result = PQexecParams(connection_,
                                    sql.c_str(),
                                    static_cast<int>(values.size()),
                                    nullptr,
                                    rawValues.data(),
                                    nullptr,
                                    nullptr,
                                    0);
    if (result == nullptr) {
        lastError_ = "Database error: null result.";
        return nullptr;
    }
    if (!resultIsOk(result, expectRows)) {
        setResultError(result);
        PQclear(result);
        return nullptr;
    }
    return result;
}

bool Database::resultIsOk(const PGresult* result, bool expectRows) const {
    if (expectRows) {
        return PQresultStatus(result) == PGRES_TUPLES_OK;
    }
    return PQresultStatus(result) == PGRES_COMMAND_OK || PQresultStatus(result) == PGRES_TUPLES_OK;
}

int Database::parseInt(PGresult* result, int row, int col) const {
    return std::stoi(PQgetvalue(result, row, col));
}

bool Database::parseBool(PGresult* result, int row, int col) const {
    const std::string value = PQgetvalue(result, row, col);
    return value == "t" || value == "true" || value == "1";
}

void Database::setResultError(PGresult* result) {
    const char* msg = PQresultErrorMessage(result);
    if (msg != nullptr && *msg != '\0') {
        lastError_ = msg;
        return;
    }
    lastError_ = "Unknown SQL error.";
}

bool Database::initSchema() {
    const char* createDoctors = R"(
        CREATE TABLE IF NOT EXISTS doctors (
            id SERIAL PRIMARY KEY,
            full_name TEXT NOT NULL,
            email TEXT NOT NULL UNIQUE,
            password_hash TEXT NOT NULL,
            created_at TIMESTAMP NOT NULL DEFAULT NOW()
        );
    )";

    const char* createPatients = R"(
        CREATE TABLE IF NOT EXISTS patients (
            id SERIAL PRIMARY KEY,
            first_name TEXT NOT NULL,
            last_name TEXT NOT NULL,
            email TEXT NOT NULL UNIQUE,
            password_hash TEXT NOT NULL,
            doctor_id INTEGER NOT NULL REFERENCES doctors(id) ON DELETE CASCADE,
            created_at TIMESTAMP NOT NULL DEFAULT NOW()
        );
    )";

    const char* createDiseases = R"(
        CREATE TABLE IF NOT EXISTS diseases (
            id SERIAL PRIMARY KEY,
            name TEXT NOT NULL UNIQUE,
            icd_code TEXT NOT NULL,
            chronic BOOLEAN NOT NULL DEFAULT FALSE,
            description TEXT NOT NULL,
            treatment TEXT NOT NULL
        );
    )";

    const char* createPatientDiseases = R"(
        CREATE TABLE IF NOT EXISTS patient_diseases (
            patient_id INTEGER NOT NULL REFERENCES patients(id) ON DELETE CASCADE,
            disease_id INTEGER NOT NULL REFERENCES diseases(id) ON DELETE CASCADE,
            PRIMARY KEY (patient_id, disease_id)
        );
    )";

    const char* indexPatientsDoctor = "CREATE INDEX IF NOT EXISTS idx_patients_doctor_id ON patients(doctor_id);";
    const char* indexPatientsSurname = "CREATE INDEX IF NOT EXISTS idx_patients_last_name ON patients(last_name);";

    return execute(createDoctors) &&
           execute(createPatients) &&
           execute(createDiseases) &&
           execute(createPatientDiseases) &&
           execute(indexPatientsDoctor) &&
           execute(indexPatientsSurname);
}

std::optional<int> Database::registerDoctor(const std::string& fullName,
                                            const std::string& email,
                                            const std::string& passwordHash,
                                            std::string& errorText) {
    const std::string sql = "INSERT INTO doctors(full_name, email, password_hash) VALUES($1, $2, $3) RETURNING id;";
    const char* values[3] = {fullName.c_str(), email.c_str(), passwordHash.c_str()};
    PGresult* result = PQexecParams(connection_,
                                    sql.c_str(),
                                    3,
                                    nullptr,
                                    values,
                                    nullptr,
                                    nullptr,
                                    0);

    if (result == nullptr) {
        errorText = "Ошибка подключения к БД.";
        return std::nullopt;
    }

    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        const char* sqlState = PQresultErrorField(result, PG_DIAG_SQLSTATE);
        if (sqlState != nullptr && std::string(sqlState) == UNIQUE_VIOLATION) {
            errorText = "Врач с таким email уже зарегистрирован.";
        } else {
            errorText = PQresultErrorMessage(result);
        }
        PQclear(result);
        return std::nullopt;
    }

    const int id = parseInt(result, 0, 0);
    PQclear(result);
    return id;
}

std::optional<int> Database::registerPatient(const std::string& firstName,
                                             const std::string& lastName,
                                             const std::string& email,
                                             const std::string& passwordHash,
                                             int doctorId,
                                             std::string& errorText) {
    const std::string sql =
        "INSERT INTO patients(first_name, last_name, email, password_hash, doctor_id) "
        "VALUES($1, $2, $3, $4, $5) RETURNING id;";
    const std::string doctorIdStr = std::to_string(doctorId);
    const char* values[5] = {
        firstName.c_str(),
        lastName.c_str(),
        email.c_str(),
        passwordHash.c_str(),
        doctorIdStr.c_str()
    };

    PGresult* result = PQexecParams(connection_,
                                    sql.c_str(),
                                    5,
                                    nullptr,
                                    values,
                                    nullptr,
                                    nullptr,
                                    0);

    if (result == nullptr) {
        errorText = "Ошибка подключения к БД.";
        return std::nullopt;
    }

    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        const char* sqlState = PQresultErrorField(result, PG_DIAG_SQLSTATE);
        if (sqlState != nullptr && std::string(sqlState) == UNIQUE_VIOLATION) {
            errorText = "Пациент с таким email уже зарегистрирован.";
        } else {
            errorText = PQresultErrorMessage(result);
        }
        PQclear(result);
        return std::nullopt;
    }

    const int id = parseInt(result, 0, 0);
    PQclear(result);
    return id;
}

std::optional<LoginRecord> Database::loginDoctor(const std::string& email, const std::string& passwordHash) {
    const std::string sql = "SELECT id, full_name FROM doctors WHERE email=$1 AND password_hash=$2;";
    PGresult* result = executeParams(sql, {email, passwordHash}, true);
    if (result == nullptr) {
        return std::nullopt;
    }

    if (PQntuples(result) == 0) {
        PQclear(result);
        return std::nullopt;
    }

    LoginRecord record;
    record.userId = parseInt(result, 0, 0);
    record.displayName = PQgetvalue(result, 0, 1);
    record.role = "doctor";
    PQclear(result);
    return record;
}

std::optional<LoginRecord> Database::loginPatient(const std::string& email, const std::string& passwordHash) {
    const std::string sql = "SELECT id, first_name, last_name FROM patients WHERE email=$1 AND password_hash=$2;";
    PGresult* result = executeParams(sql, {email, passwordHash}, true);
    if (result == nullptr) {
        return std::nullopt;
    }

    if (PQntuples(result) == 0) {
        PQclear(result);
        return std::nullopt;
    }

    LoginRecord record;
    record.userId = parseInt(result, 0, 0);
    record.displayName = std::string(PQgetvalue(result, 0, 1)) + " " + std::string(PQgetvalue(result, 0, 2));
    record.role = "patient";
    PQclear(result);
    return record;
}

std::vector<DoctorRecord> Database::getDoctors() {
    std::vector<DoctorRecord> doctors;
    const std::string sql = "SELECT id, full_name FROM doctors ORDER BY full_name;";
    PGresult* result = executeParams(sql, {}, true);
    if (result == nullptr) {
        return doctors;
    }

    const int rows = PQntuples(result);
    doctors.reserve(static_cast<size_t>(rows));
    for (int i = 0; i < rows; ++i) {
        DoctorRecord doctor;
        doctor.id = parseInt(result, i, 0);
        doctor.fullName = PQgetvalue(result, i, 1);
        doctors.push_back(doctor);
    }
    PQclear(result);
    return doctors;
}

std::vector<DiseaseRecord> Database::getDiseasesLibrary() {
    std::vector<DiseaseRecord> diseases;
    const std::string sql = "SELECT id, name, icd_code, chronic, description, treatment FROM diseases ORDER BY name;";
    PGresult* result = executeParams(sql, {}, true);
    if (result == nullptr) {
        return diseases;
    }

    const int rows = PQntuples(result);
    diseases.reserve(static_cast<size_t>(rows));
    for (int i = 0; i < rows; ++i) {
        DiseaseRecord disease;
        disease.id = parseInt(result, i, 0);
        disease.name = PQgetvalue(result, i, 1);
        disease.icdCode = PQgetvalue(result, i, 2);
        disease.chronic = parseBool(result, i, 3);
        disease.description = PQgetvalue(result, i, 4);
        disease.treatment = PQgetvalue(result, i, 5);
        diseases.push_back(disease);
    }
    PQclear(result);
    return diseases;
}

bool Database::doctorExists(int doctorId) {
    const std::string sql = "SELECT id FROM doctors WHERE id=$1;";
    PGresult* result = executeParams(sql, {std::to_string(doctorId)}, true);
    if (result == nullptr) {
        return false;
    }
    const bool exists = PQntuples(result) > 0;
    PQclear(result);
    return exists;
}

bool Database::doctorOwnsPatient(int doctorId, int patientId) {
    const std::string sql = "SELECT id FROM patients WHERE id=$1 AND doctor_id=$2;";
    PGresult* result = executeParams(sql, {std::to_string(patientId), std::to_string(doctorId)}, true);
    if (result == nullptr) {
        return false;
    }
    const bool exists = PQntuples(result) > 0;
    PQclear(result);
    return exists;
}

bool Database::patientHasDisease(int patientId, int diseaseId) {
    const std::string sql = "SELECT patient_id FROM patient_diseases WHERE patient_id=$1 AND disease_id=$2;";
    PGresult* result = executeParams(sql, {std::to_string(patientId), std::to_string(diseaseId)}, true);
    if (result == nullptr) {
        return false;
    }
    const bool exists = PQntuples(result) > 0;
    PQclear(result);
    return exists;
}

std::optional<int> Database::upsertDisease(const std::string& name,
                                           const std::string& icdCode,
                                           bool chronic,
                                           const std::string& description,
                                           const std::string& treatment,
                                           std::string& errorText) {
    const std::string sql = R"(
        INSERT INTO diseases(name, icd_code, chronic, description, treatment)
        VALUES($1, $2, $3, $4, $5)
        ON CONFLICT (name) DO UPDATE
           SET icd_code = EXCLUDED.icd_code,
               chronic = EXCLUDED.chronic,
               description = EXCLUDED.description,
               treatment = EXCLUDED.treatment
        RETURNING id;
    )";

    const std::string chronicStr = chronic ? "true" : "false";
    const char* values[5] = {
        name.c_str(),
        icdCode.c_str(),
        chronicStr.c_str(),
        description.c_str(),
        treatment.c_str()
    };

    PGresult* result = PQexecParams(connection_, sql.c_str(), 5, nullptr, values, nullptr, nullptr, 0);
    if (result == nullptr) {
        errorText = "Ошибка подключения к БД.";
        return std::nullopt;
    }

    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        const char* sqlState = PQresultErrorField(result, PG_DIAG_SQLSTATE);
        if (sqlState != nullptr && std::string(sqlState) == UNIQUE_VIOLATION) {
            errorText = "Болезнь с таким именем уже существует с другими параметрами.";
        } else {
            errorText = PQresultErrorMessage(result);
        }
        PQclear(result);
        return std::nullopt;
    }

    const int diseaseId = parseInt(result, 0, 0);
    PQclear(result);
    return diseaseId;
}

bool Database::assignDiseaseToPatient(int patientId, int diseaseId, std::string& errorText) {
    const std::string sql =
        "INSERT INTO patient_diseases(patient_id, disease_id) VALUES($1, $2) ON CONFLICT DO NOTHING;";
    const std::string patientIdStr = std::to_string(patientId);
    const std::string diseaseIdStr = std::to_string(diseaseId);
    const char* values[2] = {patientIdStr.c_str(), diseaseIdStr.c_str()};

    PGresult* result = PQexecParams(connection_, sql.c_str(), 2, nullptr, values, nullptr, nullptr, 0);
    if (result == nullptr) {
        errorText = "Ошибка подключения к БД.";
        return false;
    }

    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        errorText = PQresultErrorMessage(result);
        PQclear(result);
        return false;
    }

    PQclear(result);
    return true;
}

bool Database::updateDisease(int diseaseId,
                             const std::string& name,
                             const std::string& icdCode,
                             bool chronic,
                             const std::string& description,
                             const std::string& treatment,
                             std::string& errorText) {
    const std::string sql =
        "UPDATE diseases SET name=$1, icd_code=$2, chronic=$3, description=$4, treatment=$5 WHERE id=$6;";

    const std::string chronicStr = chronic ? "true" : "false";
    const std::string diseaseIdStr = std::to_string(diseaseId);
    const char* values[6] = {
        name.c_str(),
        icdCode.c_str(),
        chronicStr.c_str(),
        description.c_str(),
        treatment.c_str(),
        diseaseIdStr.c_str()
    };

    PGresult* result = PQexecParams(connection_, sql.c_str(), 6, nullptr, values, nullptr, nullptr, 0);
    if (result == nullptr) {
        errorText = "Ошибка подключения к БД.";
        return false;
    }

    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        const char* sqlState = PQresultErrorField(result, PG_DIAG_SQLSTATE);
        if (sqlState != nullptr && std::string(sqlState) == UNIQUE_VIOLATION) {
            errorText = "Болезнь с таким названием уже существует.";
        } else {
            errorText = PQresultErrorMessage(result);
        }
        PQclear(result);
        return false;
    }

    if (std::string(PQcmdTuples(result)) == "0") {
        errorText = "Болезнь не найдена.";
        PQclear(result);
        return false;
    }

    PQclear(result);
    return true;
}

std::vector<PatientRecord> Database::getDoctorPatients(int doctorId) {
    std::vector<PatientRecord> patients;
    const std::string sql = R"(
        SELECT p.id,
               p.first_name,
               p.last_name,
               p.email,
               dr.full_name,
               d.id,
               d.name,
               d.icd_code,
               d.chronic,
               d.description,
               d.treatment
          FROM patients p
          JOIN doctors dr ON dr.id = p.doctor_id
     LEFT JOIN patient_diseases pd ON pd.patient_id = p.id
     LEFT JOIN diseases d ON d.id = pd.disease_id
         WHERE p.doctor_id = $1
      ORDER BY p.last_name, p.first_name, d.name;
    )";

    PGresult* result = executeParams(sql, {std::to_string(doctorId)}, true);
    if (result == nullptr) {
        return patients;
    }

    std::unordered_map<int, size_t> patientIndex;
    const int rows = PQntuples(result);
    for (int i = 0; i < rows; ++i) {
        const int patientId = parseInt(result, i, 0);
        size_t index = 0;

        const auto it = patientIndex.find(patientId);
        if (it == patientIndex.end()) {
            PatientRecord patient;
            patient.id = patientId;
            patient.firstName = PQgetvalue(result, i, 1);
            patient.lastName = PQgetvalue(result, i, 2);
            patient.email = PQgetvalue(result, i, 3);
            patient.doctorName = PQgetvalue(result, i, 4);
            patients.push_back(patient);
            index = patients.size() - 1;
            patientIndex.emplace(patientId, index);
        } else {
            index = it->second;
        }

        if (!PQgetisnull(result, i, 5)) {
            DiseaseRecord disease;
            disease.id = parseInt(result, i, 5);
            disease.name = PQgetvalue(result, i, 6);
            disease.icdCode = PQgetvalue(result, i, 7);
            disease.chronic = parseBool(result, i, 8);
            disease.description = PQgetvalue(result, i, 9);
            disease.treatment = PQgetvalue(result, i, 10);
            patients[index].diseases.push_back(disease);
        }
    }

    PQclear(result);
    return patients;
}

std::optional<PatientRecord> Database::getPatientById(int patientId) {
    const std::string sql = R"(
        SELECT p.id,
               p.first_name,
               p.last_name,
               p.email,
               dr.full_name,
               d.id,
               d.name,
               d.icd_code,
               d.chronic,
               d.description,
               d.treatment
          FROM patients p
          JOIN doctors dr ON dr.id = p.doctor_id
     LEFT JOIN patient_diseases pd ON pd.patient_id = p.id
     LEFT JOIN diseases d ON d.id = pd.disease_id
         WHERE p.id = $1
      ORDER BY d.name;
    )";

    PGresult* result = executeParams(sql, {std::to_string(patientId)}, true);
    if (result == nullptr) {
        return std::nullopt;
    }

    if (PQntuples(result) == 0) {
        PQclear(result);
        return std::nullopt;
    }

    PatientRecord patient;
    patient.id = parseInt(result, 0, 0);
    patient.firstName = PQgetvalue(result, 0, 1);
    patient.lastName = PQgetvalue(result, 0, 2);
    patient.email = PQgetvalue(result, 0, 3);
    patient.doctorName = PQgetvalue(result, 0, 4);

    const int rows = PQntuples(result);
    for (int i = 0; i < rows; ++i) {
        if (PQgetisnull(result, i, 5)) {
            continue;
        }
        DiseaseRecord disease;
        disease.id = parseInt(result, i, 5);
        disease.name = PQgetvalue(result, i, 6);
        disease.icdCode = PQgetvalue(result, i, 7);
        disease.chronic = parseBool(result, i, 8);
        disease.description = PQgetvalue(result, i, 9);
        disease.treatment = PQgetvalue(result, i, 10);
        patient.diseases.push_back(disease);
    }

    PQclear(result);
    return patient;
}

std::optional<PatientRecord> Database::getPatientBySurname(const std::string& surname) {
    const std::string sql = R"(
        WITH target AS (
            SELECT id
              FROM patients
             WHERE LOWER(last_name) = LOWER($1)
             ORDER BY id
             LIMIT 1
        )
        SELECT p.id,
               p.first_name,
               p.last_name,
               p.email,
               dr.full_name,
               d.id,
               d.name,
               d.icd_code,
               d.chronic,
               d.description,
               d.treatment
          FROM target t
          JOIN patients p ON p.id = t.id
          JOIN doctors dr ON dr.id = p.doctor_id
     LEFT JOIN patient_diseases pd ON pd.patient_id = p.id
     LEFT JOIN diseases d ON d.id = pd.disease_id
      ORDER BY d.name;
    )";

    PGresult* result = executeParams(sql, {surname}, true);
    if (result == nullptr) {
        return std::nullopt;
    }

    if (PQntuples(result) == 0) {
        PQclear(result);
        return std::nullopt;
    }

    PatientRecord patient;
    patient.id = parseInt(result, 0, 0);
    patient.firstName = PQgetvalue(result, 0, 1);
    patient.lastName = PQgetvalue(result, 0, 2);
    patient.email = PQgetvalue(result, 0, 3);
    patient.doctorName = PQgetvalue(result, 0, 4);

    const int rows = PQntuples(result);
    for (int i = 0; i < rows; ++i) {
        if (PQgetisnull(result, i, 5)) {
            continue;
        }
        DiseaseRecord disease;
        disease.id = parseInt(result, i, 5);
        disease.name = PQgetvalue(result, i, 6);
        disease.icdCode = PQgetvalue(result, i, 7);
        disease.chronic = parseBool(result, i, 8);
        disease.description = PQgetvalue(result, i, 9);
        disease.treatment = PQgetvalue(result, i, 10);
        patient.diseases.push_back(disease);
    }

    PQclear(result);
    return patient;
}

bool Database::seedDemoData(const std::string& doctorPasswordHash, const std::string& patientPasswordHash) {
    std::string errorText;
    std::optional<int> doctorId = registerDoctor("Д-р Анна Ковалева", "doctor@clinic.local", doctorPasswordHash, errorText);
    if (!doctorId.has_value() && errorText.find("уже зарегистрирован") == std::string::npos) {
        lastError_ = errorText;
        return false;
    }

    PGresult* doctorResult = executeParams("SELECT id FROM doctors WHERE email='doctor@clinic.local';", {}, true);
    if (doctorResult == nullptr || PQntuples(doctorResult) == 0) {
        if (doctorResult != nullptr) {
            PQclear(doctorResult);
        }
        lastError_ = "Не удалось получить demo-врача.";
        return false;
    }
    const int demoDoctorId = parseInt(doctorResult, 0, 0);
    PQclear(doctorResult);

    registerPatient("Иван", "Иванов", "ivanov@clinic.local", patientPasswordHash, demoDoctorId, errorText);
    registerPatient("Мария", "Петрова", "petrova@clinic.local", patientPasswordHash, demoDoctorId, errorText);
    registerPatient("Алексей", "Sidorov", "sidorov@clinic.local", patientPasswordHash, demoDoctorId, errorText);

    const std::string diseaseUpsertSql = R"(
        INSERT INTO diseases(name, icd_code, chronic, description, treatment)
        VALUES ($1, $2, $3, $4, $5)
        ON CONFLICT (name) DO UPDATE
           SET icd_code = EXCLUDED.icd_code,
               chronic = EXCLUDED.chronic,
               description = EXCLUDED.description,
               treatment = EXCLUDED.treatment;
    )";

    auto upsertDisease = [&](const std::vector<std::string>& values) -> bool {
        std::vector<const char*> raw{
            values[0].c_str(), values[1].c_str(), values[2].c_str(), values[3].c_str(), values[4].c_str()
        };
        PGresult* result = PQexecParams(connection_, diseaseUpsertSql.c_str(), 5, nullptr, raw.data(), nullptr, nullptr, 0);
        if (result == nullptr) {
            lastError_ = "Ошибка вставки болезни.";
            return false;
        }
        const bool ok = resultIsOk(result, false);
        if (!ok) {
            setResultError(result);
        }
        PQclear(result);
        return ok;
    };

    if (!upsertDisease({"Гипертония",
                        "I10",
                        "true",
                        "Стойкое повышение артериального давления, увеличивающее риск сердечно-сосудистых осложнений.",
                        "Контроль давления;Снижение соли и веса;Антигипертензивная терапия по назначению врача"})) {
        return false;
    }

    if (!upsertDisease({"Гастрит",
                        "K29",
                        "false",
                        "Воспаление слизистой оболочки желудка с болевым и диспепсическим синдромом.",
                        "Щадящая диета;ИПП и гастропротекторы;Контроль Helicobacter pylori"})) {
        return false;
    }

    if (!upsertDisease({"Бронхиальная астма",
                        "J45",
                        "true",
                        "Хроническое воспалительное заболевание дыхательных путей с эпизодами бронхообструкции.",
                        "Ингаляционные ГКС;Бронхолитик для купирования;Избегание триггеров"})) {
        return false;
    }

    if (!upsertDisease({"ОРВИ",
                        "J06",
                        "false",
                        "Острое респираторное вирусное заболевание верхних дыхательных путей.",
                        "Покой и питьевой режим;Симптоматическое лечение;Контроль динамики 3-5 дней"})) {
        return false;
    }

    if (!upsertDisease({"Сахарный диабет 2 типа",
                        "E11",
                        "true",
                        "Метаболическое заболевание, характеризующееся хронической гипергликемией.",
                        "Диета и снижение веса;Контроль глюкозы/HbA1c;Сахароснижающая терапия"})) {
        return false;
    }

    const char* linkSql = R"(
        INSERT INTO patient_diseases(patient_id, disease_id)
        SELECT p.id, d.id
          FROM patients p
          JOIN diseases d ON d.name = $2
         WHERE p.email = $1
        ON CONFLICT DO NOTHING;
    )";

    auto linkDisease = [&](const std::string& patientEmail, const std::string& diseaseName) -> bool {
        const char* values[2] = {patientEmail.c_str(), diseaseName.c_str()};
        PGresult* result = PQexecParams(connection_, linkSql, 2, nullptr, values, nullptr, nullptr, 0);
        if (result == nullptr) {
            lastError_ = "Ошибка связывания болезни с пациентом.";
            return false;
        }
        const bool ok = resultIsOk(result, false);
        if (!ok) {
            setResultError(result);
        }
        PQclear(result);
        return ok;
    };

    return linkDisease("ivanov@clinic.local", "Гипертония") &&
           linkDisease("ivanov@clinic.local", "Гастрит") &&
           linkDisease("petrova@clinic.local", "Бронхиальная астма") &&
           linkDisease("petrova@clinic.local", "ОРВИ") &&
           linkDisease("sidorov@clinic.local", "Сахарный диабет 2 типа");
}
