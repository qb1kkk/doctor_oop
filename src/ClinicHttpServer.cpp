#include "ClinicHttpServer.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <map>
#include <limits>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <functional>

namespace {
struct HttpRequest {
    std::string method;
    std::string target;
    std::string path;
    std::string query;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> queryParams;
    std::map<std::string, std::string> formParams;
    std::string body;
};

struct Session {
    std::string role;
    int userId = 0;
};

std::string trim(const std::string& value) {
    size_t left = 0;
    size_t right = value.size();
    while (left < right && std::isspace(static_cast<unsigned char>(value[left])) != 0) {
        ++left;
    }
    while (right > left && std::isspace(static_cast<unsigned char>(value[right - 1])) != 0) {
        --right;
    }
    return value.substr(left, right - left);
}

std::string toLower(std::string value) {
    for (char& ch : value) {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return value;
}

std::string decodeUrl(std::string value) {
    std::string decoded;
    decoded.reserve(value.size());
    for (size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '%' && i + 2 < value.size()) {
            const std::string hex = value.substr(i + 1, 2);
            char decodedChar = static_cast<char>(std::strtol(hex.c_str(), nullptr, 16));
            decoded.push_back(decodedChar);
            i += 2;
        } else if (value[i] == '+') {
            decoded.push_back(' ');
        } else {
            decoded.push_back(value[i]);
        }
    }
    return decoded;
}

std::map<std::string, std::string> parseQuery(const std::string& query) {
    std::map<std::string, std::string> params;
    std::stringstream queryStream(query);
    std::string pair;

    while (std::getline(queryStream, pair, '&')) {
        const size_t eqPos = pair.find('=');
        if (eqPos == std::string::npos) {
            continue;
        }
        const std::string key = decodeUrl(pair.substr(0, eqPos));
        const std::string value = decodeUrl(pair.substr(eqPos + 1));
        params[key] = value;
    }

    return params;
}

bool startsWith(const std::string& value, const std::string& prefix) {
    return value.rfind(prefix, 0) == 0;
}

std::string escapeJson(const std::string& input) {
    std::string output;
    output.reserve(input.size());
    for (char ch : input) {
        switch (ch) {
            case '"':
                output += "\\\"";
                break;
            case '\\':
                output += "\\\\";
                break;
            case '\n':
                output += "\\n";
                break;
            case '\r':
                output += "\\r";
                break;
            case '\t':
                output += "\\t";
                break;
            default:
                output += ch;
                break;
        }
    }
    return output;
}

std::string jsonError(const std::string& text) {
    return std::string("{\"error\":\"") + escapeJson(text) + "\"}";
}

std::string hashPassword(const std::string& password) {
    const uint64_t hashValue = std::hash<std::string>{}(password);
    std::ostringstream out;
    out << std::hex << hashValue;
    return out.str();
}

void sendResponse(int clientSocket, const std::string& status, const std::string& body, const std::string& contentType) {
    std::ostringstream response;
    response << "HTTP/1.1 " << status << "\r\n"
             << "Content-Type: " << contentType << "\r\n"
             << "Access-Control-Allow-Origin: *\r\n"
             << "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
             << "Access-Control-Allow-Headers: Content-Type, Authorization\r\n"
             << "Connection: close\r\n"
             << "Content-Length: " << body.size() << "\r\n\r\n"
             << body;
    const std::string responseText = response.str();
    send(clientSocket, responseText.c_str(), responseText.size(), 0);
}

bool parseRequestLine(const std::string& line, HttpRequest& request) {
    std::istringstream lineStream(line);
    std::string version;
    lineStream >> request.method >> request.target >> version;
    if (request.method.empty() || request.target.empty() || version.empty()) {
        return false;
    }

    const size_t qPos = request.target.find('?');
    if (qPos == std::string::npos) {
        request.path = request.target;
    } else {
        request.path = request.target.substr(0, qPos);
        request.query = request.target.substr(qPos + 1);
        request.queryParams = parseQuery(request.query);
    }
    return true;
}

std::optional<HttpRequest> readHttpRequest(int clientSocket) {
    constexpr size_t HEADER_LIMIT = 128 * 1024;
    std::string raw;
    raw.reserve(8192);

    char buffer[4096];
    size_t headerEndPos = std::string::npos;

    while (true) {
        headerEndPos = raw.find("\r\n\r\n");
        if (headerEndPos != std::string::npos) {
            break;
        }
        if (raw.size() > HEADER_LIMIT) {
            return std::nullopt;
        }
        const ssize_t bytes = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytes <= 0) {
            return std::nullopt;
        }
        raw.append(buffer, static_cast<size_t>(bytes));
    }

