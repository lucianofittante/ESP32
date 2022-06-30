

      var botongraficas = document.querySelector(".graf");
      var botonred = document.querySelector(".red");
      var botonconfig = document.querySelector(".conf");

      botongraficas.addEventListener("click",function(event){
        event.preventDefault();
        
        console.log("graficas");

        location.href ="graficas.html" 

      });
      botonred.addEventListener("click",function(event){
        event.preventDefault();
        
        console.log("red");

      });
      botonconfig.addEventListener("click",function(event){
        event.preventDefault();
        
        console.log("config");

        location.href ="configurar.html" 

    });

    setInterval(updateValues, 2000, "luz");
    setInterval(updateValues, 2000, "temperature");
    setInterval(updateValues, 2000, "humidity");
    setInterval(updateValues, 2000, "humidityland");

    function updateValues(value) { 
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById(value).innerHTML = this.responseText;
        }
      };
      xhttp.open("GET", "/" + value, true);
      xhttp.send();
    }




