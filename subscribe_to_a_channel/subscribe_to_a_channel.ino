#include "arduino_secrets.h"
//#include <MqttClient.h>#include <HCSR04.h>#include <MKRGSM.h>#include <ThingSpeak.h>#include <MQTT.h>#include <MQTTClient.h>#include <PubSubClient.h>#include <SPI.h>//The sensitive data are set in the in the arduino_secrets.h tabconst char PINNUMBER[]     = SECRET_PIN;// APN dataconst char GPRS_APN[]      = SECRET_GPRS_APN;const char GPRS_LOGIN[]    = SECRET_GPRS_LOGIN;const char GPRS_PASSWORD[] = SECRET_GPRS_PASS;// Initialize the library instance//GSMSSLClient net; GSMClient client;//Client service for TCP connectionGPRS gprs; //GPRS accessGSM gsmAccess; //GSM access: include a 'true' parameter for debug enabledchar mqttUserName[] = "gsm2";char mqttPass[] = "NJOXL2SQHURNCH8Q";char writeAPIKey[] = "L5SYQNCEAMKWJ5HK";char readAPIKey[] = "Y0DQ8NS2S1P4UC29";long channelID = 1352833;long readChannelID = 1352833;PubSubClient mqttClient(client);String subscribeTopic = "channels/" + String(channelID) + "/subscribe/fields/field1";String MQTTAPIKey = "NJOXL2SQHURNCH8Q";void callback (char* topic, byte* payload, unsigned int length);//mqttClient( "mqtt.thingspeak.com", 1883, callback);int mqttSubscriptionCallback(char* topic, byte* payload, unsigned int length);int mqttSubscribe(long subChannelID, int field, char* readKey, int unSub);//Messages relayed from the MQTT broker invoke the callback function to handle the messageconst char* server = "mqtt.thingspeak.com";unsigned long lastConnectionTime = 0;const unsigned long postingInterval = 20L * 1000L;static const char alphanum[] ="0123456789""ABCDEFGHIJKLMNOPQRSTUVWXYZ""abcdefghijklmnopqrstuvwxyz"; // For random generation of client ID.//#include "ThingSpeak.h"//unsigned long myChannelNumber =x;//const char * myWriteAPIKey = "x";#define echoPIN 7 #define trigPIN 8long duration;int distance;void setup() {Serial.begin(115200);mqttClient.setServer(server, 1883);mqttClient.setCallback(callback); // set the MQTT message handler function//each time the main loop executes, check to see if data from the MQTT subscription is available for processing.//ThingSpeak.begin(client);pinMode(trigPIN,OUTPUT);  pinMode(echoPIN,INPUT);  while (!Serial) {    ; // wait for serial port to connect. Needed for Leonardo native USB port only  }    Serial.println("Starting Arduino web client.");  boolean connected = false;  // wait 10 seconds for connection:  delay(10000);    while (!connected) {    if ((gsmAccess.begin(PINNUMBER) == GSM_READY) &&      (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)) {      connected = true;    } else {      Serial.println("Not connected");      delay(1000);    }  }  Serial.println("connected");    ThingSpeak.begin(client);  // Initialize ThingSpeak }//mqttClient.setServer(server, 1883);void loop() {     long duration, inches, cm;     digitalWrite(trigPIN, LOW);     delayMicroseconds(2);     digitalWrite(trigPIN, HIGH);     delayMicroseconds(10);     digitalWrite(trigPIN, LOW);     duration = pulseIn(echoPIN, HIGH);     inches = microsecondsToInches(duration);     cm = microsecondsToCentimeters(duration);     distance = duration*(0.034/2);     //return distance;     Serial.print(distance);     delay(2000);              if (!mqttClient.connected()){reconnect();if (mqttSubscribe( readChannelID,1,readAPIKey,0)==1){    Serial.println("subscribed");  }}mqttClient.loop(); //call the loop to maintain connection to the serverif (millis() - lastConnectionTime > postingInterval){mqttpublish();}  }void reconnect(){while (!mqttClient.connected()){Serial.print("Attempting MQTT connection...");char clientID[9];// Generate ClientIDfor (int i = 0; i < 8; i++) {clientID[i] = alphanum[random(51)];}clientID[8]='\0';        // Connect to the ThingSpeak MQTT broker.  //char clientID[]="x";if (mqttClient.connect(clientID,mqttUserName,mqttPass)){Serial.print("Connected with Client ID: ");Serial.print(String(clientID));Serial.print(", Username: ");Serial.print(mqttUserName);Serial.print(" , Password: ");Serial.println(mqttPass);} else{Serial.print("failed, rc=");Serial.print(mqttClient.state());Serial.println(" try again in 5 seconds");delay(5000);}} //sendDistance(cm);}long microsecondsToInches(long microseconds) { // According to Parallax's datasheet for the PING))), there are  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per  // second).  This gives the distance travelled by the ping, outbound  // and return, so we divide by 2 to get the distance of the obstacle.  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf  return microseconds / 74 / 2;}long microsecondsToCentimeters(long microseconds) {  // The speed of sound is 340 m/s or 29 microseconds per centimeter.  // The ping travels out and back, so to find the distance of the  // object we take half of the distance travelled.  return microseconds / 29 / 2;}/*void mqttpublish() {//create a function (sendDistance) to send data collected to thingspeak IoT cloud platform. void sendDistance(float cm)  float cm;int sensor_analog;sensor_analog = analogRead(sensor_pin);moisture_percentage = ( 100 - ( (sensor_analog/1023.00) * 100 ) );float t = dht.getTemperature();float h = dht.getHumidity();// Create data string to send to ThingSpeakString data = String("field1=" + String(distance, DEC) + "&field2=" + String(t, DEC) + "&field3=" + String(h, DEC));int length = data.length();char msgBuffer[length];data.toCharArray(msgBuffer,length+1);Serial.println(msgBuffer);// Create a topic string and publish data to ThingSpeak channel feed.String topicString ="channels/" + String( channelID ) + "/publish/"+String(writeAPIKey);length=topicString.length();char topicBuffer[length];topicString.toCharArray(topicBuffer,length+1);mqttClient.publish( topicBuffer, msgBuffer );lastConnectionTime = millis();}*/// Use this function instead to publish to a single field directly. Don't forget to comment out the above version.void mqttpublish() {  String data = String(distance, DEC);int length = data.length();char msgBuffer[length];data.toCharArray(msgBuffer,length+1);Serial.println(msgBuffer);// Create a topic string and publish data to ThingSpeak channel feed.String topicString ="channels/" + String( channelID ) + "/publish/fields/field1/"+String(writeAPIKey);length=topicString.length();char topicBuffer[length];topicString.toCharArray(topicBuffer,length+1);mqttClient.publish( topicBuffer, msgBuffer );lastConnectionTime = millis();}//This function processes the message relayed by the MQTT brokervoid callback(char* topic, byte* payload, unsigned int length) {  char p[ length+1]; //leave an extra space to null terminate the string.  memcpy( p, payload, length);  p[ length ] = NULL; //Terminate the string  Serial.print( "Answer: ");  Serial.println( String(p)); }  /** * Process messages received from subscribed channel via MQTT broker. * topic - Subscription topic for message * payload - Field to subscribe to. Value 0 means subscribe to all fields. * mesLength - Message length. */ /**  * Subscribe to fields of achannel  * subChannelID - Channel to subscribe to  * field - Field to subscribe to. Value 0 means subscribe to all fields.  * readKey - Read API Key for the subscribe channel.  * unSub - Set to 1 for unsubscribe.  */  int mqttSubscribe( long subChannelID, int field, char* readKey, int unsubSub){  String myTopic;      //There is no field zero. So if field 0 is sent to subscribe to, then subscribe to the whole channel feed.  if (field=0){    myTopic = "channels/" + String(subChannelID)+ "/subscribe/json/" + String(readKey);  }   else {    myTopic = "channels/" + String(subChannelID)+ "/subscribe/fields/field"+ String(field) + "/" + String(readKey);  }  Serial.println("Subscribing to" +myTopic);  Serial.println("State=" + String(mqttClient.state()));   char charBuf[myTopic.length()+1];  myTopic.toCharArray(charBuf,myTopic.length()+1);  if (unsubSub==1) {    return mqttClient.unsubscribe(myTopic.c_str());  }    return mqttClient.subscribe(myTopic.c_str(), 0);    return mqttClient.subscribe(charBuf);  }  /*int mqttSubscribe(long subChannelID,int field,char* readKey){String myTopic="channels/"+String(subChannelID)+"/subscribe/fields/field"+String(field)+"/"+String(readKey);Serial.println("Subscribed to " +myTopic);Serial.print("State= " + String(mqttClient.state()));char charBuf[myTopic.length()+1];myTopic.toCharArray(charBuf,myTopic.length()+1);return mqttClient.subscribe(charBuf);}*/