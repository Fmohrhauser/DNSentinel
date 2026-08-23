#pragma once

#include <Arduino.h>
#include "idf_webserver.h"

bool checkAuthenticationIDF(httpd_req_t *req);
String generatePasswordSalt();

String hashPassword(String password);