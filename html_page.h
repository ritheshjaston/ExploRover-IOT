//HTML code for webpage
//=====================
const char webpageCode[] PROGMEM =
R"=====(

<!DOCTYPE HTML>
<html>

<head>
    <title>ESP32 Web Server</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body {
            font-family: Arial;
            text-align: center;
            margin: 0px auto;
            padding-top: 10px;
        }

        table {
            margin-left: auto;
            margin-right: auto;
        }

        td {
            padding: 8 px;
        }

        .button {
            background-color: #2f4468;
            border: none;
            color: white;
            padding: 10px 20px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 18px;
            margin: 6px 3px;
            cursor: pointer;
            -webkit-touch-callout: none;
            -webkit-user-select: none;
            -khtml-user-select: none;
            -moz-user-select: none;
            -ms-user-select: none;
            user-select: none;
            -webkit-tap-highlight-color: rgba(0, 0, 0, 0);
        }

        img {
            width: auto;
            max-width: 100%;
            height: auto;
            border-radius: 5px;
        }
    </style>
</head>
<!-------------------------------C S S------------------------------>

<!------------------------------H T M L----------------------------->

<body>
    <h2>EXPLO ROVER -let's Explore Now</h2>
    <img id="videoStream" src="#" width="640" height="480"
        onerror="this.src='https://img-c.udemycdn.com/course/480x270/4210070_6b6f_2.jpg'" /><br>
    <label for="points">Camera Position(0-180<sup>o</sup>)</label><br>
    <input type="range" class="progress" id="points" name="points" min="0" max="180"
        onchange="toggleCheckbox(this.value)">

    <table>
        <tr align="left">
            <td>Temparature: </td>
            <td id="temp"></td>
            <td><sup>o</sup>C</td>
        </tr>
        <tr align="left">
            <td>Humidity: </td>
            <td id="humidity"></td>
        </tr>
        <tr align="left">
            <td>Magnetic Field: </td>
            <td id="mgf"></td>
        </tr>
        <tr align="left">
            <td>Air Quality: </td>
            <td id="air"></td>
        </tr>
        <tr align="left">
            <td>Day/Night: </td>
            <td id="don"></td>
        </tr>

        <tr align="center">
            <td colspan="2" id="msg" style="color: red;"></td>
        </tr>

    </table>

    <table>
        <tr>
            <button class="button" style="background-color: goldenrod;" onmousedown="toggleCheckbox('slow');"
                ontouchstart="toggleCheckbox('slow');">Slow</button></td>
            <button class="button" style="background-color: green;" onmousedown="toggleCheckbox('medium');"
                ontouchstart="toggleCheckbox('medium');">Medium</button></td>
            <button class="button" style="background-color: red;" onmousedown="toggleCheckbox('fast');"
                ontouchstart="toggleCheckbox('fast');">Fast</button></td>
            <button class="button" style="background-color: blue;" onclick="toggleCheckbox('auto')">AUTO</button></td>
        </tr>
    </table>
    <table id="control">
        <tr>
            <td colspan="3" align="center"><button class="button" ontouchstart="toggleCheckbox('forward');"
                    ontouchend="toggleCheckbox('stop');">Forward</button></td>
        </tr>
        <tr>
            <td align="center"><button class="button" ontouchstart="toggleCheckbox('left');"
                    ontouchend="toggleCheckbox('stop');">Left</button></td>
            <td align="center"><button class="button" onmousedown="toggleCheckbox('stop');"
                    ontouchstart="toggleCheckbox('stop');">Stop</button></td>
            <td align="center"><button class="button" ontouchstart="toggleCheckbox('right');"
                    ontouchend="toggleCheckbox('stop');">Right</button></td>
        </tr>
        <tr>
            <td colspan="3" align="center"><button class="button" ontouchstart="toggleCheckbox('backward');"
                    ontouchend="toggleCheckbox('stop');">Backward</button></td>
        </tr>
    </table>

    <!-----------------------------JavaScript--------------------------->
    <script>
        var img = document.getElementById("videoStream");
        img.src = "http://192.168.43.249:81/stream";
        InitWebSocket()
        function InitWebSocket() {
            websock = new WebSocket('ws://' + window.location.hostname + ':81/');
            websock.onmessage = function (evt) {
                JSONobj = JSON.parse(evt.data);
                var msgdata=JSONobj.msg;
                document.getElementById('temp').innerHTML = JSONobj.temperature;
                document.getElementById('humidity').innerHTML = JSONobj.humidity;
                document.getElementById('msg').innerHTML = msgdata;
                document.getElementById('mgf').innerHTML = JSONobj.mgf;
                document.getElementById('air').innerHTML = JSONobj.air;
                if (JSONobj.air == "GOOD") {
                    document.getElementById('air').style.color = "green";
                } else {
                    document.getElementById('air').style.color = "red";
                }
                document.getElementById('don').innerHTML = JSONobj.don;
                if (JSONobj.don == "NIGHT") {
                    document.getElementById('don').style.color = "black";
                } else {
                    document.getElementById('don').style.color = "green";
                }
                if(msgdata=="AUTO MODE"){
                  document.getElementById("control").style.visibility = "hidden";
                }else{
                  document.getElementById("control").style.visibility = "visible";
                }




            }
        }
        //-------------------------------------------------------------
        function toggleCheckbox(action) {
            websock.send(action);

            // if (action === "auto") {
            //     if (document.getElementById("control").style.visibility == "visible") {
            //         document.getElementById("control").style.visibility = "hidden";
            //     } else {
            //         document.getElementById("control").style.visibility = "visible";
            //     }
            // }
        }



    </script>
</body>

</html>
)=====";