function updateBlocklist()
{
    fetch("/api/blocklist", {
        cache: "no-store"
    })

    .then(response => response.json())
    .then(domains => {

        let container =
            document.getElementById("blocklist");

            container.innerHTML = "";
            
            domains.forEach(domain => {
                let item = document.createElement("div");

                item.className =
                    "blocklist-entry";

                    const span = document.createElement("span");

                    span.textContent = domain;

                    const button = document.createElement("button");

                    button.className = "remove-domain";

                    button.textContent = "remove";

                    button.addEventListener("click", () => {
                        removeDomain(domain);
                    });

                    item.appendChild(span);

                    item.appendChild(button);

                    item.querySelector("button")
                    .addEventListener("click",() => {

                        removeDomain(domain);

                    });

                    container.appendChild(item);
            });
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

    });

}

document
.getElementById("add-domain")
.addEventListener(
    "click",
    addDomain
);

updateBlocklist();


function importBlocklist()
{

    let domains =
        document
        .getElementById("import-list")
        .value;
    const button = 
        document.getElementById("import-button");
    
    button.disabled = true;
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
        
    })
}

document
.getElementById("import-button")
.addEventListener(
    "click",
    importBlocklist
);
setTimeout(() => {

    document
    .getElementById("import-status")
    .innerHTML = "";
}, 5000);

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