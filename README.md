# Circle video player

Plays various video formats using circles. Can also be used to play video from a webcam or a mobile phone.

### Dependencies:

- opencv
- sfml

### Usage:

To compile the program run: `make`  
Usage: `./circle-video-player <args> <filename>`

#### Runtime flags:

| Flag                     | Description                                                              |
| ------------------------ | ------------------------------------------------------------------------ |
| `-b <brightness>`        | Brightness factor (double).                                              |
| `-h`                     | Show help menu and exit.                                                 |
| `-r`                     | Use a larger radius for circles allowing for them to fully fill an area. |
| `-s <target_pixel_size>` | Size of one 'pixel'. Use smaller number for more circles (integer).      |
