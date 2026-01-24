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
MIIFQTCCAymgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAKrYFJ3E9p6y
YxX0d2yq6Y5Pja/qvpDMAYA9Yg3gFyv1I3VzBkYOPxfN3C3xaQa58aeGeq/QAdzB
tG0w+zC9qK4u3x5+Jkwmo7aX6ixkmKsgNHYsYAGP82LPgAvFp8Z64KbjDg7sWXBJ
3wa9u0edPFsKnzGWxZmfZbduCMsZ/HXXIQK5vCk1vZ1tcHTTX3e8DqRL3OjaxAgq
nxsVXni4eWh05rq6ArlTc95xJMu38xpv8uKXu95syEHCqB6f+GO6zkRgZNpmjQe7
DdyCjTiMQuuLHfoalGoVYLRNvKcJsteVEh9UpAJZciV06P88GJEqn3Ejj6LikUeJ
8V+RaHcRUW2KIiMzFxLpy0X58F3RrgPf63HgVUsVTNff7kwh28ykVfoCEN0z7Lxyz
Dn5XxhxL7sRKqzZo4PMBVXgS5aXoaZySUdkGFUTkOcJCIZy9lEi5Vf3L7hIwrKyY
JZZzKzbwQ6vurIWBLL8GMDIS9ZhDJW60F7uO3cu6UytzszbmWzxubUoil58x2oyW
MhUlCT3VkOITkkpFmS6r30YIOCQM7DDDeZPA7eGRIDcHruM3aMcMBXNIN1qNbfXD
f9eKJeNEqXWiwxupCSvJzpuG1HsfrN7kYM/q9n32CuWHa3gwxObn2ymlk/wEYBjq
FcpnSUsctNkCAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4ICAQBzCqRR3YKHy+XapnwYfJOLLmObEWj1vDLteA94ppIqhzyapMI2vlA38nS/
dbidKfnUSsfx8bVsgcuyo6edSxnl2xe50Tzw9uQWGWpZJYG1ChcxrFAxo0xO+ogz
8h1Hn0pV3hokW2N7DbStO2Qd6hw2yYB9H9n1tFoZT3zh0+BTtPlqvGjufH6G+j6l
zi10BGSAdoo6gWQBaIj++ImQxGc1dQc5sKXc5teLoI0lp4rWuIwoMvV7lidh+NRO
4tW7x1YgnSUZXoqBYwygJyI072QtdgQXl3k5iADG7n2AFD+a83H8XTur2qxGn8pY
bexdFv+DE5jBqFaUG2RgxN6E466+vWXTjhBMWrMUR3pvN8MPmMXxMvmP0xSg6u40
CMgfHTCqkfNNpJBbGAbIYW/W2PASi6DPd7OJbRRqtD9h5pz2jdK5Zk90un0nLBK+
Xn1HULICwhf66A1VpzwuNFuIBqmoeZaZX6mE6oPD58Ll35H0TADaBrZEcD3xKhsR
H4HIX66D+QP9enZ5bY3bT5iAG2wE8xmPKzW0fvkYlEYwH14r2raXIiQunlslqY5T
r8j04YfGLmRoTSesxNUFDXL9uBeb5GsyhQOdE31E4n4t4WRPcNI0+vrFica8FWHZ
cizxgxYkWwaRM42CikIze8ih/7gToYtL6vhfVqlhK/SXxA==
-----END CERTIFICATE-----
)EOF";

