# Soldered-Stop-Me-Game-Soldering-Kit

Arduino code for Stop Me Game Soldering Kit by Soldered based on STM32C011J6M6 MCU.

# **NOTE, PLEASE READ THIS!**

> **Warning**
> There are two branches of this repo; one for production (main) and one for development (dev). Difference is in delay of 8 seconds on the start of the code allowing STM32 to be reprogrammed. For development dev branch HAL_Delay(8000) must be used and must exists in the code.
For production, main branch must be used and there should NOT exist HAL_Delay(8000);.

NOTE: Before programming, option byte should be modified! With STM32CubeProgrammer, in user configuration, find NRST_MODE and set it to 2 (GPIO MODE).
Otherwise, MCU will constantly reset itself.

Again, do not forget to remove delay -> HAL_Delay(8000); It's only for debug purpose only!

Whole code is written in STM32CubeIDE v1.10.1.
