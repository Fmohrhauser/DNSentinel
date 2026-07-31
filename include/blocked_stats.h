#pragma once

#include <Arduino.h>

void incrementBlockedDomain(const String& domain);

String createTopBlockedJSON();