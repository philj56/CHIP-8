# CHIP-8
CHIP-8 Interpreter written in C, as an exercise in learning emulation basics.
Supports resizing / fullscreen.
If a game does not work, try playing with the flags in `main.c`.

# Usage
        cd CHIP-8
        make
        ./chip8 path/to/rom

## Keys
        PC:             CHIP-8:
        ┌─┬─┬─┬─┐       ┌─┬─┬─┬─┐
        │1│2│3│4│       │1│2│3│C│
        ├─┼─┼─┼─┤       ├─┼─┼─┼─┤
        │Q│W│E│R│       │4│5│6│D│
        ├─┼─┼─┼─┤       ├─┼─┼─┼─┤
        │A│S│D│F│       │7│8│9│E│
        ├─┼─┼─┼─┤       ├─┼─┼─┼─┤
        │Z│X│C│V│       │A│0│B│F│
        └─┴─┴─┴─┘       └─┴─┴─┴─┘

