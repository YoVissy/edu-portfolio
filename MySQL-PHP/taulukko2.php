<?

require_once 'conf.php';

// Create connection
$conn = mysqli_connect($servername, $username, $password, $database);

// Check connection
if (!$conn) {
    die("Connection failed: " . mysqli_connect_error());
}
//        echo "Connected successfully";


$sql = "select id, date_time, wind_speed from weather";
$result = mysqli_query($conn, $sql);

if ($result->num_rows > 0) {
    echo "<table><tr>   <th>ID</th>
                    <th>Kello</th>
                    <th>Wind Speed</th>
                </tr>";
    // output data of each row
    while($row = $result->fetch_assoc()) {
        echo "<tr>
                    <td>" . $row["id"]. "</td>
                    <td>" . $row["date_time"]. "</td>
                    <td>" . $row["wind_speed"]. "</td>
                </tr>";
    }
    echo "</table>";
} else {
    echo "0 results";
}

$conn->close();