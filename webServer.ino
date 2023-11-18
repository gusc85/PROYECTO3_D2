#include <WiFi.h> 
#include <WiFiClient.h> 
#include <WebServer.h> 
#include <ESPmDNS.h> 

//Definicion de pines de la pantalla 16x2 (4 bits)
#define RS 25
#define EN 26
#define D4 27
#define D5 14
#define D6 19
#define D7 18

const char* ssid = "Gus"; //Nombre de la red
const char* password = "1234567890"; //Contraseña
char tiva1; //Variable para recibir del serial de la tiva 

WebServer server(80); //Definimos objeto para la librería Webserver en el puerto 80 ya con ese hace el request de http
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7); //Definimos el objeto para la librería LCD y asignamos los pines

//Variables booleanas para los parqueos
bool parqueo1 = false;
bool parqueo2 = false;
bool parqueo3 = false;
bool parqueo4 = false;

//Variables enteras para los totales de los parqueos
uint8_t nivel;
uint8_t total;
uint8_t p1;
uint8_t p2;
uint8_t p3;
uint8_t p4;

//Función para que cuando se este en la pagina principal se ejecute la siguiente función
void handleRoot() {
  server.send(200, "text/html", HTML()); //Código de la página web
}

//Función que llama javascript para que cuando se este en la pagina principal se ejecute la siguiente función
void handleColor1(){
  if (parqueo1 == false) {
    server.send(200, "text/plain", "parking-spot vacant"); //Si el parqueo esta vacío enviar un texto a javascript con la clase de css en HTML para asignarle al id correspondiente
  } else {
    server.send(200, "text/plain", "parking-spot occupied"); //Si el parqueo esta ocupado enviar un texto a javascript con la clase de css en HTML para asignarle al id correspondiente
  }
}

void handleColor2(){
  if (parqueo2 == false) {
    server.send(200, "text/plain", "parking-spot vacant"); //Si el parqueo esta ocupado enviar un texto a javascript con la clase de css en HTML para asignarle al id correspondiente
  } else {
    server.send(200, "text/plain", "parking-spot occupied"); //Si el parqueo esta ocupado enviar un texto a javascript con la clase de css en HTML para asignarle al id correspondiente
  }
}

void handleColor3(){
  if (parqueo3 == false) {
    server.send(200, "text/plain", "parking-spot vacant"); //Si el parqueo esta ocupado enviar un texto a javascript con la clase de css en HTML para asignarle al id correspondiente
  } else {
    server.send(200, "text/plain", "parking-spot occupied"); //Si el parqueo esta ocupado enviar un texto a javascript con la clase de css en HTML para asignarle al id correspondiente
  }
}

void handleColor4(){
  if (parqueo4 == false) {
    server.send(200, "text/plain", "parking-spot vacant"); //Si el parqueo esta ocupado enviar un texto a javascript con la clase de css en HTML para asignarle al id correspondiente
  } else {
    server.send(200, "text/plain", "parking-spot occupied"); //Si el parqueo esta ocupado enviar un texto a javascript con la clase de css en HTML para asignarle al id correspondiente
  }
}

//Funcion a llamar si el cliente escribe una direccion que no existe
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {
  Serial.begin(115200); //Iniciar comunicación serial para ver en la terminal
  Serial1.begin(115200, SERIAL_8N1, 32, 33, false, 20000UL, 112); //Habilitar el UART1 y cambiar los pins
  Serial2.begin(115200); //Habilitar el UART2
  lcd.begin(16, 2); //Setup de la LCD de 16x2
  lcd.setCursor(0,0); //Cursor en 0,0
  lcd.print("Parquematic"); //Escribir parqueomatic
  lcd.setCursor(12,0); //Cursor 12,0
  lcd.print("P:"); //Escribir p de parqueos totales
  lcd.setCursor(0,1); //Cursor en 0,1
  lcd.print("S1:"); //Escribir S1
  lcd.setCursor(8,1); //Cursor 8,1
  lcd.print("S2:"); //Escribir S2
  WiFi.mode(WIFI_STA); //Wifi como station mode para unirse a una red que ya existe
  WiFi.begin(ssid, password); //Iniciar la conexión 
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); //Escribir la IP para acceder a la página

  //Iniciar multi DNS
  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  //Definir las funciones a llamar cuando se haga el request de ciertas direcciones
  server.on("/", handleRoot);
  server.on("/l1", handleColor1);
  server.on("/l2", handleColor2);
  server.on("/l3", handleColor3);
  server.on("/l4", handleColor4);

  server.onNotFound(handleNotFound); //Cuando se pida una direccion inexistente llamar a la funcion indicada

  server.begin(); //Iniciar el servidor
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient(); //Mantener el servidor 
  check_tiva1(); //Llamar a función para revisar los parqueos de la tiva 1
  check(); //Llamar para poder realizar suma de los parqueos
  sotano1 = p1 + p2 + p3 + p4; //Suma de parqueos en sotano 1
  disponibles = p1 + p2 + p3 + p4; //Suma de disponibles totales
  lcd.setCursor(13, 0); //Cursor en 13,0
  lcd.print(String(total)); //Mostrar parqueos totales dispnibles
  lcd.setCursor(3,1); //Cursor en 3,1
  lcd.print(String(nivel)); //Mostrar los parqueos dispnibles en el sotano 1
}

