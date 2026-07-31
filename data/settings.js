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