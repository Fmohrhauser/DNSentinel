#pragma once

#include <Arduino.h>

struct Settings
{
    String upstreamDNS;

    bool blockingEnabled;

    bool cacheEnabled;

    bool queryLoggingEnabled;
};

void initializeSettings();

Settings getSettings();

void updateSettings(const Settings& newSettings);

String createSettingsJSON();

void saveSettings();
void loadSettings();