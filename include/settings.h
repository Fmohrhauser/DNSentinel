#pragma once

#include <Arduino.h>

enum BlockingMode
{
    NULL_IP,
    NXDOMAIN,
    REDIRECT
};

struct Settings
{
    String upstreamDNS;

    bool blockingEnabled;

    bool cacheEnabled;

    bool queryLoggingEnabled;
    BlockingMode blockingMode;
    String redirectIP;
};


void initializeSettings();

Settings getSettings();

void updateSettings(const Settings& newSettings);

String createSettingsJSON();

void saveSettings();
void loadSettings();