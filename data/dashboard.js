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

        });
}

setInterval(updateStats, 2000);

updateStats();

function updateLogs()
{
    fetch("/api/logs")
        .then(response => response.json())
        .then(logs => {
            let table = document.getElementById("logs");

            table.innerHTML = "";

            logs.forEach(query => {
                let row = `
                    <tr>
                        <td>${query.domain}</td>
                        <td>
                            <span class="action ${query.action.toLowerCase().replaceAll(" ", "_")}">
                                ${query.action}
                            </span>
                        </td>
                    </tr>
                `;

                table.innerHTML += row;
            });
        });
}

setInterval(updateLogs, 2000);

updateLogs();

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

            document.getElementById("uptime").innerHTML = 
                formatTime(data.uptime);

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
                document.getElementById("status-text").innerHTML = 
                    "Checking DNS...";

                document.getElementById("status-dot").style.background =
                    "#FFCC00"
            }
        });
}

function formatTime(seconds)
{
    let hours = Math.floor(seconds / 3600);

    let minutes = Math.floor(
        (seconds % 3600) / 60
    );

    return hours + "h " + minutes + "m"
}


updateSystem();

setInterval(updateSystem, 5000);

function updateSettings()
{
    fetch("/api/settings")
    .then(response => response.json())
    .then(data => {
        document.getElementById("blockingEnabled").checked = 
            data.blockingEnabled;

        document.getElementById("cacheEnabled").checked =
            data.cacheEnabled;

        document.getElementById("queryLoggingEnabled").checked =
            data.queryLoggingEnabled;

        document.getElementById("upstreamDNS").value =
            data.upstreamDNS;
    });
}

updateSettings();

function sendSettings()
{
    
    
    let settings = {
        
        blockingEnabled:
            document.getElementById("blockingEnabled").checked,

        cacheEnabled:
            document.getElementById("cacheEnabled").checked,

        queryLoggingEnabled:
            document.getElementById("queryLoggingEnabled").checked,

        upstreamDNS:
            document.getElementById("upstreamDNS").value
    };

    fetch("/api/settings", {
        method:"POST",

        headers:{
            "Content-Type":"application/json"
        },

        body: JSON.stringify(settings)
    })
    .then(response => response.json())
    .then(data => {

        settingsChanged = false;

        document.getElementById("settingStatus").innerHTML =
        "Settings saved ✓";
        document.getElementById("settingStatus").style.color =
            "var(--success)";

        setTimeout(()=>{
            document.getElementById("settingStatus").innerHTML="";
        },3000);
    });
}

document
.getElementById("save-settings")
.addEventListener("click", sendSettings)

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

const settingsButton =
    document.getElementById("settings-button");

const settingsPanel =
    document.getElementById("settings-panel");

settingsButton.addEventListener("click", function(){

    settingsPanel.classList.toggle("open");
});

function restoreSettings()
{
    fetch("/api/settings/reset", {
        method:"POST"
    })
    .then(response => response.json())
    .then(data=> {

        updateSettings();
        settingsChanged = false;

        document.getElementById("settingStatus").innerHTML =
        "Defaults restored ✓";
        document.getElementById("settingStatus").style.color =
            "var(--warning)";

        setTimeout(() => {
            document.getElementById("settingStatus").innerHTML = "";
        }, 3000);
    });
}

document
.getElementById("restore-settings")
.addEventListener("click",restoreSettings);

let settingsChanged = false;

function markSettingsChanged()
{
    if(!settingsChanged)
    {
        settingsChanged = true;

        document.getElementById("settingStatus").innerHTML =
            "Unsaved changes";
        document.getElementById("settingStatus").style.color =
            "#FF4444";
    }
}

document
.getElementById("blockingEnabled")
.addEventListener("change", markSettingsChanged);

document
.getElementById("cacheEnabled")
.addEventListener("change",markSettingsChanged);

document
.getElementById("queryLoggingEnabled")
.addEventListener("change", markSettingsChanged);

document
.getElementById("upstreamDNS")
.addEventListener("input", markSettingsChanged);





function updateDNSHealth()
{
    fetch("/api/dnshealth")
        .then(response => response.json())
        .then(data =>
        {
            
            let status = 
            document.getElementById("dns-status");
            if(!data.checked)
            {
                status.innerHTML = "🟡 Checking"
            }
            else if(data.online)
            {
                status.innerHTML = "🟢 Online";
            }
            else
            {
                status.innerHTML = "🔴 Offline"
            }

            document.getElementById("dns-latency").innerHTML =
                "Latency: " + data.averageLatency + "ms";

                document.getElementById("dns-failures").innerHTML =
                    "Failures: " + data.failures;

        });
}

updateDNSHealth();

setInterval(updateDNSHealth, 5000);