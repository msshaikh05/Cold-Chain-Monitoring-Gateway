#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

/* ================= WIFI ================= */
const char* ssid = "AirFiber-MS";
const char* password = "rwnt9678";

/* ================= AWS ================= */
#define AWS_IOT_ENDPOINT "a10u7l90ksgr1m-ats.iot.ap-south-1.amazonaws.com"
#define THING_NAME "ColdChain_Dashboard"
#define MQTT_TOPIC "coldchain/data"

/* ================= AWS CERTS ================= */

/* Root CA */
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

/* Device Certificate */
static const char AWS_CERT_CRT[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUWOVAEc3XaLAVZcCxksmLJ+0qoO8wDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI2MDEyNDA5NTIy
N1oXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANjMnF18VaXOPAuFkCBa
Z3Btiz7H0xQ2ehDLnIOauBhFIJExOfsBZf4VGda/PIiD9XeMecLCK+UWlkP8g1oK
bSwKHtRl0/lrUIjNl6iwR3yYQOoAqHMd35Z+L1/7lD+5r34JUyRDXk2qt+3FvPi8
bOwV5LuFFusyPt+W+EGiUpxwq0Gt+zeu0ZZsbsL3E+tswhUzW/ktln3xlXrOZOGO
Vrlc8mvwYlKMAvIRe8FPwy1HOdqI7YDcq2wkkqMi5UuRyz6+0dddfW3Y7w9iahwS
PJst7su2SHShly4UOC8JHHNgZxQepVC6vn/ccesTol/MrNvT0+XRmSTsVS578cSF
f78CAwEAAaNgMF4wHwYDVR0jBBgwFoAUkq/0BP/fIXoDB/LEagTO66fHteQwHQYD
VR0OBBYEFHtU8o08GjDOmg4D0kGaLHrUcghnMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQCRlobCTHdNzc+hNtqEzSTOwu93
SBfhLfQFhvU1Kz03GJMYMx4jZQmBd42cFTgDpxQUNjX5eUBFv1RAH2ewz0uQFI0+
mJYlmUyhYy/g5YwgWstFL+qu2VW+ytF7+vj8vurYqtVWWP8oq15t4FIoqsP/3rbr
1X8TO166UMTzQ52giCl15TODoJ7d90r0SR6w4gtmHfeAhJbfYypR8ZG2uXQZgOpR
gRSuEkyYm6iWlQYnofa3asVpY4aTwllEO0dlafZJbgc9gV8QOHFVrgxS29LiN3dN
kN6F/CfZPJAOP64hPuIhzsz1ZpC5M9D7nAsCsXP6wnfxEHkwK4QHZ1kcMqBn
-----END CERTIFICATE-----
)EOF";

/* Private Key */
static const char AWS_CERT_PRIVATE[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEA2MycXXxVpc48C4WQIFpncG2LPsfTFDZ6EMucg5q4GEUgkTE5
+wFl/hUZ1r88iIP1d4x5wsIr5RaWQ/yDWgptLAoe1GXT+WtQiM2XqLBHfJhA6gCo
cx3fln4vX/uUP7mvfglTJENeTaq37cW8+Lxs7BXku4UW6zI+35b4QaJSnHCrQa37
N67RlmxuwvcT62zCFTNb+S2WffGVes5k4Y5WuVzya/BiUowC8hF7wU/DLUc52ojt
gNyrbCSSoyLlS5HLPr7R1119bdjvD2JqHBI8my3uy7ZIdKGXLhQ4Lwkcc2BnFB6l
ULq+f9xx6xOiX8ys29PT5dGZJOxVLnvxxIV/vwIDAQABAoIBAEABCBjBYDe3DxsH
r2kdx3P1oypBrBtHifnzYp2ny7m+jsLOmaN04juE10AC7UgQTSOiIUBSOstDQ9GG
dvDaKyjZRAjIlzAHcpswtxliEXjDKkbrMs/pfdzcG/hmOXomw5yd/CG0kd60C+Dy
g7ubRTzdqc2CmSWOi+lOlLQry9/LCGf/A7Pm1F+yF+X+MspuM/x0tLLIOL/X91V3
IH8i1342O2q/xcy8nlYRmsIcBE2YtNsDVwg/MEYr8osyIvd/JoxBB/WzR6jbbTFz
iCkyzFlKtokIQ/F4a+lV/Np8fmp/yMeEtcncjMUm4RNRaQGVQMqIveVjQjW377XQ
hZ8Yf4ECgYEA7hQ3PcoDiyFWhEyE9opsuORVat5hOVo4vRpU4iQ2k11ld/6SLHaK
vo/H5AD3YoNr8Msam5pZsrfAsVRgaa3PrT2MANNjzzPO5tXRBpR7vYhVK/DaikHK
gL62l8zHRGsIww+I/pXi+JcFiZcpJPKqOR8UAKmNLpGPd0Gpg1tTVA8CgYEA6R5V
t/BBXi/8QJ/AstTyS9d3hBXPpsIDKc9fqDvX3FN5z9X1kdZBy6Aoc1es0IbA8vRU
n8JVjf02aGJwXfTYbLZrp82vFzqUndVRU+gwwNvRFtnQ09J1UPJ9Wk3qgU0dv/jD
M+aUyaUL9lLuMMAykButWTbahJob2wtAQka2qVECgYEAupLvI6YPltXedjl9LbQX
NeICqohxir+QnWloQfx9JIlZvmqq5NtVKbeFv8GeHP6+9WpFhNG2H59vC8kC8W57
xNjpwa/nlPaV20KO12g/fK29iXzr1tNuPey3LUkWlCCJMkwXYOPXhlbbyRLD9KeT
DMa1Jzu4X1lFJAE6Na25JLMCgYAylM5slsjYMvk83GznYKUiJ6JbGpBv0nD1RXWv
/ywbNlGb8wTRm/eywxGD1WY96L88+B7Ue0otAOApThKrkFDVZc9+HXb8R02v4XVA
C4Vok0mO97wD+gpSggsI6RcU7zKhUMyqGkH2bsVlsSDVmx5dfmPik7BeTUH2CT7s
IoPHwQKBgQDIOREombNm6h3GoEtuJsz0MEPKhPAnR8J8CPjj5Swsdb7CuV6HkTJ5
rssYIo/e9PRVqcAg4HT/neq1ta0dRIKGdh3FMaWpLgn2PyZAQkUsAYn5o2FkkFcG
WUsO4TGEp9oGqRlUrKlwnE2CcUbIS4d3S3PWAjRkZb+kVvYkdBEGDg==
-----END RSA PRIVATE KEY-----
)EOF";

