# Soldered-Stop-Me-Game-Soldering-Kit

Arduino code for Stop Me Game Soldering Kit by Soldered based on STM32C011J6M6 MCU.

NOTE: Before programming, option byte should be modified! With STM32CubeProgrammer, in user configuration, find NRST_MODE and set it to 2 (GPIO MODE).
Otherwise, MCU will constantly reset itself.

NOTE: Do not forget to remove delay on the line 100. -> HAL_Delay(8000); It's only for debug purpose only!

Whole code is written in STM32CubeIDE v1.10.1.
