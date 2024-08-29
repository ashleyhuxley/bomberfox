# Protocol Specification

The game operates in two modes. The main mode used during play (Command Mode) and a secondary mode used to transfer level data.

## Command Mode

The game communicates actions to the other flipper using an array of three bytes. The first byte indicates the player and the action that the player performed. The second two bytes contain the X and Y coordinates of the location of that action.

### First Byte

| Bit | Description |
| ----| ----------- |
| 0   | Unused      |
| 1   | Unused      |
| 2   | Unused      |
| 3   | Player      |
| 4-7 | Action      |

The 3rd bit denotes the player:

0. Fox
1. Wolf

Bits 4-7 denote the action:

| Value  | Action                                                              |
| -------| ------------------------------------------------------------------- |
| 0b0000 | (Reserved)                                                          |
| 0b0001 | The player moved to the location specified in bytes 2 and 3         |
| 0b0010 | The player dropped a bomb at the location specified in bytes 2 and 3|
| 0b0011 | The player died. (Location bytes unused and should be set to 0x00)  |

### Example:

**0x11 0x02 0x03**
The wolf has moved to position (2, 3)

**0x02 0x05 0x01**
The fox has placed a bomb in position (5, 1)

**0x13 0x00 0x00**
The wolf has died

## Level Data Mode

When the Wolf character is chosen, the application enters Level Data Mode and waits for transmission of the level data. Once the Fox character has chosen a level, the 128 bytes of level data are transferred to the Wolf, which then reverts to Command mode.

