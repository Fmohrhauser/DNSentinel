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