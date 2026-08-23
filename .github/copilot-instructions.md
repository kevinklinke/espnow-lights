# Copilot Instructions

These instructions are for AI contributors (Copilot) working on this repository.

- **Reflashing policy:** Receivers are expected to be reflashed to select their operating mode (for example `shimmer`, `solid`, `bar`) and will remain in that mode until reflashed again. Runtime mode switching via messages is optional and not required.
- **No backwards compatibility requirement:** You may change the message formats, `src/shared/Message.h`, and related firmware interfaces freely. The operator (project owner) controls both transmitter and receiver firmware and will reflash devices as needed.
- **Design guidance:** Favor simplicity and robustness over compatibility layers. Prefer compile-time or persistent configuration for mode selection; if runtime commands are added, document them explicitly.
- **Default board target:** When no explicit environment is provided, assume the standard production receiver is the ESP32-C3 target (`esp32-c3`). Do not default to `esp32dev` silently because it is a different chip and will fail to upload to a C3 device.
- **Board-specific hardware config:** Different receiver boards may have different pin mappings. Keep per-board config in the generated local config or board-specific defaults, not hard-coded in shared runtime logic. Examples from this project:
  - ESP32-C3 default receiver board uses the C3 config and default LED settings for that hardware.
  - ESP32 dev boards may require `LED_GPIO_PIN = 4`, `LED_DATA_PIN = 5`, and `LED_PIXEL_COUNT = 300` for the 300-LED strand.
- **LED strand sizing:** Receiver LED counts are board-specific and not always 200. The code should use a configurable pixel count rather than hard-coded values like `200` when different strands are deployed.
- **Safety check before flashing:** If the target environment or board type is not explicit, confirm the chip family matches the selected PlatformIO environment before uploading; `esp32-c3` and `esp32dev` are not interchangeable.

Place high-level decisions here so Copilot and other automated tools keep the project's operational assumptions in mind when making edits.