//Código HTML, CSS y Javascript para la página WEB
String HTML(){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<meta charset=\"UTF-8\">\n";
  ptr +="<title>PARQUEOMATIC</title>\n";
  ptr +="  <style>\n";
  ptr +="    /* CSS Styling */\n";
  ptr +="    body {\n";
  ptr +="      font-family: Arial, sans-serif;\n";
  ptr +="      background-color: #333;\n";
  ptr +="      color: #fff;\n";
  ptr +="      margin: 0;\n";
  ptr +="      padding: 20px;\n";
  ptr +="    }\n";
  ptr +="\n";
  ptr +="    h1 {\n";
  ptr +="      text-align: center;\n";
  ptr +="    }\n";
  ptr +="\n";
  ptr +="    .parking-lot {\n";
  ptr +="      display: flex;\n";
  ptr +="      justify-content: center;\n";
  ptr +="      flex-wrap: wrap;\n";
  ptr +="    }\n";
  ptr +="\n";
  ptr +="    .parking-spot {\n";
  ptr +="      width: 150px;\n";
  ptr +="      height: 150px;\n";
  ptr +="      border-radius: 5px;\n";
  ptr +="      margin: 10px;\n";
  ptr +="      display: flex;\n";
  ptr +="      align-items: center;\n";
  ptr +="      justify-content: center;\n";
  ptr +="      font-size: 24px;\n";
  ptr +="      font-weight: bold;\n";
  ptr +="      cursor: pointer;\n";
  ptr +="      transition: background-color 0.3s ease;\n";
  ptr +="    }\n";
  ptr +="\n";
  ptr +="    .vacant {\n";
  ptr +="      background-color: #34c759;\n";
  ptr +="    }\n";
  ptr +="\n";
  ptr +="    .occupied {\n";
  ptr +="      background-color: #ff3b30;\n";
  ptr +="    }\n";
  ptr +="  </style>\n";
  ptr +="</head>\n";
  ptr +="<body onload=\"process()\">\n";
  ptr +="  <h1>PARQUEO-MATIC</h1>\n";
  ptr +="\n";
  ptr +="  <div class=\"parking-lot\">\n";
  ptr +="<div id=\"spot1\" class=\"parking-spot\">Spot 1 &#x1F46E;</div>\n";
  ptr +="<div id=\"spot2\" class=\"parking-spot\">Spot 2 &#x1F46E;</div>\n";
  ptr +="<div id=\"spot3\" class=\"parking-spot\">Spot 3 &#x1F46E;</div>\n";
  ptr +="<div id=\"spot4\" class=\"parking-spot\">Spot 4 &#x1F46E;</div>\n";

  ptr +="  </div>\n";
  ptr +="\n";
  ptr +="<script>\n";
  ptr +="function updateColor1() {\n";
  ptr +="var xhttp = new XMLHttpRequest();\n";
  ptr +="xhttp.onreadystatechange = function() {\n";
  ptr +="if (this.readyState == 4 && this.status == 200) {\n";
  ptr +="document.getElementById('spot1').className = this.responseText;\n";
  ptr +="}\n";
  ptr +="};\n";
  ptr +="xhttp.open('GET', '/l1', true);\n";
  ptr +="xhttp.send();\n";
  ptr +="setTimeout(updateColor1, 500);}\n";

  ptr +="function updateColor2() {\n";
  ptr +="var xhttp = new XMLHttpRequest();\n";
  ptr +="xhttp.onreadystatechange = function() {\n";
  ptr +="if (this.readyState == 4 && this.status == 200) {\n";
  ptr +="document.getElementById('spot2').className = this.responseText;\n";
  ptr +="}\n";
  ptr +="};\n";
  ptr +="xhttp.open('GET', '/l2', true);\n";
  ptr +="xhttp.send();\n";
  ptr +="setTimeout(updateColor2, 500);}\n";

  ptr +="function updateColor3() {\n";
  ptr +="var xhttp = new XMLHttpRequest();\n";
  ptr +="xhttp.onreadystatechange = function() {\n";
  ptr +="if (this.readyState == 4 && this.status == 200) {\n";
  ptr +="document.getElementById('spot3').className = this.responseText;\n";
  ptr +="}\n";
  ptr +="};\n";
  ptr +="xhttp.open('GET', '/l3', true);\n";
  ptr +="xhttp.send();\n";
  ptr +="setTimeout(updateColor3, 500);}\n";

  ptr +="function updateColor4() {\n";
  ptr +="var xhttp = new XMLHttpRequest();\n";
  ptr +="xhttp.onreadystatechange = function() {\n";
  ptr +="if (this.readyState == 4 && this.status == 200) {\n";
  ptr +="document.getElementById('spot4').className = this.responseText;\n";
  ptr +="}\n";
  ptr +="};\n";
  ptr +="xhttp.open('GET', '/l4', true);\n";
  ptr +="xhttp.send();\n";
  ptr +="setTimeout(updateColor4, 500);}\n";

  ptr +="updateColor1();\n";
  ptr +="updateColor2();\n";
  ptr +="updateColor3();\n";
  ptr +="updateColor4();\n";
  ptr +="</script>\n";
  
  ptr +="</body>\n";
  ptr +="</html>";
  return ptr;
}

