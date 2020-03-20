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
uint8_t pirSensor = D2;                 // pir sensor conected to the (D2)
uint8_t relayInput1 = D0;               // relay1 conected to the (D0)
uint8_t relayInput2 = D3;               // relay2 conected to the (D3)
uint8_t relayInput3 = D4;               // relay3 conected to the (D4)

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
String msg;

//======================== variable for for storing diferent stuff statuss ===========================
String statusre = "off";
String statusye = "off";
String statuswe = "off";
String statusrelaye1 = "off";
String statusrelaye2 = "off";
String statusrelaye3 = "off";
String status_pir = "no";
String statusr = "off";
String statusy = "off";
String statusw = "off";
String statusrelay1 = "off";
String statusrelay2 = "off";
String statusrelay3 = "off";
String allleds = "off";

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
  pinMode(relayInput1, OUTPUT);           // start relay1 pin as output
  pinMode(relayInput2, OUTPUT);           // start relay2 pin as output
  pinMode(relayInput3, OUTPUT);           // start relay3 pin as output    
  digitalWrite(relayInput1, HIGH);        // turn off the relay1
  digitalWrite(relayInput2, HIGH);        // turn off the relay2
  digitalWrite(relayInput3, HIGH);        // turn off the relay3    
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
    Serial.println("device work properly");
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

  Serial.println("Enter handle control relay");                          // monitoring for start this route
  statusrelay1 = server.arg("relay1");                                   // getting the values that given to relay1 via http requst that come from client
  statusrelay2 = server.arg("relay2");                                   // getting the values that given to relay1 via http requst that come from client
  statusrelay3 = server.arg("relay3");                                   // getting the values that given to relay1 via http requst that come from client    

  // start checking that must turn on relay or not
  if (statusrelay1 == "on"){
    digitalWrite(relayInput1,LOW);
    statusrelaye1 = "on";
  }
  
  if (statusrelay1 == "off"){
    digitalWrite(relayInput1,HIGH);
    statusrelaye1 = "off";
  }
  if (statusrelay2 == "on"){
    digitalWrite(relayInput2,LOW);
    statusrelaye2 = "on";
  }
  
  if (statusrelay2 == "off"){
    digitalWrite(relayInput2,HIGH);
    statusrelaye2 = "off";
  }
  if (statusrelay3 == "on"){
    digitalWrite(relayInput3,LOW);
    statusrelaye3 = "on";
  }
  
  if (statusrelay3 == "off"){
    digitalWrite(relayInput3,HIGH);
    statusrelaye3 = "off";
  }    
  // end checking that must turn on relay or not

  JSONVar relays_status_Array;                                                // make json array for sendeng relay1 status via json type
  
  relays_status_Array["relay1_Status"] = statusrelaye1;                         // adding values to made json array
  relays_status_Array["relay2_Status"] = statusrelaye2;                         // adding values to made json array
  relays_status_Array["relay3_Status"] = statusrelaye3;                         // adding values to made json array
    
  String json_relays_status_String = JSON.stringify(relays_status_Array);      // make final json file on to strig and prepairing for send
  
  server.send(200, "text/html", json_relays_status_String);                   // send response to requests comming for this route accros to made json array
  Serial.println("sending the response for handle control relay");                                        // monitoring for response to this route
}

void handle_read_pir_data() {
  /* this function handle reading pir status and send respons for client that ask for this route and dont need login */

  Serial.println("Enter handle read pir data");                                         // monitoring for start this route
  Serial.println("string value of pir sensor is:"+status_pir);                // monitoring pir status that is detect human motion or not 
  
  JSONVar pir_status_Array;                                                   // make json array for sendeng pir status via json type
  
  pir_status_Array["motion"] = status_pir;                                    // adding values to made json array

  String json_pir_status_String = JSON.stringify(pir_status_Array);           // make final json file on to strig and prepairing for send
  
  server.send(200, "text/html", json_pir_status_String);                      // send response to requests comming for this route accros to made json array
  Serial.println("sending response for handle read pir data");                // monitoring for response to this route
  
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
  content += "<br><p>light  is : ";                        // this is datail for showing status of relay1
  content +=  String(statusrelaye1) + "</p>";                // showing relay1 status after details
  content += "<br><p>fan(in)  is : ";                        // this is datail for showing status of relay1
  content +=  String(statusrelaye2) + "</p>";                // showing relay1 status after details
  content += "<br><p>fan(out) : ";                        // this is datail for showing status of relay1
  content +=  String(statusrelaye3) + "</p>";                // showing relay1 status after details    
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
  Serial.println("sending response for handle root request");                      // monitoring for response to this route
}

void handlecontrolled(){
  /* this function handle controlling status leds bars via client requests and dont need login */

  Serial.println("Enter handle control leds");                      // monitoring for start this route
  statusr = server.arg("ledred");                     // getting the values that given to red led via http requst that come from client
  statusy = server.arg("ledyellow");                  // getting the values that given to yellow led via http requst that come from client
  statusw = server.arg("ledwhite");                   // getting the values that given to white led via http requst that come from client
  allleds = server.arg("all");                        // getting the values that given to white led via http requst that come from client
  
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
  
  if (allleds == "on"){
    digitalWrite(ledw,LOW);
    digitalWrite(ledr,LOW);
    digitalWrite(ledy,LOW);
    statuswe = "on";
    statusye = "on";
    statusre = "on";                  
  }
  
  if (allleds == "off"){
    digitalWrite(ledw,HIGH);
    digitalWrite(ledr,HIGH);
    digitalWrite(ledy,HIGH);
    statuswe = "off";
    statusye = "off";
    statusre = "off";                  
  }  
  // end checking wich led must turn on or not via get variable that collected from client request

  Serial.println("start makeing leds on and of across to the post methed comeing from client");                      // monitoring for leds statuss
  
  JSONVar led_status_Array;                                                 // make json array for sendeng leds status via json type
  
  led_status_Array["redled"] = statusre;                                    // adding wanted value to made json array
  led_status_Array["whiteled"] = statuswe;                                  // adding wanted value to made json array
  led_status_Array["yellowled"] = statusye;                                 // adding wanted value to made json array

  String json_led_status_String = JSON.stringify(led_status_Array);         // make final json file on to strig and prepairing for send
  
  server.send(200, "text/html", json_led_status_String);                    // send response to requests comming for this route accros to made json array
  Serial.println("sendeing response for handle control leds by json");                      // monitoring for response to this route

}