    HttpRequest request;
    std::istringstream headerStream(raw.substr(0, headerEndPos));
    std::string line;

    if (!std::getline(headerStream, line)) {
        return std::nullopt;
    }
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    if (!parseRequestLine(line, request)) {
        return std::nullopt;
    }

    while (std::getline(headerStream, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) {
            continue;
        }
        const size_t sep = line.find(':');
        if (sep == std::string::npos) {
            continue;
        }
        const std::string key = toLower(trim(line.substr(0, sep)));
        const std::string value = trim(line.substr(sep + 1));
        request.headers[key] = value;
    }

    size_t contentLength = 0;
    const auto contentLengthIt = request.headers.find("content-length");
    if (contentLengthIt != request.headers.end()) {
        try {
            contentLength = static_cast<size_t>(std::stoul(contentLengthIt->second));
        } catch (...) {
            contentLength = 0;
        }
    }

    const size_t bodyStart = headerEndPos + 4;
    while (raw.size() - bodyStart < contentLength) {
        const ssize_t bytes = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytes <= 0) {
            return std::nullopt;
        }
        raw.append(buffer, static_cast<size_t>(bytes));
    }

    if (contentLength > 0) {
        request.body = raw.substr(bodyStart, contentLength);
    }

    const auto contentTypeIt = request.headers.find("content-type");
    if (!request.body.empty() && contentTypeIt != request.headers.end() &&
        startsWith(toLower(contentTypeIt->second), "application/x-www-form-urlencoded")) {
        request.formParams = parseQuery(request.body);
    }

    return request;
}

std::string diseaseJson(const DiseaseRecord& disease) {
    std::ostringstream out;
    out << "{"
        << "\"id\":" << disease.id << ","
        << "\"name\":\"" << escapeJson(disease.name) << "\","
        << "\"icdCode\":\"" << escapeJson(disease.icdCode) << "\","
        << "\"chronic\":" << (disease.chronic ? "true" : "false") << ","
        << "\"description\":\"" << escapeJson(disease.description) << "\","
        << "\"treatment\":\"" << escapeJson(disease.treatment) << "\""
        << "}";
    return out.str();
}

std::string patientJson(const PatientRecord& patient) {
    std::ostringstream out;
    out << "{"
        << "\"id\":" << patient.id << ","
        << "\"firstName\":\"" << escapeJson(patient.firstName) << "\","
        << "\"lastName\":\"" << escapeJson(patient.lastName) << "\","
        << "\"email\":\"" << escapeJson(patient.email) << "\","
        << "\"doctor\":\"" << escapeJson(patient.doctorName) << "\","
        << "\"diseases\":[";

    for (size_t i = 0; i < patient.diseases.size(); ++i) {
        if (i > 0) {
            out << ",";
        }
        out << diseaseJson(patient.diseases[i]);
    }
    out << "]}";
    return out.str();
}

std::optional<int> parseIntValue(const std::string& value) {
    try {
        return std::stoi(value);
    } catch (...) {
        return std::nullopt;
    }
}

std::optional<bool> parseBoolValue(const std::string& value) {
    const std::string lowered = toLower(trim(value));
    if (lowered == "1" || lowered == "true" || lowered == "yes" || lowered == "on") {
        return true;
    }
    if (lowered == "0" || lowered == "false" || lowered == "no" || lowered == "off") {
        return false;
    }
    return std::nullopt;
}

std::optional<std::string> parseBearerToken(const HttpRequest& request) {
    const auto it = request.headers.find("authorization");
    if (it == request.headers.end()) {
        return std::nullopt;
    }
    const std::string prefix = "Bearer ";
    if (!startsWith(it->second, prefix)) {
        return std::nullopt;
    }
    const std::string token = trim(it->second.substr(prefix.size()));
    if (token.empty()) {
        return std::nullopt;
    }
    return token;
}
} // namespace

class RouteHandler {
public:
    explicit RouteHandler(Database& database) : database_(database), rng_(std::random_device{}()) {
    }

