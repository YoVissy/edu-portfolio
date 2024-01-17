<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Sääaseman tiedot</title>

    <style>
        body {font-family: "Lato", sans-serif;}

        /* Style the tab */
        div.tab {
            overflow: hidden;
            border: 1px solid #ccc;
            background-color: #f1f1f1;
        }

        /* Style the buttons inside the tab */
        div.tab button {
            background-color: inherit;
            float: left;
            border: none;
            outline: none;
            cursor: pointer;
            padding: 14px 16px;
            transition: 0.3s;
            font-size: 17px;
        }

        /* Change background color of buttons on hover */
        div.tab button:hover {
            background-color: #ddd;
        }

        /* Create an active/current tablink class */
        div.tab button.active {
            background-color: #ccc;
        }

        /* Style the tab content */
        .tabcontent {
            display: none;
            padding: 6px 12px;
            border: 1px solid #ccc;
            border-top: none;
        }
    </style>

</head>


<body>
<p>Hyvää päivää! Tässä sinulle sääaseman dataa.</p>

<div class="tab">
    <button class="tablinks" onclick="vaihdaTab(event, 'Taulukko1')">Taulukko 1</button>
    <button class="tablinks" onclick="vaihdaTab(event, 'Taulukko2')">Taulukko 2</button>
    <button class="tablinks" onclick="vaihdaTab(event, 'Taulukko3')">Taulukko 3</button>
</div>

<div id="Taulukko1" class="tabcontent">
    <h3>Ilmanpaine</h3>
    <p> <?php include taulukko1.php ?>

        Testi1
    </p>
</div>

<div id="Taulukko2" class="tabcontent">
    <h3>Tuulen nopeus</h3>
    <p>  <?php include taulukko2.php ?>

        Testi2
    </p>
</div>

<div id="Taulukko3" class="tabcontent">
    <h3>Tuulen suunta</h3>
    <p>
         <?php include taulukko3.php ?>

        Testi3
    </p>
</div>

<script>
    function vaihdaTab(evt, cityName) {
        var i, tabcontent, tablinks;
        tabcontent = document.getElementsByClassName("tabcontent");
        for (i = 0; i < tabcontent.length; i++) {
            tabcontent[i].style.display = "none";
        }
        tablinks = document.getElementsByClassName("tablinks");
        for (i = 0; i < tablinks.length; i++) {
            tablinks[i].className = tablinks[i].className.replace(" active", "");
        }
        document.getElementById(cityName).style.display = "block";
        evt.currentTarget.className += " active";
    }
</script>

</body>
</html>
