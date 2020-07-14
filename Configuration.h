/********
 * Intervals and optimization
 *******/

/* Enable debugging through the console. */
#define DEBUG_ENABLE false



/********
 * Status LED Configuration
 *******/

/* Status LED. This LED flashes to indicate the status of the device. Dimmed flashing indicates that the device
 * is connecting to a WiFi network, while a short bright flash indicates failure. Afterwards, a short bright flash 
 * indicates successful URL opening while a long one failure . */
#define LED_ENABLE true

/* Brightness of the status LED while in idle/connecting to WiFi. Can range from 0 to 1024, but setting it to 2 is suggested. */
#define LED_BRIGHTNESS_DIM 32

/* Brightness of the status LED while during a bright flash. Can range from 0 to 1024. */
#define LED_BRIGHTNESS_ON  1023



/********
 * Battery Status
 *******/

/* Use voltage instead of percentage. */
#define BATTERY_VOLTAGE false

/* Voltage conversion multiplier. Should be adjusted so that the reported voltage is same as measured on battery. 
 * You will need to enable battery voltage above to tune this.
 * Use the following formula:  NewMultiplier = OldMultiplier * (MeasuredVoltage / ReportedVoltage) */
#define BATTERY_MULTIPLIER 5.622
 
/* Battery voltage when empty. */
#define BATTERY_V_MIN 2.56

/* Battery voltage when full. */
#define BATTERY_V_MAX 4.13
