#include "NMEA2000Page.h"
#include "i2c_t3.h"
#include "board.h"

#include "Adafruit_BMP280.h"

static void handler(const tN2kMsg &msg) {
  DEBUG("Received N2K Message with pgn: %i", msg.PGN);
}

NMEA2000Page::NMEA2000Page() {
  // Make sure the CAN transceiver is enabled.
  pinMode(can_standby, OUTPUT);
  digitalWrite(can_standby, 0);

  NMEA2000.SetProductInformation("00000002", // Manufacturer's Model serial code
                                 100, // Manufacturer's product code
                                 "Simple wind monitor",  // Manufacturer's Model ID
                                 "1.0.0.11 (2015-11-10)",  // Manufacturer's Software version code
                                 "1.0.0.0 (2015-11-10)" // Manufacturer's Model version
                                 );
  //// Det device information
  NMEA2000.SetDeviceInformation(1, // Unique number. Use e.g. Serial number.
                                130, // Device function=Atmospheric. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                55, // Device class=External Environment. See codes on  http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                2046 // Just choosen free from code list on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
                               );

  NMEA2000.SetMsgHandler(handler);
  NMEA2000.EnableForward(false);
  NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode, 22);

  if (NMEA2000.Open()) {
    DEBUG("Initialized NMEA2000");
  }
  else {
    DEBUG("Something went wrong initializing NMEA2000 ... ");
  }
}

void NMEA2000Page::willAppear() {
  needsPainting = true;
}

bool NMEA2000Page::processEvent(const TickEvent &e) {
  DEBUG("send wind and parse messages...");

  // Send test message.
  tN2kMsg N2kMsg;
  SetN2kWindSpeed(N2kMsg, 1, 5.0, 10.0, N2kWind_Apprent);
  NMEA2000.SendMsg(N2kMsg);

  NMEA2000.ParseMessages();
  return true;
}

bool NMEA2000Page::processEvent(const ButtonEvent &e) {
  if (e.clickType == ButtonEventTypePressed) {
    needsPainting = true;
  }
  return true;
}

void NMEA2000Page::paint(GC &gc) {
  if (!needsPainting)
    return;

  gc.fillRectangle(Point(0, 0), Size(320, 240), ColorBlue);
  gc.drawText(Point(2, 5), FontDefault, ColorWhite, "  NMEA2000 Test Page");

  needsPainting = false;
}