#include <ComputhermRF.h>
#include <OpenTherm.h>

#define OTPIN_IN	4
#define OTPIN_OUT	5
#define DOUT_PIN  14
#define FLOW_TEMP	60.0 //60 Celsius forward water temperature
#define OTTIMER		997 //1 sec

const String ADDR1 = "84BF7";
const String ADDR2 = "E3DF7";

ComputhermRF rf = ComputhermRF(DOUT_PIN, 255);
OpenTherm ot(OTPIN_IN, OTPIN_OUT, false);

void ProcessOpenTherm(bool boilerON)
{
  //Serial.print("Command: ");
  //Serial.println(boilerON);
  unsigned long request;

  if (!boilerON)
  {
    request = ot.buildSetBoilerStatusRequest(0, 1, 0, 0, 0);
  }
  else
  {
    request = ot.buildSetBoilerStatusRequest(1, 1, 0, 0, 0);
    ot.sendRequest(request);
    request = ot.buildSetBoilerTemperatureRequest(FLOW_TEMP);
  }
  ot.sendRequest(request);
}

void ICACHE_RAM_ATTR handleInterrupt()
{
  ot.handleInterrupt();
}

void setup() {
  Serial.begin(115200);
  delay(100);
  rf.startReceiver();
  ot.begin(handleInterrupt);
}

void loop() {
  String receivedAddress;
  bool receivedCommand;
  static bool command1;
  static bool command2;
  static unsigned long otTimer;

  if (rf.isDataAvailable())
  {
    rf.getData(receivedAddress, receivedCommand);
    Serial.print("Address: ");
    Serial.println(receivedAddress);
    Serial.print("Command: ");
    Serial.println(receivedCommand);
    Serial.println();
    if (ADDR1 == receivedAddress)
    {
      command1 = receivedCommand;
    }
    if (ADDR2 == receivedAddress)
    {
      command2 = receivedCommand;
    }
  }

  if (millis() - otTimer > OTTIMER)
  {
    ProcessOpenTherm(command1 || command2);
    otTimer = millis();
  }
}
