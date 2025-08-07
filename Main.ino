#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#define LED_BUILTIN 02
// TFT display pin definitions
#define TFT_CS 05
#define TFT_RST 22
#define TFT_DC 21
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_BACKLIGHT 19

#define pot1_PIN 25
#define pot2_PIN 26

#define motA_PIN 14
#define motB_PIN 12

#define probe1_PIN 32
#define probe2_PIN 35
#define probe3_PIN 34

#define Node1_PIN 25
#define Node2_PIN 26
#define Node3_PIN 27

#define valve1_PIN 4
#define valve1_PIN 4
#define valve2_PIN 16
#define valve3_PIN 17
#define pump_PIN 15
#define waterDetetectThreshold 1000

#define LED_BUILTIN 02
uint8_t node1Address[] = {0xBC, 0xDD, 0xC2, 0x5B, 0x9D, 0xE2};
uint8_t node2Address[] = {0xDC, 0x4F, 0x22, 0x53, 0x71, 0xF1};
uint8_t node3Address[] = {0x98, 0xF4, 0xAB, 0xBE, 0xB3, 0x33};

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);



//Servo servo1;
typedef struct tx_message {
    char character[100];
  //  Unsigned long Current time;// this is used to synchronise
    //all controllers in terms of timer
    int ServoValue;
    bool channelActive;// this signifies to the recipeint so.
    //that transmission can be validated and debug able 
    bool zeroScale;// flag to signal to the 
    bool getWeight;// flag to signal to the 
    //recipient for data request 
} struct_txMessage;

typedef struct rx_message {
    int weight;
    bool channelActive;// this signifies to the recipeint so.
    int nodeNo;// to identify Node
    //recipient for data request 
} struct_rxMessage;

unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 5000;           // interval at which to perform next operation
struct_rxMessage messageRx;
struct_txMessage messageTx;

void data_receive(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&messageRx, incomingData, sizeof(messageRx));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("weight : ");
  Serial.println(messageRx.weight);
  Serial.print("Bool channel active: ");
  Serial.println(messageRx.channelActive);
  Serial.print("node number : ");
  Serial.println(messageRx.nodeNo);
  
  Serial.println(); 
}
 void data_sent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nStatus of Last Message Sent:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
// Initialize TFT display
  tft.initR(INITR_BLACKTAB);
  tft.setSPISpeed(1000000); // Set SPI speed to 1 MHz
  tft.fillScreen(ST7735_BLACK);

  // Turn on the backlight
  pinMode(TFT_BACKLIGHT, OUTPUT);
  digitalWrite(TFT_BACKLIGHT, HIGH);  

  pinMode(LED_BUILTIN, OUTPUT);
  //servo1.attach(servoPin);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
   esp_now_register_send_cb(data_sent);
  
  esp_now_peer_info_t peerInfo;
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  memcpy(peerInfo.peer_addr, node1Address, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add node1 peer");
    return;
  }
  memcpy(peerInfo.peer_addr, node2Address, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add node2 peer");
    return;
  }
  memcpy(peerInfo.peer_addr, node3Address, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add node3 peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(esp_now_recv_cb_t(data_receive));
}
 
void loop() {
  delay(15); 
  unsigned long currentMillis = millis();
static int nodeCount=0;
  if (currentMillis - previousMillis >= interval) {
   fillWater(nodeCount++);
    previousMillis = currentMillis;

static int countt=0;
 countt++;
  
  }  
}

void speakToNode(int nodeNum){
  if(nodeNum==1){
    esp_err_t outcome = esp_now_send(node1Address, (uint8_t *) &messageTx, sizeof(messageTx));
  if (outcome == ESP_OK) {
    Serial.println("Mesage sent successfully!");
  }
  else {
    Serial.println("Error sending the message");
  }
  }
  else  if(nodeNum==2){
    esp_err_t outcome = esp_now_send(node2Address, (uint8_t *) &messageTx, sizeof(messageTx));
  if (outcome == ESP_OK) {
    Serial.println("Mesage sent successfully!");
  }
  else {
    Serial.println("Error sending the message");
  }
  }
   else  if(nodeNum==3){
    esp_err_t outcome = esp_now_send(node3Address, (uint8_t *) &messageTx, sizeof(messageTx));
  if (outcome == ESP_OK) {
    Serial.println("Mesage sent successfully!");
  }
  else {
    Serial.println("Error sending the message");
  }
  }
}

void fillWater(int nodeNum){
 
 if(nodeNum==1){
   digitalWrite(valve1_PIN, 1);
   delay(200);
  digitalWrite(pump_PIN, 1);
while(analogRead(probe1_PIN) > waterDetetectThreshold);// the value drops in presence of water 
 digitalWrite(valve1_PIN, 0);
  digitalWrite(pump_PIN, 0);
 } 
 else if(nodeNum==2){
   digitalWrite(valve2_PIN, 1);
   delay(200);
  digitalWrite(pump_PIN, 1);
while(analogRead(probe1_PIN) > waterDetetectThreshold);// the value drops in presence of water 
 digitalWrite(valve1_PIN, 0);
  digitalWrite(pump_PIN, 0);
 } 
  else if(nodeNum==3){
   digitalWrite(valve2_PIN, 1);
   delay(200);
  digitalWrite(pump_PIN, 1);
while(analogRead(probe1_PIN) > waterDetetectThreshold);// the value drops in presence of water 
 digitalWrite(valve1_PIN, 0);
  digitalWrite(pump_PIN, 0);
 } 
}

/* typedef struct tx_message {
    char character[100];
  //  Unsigned long Current time;// this is used to synchronise
    //all controllers in terms of timer
    int ServoValue;
    bool channelActive;// this signifies to the recipeint so.
    //that transmission can be validated and debug able 
    bool zeroScale;// flag to signal to the 
    bool getWeight;// flag to signal to the 
    //recipient for data request 
} struct_txMessage;
*/
void openNode(int nodeNum){
  messageTx.ServoValue = 135;
  speakToNode(nodeNum);
}


void closeNode(int nodeNum){
  messageTx.ServoValue = 55;
  speakToNode(nodeNum);
}
void zeroScale(int nodeNum){
  messageTx.zeroScale = 1;// 0 or 1
  speakToNode(nodeNum);
  messageTx.zeroScale = 0;// reset value back to 0

}