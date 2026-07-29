#include "dashboard.h"
#include <WebServer.h>
#include "stats.h"
#include "query_log.h"


WebServer server(80);

void startDashboard()
{
    server.on("/", [](){

        String page;

        page += "<!DOCTYPE html>";
        page += "<html>";

        page += "<head>";

        page += "<title>DNSentinel Dashboard</title>";

        page += "<style>";

        page += "body{";
        page += "background:#121212;";
        page += "color:white;";
        page += "font-family: Arial,sans-serif;";
        page += "margin:40px;";
        page += "}";

        page += "h1{";
        page += "color:#00d084";
        page += "}";

        page += "table{";
        page += "border-collapse:collapse;";
        page += "width:100%;";
        page += "}";

        page += "th,td{";
        page += "border: 1px solid #444;";
        page += "padding:10px;";
        page += "}";

        page += "th{";
        page += "background:#222;";
        page += "}";

        page += "tr:nth-child(even){";
        page += "background:#1b1b1b;";
        page += "}";

        page += "</style>";

        page += "</head>";

        page += "<body>";

        page += "<h1>DNSentinel Dashboard</h1>";

        page += "<hr>";

        page += "<h2>Statistics</h2>";

        page += "<p>Total Requests: ";
        page += totalRequests;
        page += "</p>";

        page += "<p>Blocked Requests: ";
        page += blockedRequests;
        page += "</p>";

        page += "<p>Forwarded Requests: ";
        page += forwardedRequests;
        page += "</p>";

        page += "<hr>";

        page += "<h2> Recent Queries</h2>";

        page += "<table border='1'>";

        page += "<tr>";
        page += "<th>Domain</th>";
        page += "<th>Action</th>";
        page += "</tr>";

        for(int i = getQueryCount() - 1; i>=0; i--)
        {
            QueryLogEntry query = getQuery(i);

            page += "<tr>";

            page += "<td>";
            page += query.domain;
            page += "</td>";

            page += "<td>";
            page += actionToString(query.action);
            page += "</td>";

            page += "</tr>";
        }
        page += "<table>";

        page += "</body>";

        page += "</html>";

        server.send(
            200,
            "text/html",
            page
        );
    });

    server.begin();

    Serial.println("Dashboard started!");
}

void handleDashboard()
{
    server.handleClient();
}