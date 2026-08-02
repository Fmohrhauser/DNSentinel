let queryLimit = 25;
function updateLogs()
{
    fetch("/api/logs?limit=" + queryLimit)
        .then(response => response.json())
        .then(logs => {
            let table = document.getElementById("logs");

            table.innerHTML = "";

            sortQueries(limitQueries(logs)).forEach(query => {
                    let time = (query.timestamp)

                let row = `
                    <tr>
                        <td>${time}</td>
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
            filterQueries();
        });
        
}

setInterval(updateLogs, 2000);

updateLogs();

function filterQueries()
{
    const search =
        document.getElementById("querySearch")
        .value
        .toLowerCase();

    const filter =
        document.getElementById("queryFilter").value;

    const rows =
        document.querySelectorAll("#logs tr");

    rows.forEach(row => {

        const domain =
            row.children[1].textContent.toLowerCase();

        const action =
            row.children[2].textContent.toLowerCase();

        let visible =
            domain.includes(search);

        if(filter == "blocked")
            visible = visible && action.includes("blocked");

        if(filter == "forwarded")
            visible = visible && action.includes("forwarded");

        if(filter == "cache")
            visible = visible && action.includes("cache");

        row.style.display =
            visible ? "" : "none";
    });
}

document
.getElementById("querySearch")
.addEventListener("input", filterQueries);

document
.getElementById("queryFilter")
.addEventListener("change", filterQueries);


function sortQueries(logs)
{
    const sort =
        document.getElementById("querySort").value;

    if(sort == "newest")
    {
        return logs;
    }

    if(sort == "oldest")
    {
        return[...logs].reverse();
    }

    if(sort == "az")
    {
        return[...logs].sort((a,b) =>
            a.domain.localeCompare(b.domain)
        );
    }

    if(sort == "za")
    {
        return[...logs].sort((a,b)=>
            b.domain.localeCompare(a.domain)
        );
    }

    return logs;
}

document
.getElementById("querySort")
.addEventListener("change", updateLogs);

function clearQueries()
{
    if(!confirm("Are you sure you want to clear all query logs?"))
    {
        return;
    }

    fetch("/api/logs/clear", {
        method: "POST"
    })
    .then(response => response.json())
    .then(data => {

        updateLogs();

        document.getElementById("queryStatus").innerHTML =
            "Query logs cleared ✓";

        document.getElementById("queryStatus").style.color =
            "var(--success)";

        setTimeout(() => {

            document.getElementById("queryStatus").innerHTML = "";
        }, 3000);

    });
}

document
.getElementById("clearQueries")
.addEventListener("click", clearQueries);

function limitQueries(logs)
{
    const limit =
        Number(document.getElementById("queryLimit").value);

    return logs.slice(0, limit);
}

document
.getElementById("queryLimit")
.addEventListener("change", function(){

    queryLimit = Number(this.value);

    updateLogs();
});

