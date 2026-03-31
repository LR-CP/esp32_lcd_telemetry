# Notes

This file contains my notes as I go through and work on this project.  

## LCD Driver

Memory:
0x00 → 0x0F   (first row)
0x10 → 0x3F   (NOT visible)
0x40 → 0x4F   (second row)

### Pin Mapping (PCF8574 <--> LCD1602)

- P0 = RS
- P1 = RW
- P2 = E
- P3 = LED
- P4 = D4
- P5 = D5
- P6 = D6
- P7 = D7

### Message Structure

