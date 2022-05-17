#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Ticker.h>
#include <DHT.h>
#include <Adafruit_SSD1306.h>	   	// libreria para controlador SSD1306
#include <Adafruit_Sensor.h>
#include <EEPROM.h>
#include <ArduinoJson.h>



//defines the I2C pins to which the display is connected
#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_RESET 4			// necesario por la libreria pero no usad

#define DHTPIN 4  
#define EEPROM_SIZE 9


#define ANCHO 128			// reemplaza ocurrencia de ANCHO por 128
#define ALTO 64				// reemplaza ocurrencia de ALTO por 64



AsyncWebServer server(80);
Adafruit_SSD1306 display(ANCHO, ALTO, &Wire, -1);

//WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP, "3.south-america.pool.ntp.org", 19800, 60000); 


const char *ssid = "Guille2.4";
const char *password = "Guille2360";


IPAddress ip(192,168,1,4);     
IPAddress gateway(192,168,1,1);   
IPAddress subnet(255,255,255,0); 

//conexiones 

const int SENSORSUELO1=34;
const int SENSORSUELO2=35;
const int BOMBA= 32;
const int VENTILACION=33;
const int LUZ=25;
const int EXTRACTOR= 26;
const int HUMIFICADOR=19;


int h;
int t;


// Declaraciones objetos
  
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

Ticker temp;

// declaraciones de variables para traspaso al web server 

String ledState;
String temperature;
String humidity;
String tempMaxidiurna;
String tempMinidiurna;
String tempMaxiNocturna;
String tempMiniNocturna;
String humidityMax;
String humidityMin;
String timeON;
String hours;
String minute;
String humidityground;
String humiditygroundMax;
String humiditygroundMin;
String days;

////////////////////////////VARIABLES DE LAS GRAFICAS////////////////////////

StaticJsonDocument<2048> G;


///// Direcciones de la EEPROM

const int DIRTEMPERATURAMAXIMANOCTURNA=0;
const int DIRTEMPERATURAMINIMADIURNA=1;
const int DIRTEMPERATURAMINIMANOCTURNA=2;
const int DIRTEMPERATURAMAXIMADIURNA=3;
const int DIRHUMEDADMAXIMA=4;
const int DIRHUMEDADMINIMA=5;
const int DIRTIEMPOENCENDIDO=6;
const int DIRHUEMDADMAXIMASUELO=7;
const int DIRHUEMDADMINIMASUELO=8;


// parametros de confiuguracion

unsigned int temperaturaMaximaNocturna;
unsigned int temperaturaMinimaNocturna;
unsigned int temperaturaMaximaDiurna;
unsigned int temperaturaMinimaDiurna;
unsigned int humedadMaxima;
unsigned int humedadMinima;
unsigned int humedadMaximaSuelo;
unsigned int humedadMinimaSuelo;
unsigned int dias = 0;


unsigned int tiempoEncendido;
unsigned int tiempoApagado ;


// variables

int segundos;
int minutos;
int horas;

int segundosregado;
int riegos;
int segundosdepantalla;
int pantalla;
int minutosregado;
int humedadSuelo1;
int humedadSuelo2;
int humedadSueloProm;


boolean bomba;
boolean regando;
boolean conteo=false;
boolean encender;
boolean flora;


////////////////////////////////DECLARACION PARA EL JSON/////////////////////7


////////////////////////////FUNCIONES DE CONFIUGURACION////////////////////

