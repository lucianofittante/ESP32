

      var botongraficas = document.querySelector(".graf");
      var botonred = document.querySelector(".red");
      var botonconfig = document.querySelector(".conf");

      botongraficas.addEventListener("click",function(event){
        event.preventDefault();
        
        console.log("graficas");

      });
      botonred.addEventListener("click",function(event){
        event.preventDefault();
        
        console.log("red");

      });
      botonconfig.addEventListener("click",function(event){
        event.preventDefault();
        
        console.log("config");

    });


    setInterval(updateValues, 500, "luz");
    setInterval(updateValues, 10000, "temperature");
    setInterval(updateValues, 10000, "humidity");
    setInterval(updateValues, 10000, "humidityland");


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




