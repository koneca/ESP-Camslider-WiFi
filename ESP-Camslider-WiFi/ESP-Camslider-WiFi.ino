#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

char * WiFiAPPSK;
#define PASSWORD "passwort123"

#define DEFAULT_PORT        2390
#define MAX_PACKET_LENGTH   255
#define MAX_VALUE_LENGTH    20

WiFiUDP Udp;


char packetBuffer[255]; //buffer to hold incoming packet
char  ReplyBuffer[] = "acknowledged";       // a string to send back

typedef struct _slider_tlv_data
{
  unsigned short          Id;
  uint8_t                 Type;
  uint8_t                 Length;
  uint8_t         Value[MAX_VALUE_LENGTH];
 
}
SLIDER_TLV_DATA, * PSLIDER_TLV_DATA;

char *
BuildUniqueishNetName(
  )
{

  char * WifiName = 0;
  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "WifiSlider" + macID;
    char AP_NameChar[AP_NameString.length() + 1];

  WifiName = (char *)malloc(sizeof(char)*AP_NameString.length() + 1);
  
  for (int i=0; i<AP_NameString.length(); i++)
  {
    WifiName[i] = AP_NameString.charAt(i);
  }
  return WifiName;
}


void
setup(
  void
  )
{
  Serial.begin(115200);
  while (!Serial)
  {
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

  WiFiAPPSK = BuildUniqueishNetName();
  
  WiFi.softAP(WiFiAPPSK, PASSWORD);
  WiFi.printDiag(Serial);
  Serial.println("");

  Serial.print("WiFi-Ap started with SSID:");
  Serial.println(WiFi.SSID());
  Serial.println("or");
  Serial.println(WiFiAPPSK);
  Udp.begin(DEFAULT_PORT);
}


/* -------------------------------------------------------------------------- */

/*!  \ingroup test-client
 * \brief dunp one Buffer to stdout
 * @Param [IN]Desc Text to be displazed before the dumped context
 * @Param [IN]Addr The Buffer, to be dumped
 * @Param {IN}Len  Length of the Buffer which has to be parsed
 */
void
HexDump (
  char        * Desc,
  void          * Addr,
  int           Len
  )
{
    int           i;
    unsigned char       Buff[17];
    unsigned char       *pc = (unsigned char*)Addr;

    // Output description if given.
    if (Desc != NULL)
    {
        Serial.println(Desc);
    }

    if (Len == 0)
    {
        return;
    }
    if (Len < 0)
    {
        return;
    }

    // Process every byte in the data.
    for (i = 0; i < Len; i++)
    {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0)
        {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                Serial.println((char*)Buff);

            // Output the offset.
            Serial.println(i, HEX);
        }

        // Now the hex code for the specific character.
        Serial.println(pc[i],HEX);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
        {
            Buff[i % 16] = '.';
        }
        else
        {
            Buff[i % 16] = pc[i];
        }
        
        Buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0)
    {
        Serial.println("   ");
        i++;
    }

    // And print the final ASCII bit.
    Serial.println((char *)Buff);
}

void loop(void)
{
  //Serial.println("aaaaaa");
//  pause(300);
   // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    PSLIDER_TLV_DATA Data = 0;
    Data = (PSLIDER_TLV_DATA)&packetBuffer;
    Serial.print("id");
    //Serial.println(Data->Id);
    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0;
    }
    
    HexDump("Contents:", &packetBuffer, len);

    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
    Udp.endPacket();
  }
}