void guardarValoresdeGraficas(){

// VALORES DE LA TEMPERTURA
  G["t0"] = 16;
  G["t1"] = 15;
  G["t2"] = 12;
  G["t3"] = 15;
  G["t4"] = 12;
  G["t5"] = 15;
  G["t6"] = 13;
  G["t7"] = 15;
  G["t8"] = 12;
  G["t9"] = 15;
  G["t10"] = 14;
  G["t11"] = 15;
  G["t12"] = 13;
  G["t13"] = 15;
  G["t14"] = 12;
  G["t15"] = 15;
  G["t16"] = 12;
  G["t17"] = 15;
  G["t18"] = 12;
  G["t19"] = 15;
  G["t20"] = 13;
  G["t21"] = 15;
  G["t22"] = 12;
  G["t23"] = 15;

// VALORES DE LA HUMEDAD

  G["h0"] = 21;
  G["h1"] = 25;
  G["h2"] = 22;
  G["h3"] = 25;
  G["h4"] = 22;
  G["h5"] = 25;
  G["h6"] = 23;
  G["h7"] = 25;
  G["h8"] = 22;
  G["h9"] = 25;
  G["h10"] = 24;
  G["h11"] = 25;
  G["h12"] = 23;
  G["h13"] = 25;
  G["h14"] = 22;
  G["h15"] = 25;
  G["h16"] = 22;
  G["h17"] = 25;
  G["h18"] = 22;
  G["h19"] = 25;
  G["h20"] = 23;
  G["h21"] = 25;
  G["h22"] = 22;
  G["h23"] = 25;
  
  // VALORES DE LA HUMEDAD DEL SUELO 1
  
  G["hs0"] = 22;
  G["hs1"] = 25;
  G["hs2"] = 22;
  G["hs3"] = 25;
  G["hs4"] = 22;
  G["hs5"] = 25;
  G["hs6"] = 23;
  G["hs7"] = 25;
  G["hs8"] = 22;
  G["hs9"] = 25;
  G["hs10"] = 24;
  G["hs11"] = 25;
  G["hs12"] = 23;
  G["hs13"] = 25;
  G["hs14"] = 22;
  G["hs15"] = 25;
  G["hs16"] = 22;
  G["hs17"] = 25;
  G["hs18"] = 22;
  G["hs19"] = 25;
  G["hs20"] = 23;
  G["hs21"] = 25;
  G["hs22"] = 22;
  G["hs23"] = 25;

}

void graficar(){

File dt = SPIFFS.open("/data.txt", "w");

if (!dt) {
Serial.println("Fallo apertura de archivo"); 
}

Serial.println("====== Escribiendo en el archivo SPIFFS =========");
// write 10 strings to file
serializeJson(G, dt);

serializeJson(G, Serial);

Serial.println();

dt.close();

}


void guardarConfiguracion(){

         EEPROM.write(DIRTEMPERATURAMAXIMADIURNA, temperaturaMaximaDiurna);
         EEPROM.write(DIRTEMPERATURAMINIMADIURNA, temperaturaMinimaDiurna);
         EEPROM.write(DIRTEMPERATURAMAXIMANOCTURNA,temperaturaMaximaNocturna);
         EEPROM.write(DIRTEMPERATURAMINIMANOCTURNA, temperaturaMinimaNocturna);
         EEPROM.write(DIRHUMEDADMAXIMA,humedadMaxima);
         EEPROM.write(DIRHUMEDADMINIMA, humedadMinima);
         EEPROM.write(DIRHUEMDADMAXIMASUELO,humedadMaximaSuelo);
         EEPROM.write(DIRHUEMDADMINIMASUELO, humedadMinimaSuelo);
         EEPROM.write(DIRTIEMPOENCENDIDO, tiempoEncendido);
  
         EEPROM.commit();

        Serial.println("se guardaron los caretas");

        delay(400);

}
/*
void tiempoActual(){

  display.clearDisplay();		            	
  display.setTextColor(WHITE);	            
  display.setCursor(40, 0);			            
  display.setTextSize(2);		              	
  display.print("HORA"); 
  display.setCursor (15, 30);	                	
  display.setTextSize(2);		  	 	      
  display.print(timeClient.getFormattedTime());	

  horas=timeClient.getHours();
  minutos=timeClient.getMinutes();
  segundos=timeClient.getMinutes();
  

  display.display();

}
*/
//////////////FUNCIONES //////////////////////////////////////////

//procesador

// funcion para poner el dato en HTML