/* ================= BLE UUIDs ================= */
static BLEUUID serviceUUID("12345678-9abc-def0-f0de-bc9a78563412");
static BLEUUID charUUID("abcdef01-2345-6789-8967-452301efcdab");

/* ================= OBJECTS ================= */
BLEAdvertisedDevice* myDevice;
BLEClient* pClient;
BLERemoteCharacteristic* pRemoteChar;

bool doConnect = false;
bool bleConnected = false;
bool awsConnected = false;

WiFiClientSecure net;
PubSubClient mqtt(net);

/* ================= TIME ================= */
void setClock() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  Serial.print("Syncing time");
  time_t now = time(nullptr);
  while (now < 1672531200) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("\nTime synced");
  delay(2000);
}

/* ================= WIFI ================= */
void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

/* ================= AWS ================= */
void connectAWS() {
  net.setHandshakeTimeout(60);
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);
  mqtt.setServer(AWS_IOT_ENDPOINT, 8883);

  Serial.print("Connecting to AWS IoT...");
  if (!mqtt.connect(THING_NAME)) {
    Serial.print("FAILED rc=");
    Serial.println(mqtt.state());
    return;
  }
  Serial.println("Connected to AWS IoT");
  awsConnected = true;
}

/* ================= BLE CALLBACK ================= */
void notifyCallback(
  BLERemoteCharacteristic*,
  uint8_t* pData,
  size_t length,
  bool) {

  if (length != 8) return;

  float temp, hum;
  memcpy(&temp, pData, 4);
  memcpy(&hum, pData + 4, 4);

  Serial.printf("Temp: %.2f | Hum: %.2f\n", temp, hum);

  if (!mqtt.connected()) {
    Serial.println("AWS not connected yet, skipping publish");
    return;
  }

  StaticJsonDocument<256> doc;

  // ✅ ADD IDENTIFIERS
  doc["warehouse_id"] = "WH-PUNE-01";
  doc["box_id"] = "BOX-CC-007";

  // existing fields
  doc["temperature"] = temp;
  doc["humidity"] = hum;

  char buffer[256];
  serializeJson(doc, buffer);

  mqtt.publish(MQTT_TOPIC, buffer);
  Serial.println("Published to AWS");
}


/* ================= BLE SCAN ================= */
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.haveServiceUUID() &&
        advertisedDevice.isAdvertisingService(serviceUUID)) {
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
    }
  }
};

/* ================= BLE CONNECT ================= */
bool connectToServer() {
  pClient = BLEDevice::createClient();
  if (!pClient->connect(myDevice)) return false;

  BLERemoteService* service = pClient->getService(serviceUUID);
  if (!service) return false;

  pRemoteChar = service->getCharacteristic(charUUID);
  if (!pRemoteChar) return false;

  if (pRemoteChar->canNotify()) {
    pRemoteChar->registerForNotify(notifyCallback);
  }

  bleConnected = true;

  // ✅ ADD THIS LINE
  Serial.println("BLE Slave Connected");

  return true;
}


/* ================= SETUP ================= */
void setup() {
  Serial.begin(115200);
  mqtt.setBufferSize(512);
}

/* ================= LOOP ================= */
void loop() {

  /* AWS FIRST */
  if (!awsConnected) {
    if (WiFi.status() != WL_CONNECTED) connectWiFi();
    setClock();
    connectAWS();
    return;
  }

  /* BLE AFTER AWS */
  if (!bleConnected && !doConnect) {
    BLEDevice::init("");
    BLEScan* scan = BLEDevice::getScan();
    scan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    scan->setActiveScan(true);
    scan->start(0, true);
    doConnect = true;
  }

  if (doConnect && !bleConnected) {
    connectToServer();
    doConnect = false;
  }

  if (mqtt.connected()) mqtt.loop();
}
