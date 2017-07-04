# Text Transcoder
## Background
This program transcodes between text sequences of binary or hex bytes and ASCII characters.

## Instructions
To use, build `textTc.exe` in a *nix terminal using the `make` command, put the text to be transcoded in a file named `input.txt` in the same directory as the executable, and run with the appropriate arguments:

`textTc.exe -a2b` to transcode the text FROM ASCII TO BINARY

`textTc.exe -b2a` to transcode the text FROM BINARY TO ASCII

`textTc.exe -x2b` to transcode the text FROM HEX TO BINARY

`textTc.exe -x2b -f` to write the transcoded text TO AN `output.txt` FILE in the same directory

`textTc.exe -x2b -s` to SEPARATE transcoded hex or binary bytes by a SPACE

`textTc.exe -h` to display these HELP instructions

## License

[GNU General Public License v3.0](https://www.gnu.org/licenses/gpl-3.0.html)