    std::pair<std::string, std::string> handle(const HttpRequest& request) {
        if (request.method == "OPTIONS") {
            return {"204 No Content", ""};
        }

        if (request.method == "GET" && request.path == "/api/health") {
            return {"200 OK", R"({"status":"ok"})"};
        }

        if (request.method == "GET" && request.path == "/api/doctors") {
            return doctorsList();
        }

        if (request.method == "POST" && request.path == "/api/register/doctor") {
            return registerDoctor(request);
        }

        if (request.method == "POST" && request.path == "/api/register/patient") {
            return registerPatient(request);
        }

        if (request.method == "POST" && request.path == "/api/login") {
            return login(request);
        }

        if (request.method == "GET" && request.path == "/api/doctor/patients") {
            return doctorPatients(request);
        }

        if (request.method == "GET" && request.path == "/api/doctor/diseases/library") {
            return doctorDiseasesLibrary(request);
        }

        if (request.method == "POST" && request.path == "/api/doctor/patient/add-disease") {
            return doctorAddDisease(request);
        }

        if (request.method == "POST" && request.path == "/api/doctor/patient/update-disease") {
            return doctorUpdateDisease(request);
        }

        if (request.method == "GET" && request.path == "/api/patient/diseases") {
            return patientDiseases(request);
        }

        if (request.method == "GET" && request.path == "/api/diseases") {
            return diseasesBySurname(request);
        }

        return {"404 Not Found", jsonError("Маршрут не найден")};
    }

private:
    std::pair<std::string, std::string> doctorsList() {
        const auto doctors = database_.getDoctors();
        std::ostringstream json;
        json << "{\"doctors\":[";
        for (size_t i = 0; i < doctors.size(); ++i) {
            if (i > 0) {
                json << ",";
            }
            json << "{"
                 << "\"id\":" << doctors[i].id << ","
                 << "\"fullName\":\"" << escapeJson(doctors[i].fullName) << "\""
                 << "}";
        }
        json << "]}";
        return {"200 OK", json.str()};
    }

    std::pair<std::string, std::string> registerDoctor(const HttpRequest& request) {
        const auto fullNameIt = request.formParams.find("fullName");
        const auto emailIt = request.formParams.find("email");
        const auto passwordIt = request.formParams.find("password");
        if (fullNameIt == request.formParams.end() || emailIt == request.formParams.end() || passwordIt == request.formParams.end() ||
            fullNameIt->second.empty() || emailIt->second.empty() || passwordIt->second.empty()) {
            return {"400 Bad Request", jsonError("Поля fullName, email, password обязательны")};
        }

        std::string errorText;
        const auto id = database_.registerDoctor(fullNameIt->second, emailIt->second, hashPassword(passwordIt->second), errorText);
        if (!id.has_value()) {
            return {"400 Bad Request", jsonError(errorText.empty() ? "Не удалось зарегистрировать врача" : errorText)};
        }

        std::ostringstream json;
        json << "{\"status\":\"ok\",\"doctorId\":" << *id << "}";
        return {"201 Created", json.str()};
    }

    std::pair<std::string, std::string> registerPatient(const HttpRequest& request) {
        const auto firstNameIt = request.formParams.find("firstName");
        const auto lastNameIt = request.formParams.find("lastName");
        const auto emailIt = request.formParams.find("email");
        const auto passwordIt = request.formParams.find("password");
        const auto doctorIdIt = request.formParams.find("doctorId");

        if (firstNameIt == request.formParams.end() || lastNameIt == request.formParams.end() ||
            emailIt == request.formParams.end() || passwordIt == request.formParams.end() || doctorIdIt == request.formParams.end() ||
            firstNameIt->second.empty() || lastNameIt->second.empty() || emailIt->second.empty() ||
            passwordIt->second.empty() || doctorIdIt->second.empty()) {
            return {"400 Bad Request", jsonError("Поля firstName, lastName, email, password, doctorId обязательны")};
        }

        const auto doctorId = parseIntValue(doctorIdIt->second);
        if (!doctorId.has_value() || !database_.doctorExists(*doctorId)) {
            return {"400 Bad Request", jsonError("Некорректный doctorId")};
        }

        std::string errorText;
        const auto id = database_.registerPatient(firstNameIt->second,
                                                  lastNameIt->second,
                                                  emailIt->second,
                                                  hashPassword(passwordIt->second),
                                                  *doctorId,
                                                  errorText);
        if (!id.has_value()) {
            return {"400 Bad Request", jsonError(errorText.empty() ? "Не удалось зарегистрировать пациента" : errorText)};
        }

        std::ostringstream json;
        json << "{\"status\":\"ok\",\"patientId\":" << *id << "}";
        return {"201 Created", json.str()};
    }

