#include "ClinicHttpServer.h"
#include "Database.h"
#include "Doctor.h"

#include <cstdlib>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace {
Doctor buildDemoDoctor() {
    Doctor doctor("Д-р Анна Ковалева");

    Patient ivanov("Иван", "Иванов");
    ivanov.addDisease(Disease("Гипертония", "I10", true));
    ivanov.addDisease(Disease("Гастрит", "K29", false));

    Patient petrova("Мария", "Петрова");
    petrova.addDisease(Disease("Бронхиальная астма", "J45", true));
    petrova.addDisease(Disease("ОРВИ", "J06", false));

    Patient sidorov("Алексей", "Sidorov");
    sidorov.addDisease(Disease("Сахарный диабет 2 типа", "E11", true));

    doctor.addPatient(ivanov);
    doctor.addPatient(petrova);
    doctor.addPatient(sidorov);
    return doctor;
}

void printPatientDiseases(const Doctor& doctor, const std::string& surname) {
    const auto patient = doctor.findPatientByLastName(surname);
    if (!patient.has_value()) {
        std::cout << "Пациент с фамилией \"" << surname << "\" не найден.\n";
        return;
    }

    std::cout << "Пациент: " << patient->getFirstName() << " " << patient->getLastName() << "\n";
    std::cout << "Список болезней:\n";
    if (patient->getDiseases().empty()) {
        std::cout << "  - Болезни отсутствуют.\n";
        return;
    }

    int index = 1;
    for (const auto& disease : patient->getDiseases()) {
        std::cout << "  " << index++ << ". " << disease.getName()
                  << " (МКБ-10: " << disease.getIcdCode()
                  << ", хроническая: " << (disease.isChronic() ? "да" : "нет") << ")\n";
    }
}

int parsePort(const std::string& arg, int defaultValue) {
    const std::string prefix = "--port=";
    if (arg.rfind(prefix, 0) == 0) {
        try {
            return std::stoi(arg.substr(prefix.size()));
        } catch (...) {
            return defaultValue;
        }
    }
    return defaultValue;
}

std::string hashPassword(const std::string& password) {
    const uint64_t hashValue = std::hash<std::string>{}(password);
    std::ostringstream out;
    out << std::hex << hashValue;
    return out.str();
}
} // namespace

int main(int argc, char* argv[]) {
    const Doctor doctor = buildDemoDoctor();

    bool consoleMode = true;
    bool serverMode = true;
    int port = 8080;

    for (int i = 1; i < argc; ++i) {
        const std::string arg(argv[i]);
        if (arg == "--console-only") {
            serverMode = false;
        } else if (arg == "--server-only") {
            consoleMode = false;
        } else if (arg.rfind("--port=", 0) == 0) {
            port = parsePort(arg, 8080);
        }
    }

    if (consoleMode) {
        std::cout << "Введите фамилию пациента: ";
        std::string surname;
        std::getline(std::cin, surname);
        printPatientDiseases(doctor, surname);
    }

    if (serverMode) {
        const char* envConnection = std::getenv("DATABASE_URL");
        const std::string connectionInfo = envConnection != nullptr
                                               ? std::string(envConnection)
                                               : "host=localhost port=5432 dbname=doctor_oop user=doctor_app password=doctor_app";

        Database database(connectionInfo);
        if (!database.isConnected()) {
            std::cerr << "Ошибка подключения к PostgreSQL: " << database.getLastError() << "\n";
            return 1;
        }

        if (!database.initSchema()) {
            std::cerr << "Ошибка инициализации схемы БД: " << database.getLastError() << "\n";
            return 1;
        }

        if (!database.seedDemoData(hashPassword("doctor123"), hashPassword("patient123"))) {
            std::cerr << "Ошибка наполнения demo-данных: " << database.getLastError() << "\n";
            return 1;
        }

        ClinicHttpServer server(database, port);
        if (!server.run()) {
            return 1;
        }
    }

    return 0;
}
