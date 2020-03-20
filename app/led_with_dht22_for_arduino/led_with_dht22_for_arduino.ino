// includeing needy librarys 
#include <Arduino_JSON.h>               // arduino json library
#include <ESP8266WiFi.h>                // arduino wifi library  for esp8266 bourds
#include <ESP8266WebServer.h>           // arduino webserver wifi library for esp8266 bourds
#include <ESP8266HTTPClient.h>          // arduino http connection library
#include <SoftwareSerial.h>             // arduino virtual serial pin and cominucation library
#include <ESP8266mDNS.h>                // arduino mdns library for running local dns server for virtual domin
#include "DHT.h"                        // arduino dht temperature and humadity sensor library

//some define for bourds and stuff conected to it
#define ledr    14                      // red led conected to (D5) 
#define ledw    12                      // white led conected to (D6)
#define DHTPin    13                    // dht22 conected to (D7)
#define DHTTYPE DHT22                   // DHT 22 (AM2302), AM2321

uint8_t ledy = D1;                      // yellow led conected to the (D1)
uint8_t pirSensor = D0;                 // TODO: connect this pins and devices to esp8266 and upload this sketch into it then make comment for it.
uint8_t relayInput = D2;                // TODO: connect this pins and devices to esp8266 and upload this sketch into it then make comment for it.

DHT dht(DHTPin, DHTTYPE);               // Initialize DHT sensor.       

float Temperature;                      // temperature variable for storing room temperature
float Humidity;                         // Humidity variable for storing room Humidity
int pirsensorValue = 0;                 // pirsensorValue variable for storing human motion detect outside

MDNSResponder mdns;                     // make mdns local server for reserveing domin for given IP

// Configuration parameters for Access Point
char * ssid_ap = "mouse station";       // its main server ssid (name) for makeing connection on to it
char * password_ap = "mgstudio884";     // its main server password for makeing connectiion on to it.
int max_connection = 8;                 // its number of client for that can makeconnection to main server 

//=============================== variable for makeing http conection ================================

int    httpCode;
String URL;
String payload;
String ID;
String state;

//======================== variable for for storing diferent stuff statuss ===========================
String statusre = "off";
String statusye = "off";
String statuswe = "off";
String statusrelaye = "off";
String status_pir = "no";
String statusr = "off";
String statusy = "off";
String statusw = "off";
String statusrelay = "off";

// Set up the server object
ESP8266WebServer server(80);            // start server at 10.10.10.1 IP at port : 80
HTTPClient http;                        // start http client and server object

//========== Check if header is present and correct and client is authentified or no ==================

bool is_authentified() {
  Serial.println("Enter is authentified");
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      Serial.println("Authentification Successful");
      return true;
    }
  }
  Serial.println("Authentification Failed");
  return false;
}

//===============================================dafine the IP Address of the hotspot==================
IPAddress ip (10, 10, 10, 1);
IPAddress gateway (10, 10, 10, 1);
IPAddress subnet(255,255,255,0);

// Keep track of the sensor data that's going to be sent by the client

void setup() {
  
  dht.begin();                            // start dht temperature and humadity measerments
  Serial.begin(115200);                   // start serial cominucation for monitoring datas
  pinMode(DHTPin, INPUT);                 // start dht pin as input
  pinMode(pirSensor, INPUT);              // start pir pin as input
  pinMode(ledr, OUTPUT);                  // start led red pin as output
  pinMode(ledw, OUTPUT);                  // start led white pin as output
  pinMode(ledy, OUTPUT);                  // start led yellow pin as output
  pinMode(relayInput, OUTPUT);            // start relay1 pin as output
  digitalWrite(relayInput, HIGH);         // turn off the relay1
  digitalWrite(ledr, HIGH);               // turn off the red led
  digitalWrite(ledw, HIGH);               // turn off the white led
  digitalWrite(ledy, HIGH);               // turn off the yellow led

  WiFi.mode(WIFI_AP);                                                             // start wifi as server and client mode
  Serial.println("\nSetup Access point");
  Serial.println("Disconnect from any other modes");
  WiFi.disconnect(); // WiFi.softAPdisconnect();                                  // disconnect all old wifi from server
  WiFi.softAP(ssid_ap, password_ap, 1, false, max_connection);                    // set wifi from given settings
  WiFi.softAPConfig(ip, gateway, subnet);                                         // set up server to start in given settings
  
  // Getting and Printing IP Address as a sanity check
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("IP Address: ");Serial.println(myIP);  
  
  // Configure the server's routes
  server.on("/", handleroot);
  server.on("/login", handlelogin);
  server.on("/readroomddata", handle_read_room_d_data);
  server.on("/control_led", handlecontrolled);
  server.on("/control_relay", handlecontrolrelay);
  server.on("/readpirdata", handle_read_pir_data);    
  server.onNotFound(handleNotFound);

  // route for checking server is running or not and this handle client route dont need login 
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works without need of authentification");
  });
  
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent", "Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  
  //ask server to track these headers and collect them in variables
  server.collectHeaders(headerkeys, headerkeyssize );
  
  //starting server ...
  server.begin();
  Serial.println("HTTP server started");
  
}

