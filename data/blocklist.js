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

                    item.innerHTML = `
                        <span>${domain}</span>
                        
                        <button class = "remove-domain">
                            Remove
                        </button>
                    `;

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

        document
        .getElementById("import-status")
        .innerHTML =
            `Added ${result.added} domains.<br>
             Skipped ${result.duplicates} duplicates.<br>
             Ignored ${result.ignored} invalid/comment lines.`;
        
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