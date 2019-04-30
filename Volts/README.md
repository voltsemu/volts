# Volts

## Gubbins and stuffing
if you clicked on this directory you obviously want to poke around the code, enjoy your stay.

### Application loop
the program has a "lifecycle" of sorts that describes where code execution generally flows.

1. Native main function creates gui for frontend user interface
2. Native then calls Init function inside volts core
3. Native controls all interaction until a game is played/firmware loaded/whatever else
4. when an interaction is required the frontend calls the emulator
5. the emulator will then do its work, possibly logging to frontend.
6. once emulator is done work frontend then has control again and repeat until user closes program.
