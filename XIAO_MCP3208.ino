#include <SPI.h>

// SPI pin definitions
const int CS1 = D7;  // Chip Select for MCP3208 (1)
const int CS2 = D6;  // Chip Select for MCP3208 (2)

bool led_blink = 0;

// SPI settings
SPISettings settings(1000000, MSBFIRST, SPI_MODE0);

// Array to store 16 ADC values
int adcValues[16];

/**
 * Read a single channel from MCP3208 (Single-Ended Mode)
 */
int readADC(uint8_t cs, uint8_t channel) {
  if (channel > 7) return -1;  // Invalid channel

  digitalWrite(cs, LOW);  // Select the ADC
  SPI.beginTransaction(settings);

  // MCP3208 Single-Ended Read Command
  uint8_t command1 = 0b00000110 | ((channel & 0b100) >> 2);  // Start bit + Single-End + D2
  uint8_t command2 = ((channel & 0b011) << 6);               // D1 + D0 + "000000"

  SPI.transfer(command1);                  // First byte transfer
  byte highByte = SPI.transfer(command2);  // Second byte transfer
  byte lowByte = SPI.transfer(0x00);       // Dummy transfer

  digitalWrite(cs, HIGH);  // Deselect the ADC
  SPI.endTransaction();

  return ((highByte & 0x0F) << 8) | lowByte;  // Convert to 12-bit value
}

/**
 * Read all 16 ADC channels and store in the array
 */
void readAllADC() {
  // Read 8 channels from MCP3208 (1)
  for (uint8_t ch = 0; ch < 8; ch++) {
    adcValues[ch] = readADC(CS1, ch);
  }

  // Read 8 channels from MCP3208 (2)
  for (uint8_t ch = 0; ch < 8; ch++) {
    adcValues[ch + 8] = readADC(CS2, ch);
  }
}

void setup() {
  Serial.begin(115200);
  SPI.begin();

  pinMode(CS1, OUTPUT);
  pinMode(CS2, OUTPUT);
  digitalWrite(CS1, HIGH);
  digitalWrite(CS2, HIGH);


  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  readAllADC();  // Read all 16 channels

  Serial.print("AD16");  // Output identifier

  // Output the ADC values from the array
  for (uint8_t i = 0; i < 16; i++) {
    Serial.print(",");
    Serial.print(adcValues[i]);
  }

  Serial.println();  // New line
  delay(100);        // Delay for 100ms

  led_blink = !led_blink;     //Blink LED
  digitalWrite(LED_BUILTIN, led_blink);
}
