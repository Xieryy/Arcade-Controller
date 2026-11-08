
#include "Adafruit_TinyUSB.h"

uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_GAMEPAD()
};

Adafruit_USBD_HID usb_hid;
hid_gamepad_report_t gp;

// Joystick GPIO assignments
const int PIN_UP      = 18;
const int PIN_DOWN    = 19;
const int PIN_RIGHT   = 20;
const int PIN_LEFT    = 21;

// Button GPIO assignments
const int BUTTON_PINS[10] = {
  2,  // Red button 1
  3,  // Red button 2
  4,  // Yellow button 1
  5,  // Yellow button 2
  7,  // Start
  8,  // Select
  10, // Blue button 1
  11, // Blue button 2
  12, // Green button 1
  13  // Green button 2
};

void setup() {
  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }

  pinMode(PIN_UP, INPUT_PULLUP);
  pinMode(PIN_DOWN, INPUT_PULLUP);
  pinMode(PIN_LEFT, INPUT_PULLUP);
  pinMode(PIN_RIGHT, INPUT_PULLUP);

  for (int i = 0; i < 10; i++) {
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);
  }

  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor(
    desc_hid_report, sizeof(desc_hid_report)
  );
  usb_hid.begin();

  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }
}

void loop() {
#ifdef TINYUSB_NEED_POLLING_TASK
  TinyUSBDevice.task();
#endif

  if (!TinyUSBDevice.mounted()) return;
  if (!usb_hid.ready()) return;

  bool up    = digitalRead(PIN_UP) == LOW;
  bool down  = digitalRead(PIN_DOWN) == LOW;
  bool left  = digitalRead(PIN_LEFT) == LOW;
  bool right = digitalRead(PIN_RIGHT) == LOW;

  gp.x = 0;
  gp.y = 0;
  gp.hat = 0;
  gp.buttons = 0;

  // Joystick directions
  if (up && right)        gp.hat = 2;
  else if (right && down) gp.hat = 4;
  else if (down && left)  gp.hat = 6;
  else if (left && up)    gp.hat = 8;
  else if (up)            gp.hat = 1;
  else if (right)         gp.hat = 3;
  else if (down)          gp.hat = 5;
  else if (left)          gp.hat = 7;

  // Arcade buttons
  for (int i = 0; i < 10; i++) {
    if (digitalRead(BUTTON_PINS[i]) == LOW) {
      gp.buttons |= (1 << i);
    }
  }

  usb_hid.sendReport(0, &gp, sizeof(gp));

  delay(5);
}
