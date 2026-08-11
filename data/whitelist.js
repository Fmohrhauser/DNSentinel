let currentWhitelist = [];
function updateWhitelist()
{
    fetch("/api/whitelist", {
        cache: "no-store"
    })
    .then(response => response.json())
    .then(domains => {

        currentWhitelist = domains;

        renderWhitelist(currentWhitelist);
    });
}

function addDomain()
{
    let domain =
        document.getElementById("new-domain").value;

    fetch("/api/whitelist/add", 
    {
        method:"POST",

        headers:
        {
            "content-Type":"application/json"
        },

        body:JSON.stringify({
            domain:domain
        })

    })
    .then(() => {

        document.getElementById("new-domain").value="";

        updateWhitelist();
        updateWhitelistCount();
    });
}

function removeDomain(domain)
{

    fetch("/api/whitelist/remove",{
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

        updateWhitelist();
        updateWhitelistCount();

    });
}

document
.getElementById("add-domain")
.addEventListener(
    "click",
    addDomain
);
function importWhitelist()
{

    let domains =
        document
        .getElementById("import-list")
        .value;
    const button =
        document.getElementById("import-btn");

    const progress =
        document.getElementById("import-progress");

    button.disabled = true;
    button.innerText = "Importing ...";
    progress.classList.remove("hidden");
    fetch("/api/whitelist/import", {
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
        button.innerText = "Import Whitelist";
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

        updateWhitelist();
        document
        .getElementById("import-list")
        .value = "";
        button.disabled = false;
        button.innerText = "Import Whitelist";

        progress.classList.add("hidden");

        updateWhitelistCount();
    })
}

document
.getElementById("import-btn")
.addEventListener(
    "click",
    importWhitelist()
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

function updateWhitelistCount()
{
    fetch("/api/whitelist/count")
    .then(response => response.json())
    .then(data => {

        document
        .getElementById("whitelist-count")
        .innerText = "(" + data.count + ")";
    });
}

function renderWhitelist(domains)
{
    let container =
        document.getElementById("whitelist");

    document
    .getElementById("whitelist-results")
    .innerText =
    `Showing ${domains.length} of ${currentWhitelist.length} domains`;

    container.innerHTML = "";

    domains.forEach(domain => {

        let item =
            document.createElement("div");

        item.className =
            "filterlist-entry";

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

function filterWhitelist()
{
    let search =
        document
        .getElementById("whitelist-search")
        .value
        .toLowerCase();

    let filtered =
        currentWhitelist.filter(domain =>
            domain
            .toLowerCase()
            .includes(search)
        );

        renderWhitelist(filtered);
}

function resetWhitelist()
{
    let confirmReset =
        confirm(
            "Are you sure you want to remove all whitelisted domains?"
        );

    if(!confirmReset)
        return;

    fetch("/api/whitelist/reset",
        {
            method:"POST"
        })
        .then(() => {
            updateWhitelist();
            updateWhitelistCount();
        });
}

document
.getElementById("reset-whitelist")
.addEventListener(
    "click",
    resetWhitelist
);
document
.getElementById("whitelist-search")
.addEventListener(
    "input",
    filterWhitelist
);
updateWhitelist();
updateWhitelistCount();