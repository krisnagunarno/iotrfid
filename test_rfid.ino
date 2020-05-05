#include <BluetoothSerial.h>
#include <SPI.h>
#include <MFRC522.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

//setting timer kirim data
unsigned long previousMillis = 0;    // Stores last time temperature was published
const long interval = 5000;

 
//#define RST_PIN 9 // Configurable, see typical pin layout above
//#define SS_PIN 10 // Configurable, see typical pin layout above
const int RST_PIN = 22; // Reset pin
const int SS_PIN = 21; // Slave select pin
const int LED1 = 5;
const int LED2 = 15;

//data terima
String pesan="";
 
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

 
void setup() {
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  Serial.begin(115200); // Initialize serial communications with the PC
  while (!Serial); // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin(); // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522
  mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details

  SerialBT.begin("PintuRFID"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");

}
 
void loop() {

//BAGIAN RFID
// Look for new cards
if ( ! mfrc522.PICC_IsNewCardPresent()) {
  return;
}
 
// Select one of the cards
if ( ! mfrc522.PICC_ReadCardSerial()) {
  return;
}
 
// Dump debug info about the card; PICC_HaltA() is automatically called
//mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

String uid;
String temp;
for(int i=0;i<4;i++){
  if(mfrc522.uid.uidByte[i]<0x10){
    temp = "0" + String(mfrc522.uid.uidByte[i],HEX);
  }
  else temp = String(mfrc522.uid.uidByte[i],HEX);
  
  if(i==3){
    uid =  uid + temp;
  }
  else uid =  uid + temp+ " ";
}
Serial.println("UID "+uid);
String grantedAccess = "09 C2 C3 7E"; //Akses via RFID yang ditunjuk
grantedAccess.toLowerCase();

//BAGIAN BLUETOOTH
  if (SerialBT.available()){
    char pesanMasuk = SerialBT.read();
    if (pesanMasuk != '\n'){
      pesan += String(pesanMasuk);}
    else{
      pesan = "";}
    Serial.write(pesanMasuk);  
  }


//HASIL DARI BAGIAN RFID DAN BLUETOOTH MENENTUKAN AKSES BOLEH DIBERIKAN ATAU TIDAK
  if (uid == grantedAccess || pesan == "1") {
     digitalWrite(LED1,HIGH);
     digitalWrite(LED2,LOW);
     SerialBT.println("UID "+uid);
     SerialBT.println("Access Granted");
  }
  else{
    digitalWrite(LED1,LOW);
    digitalWrite(LED2,HIGH);
    SerialBT.println("UID "+uid);
    SerialBT.println("Access Denied");
  }
  Serial.println("\n");
  pesan = "";
  SerialBT.flush();
  mfrc522.PICC_HaltA();
  delay(3000);
  digitalWrite(LED1,LOW);
  digitalWrite(LED2,LOW);

}
