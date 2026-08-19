#ifndef DOMAIN_UTILS_H
#define DOMAIN_UTILS_H

#include <Arduino.h>

String normalizeDomain(String line);

bool validDomain(const String &domain);

#endif