void loop() {

  server.handleClient();                                  // make server to handle all requests comeing from the clients
  read_pir_Sensor();                                      // read pir sensor status 
}

void handlecontrolrelay(){
  /* this function handle controlling 3 status led for showing pysically all results and dont need login */ 
  
  statusrelay = server.arg("relay1");                                   // getting the values that given to relay1 via http requst that come from client

  // start checking that must turn on relay or not
  if (statusrelay == "on"){
    digitalWrite(relayInput,LOW);
    statusrelaye = "on";
  }
  
  if (statusrelay == "off"){
    digitalWrite(relayInput,HIGH);
    statusrelaye = "off";
  }
  // end checking that must turn on relay or not

  JSONVar relay_status_Array;                                                // make json array for sendeng relay1 status via json type
  
  relay_status_Array["relay_Status"] = statusrelaye;                         // adding values to made json array

  String json_relay_status_String = JSON.stringify(relay_status_Array);      // make final json file on to strig and prepairing for send
  
  server.send(200, "text/html", json_relay_status_String);                   // send response to requests comming for this route accros to made json array
}

void handle_read_pir_data() {
  /* this function handle reading pir status and send respons for client that ask for this route and dont need login */

  Serial.println("string value of pir sensor is:"+status_pir);                // monitoring pir status that is detect human motion or not 
  
  JSONVar pir_status_Array;                                                   // make json array for sendeng pir status via json type
  
  pir_status_Array["motion"] = status_pir;                                    // adding values to made json array

  String json_pir_status_String = JSON.stringify(pir_status_Array);           // make final json file on to strig and prepairing for send
  
  server.send(200, "text/html", json_pir_status_String);                      // send response to requests comming for this route accros to made json array
  
}

void read_pir_Sensor() {
  /* this function start reading pir sensor status and store them into variables */  

  pirsensorValue = digitalRead(pirSensor);                  // read the pir sensor value on/off[]

  // start storing datas from declare pir status  
  if (pirsensorValue == 1){
      status_pir = "yes";
  }
  else{
      status_pir = "no";  
  }
  // end storing datas from declare pir status
}

void handleroot() {
  /* this function handle main showeing gui page for just small check from datas and need login at start */

  Serial.println("Enter handleRoot");                      // monitoring for start this route 
  
  readSensor();                                            // reading dht22 sensor status
  String header;                                           // make header for seting cookies at first time for just one time login
  
  // start checking that your authentified or no if you did not authentified its return you to login page
  if (!is_authentified()) {
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  }
  // end checking that your authentified or no if you did not authentified its return you to login page

  // start makeing html for showing in "/" route of server with sensor and status variable that stored in other parts of this sketch
  String content = "<head><title>fast review</title></head><body style='background: #80c6f7'><h1 align ='center'><b><u><i><strong>Mouse House</strong></i></u></b></h1><br>";
  content += "<br><p>led red is : " ;                       // this is datail for showing status of red led
  content +=  String(statusre) + "</p>";                    // showing red led status after details
  content += "<br><p>led yellow is : " ;                    // this is datail for showing status of yellow led
  content +=  String(statusye) + "</p>";                    // showing yellow led status after details
  content += "<br><p>led white  is : " ;                    // this is datail for showing status of white led
  content +=  String(statuswe) + "</p>";                    // showing white led status after details
  content += "<br><p>room temperature is : ";               // this is datail for showing temperature of the mouse room
  content +=  String(Temperature) + "</p>";                 // showing value of temperature after details
  content += "<br><p>room humadity is : ";                  // this is datail for showing humadity of the mouse room
  content +=  String(Humidity) + "</p>";                    // showing value of humadity after details
  content += "<br><p>relay1  is : ";                        // this is datail for showing status of relay1
  content +=  String(statusrelaye) + "</p>";                // showing relay1 status after details
  content += "<br><p>human motion detect : ";               // this is datail for showing status of pir human motion detect
  content +=  String(status_pir) + "</p>";
  content += "<br><p><marquee direction='right'>Developed by ::  MOHAMMAD GHAREHBAGH ::</marquee></p>";            
  content += "<br><br> <footer><p>Posted by: mohammad gharehbagh</p><p><a href=\"mailto:mgstudio884@gmail.com\">mgstudio884@gmail.com</a>.</p></footer></body>";
  // end makeing html for showing in "/" route of server with sensor and status variable that stored in other parts of this sketch

  // start checking the cookies that collected from the user
  if (server.hasHeader("User-Agent")) {
    content += "the user agent used is : " + server.header("User-Agent") + "<br><br>";
  }
  // end checking the cookies that collected from the user

  // make button for disconnecting from server and after it login again
  content += "You can access this page until you <a href=\"/login?DISCONNECT=YES\">disconnect</a></body></html>";
  server.send(200, "text/html", content);                   // sending response for clients that send request for this route
}

