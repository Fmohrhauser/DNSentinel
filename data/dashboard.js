let queryHistory = [];
let previousRate = 0;
let firstRateUpdate = true;

function updateTimestamp(element)
{
    document.getElementById(element).innerHTML =
        "Updated " + new Date().toLocaleTimeString();
}
function updateStats()
{
    fetch("/api/stats")
        .then(response => response.json())
        .then(data => {

            document.getElementById("total").innerHTML = data.total;
            document.getElementById("blocked").innerHTML = data.blocked;
            document.getElementById("forwarded").innerHTML = data.forwarded;
            document.getElementById("cache_hits").innerHTML = data.cache_hits;

            let total = data.total

            if(total >0)
            {
                let forwarded = 
                    (data.forwarded / total) * 100;

                let blocked = 
                    (data.blocked / total) * 100;

                let cache = 
                    (data.cache_hits / total) *100;

                document.getElementById("forwarded-bar").style.width = 
                    forwarded + "%";

                document.getElementById("blocked-bar").style.width = 
                    blocked + "%";

                document.getElementById("cache-bar").style.width = 
                    cache + "%";
                
                
                document.getElementById("forwarded-percent").innerHTML =
                    Math.round(forwarded) + "%";

                document.getElementById("blocked-percent").innerHTML =
                    Math.round(blocked) + "%";

                document.getElementById("cache-percent").innerHTML =
                    Math.round(cache) + "%";

            }

            updateTimestamp("stats-updated");

        });
}

setInterval(updateStats, 2000);

updateStats();



function updateTopBlocked()
{
    fetch("/api/topblocked")
        .then(response => response.json())
        .then(domains => {
            let table =
                document.getElementById("top-blocked");

            table.innerHTML = "";

            domains.forEach((entry, index)=> {

                let row =`
                    <tr>
                        <td>${index + 1}</td>
                        <td>${entry.domain}</td>
                        <td>${entry.count}</td>
                    </tr>

                    `;

                    table.innerHTML += row;
            });
        });
}

updateTopBlocked();

setInterval(updateTopBlocked, 5000);


let currentIP = "";


function updateSystem()
{
    fetch("/api/system")
        .then(response => response.json())
        .then(data => {

            currentIP = data.ip;

            document.getElementById("ip").innerHTML = currentIP;

            document.getElementById("chip").innerHTML =
                data.chip;

            currentUptime = data.uptime;

            document.getElementById("uptime").innerHTML =
                formatTime(currentUptime);

            document.getElementById("memory").innerHTML = 
                Math.round(data.memory / 1024) + " KB";

            if(data.dnsStatus == 1)
            {
                document.getElementById("status-text").innerHTML =
                    "Protection Active";

                document.getElementById("status-dot").style.background = 
                    "#00D084";
            }
            else if(data.dnsStatus == 2)
            {
                document.getElementById("status-text").innerHTML =
                    "Upstream DNS Offline";
                
                document.getElementById("status-dot").style.background =
                    "#FF4444";
            }
            else
            {
                document.getElementById("dns-status").innerHTML = 
                    "Checking DNS...";

            }
        });
}
setInterval(()=> {

    currentUptime++;

    document.getElementById("uptime").innerHTML =
        formatTime(currentUptime);
}, 1000);

function pad(n){
    return String(n).padStart(2,"0");
}

function formatTime(seconds)
{
    let hours = Math.floor(seconds / 3600);

    let minutes = Math.floor(
        (seconds % 3600) / 60
    );

    let secs = 
        seconds % 60;

    return(
     hours + "h " +
     minutes + "m " +
     secs + "s"
    );
}


updateSystem();

setInterval(updateSystem, 5000);


function updateDNSHealth()
{
    fetch("/api/dnshealth")
        .then(response => response.json())
        .then(data =>
        {
            
            let status = 
                document.getElementById("dns-status");
            let dot =
                document.getElementById("dns-dot")
            if(!data.checked)
            {
                status.innerHTML = "Checking"
                dot.style.background =
                    "var(--checking)";
            }
            else if(data.online)
            {
                status.innerHTML = "Online";
                dot.style.background =
                    "var(--success)";
            }
            else
            {
                status.innerHTML = "Offline"
                dot.style.background =
                    "var(--danger)"
            }

            document.getElementById("dns-latency").innerHTML =
                "Latency: " + data.averageLatency + "ms";

                document.getElementById("dns-failures").innerHTML =
                    "Failures: " + data.failures;

        });
}

updateDNSHealth();

setInterval(updateDNSHealth, 5000);
document.getElementById("hideIP").addEventListener("change", function(){

    if(this.checked)
    {
        document.getElementById("ip").innerHTML = "Hidden";
    }
    else
    {
        document.getElementById("ip").innerHTML = currentIP;
    }

});

function updateLastQuery()
{
    fetch("/api/logs?limit=1")
        .then(response => response.json())
        .then(logs =>{

            if(logs.length > 0)
            {
                document.getElementById("last-query").innerHTML =
                    logs[0].domain;

                    updateTimestamp("query-updated");
            }
            else
            {
                document.getElementById("last-query").innerHTML =
                    "None"
                
                    updateTimestamp("query-updated");
            }
        });
}

setInterval(updateLastQuery, 2000);

updateLastQuery();

function updateQueryRate()
{
    fetch("/api/stats")
        .then(response => response.json())
        .then (data=>{

            const now = Date.now();

            queryHistory.push({
                time: now,
                total: Number(data.total)
            });

            

            while(
                    queryHistory.length > 0 &&
                    now - queryHistory[0].time > 60000
            )
            {
                queryHistory.shift();
            }

            let rate = 0;

            if(queryHistory.length >= 2)
            {
                const oldest = queryHistory[0];
                const newest = queryHistory[queryHistory.length - 1];

                const queryDifference =
                    newest.total - oldest.total;

                const minuteDifference =
                    (newest.time - oldest.time) / 60000;

                if(minuteDifference > 0)
                {
                    rate = queryDifference / minuteDifference;
                }
            }

  

            const rateElement =
                document.getElementById("query-rate");

            const trend =
                document.getElementById("query-trend");

            const displayRate = Math.round(rate);

            rateElement.innerHTML = displayRate;

            if(rate >= 20)
            {
                rateElement.style.color =
                    "var(--success)";
            }
            else if(rate >= 5)
            {
                rateElement.style.color =
                    "var(--warning)";
            }
            else
            {
                rateElement.style.color = "var(--secondary-text)"
            }
            if(rate > previousRate + 1)
            {
                trend.innerHTML = "▲";
                trend.style.color = "var(--success)";
            }
            else if(rate < previousRate - 1)
            {
                trend.innerHTML = "▼";
                trend.style.color = "var(--danger)";
            }
            else
            {
                trend.innerHTML = "▬";
                trend.style.color = "var(--secondary-text)";
            }
            previousRate = rate;
    });
}

setInterval(updateQueryRate, 5000);

updateQueryRate();