//Función para chequear la información de la Tiva1
void check_tiva1(){
  tiva1 = Serial2.read(); //Leer de UART2

  //Chequear para los parqueos
  if (tiva1 == 0){
    parqueo1 = false;
    parqueo2 = false;
    parqueo3 = false;
    parqueo4 = false;
  }
  else if (tiva1 == 1){
    parqueo1 = true;
    parqueo2 = false;
    parqueo3 = false;
    parqueo4 = false;
  }
  else if (tiva1 == 2){
    parqueo1 = false;
    parqueo2 = true;
    parqueo3 = false;
    parqueo4 = false;
  }
  else if (tiva1 == 3){
    parqueo1 = true;
    parqueo2 = true;
    parqueo3 = false;
    parqueo4 = false;
  }
  else if (tiva1 == 4){
    parqueo1 = false;
    parqueo2 = false;
    parqueo3 = true;
    parqueo4 = false;
  }
  else if (tiva1 == 5){
    parqueo1 = true;
    parqueo2 = false;
    parqueo3 = true;
    parqueo4 = false;
  }
  else if (tiva1 == 6){
    parqueo1 = false;
    parqueo2 = true;
    parqueo3 = true;
    parqueo4 = false;
  }
  else if (tiva1 == 7){
    parqueo1 = true;
    parqueo2 = true;
    parqueo3 = true;
    parqueo4 = false;
  }
  else if (tiva1 == 8){
    parqueo1 = false;
    parqueo2 = false;
    parqueo3 = false;
    parqueo4 = true;
  }
  else if (tiva1 == 9){
    parqueo1 = true;
    parqueo2 = false;
    parqueo3 = false;
    parqueo4 = true;
  }
  else if (tiva1 == 10){
    parqueo1 = false;
    parqueo2 = true;
    parqueo3 = false;
    parqueo4 = true;
  }
  else if (tiva1 == 11){
    parqueo1 = true;
    parqueo2 = true;
    parqueo3 = false;
    parqueo4 = true;
  }
  else if (tiva1 == 12){
    parqueo1 = false;
    parqueo2 = false;
    parqueo3 = true;
    parqueo4 = true;
  }
  else if (tiva1 == 13){
    parqueo1 = true;
    parqueo2 = false;
    parqueo3 = true;
    parqueo4 = true;
  }
  else if (tiva1 == 14){
    parqueo1 = false;
    parqueo2 = true;
    parqueo3 = true;
    parqueo4 = true;
  }
  else if (tiva1 == 15){
    parqueo1 = true;
    parqueo2 = true;
    parqueo3 = true;
    parqueo4 = true;
  }
}

//Función para las sumas
void check(){
  //Revisar si están ocupados los parqueos
  if (parqueo1 == true){
    p1 = 1;
  }
  else{
    p1 = 0;
  }
  if (parqueo2 == true){
    p2 = 1;
  }
  else{
    p2 = 0;
  }
  if (parqueo3 == true){
    p3 = 1;
  }
  else{
    p3 = 0;
  }
  if (parqueo4 == true){
    p4 = 1;
  }
  else{
    p4 = 0;
  }
}
