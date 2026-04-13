#pragma once

#include "Database.h"

#include <string>

class ClinicHttpServer {
public:
    ClinicHttpServer(Database& database, int port);
    bool run();

private:
    Database& database_;
    int port_;
};
