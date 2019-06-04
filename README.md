# Mines
Minesweeper built in C using GTK+ 3.0 as the gui.
This was initially a high school project for a CS course so don't judge.
So far it only works in Unix based systems with GTK and it looks very weird in i3wm.
It has been tested in Linux and FreeBSD.
つ´Д`)つ


How to play the game: 
Left click a tile to reveal the what is underneath it and Right click to flag the tile. 
Left clicking a nonbomb tile will reveal a number that represents the number of bombs surronding that space.
Left click a tile with zero or no bombs around it will reveal everything around it as well.
Left clicking a bomb makes you lose the game.
You can restart the game by clicking the face at the top of the window. 
You win by revealing every tile that is not a bomb.

Secret move:
Middle click! Many people don't know about this move in minesweeper.
You middle click an already revealed tile to reveal everything around it that isn't flagged.
This only works if there are the same amount of flags around it as the number of bombs around it.
So if a 3 is shown, you need 3 flags around the space to middle click. 
Then, everything around it is revealed and if one of them has no bombs then everything around that is revealed as well.

That's how minesweeper works.


Command to compile the code in Linux: 
  gcc `pkg-config --cflags --libs gtk+-3.0` Tile.c Board.c graphic_main.c

(I believe in FreeBSD remove the 'g' from gcc to be cc)

To run the compiled code:
  ./a.out


Known Problems:
- Need to comment my program
- Needs a menu to change the size or board and number of bombs
- Generally, everything that isn't the board looks bad
- Needs a way to style the graphics. I think it uses CSS and I don't know how to use that for this.
- Doesn't work in Windows and I don't know how to setup or use gtk in Windows.
- There are probably efficiency errors that are both known and unknown. \_(ツ)_/
