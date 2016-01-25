#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char WiFiAPPSK[] = "Camslider";
#define PASSWORD "passwort123"

#define PORT 2390      // local port to listen on

WiFiUDP Udp;

char * BuildUniqueishNetName()
{
   // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "ESP8266 Thing " + macID;
    char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i=0; i<AP_NameString.length(); i++)
  {
    AP_NameChar[i] = AP_NameString.charAt(i);
  }
  return AP_NameChar[i];
}

void setup(void){
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD)
  {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }
  WiFi.mode(WIFI_AP);

  BuildUniqueishNetName();
  
  WiFi.softAP(AP_NameChar, PASSWORD);
  WiFi.printDiag(Serial);
  Serial.println("");

  Serial.print("WiFi-Ap started with SSID:");
  Serial.println(WiFi.SSID());

  Udp.begin(PORT);
}

void loop(void)
{
  
}


