function updateStats()
{
    fetch("/api/stats")
        .then(response => response.json())
        .then(data => {

            document.getElementById("total").innerHTML = data.total;
            document.getElementById("blocked").innerHTML = data.blocked;
            document.getElementById("forwarded").innerHTML = data.forwarded;

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


let currentIP = "";


function updateSystem()
{
    fetch("/api/system")
        .then(response => response.json())
        .then(data => {

            currentIP = data.ip;

            document.getElementById("ip").innerHTML = currentIP;

        });
}


updateSystem();

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