    std::pair<std::string, std::string> login(const HttpRequest& request) {
        const auto emailIt = request.formParams.find("email");
        const auto passwordIt = request.formParams.find("password");
        const auto roleIt = request.formParams.find("role");
        if (emailIt == request.formParams.end() || passwordIt == request.formParams.end() || roleIt == request.formParams.end() ||
            emailIt->second.empty() || passwordIt->second.empty() || roleIt->second.empty()) {
            return {"400 Bad Request", jsonError("Поля email, password, role обязательны")};
        }

        const std::string role = toLower(roleIt->second);
        std::optional<LoginRecord> loginRecord;
        if (role == "doctor") {
            loginRecord = database_.loginDoctor(emailIt->second, hashPassword(passwordIt->second));
        } else if (role == "patient") {
            loginRecord = database_.loginPatient(emailIt->second, hashPassword(passwordIt->second));
        } else {
            return {"400 Bad Request", jsonError("role должен быть doctor или patient")};
        }

        if (!loginRecord.has_value()) {
            return {"401 Unauthorized", jsonError("Неверный email или пароль")};
        }

        const std::string token = generateToken();
        sessions_[token] = Session{loginRecord->role, loginRecord->userId};

        std::ostringstream json;
        json << "{"
             << "\"status\":\"ok\","
             << "\"token\":\"" << escapeJson(token) << "\","
             << "\"role\":\"" << escapeJson(loginRecord->role) << "\","
             << "\"userId\":" << loginRecord->userId << ","
             << "\"displayName\":\"" << escapeJson(loginRecord->displayName) << "\""
             << "}";
        return {"200 OK", json.str()};
    }

    std::optional<Session> authorized(const HttpRequest& request, const std::string& expectedRole) {
        const auto token = parseBearerToken(request);
        if (!token.has_value()) {
            return std::nullopt;
        }
        const auto it = sessions_.find(*token);
        if (it == sessions_.end()) {
            return std::nullopt;
        }
        if (!expectedRole.empty() && it->second.role != expectedRole) {
            return std::nullopt;
        }
        return it->second;
    }

    std::pair<std::string, std::string> doctorPatients(const HttpRequest& request) {
        const auto session = authorized(request, "doctor");
        if (!session.has_value()) {
            return {"401 Unauthorized", jsonError("Требуется авторизация врача")};
        }

        const auto patients = database_.getDoctorPatients(session->userId);
        std::ostringstream json;
        json << "{\"patients\":[";
        for (size_t i = 0; i < patients.size(); ++i) {
            if (i > 0) {
                json << ",";
            }
            json << patientJson(patients[i]);
        }
        json << "]}";
        return {"200 OK", json.str()};
    }

    std::pair<std::string, std::string> doctorDiseasesLibrary(const HttpRequest& request) {
        const auto session = authorized(request, "doctor");
        if (!session.has_value()) {
            return {"401 Unauthorized", jsonError("Требуется авторизация врача")};
        }

        const auto diseases = database_.getDiseasesLibrary();
        std::ostringstream json;
        json << "{\"diseases\":[";
        for (size_t i = 0; i < diseases.size(); ++i) {
            if (i > 0) {
                json << ",";
            }
            json << diseaseJson(diseases[i]);
        }
        json << "]}";
        return {"200 OK", json.str()};
    }

