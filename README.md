# beaugeste
A touchpad gesture recognition program for libinput

Usage: `beaugeste&` or something.

Sample configuration: (in `$HOME/BeauGeste.conf`)

`(swipe (3 'north) "xdotool set_desktop --relative -- -1")
(swipe (3 'south) "xdotool set_desktop --relative -- 1")
(swipe (3 'east) "xdotool set_desktop --relative -- -1")
(swipe (3 'west) "xdotool set_desktop --relative -- 1")
`

You can use 3 or 4 fingers in any of the cardinal directions. North-east and the other diagonals might work; can't remember.
2 fingers might work too, but it'd be annoying.

Set any script you like in the quotes. It's called verbatim.