/* Device Certificate */
static const char AWS_CERT_CRT[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUVrm4rF1cC0Hee6DlQFw3lwAGZUowDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI2MDEyMzE5MTgw
NFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAOWc3qas+i1XAxU6+Xn1
wbZVGv4cJnfnRc+uoNWwTMq57K5IBL6VN0Zy0K79x2XUHcTy1g3TqhvvcdzIPNQW
ueCSijj/vKLWvolppcp9oGZoN977gq9NoKYHbZXHOeTHo12eS+ZmVL0IOQqy0u6J
mtGTVrFo5S37uaCzKCVHT4teU5+ZVntrEGi7hP6BKSIJb5evozACdT8MGMkIyy6N
I7Wz1JmYEpH3sS8c19dT7xHxUaVM9haImUENbKIFTW9f8XqIkgFti8AlunVJMbVv
fi1SsyZh4sFsJxBNbvcTlQzR+4nyAPVMYfy1294WxN/dI2/rmQu0vI8u2FbHwc6w
incCAwEAAaNgMF4wHwYDVR0jBBgwFoAUk7k+10HXikf6MVi3wXhw/1XuINMwHQYD
VR0OBBYEFAZMMLqba8gBLp6lmeoEh9MXmnkvMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQBQConcTq31MLJPsAjoYQYJzFy4
FLJZpOaUoSrdUZ2dH372U72FEqf6QLzkT6MOxYoknljwSLkHwUHJ7ccDEWW44A8s
fPjXWEunOEb0isRpxp2oD0vUYQbCGhRWSHFyg7igFyWUMO8tCSYyxre5yL8C120f
PKPX/SDyjM6+dbo0Hmpu2CqeysOsMY3Huc0QkAJU+RI46Lart8o5RWW90VWvf6Qd
6/B0j52TxmdG31bIWFU4vzOGFRBFDH4057+m1XjwIaVeSMQExQJ251W8Oq3k/e9B
xR0W/GSvkS0a6FF6Ny6do6uUdSIKU1SsNPvKSoTYl9R9LW0OTZ8s+9Wrp46S
-----END CERTIFICATE-----
)EOF";

/* 🔴 PRIVATE KEY (MOST IMPORTANT) */
static const char AWS_CERT_PRIVATE[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEA5Zzepqz6LVcDFTr5efXBtlUa/hwmd+dFz66g1bBMyrnsrkgE
vpU3RnLQrv3HZdQdxPLWDdOqG+9x3Mg81Ba54JKKOP+8ota+iWmlyn2gZmg33vuC
r02gpgdtlcc55MejXZ5L5mZUvQg5CrLS7oma0ZNWsWjlLfu5oLMoJUdPi15Tn5lW
e2sQaLuE/oEpIglvl6+jMAJ1PwwYyQjLLo0jtbPUmZgSkfexLxzX11PvEfFRpUz2
FoiZQQ1sogVNb1/xeoiSAW2LwCW6dUkxtW9+LVKzJmHiwWwnEE1u9xOVDNH7ifIA
9Uxh/LXb3hbE390jb+uZC7S8jy7YVsfBzrCKdwIDAQABAoIBAQCIYvWd64FvDRA3
vCAT4XZ+525xIid3cVh2VDQUufPjNyzvGLg4vElo/yoVx4h5Y9WK9vNOozuX/FKV
4hq/oAAo9OJFmSJjV0s2NBrWfGiaa61ENgEaP/H8rMza+svYqPzQzwYmhQIwhoHb
aP0B05qygeIs3zHCbpOEcOaXvhPxo6W5mfIpMWxelZe9orWXAHl1MvTyytVUevDL
3b5mG7P0wL+dhqRRWGyZpHFpPnt/3CwZaiecX0WSlkCGnbWqHzG4FjqiBwPOTR1T
e+ljQeIdi68sDqweyEWJ4tkHDJPBr/nh70p8PtIIRw23/6JJWXhzQHOirJIPAE6z
eyHOq6wBAoGBAPqaBUsT5OFwQeMO5t5QpH7Q2XEwXZz1ebRCG8OLITwUOjVwyjuX
Hlp1Xa5PsbAjapvrKv7P+A0K1Hg/GDJORSnmO1KXUNwZaECL0EC8njGLrOQB8EGI
UcVLlgfM+F8PBLunWdLLxRFot7RjV5db45MimsM5Hc6beZbiYpgM8VQhAoGBAOqP
GlU/hFuwYvf8Dh4+/lkeFmr9b4zZZ9lhsAo+WA6qEno9EgKX9rVo7lVRBn07mhjO
tvNW7wW6gudcGTeyl34Wgv9IAo2hXiz+COcWNHyCn06eMoyd3/TaBVoNWCrZefHM
pfzX7AWCd27uFc+0nUYkv2BFfQOoChP5ifSOaouXAoGBAKIWfvf5jR/3cNZA7Di9
JjhUM2BPrSz5ZVefLsACvGNUnfW010lFzD8sDITTTRWlQq5IpE2eDE27RIPmI2te
TsP0A0i/UHCcCl89T1yf4x4evpvXWSO53VHkOs0e2UL6vs/1RVbfr7474h5N5vii
WYbwafN9fPcGftWF5cl9DZahAoGBANXkx1ujv1wpXibLqEg2o5S61jCcicPEYWof
JfxBW+WLe6wS0sCguS420NfmOmerGVuz7au7w93SdhrqpmhfKLynZK/Mh9+Fnq6i
vZWVbyl3qDR2GJyYPyk2Q5kaACObDBPOzjLi5+0iSy1SWXt+NclIK2hsFZNnAkGy
1fW4hTpjAoGAGE5cNtABmxjmrZf1cF0AcbzZ8Fps7iV0CkytcCRSdI0zJD2lVVPa
aDlBUq6qC4HY6l12UJSFLUPeYdzdvUH3IGWwpvahW+JZ+9Fdjxz/aiTIlp+8WQ2f
3Qqf3+At9++pyMbHLHzgHMfFmzBtuXkJtHmTg0DYG5+kKXFnZ0BL110=
-----END RSA PRIVATE KEY-----
)EOF";