    std::pair<std::string, std::string> doctorAddDisease(const HttpRequest& request) {
        const auto session = authorized(request, "doctor");
        if (!session.has_value()) {
            return {"401 Unauthorized", jsonError("Требуется авторизация врача")};
        }

        const auto patientIdIt = request.formParams.find("patientId");
        if (patientIdIt == request.formParams.end() || patientIdIt->second.empty()) {
            return {"400 Bad Request", jsonError("Поле patientId обязательно")};
        }

        const auto patientId = parseIntValue(patientIdIt->second);
        if (!patientId.has_value() || !database_.doctorOwnsPatient(session->userId, *patientId)) {
            return {"403 Forbidden", jsonError("Пациент не относится к вашему профилю")};
        }

        int diseaseId = 0;
        const auto diseaseIdIt = request.formParams.find("diseaseId");
        if (diseaseIdIt != request.formParams.end() && !diseaseIdIt->second.empty()) {
            const auto parsedDiseaseId = parseIntValue(diseaseIdIt->second);
            if (!parsedDiseaseId.has_value()) {
                return {"400 Bad Request", jsonError("Некорректный diseaseId")};
            }
            diseaseId = *parsedDiseaseId;
        } else {
            const auto nameIt = request.formParams.find("name");
            const auto icdIt = request.formParams.find("icdCode");
            const auto chronicIt = request.formParams.find("chronic");
            const auto descriptionIt = request.formParams.find("description");
            const auto treatmentIt = request.formParams.find("treatment");

            if (nameIt == request.formParams.end() || icdIt == request.formParams.end() || chronicIt == request.formParams.end() ||
                descriptionIt == request.formParams.end() || treatmentIt == request.formParams.end() || nameIt->second.empty() ||
                icdIt->second.empty() || descriptionIt->second.empty() || treatmentIt->second.empty()) {
                return {"400 Bad Request", jsonError("Поля name, icdCode, chronic, description, treatment обязательны для новой болезни")};
            }

            const auto chronicValue = parseBoolValue(chronicIt->second);
            if (!chronicValue.has_value()) {
                return {"400 Bad Request", jsonError("chronic должен быть true/false")};
            }

            std::string errorText;
            const auto newDiseaseId = database_.upsertDisease(nameIt->second,
                                                              icdIt->second,
                                                              *chronicValue,
                                                              descriptionIt->second,
                                                              treatmentIt->second,
                                                              errorText);
            if (!newDiseaseId.has_value()) {
                return {"400 Bad Request", jsonError(errorText.empty() ? "Не удалось сохранить болезнь" : errorText)};
            }
            diseaseId = *newDiseaseId;
        }

        std::string errorText;
        if (!database_.assignDiseaseToPatient(*patientId, diseaseId, errorText)) {
            return {"400 Bad Request", jsonError(errorText.empty() ? "Не удалось назначить болезнь пациенту" : errorText)};
        }

        return {"200 OK", R"({"status":"ok"})"};
    }

    std::pair<std::string, std::string> doctorUpdateDisease(const HttpRequest& request) {
        const auto session = authorized(request, "doctor");
        if (!session.has_value()) {
            return {"401 Unauthorized", jsonError("Требуется авторизация врача")};
        }

        const auto patientIdIt = request.formParams.find("patientId");
        const auto diseaseIdIt = request.formParams.find("diseaseId");
        const auto nameIt = request.formParams.find("name");
        const auto icdIt = request.formParams.find("icdCode");
        const auto chronicIt = request.formParams.find("chronic");
        const auto descriptionIt = request.formParams.find("description");
        const auto treatmentIt = request.formParams.find("treatment");

        if (patientIdIt == request.formParams.end() || diseaseIdIt == request.formParams.end() || nameIt == request.formParams.end() ||
            icdIt == request.formParams.end() || chronicIt == request.formParams.end() || descriptionIt == request.formParams.end() ||
            treatmentIt == request.formParams.end() || patientIdIt->second.empty() || diseaseIdIt->second.empty() ||
            nameIt->second.empty() || icdIt->second.empty() || descriptionIt->second.empty() || treatmentIt->second.empty()) {
            return {"400 Bad Request", jsonError("Поля patientId, diseaseId, name, icdCode, chronic, description, treatment обязательны")};
        }

        const auto patientId = parseIntValue(patientIdIt->second);
        const auto diseaseId = parseIntValue(diseaseIdIt->second);
        const auto chronicValue = parseBoolValue(chronicIt->second);

        if (!patientId.has_value() || !diseaseId.has_value()) {
            return {"400 Bad Request", jsonError("Некорректные patientId или diseaseId")};
        }
        if (!chronicValue.has_value()) {
            return {"400 Bad Request", jsonError("chronic должен быть true/false")};
        }

        if (!database_.doctorOwnsPatient(session->userId, *patientId)) {
            return {"403 Forbidden", jsonError("Пациент не относится к вашему профилю")};
        }
        if (!database_.patientHasDisease(*patientId, *diseaseId)) {
            return {"404 Not Found", jsonError("У пациента нет указанной болезни")};
        }

        std::string errorText;
        if (!database_.updateDisease(*diseaseId,
                                     nameIt->second,
                                     icdIt->second,
                                     *chronicValue,
                                     descriptionIt->second,
                                     treatmentIt->second,
                                     errorText)) {
            return {"400 Bad Request", jsonError(errorText.empty() ? "Не удалось обновить болезнь" : errorText)};
        }

        return {"200 OK", R"({"status":"ok"})"};
    }