String processor(const String& var){

Serial.println(var);
  if(var == "STATE"){
    if(digitalRead(bomba)){
      ledState = "ON";
    }
    else{
      ledState = "OFF";
    }
    return ledState;
  }
   if(var == "TEMPERATURE"){
    return temperature;
  }
  else if (var == "ENCENDIDOLUZ"){
    return  timeON;
  }
  else if(var == "HUMIDITY"){
    return humidity;
  }
  else if (var == "TEMPMAXIMADIURNA"){
    return  tempMaxidiurna;
  }
  else if (var == "TEMPMINIMADIURNA"){
    return  tempMinidiurna;
  }
   else if (var == "TEMPMINIMANOCTURNA"){
    return  tempMiniNocturna;
  }
  else if (var == "TEMPMAXIMANOCTURNA"){
    return  tempMaxiNocturna;
  }
  else if (var == "HUMEDADMINIMA"){
    return  humidityMin;
  }
  else if (var == "HUMEDADMAXIMA"){
    return  humidityMax;
  }
  else if (var == "HUMIDITYLAND"){
    return  humidityground;
  }
  else if (var == "HUMEDADMINIMASUELO"){
    return  humiditygroundMin;
  }
  else if (var == "HUMEDADMAXIMASUELO"){
    return  humiditygroundMax;
  }
  else if (var == "HORAS"){
    return  hours;
  }
  else if (var == "MINUTOS"){
    return  minute;
  }
  else if (var == "DIAS"){
    return  days;
  }
  return String();
}

void verificarHumedad(){

     humedadSueloProm  = ((analogRead(SENSORSUELO1))/40);


    humidityground= humedadSueloProm;

    if(humedadSueloProm < humedadMinimaSuelo){
    regando=true;
    digitalWrite(BOMBA, HIGH);
    }
    if(humedadSueloProm >= humedadMinimaSuelo){
    regando=false;
    digitalWrite(BOMBA, LOW);
    segundosregado=0;
    minutosregado=0;    
    }


// ESTE CODIGO SE USARIA SI TENEMOS DOS SENSORES DE HUMEDA DE SUELO. 

   /*
    humedadSuelo1  = ((analogRead(SENSORSUELO1))/40);
    humedadSuelo2  = ((analogRead(SENSORSUELO2))/40);

    humedadSueloProm = ((humedadSuelo1 + humedadSuelo2)/2);

    humidityground= humedadSueloProm;

    if(humedadSueloProm < humedadMinimaSuelo){
    regando=true;
    digitalWrite(BOMBA, HIGH);
    }
    if(humedadSueloProm >= humedadMinimaSuelo){
    regando=false;
    digitalWrite(BOMBA, LOW);
    segundosregado=0;
    minutosregado=0;    
    }
      */
}


void regar(){

  display.clearDisplay();		                	// limpia pantalla
  display.setTextColor(WHITE);	               	// establece color al unico disponible (pantalla monocromo)
  display.setCursor(15, 0);		            	// ubica cursor en inicio de coordenadas 0,0
  display.setTextSize(2);		                	// establece tamano de texto en 1
  display.print("Regando");                       	// escribe en pantalla el texto

  if(minutosregado>=10){display.setCursor (15,30);}
  if(minutosregado<10){display.setCursor (33,30);}
  display.setTextSize(3);			                   // establece tamano de texto en 2
  display.print(minutosregado);		

  display.setCursor (50,30);	                	// ubica cursor en coordenas 10,30
  display.setTextSize(3);		                  	// establece tamano de texto en 2
  display.print(":");	

  if(segundosregado>=10){display.setCursor (65,30);}
  if(segundosregado<10){display.setCursor (75,30);}

  display.setTextSize(3);		              	// establece tamano de texto en 2
  display.print(segundosregado);        		// escribe valor de millis() dividido por 1000
  display.display();		                	// muestra en pantalla todo lo establecido anteriormente

}


