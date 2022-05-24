

    var botonguardar = document.querySelector('.boton');
    var tiempoencendido =document.querySelector('#tiempoencendido');
    var temperaturamaxima =document.querySelector('#temperaturamaxima'); 
    var temperaturaminima =document.querySelector('#temperaturaminima');
    var huemadadminima =document.querySelector('#huemedadminima');  


    botonguardar.addEventListener("click",function(event){
        event.preventDefault();
        
      
        let data = {

            tenc: (tiempoencendido.value).toString(),
            tmax: (temperaturamaxima.value).toString(),
            tmin: (temperaturaminima.value).toString(),
            hmin: (huemadadminima.value).toString(),

        }   

        console.log(data);

      });
