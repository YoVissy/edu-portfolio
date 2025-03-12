<?php

require_once 'conf.php';

// Create connection
$conn = mysqli_connect($servername, $username, $password, $database);

// Check connection
if (!$conn) {
    die("Connection failed: " . mysqli_connect_error());
}
//        echo "Connected successfully";

$sql = "select id, date_time, luminance, pressure, temperature, wind_speed, wind_direction, humidity_in, humidity_out from weather";
$result = mysqli_query($conn, $sql);

if ($result->num_rows > 0) {
    echo "<table><tr>   <th>ID</th>
                        <th>Kello</th>
                        <th>Luminance</th>
                        <th>Pressure</th>
                        <th>Temperature</th>
                        <th>Wind Speed</th>
                        <th>Wind Direction</th>
                        <th>Humidity In</th>
                        <th>Humidity Out</th>
                 </tr>";
    // output data of each row
    while($row = $result->fetch_assoc()) {
        echo "<tr>     
                        <td>" . $row["id"]. "</td>
                        <td>" . $row["date_time"]. "</td>
                        <td>" . $row["luminance"]. "</td>
                        <td>" . $row["pressure"]. "</td>
                        <td> " . $row["temperature"]. "</td>
                        <td>" . $row["wind_speed"]. "</td>
                        <td>" . $row["wind_direction"]. "</td>
                        <td>" . $row["humidity_in"]. "</td>
                        <td>" . $row["humidity_out"]. "</td>
                        </tr>";
    }
    echo "</table>";
} else {
    echo "0 results";
}

$conn->close();