void muestraHumyTemp(){
   
   h = dht.readHumidity();

   t = dht.readTemperature();

   temperature = t;
   humidity= h;
   
  display.clearDisplay();		              	// limpia pantalla
  display.setTextColor(WHITE);	        	// establece color al unico disponible (pantalla monocromo)
  display.setCursor(10, 0);		                 	// ubica cursor en inicio de coordenadas 0,0
  display.setTextSize(2);		                    	// establece tamano de texto en 1
  display.print("TEMP/HUM");            	// escribe en pantalla el texto
  display.setCursor (23, 30);	                	// ubica cursor en coordenas 10,30
  display.setTextSize(2);		              	// establece tamano de texto en 2
  display.print(t);	                  	// escribe valor de millis() dividido por 1000
  display.print("C");		                  	// escribe texto
  display.setCursor (67, 30);	        	// ubica cursor en coordenas 10,30
  display.setTextSize(2);		            	// establece tamano de texto en 2
  display.print(h);	                         	// escribe valor de millis() dividido por 1000
  display.print("%");
  display.display();		              	// muestra en pantalla todo lo establecido anteriormente

}

void mostrarTiempo(){

  display.clearDisplay();		            	
  display.setTextColor(WHITE);	            
  display.setCursor(5, 0);			            
  display.setTextSize(2);		              	
  display.print("FLORACION"); 
  display.setCursor (30, 20);	                	
  display.setTextSize(2);		  	 	      
  display.print(tiempoEncendido);	
  display.setCursor (60, 20);	                	
  display.setTextSize(2);
  display.print("ON");	           	                	
  display.setCursor (30, 40);	             	 
  display.setTextSize(2);		          	 
  display.print(tiempoApagado);	         
  display.setCursor (60, 40);	             	 
  display.setTextSize(2);		          	
  display.print("OFF");
  display.display();	
}

void tiempoON(){

  display.clearDisplay();		                	// limpia pantalla
  display.setTextColor(WHITE);	            	// establece color al unico disponible (pantalla monocromo)
  display.setCursor(5, 0);		                 	// ubica cursor en inicio de coordenadas 0,0
  display.setTextSize(2);		              	// establece tamano de texto en 1
  display.print("TIEMPO ON"); 	                  // escribe en pantalla el texto

  if(horas>=10){display.setCursor (10,30);}
  if(horas<10){display.setCursor (28,30);}
  display.setTextSize(3);		                  	// establece tamano de texto en 2
  display.print(horas);		

  display.setCursor (45,30);	                   	// ubica cursor en coordenas 10,30
  display.setTextSize(3);			                     // establece tamano de texto en 2
  display.print(":");	

  if(minutos>=10){display.setCursor (60,30);}
  if(minutos<10){display.setCursor (70,30);}

  display.setTextSize(3);		                    	// establece tamano de texto en 2
  display.print(minutos);	                          	// escribe valor de millis() dividido por 1000


  display.setCursor (105,30);	                     	// ubica cursor en coordenas 10,30
  display.setTextSize(1);		                      	// establece tamano de texto en 
  display.print(segundos);	                    	// escribe valor de millis() dividido por 100
  display.display();			                            // muestra en pantalla todo lo establecido anteriormente

}

void mostrarTiempoOFF(){

  display.clearDisplay();			// limpia pantalla
  display.setTextColor(WHITE);		// establece color al unico disponible (pantalla monocromo)
  display.setCursor(20, 0);			// ubica cursor en inicio de coordenadas 0,0
  display.setTextSize(2);			// establece tamano de texto en 1
  display.print("LUZ OFF"); 	// escribe en pantalla el texto

  if(horas>=10){display.setCursor (10,30);}
  if(horas<10){display.setCursor (28,30);}
  display.setTextSize(3);			// establece tamano de texto en 2
  display.print(horas);		

  display.setCursor (45,30);		// ubica cursor en coordenas 10,30
  display.setTextSize(3);			// establece tamano de texto en 2
  display.print(":");	

  if(minutos>=10){display.setCursor (60,30);}
  if(minutos<10){display.setCursor (70,30);}

  display.setTextSize(3);			// establece tamano de texto en 2
  display.print(minutos);		// escribe valor de millis() dividido por 1000


  display.setCursor (105,30);		// ubica cursor en coordenas 10,30
  display.setTextSize(1);			// establece tamano de texto en 
  display.print(segundos);	    	// escribe valor de millis() dividido por 100
  display.display();			       // muestra en pantalla todo lo establecido anteriormente

}


