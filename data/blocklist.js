let currentBlocklist = [];
function updateBlocklist()
{
    fetch("/api/blocklist", {
        cache: "no-store"
    })

    .then(response => response.json())
    .then(domains => {

        currentBlocklist = domains;

        renderBlocklist(currentBlocklist);
    
    });
}


function addDomain()
{
    let domain =
        document.getElementById("new-domain").value;

    fetch("/api/blocklist/add",
    {
        method:"POST",

        headers:
        {
            "content-Type":"application/json"
        },

        body:JSON.stringify(
        {
            domain:domain
        })

    })
    .then(() => {

        document.getElementById("new-domain").value="";

        updateBlocklist();
        updateBlocklistCount();
    });
}  


function removeDomain(domain)
{

    fetch("/api/blocklist/remove",{
        method:"POST",

        headers:
        {
            "content-Type":"application/json"
        },

        body:JSON.stringify(
        {
            domain:domain
        })
    })
    .then(() => {

        updateBlocklist();
        updateBlocklistCount();

    });

}

document
.getElementById("add-domain")
.addEventListener(
    "click",
    addDomain
);




function importBlocklist()
{

    let domains =
        document
        .getElementById("import-list")
        .value;
    const button = 
        document.getElementById("import-button");

    const progress =
        document.getElementById("import-progress");
    
    button.disabled = true;
    button.innerText = "Importing ...";
    progress.classList.remove("hidden");
    fetch("/api/blocklist/import", {
        method: "POST",

        headers: {

            "Content-Type":"application/json"

        },

        body: JSON.stringify({

            domains: domains
        })

    })
    .then(response => response.json())
    .catch(error => {

        console.error(error);

        button.disabled = false;
        button.innerText = "Import Blocklist";

        progress.classList.add("hidden");
    })
    .then(result => {

        document.getElementById("import-summary")
        .classList
        .remove("hidden");
        setTimeout(() => {

            document
            .getElementById("import-summary")
            .classList
            .add("hidden");
        }, 8000);

        document
        .getElementById("import-added")
        .innerHTML =
        result.added;

        document
        .getElementById("import-duplicates")
        .innerHTML =
        result.duplicates;

        document
        .getElementById("import-ignored")
        .innerHTML =
        result.ignored;
        
        updateBlocklist();
        document
        .getElementById("import-list")
        .value = "";
        button.disabled = false;
        button.innerText = "Import Blocklist";

        progress.classList.add("hidden");

        updateBlocklistCount();
        
    })
}

document
.getElementById("import-button")
.addEventListener(
    "click",
    importBlocklist
);

document
.getElementById("close-summary")
.addEventListener(
    "click",
    () => {

        document
        .getElementById("import-summary")
        .classList
        .add("hidden");
    }
)

function updateBlocklistCount()
{
    fetch("/api/blocklist/count")
    .then(response => response.json())
    .then(data => {

        document
        .getElementById("blocklist-count")
        .innerText = "(" + data.count + ")";
    });
}

function renderBlocklist(domains)
{
    let container = 
        document.getElementById("blocklist");

    document
    .getElementById("blocklist-results")
    .innerText =
    `Showing ${domains.length} of ${currentBlocklist.length} domains`;

    container.innerHTML = "";

    domains.forEach(domain => {

        let item =
            document.createElement("div");

        item.className =
            "blocklist-entry";

        const span =
            document.createElement("span");

        span.textContent = domain;

        const button =
            document.createElement("button");

        button.className =
            "remove-domain";

        button.textContent =
            "Remove";

        button.addEventListener(
            "click",
            () => removeDomain(domain)
        );

        item.appendChild(span);
        item.appendChild(button);

        container.appendChild(item);
    });
}

function filterBlocklist()
{
    let search =
        document
        .getElementById("blocklist-search")
        .value
        .toLowerCase();

    let filtered =
        currentBlocklist.filter(domain =>
            domain
            .toLowerCase()
            .includes(search)
        );

        renderBlocklist(filtered);
}

function resetBlocklist()
{
    let confirmReset =
        confirm(
            "Are you sure you want to remove all blocked domains?"
        );

    if(!confirmReset)
        return;

    fetch("/api/blocklist/reset",
        {
            method:"POST"
        })
        .then(() =>{
            updateBlocklist();
            updateBlocklistCount();
        });
    
}
document
.getElementById("reset-blocklist")
.addEventListener(
    "click",
    resetBlocklist
);
document
.getElementById("blocklist-search")
.addEventListener(
    "input",
    filterBlocklist
);
updateBlocklist();
updateBlocklistCount();