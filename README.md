# z88dk Background Animation

Animate background tiles

## Getting Started

There are a few ways to animate background tiles. The most common is to just print the characters with PrintAtInv in the same position to achieve it, but this has a lot of overhead.

A more optimized way is to print a tile outside the main loop and inside the main loop change it's correspondent graphic every now and then and invalidate just the specific tile coordinate, or a list of tiles.

## Description

To achieve the above will use:

* sp1_TileEntry, to assign a tile to a letter
* ssp1_GetUpdateStruct to get the update structure region where the tile lives
* sp1_IterateUpdateArr to invalidate a previously list of tiles locations and force SP1 to draw the updated grahics
* sp1_IterateUpdateRect to invalidate a rectangle in the screen and force SP1 to draw the updated grahics (not covered)

Note: both sp1_IterateUpdateArr and sp1_IterateUpdateRect have it's merits, being the only visible difference the ability of the first to update other than only square regions.

## Program

The code is fully documented and can be compiled with

```
zcc +zx -vn -startup=31 -clib=sdcc_iy sprites.asm tiles.asm int.c main.c -o test_bed -create-app
```

You also have a compiled TAP file. Use QAOP keys to move the bubble.

## License

You can freely use the code or ideas as you wish. I have commented most of it, you should be able to follow it easily.
If it helps you in any way, I'll be happy.

## Acknowledgments

As always

* [z88dk](https://z88dk.org/) - z88dk home
* [World of Spectrum forums](https://worldofspectrum.org/forums)