void muestraHuSuelo(){
   
  display.clearDisplay();		            	
  display.setTextColor(WHITE);	            	
  display.setCursor(5, 0);			             
  display.setTextSize(2);		              	
  display.print("HUM SUELO"); 


  display.setCursor (20, 30);	                	
  display.setTextSize(2);		  	 	      
  display.print("H1: ");	
  
  display.setCursor (60, 30);	                	
  display.setTextSize(2);		  	 	      
  display.print(humedadSuelo1);	
  display.print("%");
  	          	                	
  /*
  display.setCursor (20,40);	                	
  display.setTextSize(2);		  	 	      
  display.print("H2: ");

  display.setCursor (60,40);	                	
  display.setTextSize(2);		  	 	      
  display.print(humedadSuelo2);	
  display.print("%");	 
  */           

  display.display();	        	        	
     	        	
}

void funciontiempo(void){

    hours = horas;
    minute = minutos;
    days = dias;

  if(segundos==59){ 
      minutos++;
      if((minutos==59) && (segundos=59)){horas++;}
    }
               
  if(segundos==59){segundos=0;}
  if(minutos==60){minutos=0;}
  if(horas==24){horas=0;regando=0;}

  if(segundosregado==59){ 
    minutosregado++;
    }

  segundos++;
  segundosregado++;

  if(segundosregado==59){segundosregado=0;}
  if(minutosregado==59){minutosregado=0;}
  
  segundosdepantalla++;

  if(segundosdepantalla==5){pantalla++;segundosdepantalla=0;}

  if(pantalla>2){pantalla=0;}

  if((minutos==59) && (segundos==59) && (horas==59)){dias++;}

}

void leerConfiguracion(){

  temperaturaMaximaDiurna = EEPROM.read(DIRTEMPERATURAMAXIMADIURNA);
  temperaturaMinimaDiurna = EEPROM.read(DIRTEMPERATURAMINIMADIURNA);
  temperaturaMaximaNocturna = EEPROM.read(DIRTEMPERATURAMAXIMANOCTURNA);
  temperaturaMinimaNocturna = EEPROM.read(DIRTEMPERATURAMINIMANOCTURNA);
  humedadMaxima = EEPROM.read(DIRHUMEDADMAXIMA);
  humedadMinima = EEPROM.read(DIRHUMEDADMINIMA);
  humedadMaximaSuelo = EEPROM.read(DIRHUEMDADMAXIMASUELO);
  humedadMinimaSuelo = EEPROM.read(DIRHUEMDADMINIMASUELO);
  tiempoEncendido = EEPROM.read(DIRTIEMPOENCENDIDO);


  tiempoApagado = (24 - tiempoEncendido);


   tempMaxidiurna = temperaturaMaximaDiurna;
   tempMinidiurna = temperaturaMinimaDiurna;
   tempMaxiNocturna = temperaturaMaximaNocturna;
   tempMiniNocturna = temperaturaMinimaNocturna;
   humidityMax = humedadMaxima;
   humidityMin = humedadMinima;
   humiditygroundMax = humedadMaximaSuelo;
   humiditygroundMin = humedadMinimaSuelo;
   timeON = tiempoEncendido;

  Serial.println("se leyeron los carretas");
  delay(200);

}