void handle_read_room_d_data(){
  /* this function handle read temperature and humadity dynamic datas that stored in other parts of this sketch and dont need login */

  Serial.println("Enter handle_read_room_d_data");                                                // monitoring for start this route
  Serial.println("start reading dht22 sensor values");                                            // start reading dht22 sensor status
  
  readSensor();                                                                                   // reading DHT22 sensor temperarture and humadity values
  
  JSONVar room_status_Array;                                                                      // make json array for sendeng leds status via json type
  
  room_status_Array["temperature"] = Temperature;                                                 // adding wanted value to made json array
  room_status_Array["humadity"] = Humidity;                                                       // adding wanted value to made json array

  String json_room_status_String = JSON.stringify(room_status_Array);                             // make final json file on to strig and prepairing for send
  
  server.send(200, "text/html", json_room_status_String);                                         // send response to requests comming for this route accros to made json array
  Serial.println("sending response for handle read romm dynamic datas");                          // monitoring for response to this route

}

void readSensor(){
  /* this function read dht22 datas from real world and store them into variable */
  
  delay(1000);                                                                  // wait 1 secound beacuse of delay of sht22 sensor
  Temperature = dht.readTemperature();                                          // Gets the values of the temperature
  Humidity = dht.readHumidity();                                                // Gets the values of the humidity

  // start checking that sensor is connected or work well if not monitoring it
  if (isnan(Temperature) || isnan(Humidity)) {
    Serial.println(F("Failed to read from DHT sensor!"));                       // monitoring that dht22 is not connected or have problem if check dht22 sensor become feult
    return;
  }
  // end checking that sensor is connected or work well if not monitoring it
  
  Serial.println("room temperature is : "+String(Temperature));                 // monitoring dht22 temperature
  Serial.println("room humadity is : "+String(Humidity));                       // monitoring dht22 temperature
  Serial.println("End reading datas from the dht22 sensor");                    // monitoring for start this route

}

//login page, and also sometime trigerd for requests comeing for disconnect from the server
void handlelogin() {
  /* this funstion handle login page and send username and pass that collected from the page to server and validate it if atuntication was true enter to main page if not pass */

  Serial.println("Enter handlelogin");                      // monitoring for start this route
  // start checking that person atunticate truely from takeing system cookies from the comeing computer and if you login it set your cookies and save them for next time
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
  // end checking that person atunticate truely from takeing system cookies from the comeing computer and if you login it set your cookies and save them for next time

  Serial.println("start checking given pass and user");                      // monitoring for start checking given pass and user
  
  // start checking username and password that collected from the person if it was true set the cokies if not return login page again
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")) {
    if (server.arg("USERNAME") == "mgmgst" &&  server.arg("PASSWORD") == "1051154731" ) // enter ur username and password you want
    {
      server.sendHeader("Location", "/");
      server.sendHeader("Cache-Control", "no-cache");
      server.sendHeader("Set-Cookie", "ESPSESSIONID=1");
      server.send(301);                                                                        // send http respons (301) code >> atuticate was true
      Serial.println("Log in Successful");                                                     // if athuntication was true monitoring that login was successful
      return;
    }
    msg = "<center><p>!!! Wrong username/password! try again. !!!</p></center>";               // if login was failed send message to the html page as exact given message
    Serial.println("Log in Failed");                                                           // monitoring that login was failed
  }
  // end checking username and password that collected from the person if it was true set the cokies if not return login page again
  
  // start makeing response html page for sendeing response for requests comming from the client for the login route (ask)
  String content = "<html><body style='background-color:MediumAquaMarine'><form action='/login' method='POST'><p  align ='center' style='font-size:300%;'><u><b><i>  Log In  </i></b></u></p><br>";
  content += " <p   align ='center' style='font-size:160%'><b> UserName:<input type='text' name='USERNAME' placeholder='user name' required></b></p><br>";
  content += "<p  align ='center' style='font-size:160%'><b>Password:<input type='password' name='PASSWORD' placeholder='password' required></b></p><br>";
  content += "<p  align ='center' style='font-size:160%'><input type='submit' name='SUBMIT' value='Submit'></form>" + msg + "</p><br></body></html>";
  // end makeing response html page for sendeing response for requests comming from the client for the login route (ask)
  
  server.send(200, "text/html", content);                                                      // sendeing the response for this route   
  Serial.println("sending response for requsts to handle login");                              // monitoring for response to this route
}

void handleNotFound() {
  /* this function handle 404 page if some one request or send somtheing that we dont need */

  Serial.println("enter handlenotdount (404)");                              // monitoring for start this route
  // start makeing response for sending to the requests comeing from the client
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
  // end makeing response for sending to the requests comeing from the client
  
  server.send(404, "text/plain", message);                                                       // sendeing the response for this route
    Serial.println("sending response for requsts to handle not found page (404)");                              // monitoring for response to this route
}
