# Mechanical-Style Arduino Metronome

This repository contains an Arduino-based metronome I built for my sister. It emulates the sound and feel of a mechanical metronome by using a 4-relay module for click sounds, a vibration motor attached to an old wristwatch for tactile feedback, and a row of LEDs for visual beats. The user enters BPM and the time signature (beat divisions) on a 12-key membrane keypad and the device keeps the relays, LEDs and vibration synchronized.

## Features

- Mechanical 'click' sound using a 4-channel relay board to approximate the sound of a wind-up metronome.
- Tactile feedback using a vibration motor mounted to an old watch so you can feel each beat.
- Visual beat indicator using 8 LEDs that sweep back-and-forth in time with the metronome.
- Input via 12-key membrane matrix keypad (enter BPM and time signature).
- OLED 128x64 (I2C) display for BPM, current beat and status messages.
- Simple interface: enter numbers on keypad, press `#` to confirm, `A`/`B` to adjust BPM by +5/-5, and `*` to re-enter settings.

## Bill of Materials (what I used)

- Arduino Mega (any compatible Arduino with sufficient I/O pins should work)
- 4-channel relay module
- 12-key membrane matrix keypad (4x3 mapped into a 4x4 Keypad library layout)
- OLED 128x64 I2C display (0.96", address 0x3C)
- Vibration motor / Vibracall module
- LEDs (I used 2 red, 2 orange, 4 yellow — any colors work)
- Misc wires, resistors (for LEDs if needed), and a small enclosure or old watch case for the vibration motor

See the `Images/` folder for photos of the assembled device.

## Pinout (as used in the code)

These pin assignments match `Metronomo.ino` and can be changed in the source if you need different pins:

- OLED: I2C (SDA/SCL) — uses default Wire pins for your board; OLED address 0x3C in the code
- Keypad rows: pins 9, 8, 7, 6
- Keypad cols: pins 5, 4, 3, 2
- LEDs (8): pins 51, 50, 49, 48, 47, 46, 45, 44
- Relays (4): pins 22, 23, 24, 25
- Vibration motor: pin 10

If you use a smaller Arduino (Uno), adapt the pins to available digital I/O and adjust the code accordingly.

## How it works (high level)

1. On power-up the OLED shows a welcome message and then prompts the user to enter BPM and then the time signature (numerator). The keypad numeric entries are accepted until `#` is pressed.
2. The device calculates the interval per beat from BPM and divides each beat into sub-steps to sequence LEDs and to time the relay "clicks" and vibration pulses so they feel/appear synchronous.
3. The relays are toggled in a pattern to create an audible clicking sound similar to a mechanical escapement. The vibration motor is activated briefly at the start of each beat for tactile feedback. LEDs sweep left/right following the beat.
4. Pressing `A` increases BPM by 5, `B` decreases BPM by 5, and `*` re-enters BPM/time signature setup mode.

## Software / Libraries

You will need the following Arduino libraries (available via Library Manager or GitHub):

- Keypad (https://github.com/Chris--A/Keypad)
- Adafruit GFX (https://github.com/adafruit/Adafruit-GFX-Library)
- Adafruit SSD1306 (https://github.com/adafruit/Adafruit_SSD1306)

Install them in the Arduino IDE (Sketch → Include Library → Manage Libraries...) before compiling.

## Uploading

1. Open `Metronomo.ino` with the Arduino IDE.
2. Select the correct board (e.g. Arduino Mega) and port.
3. Verify and upload.

## Usage

- When prompted, type the BPM using the keypad and press `#`.
- Then type the time signature numerator (the number of beats per bar) and press `#`.
- The metronome starts automatically.
- Adjust BPM quickly with `A` (increase +5) and `B` (decrease -5).
- Press `*` to re-enter BPM/time signature.

## Customization / How to add features

- To change pin assignments: edit the `ledPins`, `relayPins`, `rowPins`, `colPins`, and `VIBRATION_PIN` arrays at the top of `Metronomo.ino`.
- To change vibration duration or pattern: edit `endVibrationMillis` and where the vibration pin is asserted/deasserted inside `updateMetronome()`.
- To change the LED pattern or number of LEDs: update the `ledPins` array and the sequencing logic in `updateMetronome()`.
- To use an Arduino Uno or Pro Mini: remap pins to avoid conflicts with I2C (A4/A5 on Uno) and ensure you have enough digital pins for relays and LEDs.

If you want a software-only metronome (no relays), remove or comment out relay-related code and use a piezo buzzer instead for a simple click sound.

## Safety notes

- If you switch mains AC with relays, be extremely careful. Enclose exposed mains connections, follow local electrical safety practices, and consider using an opto-isolated relay board or a qualified electrician.
- Drive the vibration motor and LEDs through suitable driver transistors or resistors if their current exceeds what the Arduino pin can safely supply.
- Use diodes or snubbers if you change to inductive loads (motors, solenoids) to avoid damaging the microcontroller.

## Troubleshooting

- If the OLED does not show any text: check I2C wiring (SDA/SCL), power (3.3V or 5V depending on module), and correct I2C address (0x3C expected).
- If the keypad misses keys: verify row/column wiring and shield/noise; try enabling internal pull-ups or add small capacitors for debouncing.
- If timing feels off at high BPM: the code uses active-waiting in some loops. Consider replacing busy-wait loops with non-blocking timing using millis() or using interrupts for improved precision.

## Code contract / assumptions

- Input: numeric BPM (> 0) and integer numerator (beats per bar).
- Output: relay toggles, LED sequence, vibration pulses and OLED text representing the metronome state.
- Error modes: code expects reasonable BPM values (the sketch enforces a minimum of 30 when adjusting with `A`/`B`). Very large BPM values may lead to unusable behavior.

Edge cases to watch for:
- BPM = 0 or extremely large values — validate input before using in arithmetic that divides time.
- numerator = 0 — leads to modulo arithmetic issues; avoid zero.
- Long blocking waits — current code uses short busy-wait loops that may skew timing under heavy CPU load.

## Contributing

If you'd like to contribute enhancements (better timing, additional UI, support for more boards), please:

1. Fork the repository.
2. Create a new branch for your feature.
3. Open a pull request with a clear description and, if applicable, photos showing hardware changes.

Small improvements I recommend:
- Replace busy-wait loops with non-blocking timing using millis() to improve accuracy and responsiveness.
- Add configuration persistence using EEPROM so the device remembers last BPM and time signature after power cycle.
- Add support for a piezo buzzer mode (no relays) for a quieter, lower-power option.

## License

This repository is provided under the MIT license by default — change this file if you'd prefer a different license.

---

If you'd like, I can also:

- Add a wiring diagram image into `Images/`.
- Add an example schematic (Fritzing or hand-drawn) and pin labels on a photo.
- Add a small quick-start section with exact Arduino IDE steps and screenshots.

Tell me which of the extras you'd like and I'll add them.