void setup()   {
  
  dht.begin();
  Wire.begin(OLED_SDA,OLED_SCL);
  Serial.begin(115200);

  //define the type of display used and the I2C address
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  pinMode(BOMBA, OUTPUT);
  pinMode(LUZ, OUTPUT);
  pinMode(VENTILACION, OUTPUT);
  pinMode(EXTRACTOR, OUTPUT);
  pinMode(HUMIFICADOR, OUTPUT);

  EEPROM.begin(EEPROM_SIZE);

  leerConfiguracion();

  temp.attach(0.5,funciontiempo);

   // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet);
  

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html",String(), false, processor);
  });
  //  OTRAS RUTAS DE ACCESO EN LA WEB
    server.on("/styleindex.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/styleindex.css");
  });
    server.on("/reset.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/reset.css");
  });

  // envio de imagenes//
  
    server.on("/image/chalas.jpg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/image/chalas.jpg");
  });
    server.on("/image/configurar.jpg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/image/configurar.jpg");
  });
    server.on("/image/red.jpg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/image/red.jpg");
  });
    server.on("/image/graficas.jpg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/image/graficas.jpg");
  });
  
    server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/config.html", String(), false, processor);
    leerConfiguracion();
  });
   server.on("/grafica", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/graficas.html", String(), false, processor);
  });
  server.on("/data.txt", HTTP_GET, [](AsyncWebServerRequest *request){
   request->send(SPIFFS, "/data.txt", String(), false, processor);
  });
  server.on("/data.json", HTTP_GET, [](AsyncWebServerRequest *request){
   request->send(SPIFFS, "/data.json");
  });
  // PASO DE VARIABLES
    server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", temperature.c_str());
  });
    server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", humidity.c_str());
  });
    server.on("/tempMaxidiurna", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain",tempMaxidiurna.c_str()); 
  });
    server.on("/tempMinidiurna", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain",tempMinidiurna.c_str());
  });
    server.on("/tempMaxinocturna", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain",tempMaxiNocturna.c_str());
  });
    server.on("/tempMininocturna", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain",tempMiniNocturna.c_str());
  });
    server.on("/humidityMax", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain",humidityMax.c_str());
  });
    server.on("/humidityMin", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain",humidityMin.c_str());
  });
    server.on("/humiditygroundMax", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain",humiditygroundMax.c_str());
  });
    server.on("/humiditygroundMin", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain",humiditygroundMin.c_str());
  });
   server.on("/humidityground", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain",humidityground.c_str());
  });
    server.on("/encendidoLuz", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain",timeON.c_str());
  });
   server.on("/horas", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain",hours.c_str());
  });
   server.on("/minutos", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain",minute.c_str());
  });
    server.on("/dias", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain",days.c_str());
  });

  // PARAMETROS DE CONFIGURACION BOTONES SUBIR Y BAJAR Y GUARDAR
   server.on("/bajarEncendido", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/config.html", String(), false, processor);
    tiempoEncendido--;
    timeON = tiempoEncendido;
  });
   server.on("/SubirEncendido", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/config.html", String(), false, processor);
    tiempoEncendido++;
    timeON = tiempoEncendido;
  });
  server.on("/SubirTempDiurnaMax", HTTP_GET, [](AsyncWebServerRequest *request){
    temperaturaMaximaDiurna++;  
    tempMaxidiurna = temperaturaMaximaDiurna; 
    request->send(SPIFFS, "/config.html", String(), false, processor);
  });
   server.on("/bajarTempDiurnaMax", HTTP_GET, [](AsyncWebServerRequest *request){
    temperaturaMaximaDiurna--;   
    tempMaxidiurna= temperaturaMaximaDiurna; 
    request->send(SPIFFS, "/config.html", String(), false, processor);
  });
  server.on("/SubirTempDiurnaMin", HTTP_GET, [](AsyncWebServerRequest *request){
    temperaturaMinimaDiurna++;  
    tempMinidiurna = temperaturaMinimaDiurna;
    request->send(SPIFFS, "/config.html", String(), false, processor);
  });
   server.on("/bajarTempDiurnaMin", HTTP_GET, [](AsyncWebServerRequest *request){
    temperaturaMinimaDiurna--;   
    tempMinidiurna = temperaturaMinimaDiurna; 
    request->send(SPIFFS, "/config.html", String(), false, processor);
  });
   server.on("/SubirTempNocturnaMax", HTTP_GET, [](AsyncWebServerRequest *request){
    temperaturaMaximaNocturna++;  
    tempMaxiNocturna = temperaturaMaximaNocturna;
    request->send(SPIFFS, "/config.html", String(), false, processor);
  });
   server.on("/bajarTempNocturnaMax", HTTP_GET, [](AsyncWebServerRequest *request){
    temperaturaMaximaNocturna--;  
    tempMaxiNocturna= temperaturaMaximaNocturna;; 
    request->send(SPIFFS, "/config.html", String(), false, processor);
  });
   server.on("/SubirTempNocturnaMin", HTTP_GET, [](AsyncWebServerRequest *request){
    temperaturaMinimaNocturna++;  
    tempMiniNocturna = temperaturaMinimaNocturna;
    request->send(SPIFFS, "/config.html", String(), false, processor);
  });
   server.on("/bajarTempNocturnaMin", HTTP_GET, [](AsyncWebServerRequest *request){
    temperaturaMinimaNocturna--;  
    tempMiniNocturna=temperaturaMinimaNocturna;; 
    request->send(SPIFFS, "/config.html", String(), false, processor);
  });
   server.on("/bajarHumedadMax", HTTP_GET, [](AsyncWebServerRequest *request){
    humedadMaxima--;
    humidityMax = humedadMaxima;
    request->send(SPIFFS, "/config.html", String(), false, processor);
  });
   server.on("/SubirHumedadMax", HTTP_GET, [](AsyncWebServerRequest *request){
    humedadMaxima++;  
    humidityMax = humedadMaxima; 
    request->send(SPIFFS, "/config.html", String(), false, processor);
  });
   server.on("/bajarHumedadMin", HTTP_GET, [](AsyncWebServerRequest *request){
    humedadMinima--;
    humidityMin =  humedadMinima;
    request->send(SPIFFS, "/config.html", String(), false, processor);
  });
   server.on("/SubirHumedadMin", HTTP_GET, [](AsyncWebServerRequest *request){
    humedadMinima++;  
    humidityMin = humedadMinima; 
    request->send(SPIFFS, "/config.html", String(), false, processor);
  });
   server.on("/bajarHumedadSueloMax", HTTP_GET, [](AsyncWebServerRequest *request){
    humedadMaximaSuelo--;  
    humiditygroundMax = humedadMaximaSuelo;
    request->send(SPIFFS, "/config.html", String(), false, processor);
  });
   server.on("/subirHumedadSueloMax", HTTP_GET, [](AsyncWebServerRequest *request){
    humedadMaximaSuelo++;  
    humiditygroundMax = humedadMaximaSuelo; 
    request->send(SPIFFS, "/config.html", String(), false, processor);
  });
    server.on("/subirHumedadSueloMin", HTTP_GET, [](AsyncWebServerRequest *request){
    humedadMinimaSuelo++;
    humiditygroundMin =  humedadMinimaSuelo;
    request->send(SPIFFS, "/config.html", String(), false, processor);
  });
   server.on("/bajarHumedadSueloMin", HTTP_GET, [](AsyncWebServerRequest *request){
    humedadMinimaSuelo--;  
    humiditygroundMin = humedadMinimaSuelo; 
    request->send(SPIFFS, "/config.html", String(), false, processor);
  });
   server.on("/iniciar", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
    server.on("/guardar", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/config.html", String(), false, processor);
    guardarConfiguracion();
  });
    server.on("/pass", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/pass.html", String(), false, processor);
  });
    server.on("/volver", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
    WiFi.begin(ssid, password);
  });
    server.on("/winter", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/winter.jpg", "image/jpg");
  });

  //////////// carga de parametros de la contraseña y el nombre de la red/////////////

  server.begin();


   //// reloj///////

   //timeClient.begin();

   //timeClient.setTimeOffset(61226);


  }

void loop() {

         if(horas<tiempoEncendido){
           
           digitalWrite(LUZ,HIGH);

           if(pantalla==0){tiempoON();}
           if(pantalla==1){muestraHumyTemp();}
           if(pantalla==2){muestraHuSuelo();}
           if(pantalla==3){}


         }



         if(horas>=tiempoEncendido){
           
          digitalWrite(LUZ,LOW);

          if(regando==true){
            verificarHumedad();
            regar();
            conteo=true;
            }

          if(regando==false){
              verificarHumedad();
              if(conteo==true && regando==false){riegos++;}
              conteo=false;

              if(pantalla==0){tiempoON();}
              if(pantalla==1){muestraHumyTemp();}
              if(pantalla==2){muestraHuSuelo();}
              if(pantalla==3){}

            }

         }
          
       }




