#include <SPI.h>
#include <LoRa.h>
#include <EEPROM.h>

//define the pins used by the transceiver module
// Arduino Uno
#define ss 10
#define rst 9
#define dio0 2 //*/

/*// ESP32
#define ss 5
#define rst 14
#define dio0 2 //*/

#define EEPROM_SIZE 1 // ESP32 max 512, Arduino Uno max 1024
#define EEPROM_ADDRESS_NODEID 0 // tempat penyimpanan NODEID

int nodeId;
int counter, alert_status = 0;

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);
  // EEPROM.begin(EEPROM_SIZE); // gunakan untuk ESP32

  lora_setup();
}

void lora_setup() {
  Serial.println("=== LoRa Receiver ===");
  
  // setup module LoRa
  LoRa.setPins(ss, rst, dio0);
  
  //Gantikan LoRa.begin(---E-) dengan argumen sesuai frekuensi lokasi
  //433E6 Asia
  //866E6 Europe
  //915E6 North America
  while (!LoRa.begin(915E6)) {
    Serial.println(".");
    delay(500);
  }
  
  // Ubah sync word menjadi (0xF3) untuk menyamakan dengan Receiver
  // Sync word memastikan tidak akan menangkap atau mengirimkan LoRa messages ke LoRa transceivers lainnya
  // dengan range dari 0-0xFF
  LoRa.setSyncWord(0xF0); // (apa ini?)
  Serial.println("...LoRa Initializing OK!");

  // cari tau nodeId diri sendiri
  nodeId = (int) EEPROM.read(EEPROM_ADDRESS_NODEID);
  Serial.print("nodeId at EEPROM address '" + (String) EEPROM_ADDRESS_NODEID + "' is: ");
  Serial.println((String) nodeId);
}

void send_packet() {
  Serial.print("nodeId " + (String) nodeId + " Sending datagram: ");
  String datagramString = (String) nodeId + "," + "0" + "," + (String) alert_status + "," + (String) counter + "," + (String) counter + ";";
  Serial.println(datagramString);

  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print(datagramString);
  LoRa.endPacket();
}

void retrieve_packet() {
  // received a packet
  Serial.print("nodeId " + (String) nodeId + " Received datagram: '");

  // read packet
  while (LoRa.available()) {
    // masukkan ke dalam format      
    String datagramString = LoRa.readString();
    Serial.print(datagramString); 
  }

  // print RSSI of packet
  Serial.print("' with RSSI ");
  Serial.println(LoRa.packetRssi());
}

void loop() {
  // try to parse packet  
  int packetSize = LoRa.parsePacket(); // watchdog
  if (packetSize) {
    retrieve_packet();

    // send packet
    for (int i=0; i<10; i++) {
      send_packet();
      delay(10);
    }
  }

  counter++;
}
