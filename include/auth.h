#pragma once

#include <Arduino.h>
#include <WebServer.h>

bool checkAuthentication(WebServer &server);

String hashPassword(String password);