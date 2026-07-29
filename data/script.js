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