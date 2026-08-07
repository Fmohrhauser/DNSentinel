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
    .then(() => {
        settingsChanged = false;

        showStatus( "Settings saved", "var(--success)", 3000);
    })
    .catch(error => {

        showStatus(error.message, "var(--danger)");
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
    .then(()=> {

        updateSettings();
        settingsChanged = false;

        showStatus("Defaults restored", "var(--warning)", 3000);
    });
}

document
.getElementById("restore-settings")
.addEventListener("click",restoreSettings);


function markSettingsChanged()
{
    if(settingsChanged)
    {
        return;
    }
    settingsChanged = true;

    showStatus(
        "Unsaved changes",
        "var(--warning)"
    );
    
}

function updateRedirectVisibility()
{
    const mode = document.getElementById("blockingMode").value;
    const container = document.getElementById("redirectIPContainer");

    if(mode === "2")
    {
        container.classList.remove("hidden");
    }
    else
    {
        container.classList.add("hidden");
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

function showStatus(message, color, clearAfter = 0){
    const status = document.getElementById("settingStatus");

    status.textContent = message;
    status.style.color = color;

    if(clearAfter > 0){
        setTimeout(() => {
            status.textContent = "";
        }, clearAfter);
    }
}

updateSettings();
