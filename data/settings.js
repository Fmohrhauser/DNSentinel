let settingsChanged = false;
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

        document.getElementById("blockingMode").value =
            data.blockingMode;

        document.getElementById("redirectIP").value =
            data.redirectIP;
        updateRedirectVisibility();
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
            document.getElementById("upstreamDNS").value,

        blockingMode:
            Number(document.getElementById("blockingMode").value),

        redirectIP:
            document.getElementById("redirectIP").value
    };

    fetch("/api/settings", {
        method:"POST",

        headers:{
            "Content-Type":"application/json"
        },

        body: JSON.stringify(settings)
    })
    .then(async response => {

        const data = await response.json();

        if(!response.ok)
        {
            throw new Error(data.error);
        }

        return data;
    })
    .then(data => {
        settingsChanged = false;

        document.getElementById("settingStatus").innerHTML =
            "Settings saved ✓";

            document.getElementById("settingStatus").style.color =
                "var(--success)";

                setTimeout(() => {
                    document.getElementById("settingStatus").innerHTML = "";
                }, 3000);
    })
    .catch(error => {

        document.getElementById("settingStatus").innerHTML =
            error.message;

            document.getElementById("settingStatus").style.color =
                "var(--danger)";
    });
}

document
.getElementById("save-settings")
.addEventListener("click", sendSettings);


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

function updateRedirectVisibility()
{
    const mode = document.getElementById("blockingMode").value;
    const container = document.getElementById("redirectIPContainer");

    console.log("Blocking mode:", mode);

    if(mode == "2")
    {
        container.style.display = "block";
    }
    else
    {
        container.style.display = "none";
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

document
.getElementById("blockingMode")
.addEventListener("change", function ()
{
    markSettingsChanged();
    updateRedirectVisibility();
});

document
.getElementById("redirectIP")
.addEventListener("input", markSettingsChanged);


