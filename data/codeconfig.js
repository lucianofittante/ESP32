

    var botonguardar = document.querySelector('.boton');
    var tiempoencendido = document.querySelector('#tiempoencendido');
    var temperaturamaxima = document.querySelector('#temperaturamaxima'); 
    var temperaturaminima = document.querySelector('#temperaturaminima');
    var humedadminima = document.querySelector('#humedadminima');
    var humedadmaxima = document.querySelector('#humedadmaxima');
    var humedadmaximasuelo = document.querySelector('#humedadmaximasuelo');
    var humedadminimasuelo = document.querySelector('#humedadminimasuelo');   



    botonguardar.addEventListener("click",function(event){
        event.preventDefault();

        if((tiempoencendido.value<10) && (temperaturamaxima.value<10) && (temperaturaminima.value<10) && (humedadmaxima.value<10) && (humedadminima.value<10) && (humedadmaximasuelo.value<10) && (humedadminimasuelo.value<10)){

            alert("Los valores deben ser mayores a 10 y menores a 100");
        }

        else{
        var ladata = tiempoencendido.value + temperaturamaxima.value + temperaturaminima.value + humedadmaxima.value + humedadminima.value + humedadmaximasuelo.value + humedadminimasuelo.value;
        }

        console.log(ladata);

        });


      function updateValues(value) { 
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            document.getElementById(value).innerHTML = this.responseText;
          }
        };
        xhttp.open("GET", "/ladata" + value, true);
        xhttp.send();
      }

  