    std::pair<std::string, std::string> patientDiseases(const HttpRequest& request) {
        const auto session = authorized(request, "patient");
        if (!session.has_value()) {
            return {"401 Unauthorized", jsonError("Требуется авторизация пациента")};
        }

        const auto patient = database_.getPatientById(session->userId);
        if (!patient.has_value()) {
            return {"404 Not Found", jsonError("Пациент не найден")};
        }

        std::ostringstream json;
        json << "{"
             << "\"patient\":" << patientJson(*patient)
             << "}";
        return {"200 OK", json.str()};
    }

    std::pair<std::string, std::string> diseasesBySurname(const HttpRequest& request) {
        const auto surnameIt = request.queryParams.find("surname");
        if (surnameIt == request.queryParams.end() || surnameIt->second.empty()) {
            return {"400 Bad Request", jsonError("Параметр surname обязателен")};
        }

        const auto patient = database_.getPatientBySurname(surnameIt->second);
        if (!patient.has_value()) {
            return {"404 Not Found", jsonError("Пациент не найден")};
        }

        std::ostringstream json;
        json << "{"
             << "\"doctor\":\"" << escapeJson(patient->doctorName) << "\","
             << "\"patient\":{"
             << "\"firstName\":\"" << escapeJson(patient->firstName) << "\","
             << "\"lastName\":\"" << escapeJson(patient->lastName) << "\""
             << "},"
             << "\"diseases\":[";

        for (size_t i = 0; i < patient->diseases.size(); ++i) {
            if (i > 0) {
                json << ",";
            }
            json << diseaseJson(patient->diseases[i]);
        }

        json << "]}";
        return {"200 OK", json.str()};
    }

    std::string generateToken() {
        std::uniform_int_distribution<uint64_t> dist(0, std::numeric_limits<uint64_t>::max());
        std::ostringstream token;
        for (int i = 0; i < 4; ++i) {
            token << std::hex << std::setw(16) << std::setfill('0') << dist(rng_);
        }
        return token.str();
    }

    Database& database_;
    std::unordered_map<std::string, Session> sessions_;
    std::mt19937_64 rng_;
};

ClinicHttpServer::ClinicHttpServer(Database& database, int port) : database_(database), port_(port) {
}

bool ClinicHttpServer::run() {
    const int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Ошибка: не удалось создать сокет.\n";
        return false;
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port_);

    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) {
        std::cerr << "Ошибка: порт " << port_ << " занят или недоступен.\n";
        close(serverSocket);
        return false;
    }

    if (listen(serverSocket, 20) < 0) {
        std::cerr << "Ошибка: не удалось запустить прослушивание порта.\n";
        close(serverSocket);
        return false;
    }

    RouteHandler routes(database_);

    std::cout << "HTTP API запущен: http://localhost:" << port_ << "\n";
    std::cout << "Маршруты: /api/login, /api/register/*, /api/doctor/patients, /api/doctor/patient/add-disease,\n";
    std::cout << "          /api/doctor/patient/update-disease, /api/doctor/diseases/library, /api/patient/diseases\n";
    std::cout << "Нажмите Ctrl+C для остановки сервера.\n";

    while (true) {
        sockaddr_in clientAddress{};
        socklen_t clientLen = sizeof(clientAddress);
        const int clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddress), &clientLen);
        if (clientSocket < 0) {
            continue;
        }

        const auto request = readHttpRequest(clientSocket);
        if (!request.has_value()) {
            sendResponse(clientSocket, "400 Bad Request", jsonError("Некорректный HTTP запрос"), "application/json; charset=UTF-8");
            close(clientSocket);
            continue;
        }

        if (request->method != "GET" && request->method != "POST" && request->method != "OPTIONS") {
            sendResponse(clientSocket,
                         "405 Method Not Allowed",
                         jsonError("Разрешены только GET, POST и OPTIONS"),
                         "application/json; charset=UTF-8");
            close(clientSocket);
            continue;
        }

        const auto [status, body] = routes.handle(*request);
        sendResponse(clientSocket, status, body, "application/json; charset=UTF-8");
        close(clientSocket);
    }

    close(serverSocket);
    return true;
}
