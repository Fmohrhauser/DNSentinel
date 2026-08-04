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

    //Authenication
    bool authEnabled;
    String username;
    String passwordHash;
};


void initializeSettings();

Settings getSettings();

void updateSettings(const Settings& newSettings);

String createSettingsJSON();

void saveSettings();
void loadSettings();
bool validIP(String ip);