void handlecontrolled(){
  /* this function handle controlling status leds bars via client requests and dont need login */
  
  statusr = server.arg("ledred");                     // getting the values that given to red led via http requst that come from client
  statusy = server.arg("ledyellow");                  // getting the values that given to yellow led via http requst that come from client
  statusw = server.arg("ledwhite");                   // getting the values that given to white led via http requst that come from client

  // start checking wich led must turn on or not via get variable that collected from client request   
  if (statusr == "on"){
    digitalWrite(ledr,LOW);
    statusre = "on";
  }
  
  if (statusr == "off"){
    digitalWrite(ledr,HIGH);
    statusre = "off";
  }

  if (statusy == "on"){
    digitalWrite(ledy,LOW);
    statusye = "on";
  }
  
  if (statusy == "off"){
    digitalWrite(ledy,HIGH);
    statusye = "off";
  }

  if (statusw == "on"){
    digitalWrite(ledw,LOW);
    statuswe = "on";
  }
  
  if (statusw == "off"){
    digitalWrite(ledw,HIGH);
    statuswe = "off";  
  }  
  // end checking wich led must turn on or not via get variable that collected from client request

  JSONVar led_status_Array;                                                 // make json array for sendeng leds status via json type
  
  led_status_Array["redled"] = statusre;                                    // adding wanted value to made json array
  led_status_Array["whiteled"] = statuswe;                                  // adding wanted value to made json array
  led_status_Array["yellowled"] = statusye;                                 // adding wanted value to made json array

  String json_led_status_String = JSON.stringify(led_status_Array);         // make final json file on to strig and prepairing for send
  
  server.send(200, "text/html", json_led_status_String);                    // send response to requests comming for this route accros to made json array

}

void handle_read_room_d_data(){
  /* this function handle read temperature and humadity dynamic datas that stored in other parts of this sketch and dont need login */

  Serial.println("Enter handle_read_room_d_data");    
  readSensor();
  
  JSONVar room_status_Array;
  
  room_status_Array["temperature"] = Temperature;
  room_status_Array["humadity"] = Humidity;

  String json_room_status_String = JSON.stringify(room_status_Array);
  
  server.send(200, "text/html", json_room_status_String);

}

void readSensor()
{
  delay(1000);  
  Temperature = dht.readTemperature(); // Gets the values of the temperature
  Humidity = dht.readHumidity(); // Gets the values of the humidity
  
  if (isnan(Temperature) || isnan(Humidity)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  
  Serial.println("room temperature is : "+String(Temperature));
  Serial.println("room humadity is : "+String(Humidity));
}

//login page, also called for disconnect
void handlelogin() {
  String msg;
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }
  if (server.hasArg("DISCONNECT")) {
    Serial.println("Disconnection");
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Set-Cookie", "ESPSESSIONID=0");
    server.send(301);
    return;
  }
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")) {
    if (server.arg("USERNAME") == "mgmgst" &&  server.arg("PASSWORD") == "1051154731" ) // enter ur username and password you want
    {
      server.sendHeader("Location", "/");
      server.sendHeader("Cache-Control", "no-cache");
      server.sendHeader("Set-Cookie", "ESPSESSIONID=1");
      server.send(301);
      Serial.println("Log in Successful");
      return;

    }

    msg = "<center><p>!!! Wrong username/password! try again. !!!</p></center>";
    Serial.println("Log in Failed");
  }
  String content = "<html><body style='background-color:MediumAquaMarine'><form action='/login' method='POST'><p  align ='center' style='font-size:300%;'><u><b><i>  Log In  </i></b></u></p><br>";
  content += " <p   align ='center' style='font-size:160%'><b> UserName:<input type='text' name='USERNAME' placeholder='user name' required></b></p><br>";
  content += "<p  align ='center' style='font-size:160%'><b>Password:<input type='password' name='PASSWORD' placeholder='password' required></b></p><br>";
  content += "<p  align ='center' style='font-size:160%'><input type='submit' name='SUBMIT' value='Submit'></form>" + msg + "</p><br></body></html>";
  server.send(200, "text/html", content);
}

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