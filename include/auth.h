#pragma once

#include <Arduino.h>
#include <WebServer.h>
#include "idf_webserver.h"

bool checkAuthentication(WebServer &server);
bool checkAuthenticationIDF(httpd_req_t *req);

String hashPassword(String password);