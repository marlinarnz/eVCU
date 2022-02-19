/* ======================================================================
 * Using the Arduino IDE, the code needs a setup that is executed
 * once during power-up and a loop that runs infinitely until shutdown.
 */

#include <Arduino.h>
#include "Constants.h"

void setup() {
  if (DEBUG) {START_SERIAL}
  VehicleController vehicleController;
  vehicleController.Run();
}

void loop()
{
  vTaskDelete(NULL); // We don't need that loop
}