/* ================= BLE UUIDs ================= */
static BLEUUID serviceUUID(
  "12345678-9abc-def0-f0de-bc9a78563412"
);

static BLEUUID charUUID(
  "abcdef01-2345-6789-8967-452301efcdab"
);

/* ================= BLE OBJECTS ================= */
BLEAdvertisedDevice* myDevice;
BLEClient* pClient;
BLERemoteCharacteristic* pRemoteChar;

bool doConnect = false;
bool bleConnected = false;
bool awsConnected = false;

/* ================= AWS OBJECTS ================= */
WiFiClientSecure net;
PubSubClient mqtt(net);

/* ================= TIME SYNC ================= */
void setClock() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  Serial.print("Syncing time");
  time_t now = time(nullptr);
  while (now < 1700000000) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("\nTime synced");
}

/* ================= WIFI CONNECT ================= */
void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.println(WiFi.localIP());
}

/* ================= AWS CONNECT ================= */
void connectAWS() {
  net.setHandshakeTimeout(30);
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  mqtt.setServer(AWS_IOT_ENDPOINT, 8883);

  Serial.print("Connecting to AWS IoT...");
  if (!mqtt.connect(THING_NAME, NULL, NULL)) {
    Serial.print(" FAILED, rc=");
    Serial.println(mqtt.state());
    return;
  }

  Serial.println("\nConnected to AWS IoT");
  awsConnected = true;
}

/* ================= BLE NOTIFY CALLBACK ================= */
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
  doc["warehouse"] = 1;
  doc["box"] = 4;
  doc["temperature"] = temp;
  doc["humidity"] = hum;

  char buffer[256];
  serializeJson(doc, buffer);

  mqtt.publish(MQTT_TOPIC, buffer);
  Serial.println("Published to AWS");
}

/* ================= BLE SCAN CALLBACK ================= */
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.haveServiceUUID() &&
        advertisedDevice.isAdvertisingService(serviceUUID)) {

      Serial.println("ColdChain Slave Found");
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
    }
  }
};

/* ================= BLE CONNECT ================= */
bool connectToServer() {
  Serial.println("Connecting to Slave...");
  pClient = BLEDevice::createClient();

  if (!pClient->connect(myDevice)) return false;

  BLERemoteService* service = pClient->getService(serviceUUID);
  if (!service) return false;

  pRemoteChar = service->getCharacteristic(charUUID);
  if (!pRemoteChar) return false;

  if (pRemoteChar->canNotify()) {
    pRemoteChar->registerForNotify(notifyCallback);
    uint8_t notifyOn[] = {0x01, 0x00};
    pRemoteChar->getDescriptor(BLEUUID((uint16_t)0x2902))
                ->writeValue(notifyOn, 2, true);
  }

  bleConnected = true;
  Serial.println("BLE Connected");
  return true;
}

/* ================= SETUP ================= */
void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 BLE + WiFi + AWS Gateway Starting");

  BLEDevice::init("");
  BLEScan* scan = BLEDevice::getScan();
  scan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  scan->setActiveScan(true);
  scan->start(5, false);
}

/* ================= LOOP ================= */
void loop() {

  if (doConnect && !bleConnected) {
    connectToServer();
    doConnect = false;
  }

  if (bleConnected && !awsConnected) {
    connectWiFi();
    setClock();
    connectAWS();
  }

  if (mqtt.connected()) {
    mqtt.loop();
  }

  delay(50);
}
