# Mines
Minesweeper built in C using GTK+ 3.0 as the gui.
This was initially a high school project for a CS course.
So far it only works in Linux based systems with GTK.

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

Menu and Exit: 
On the top right there is text that says "Menu".
Clicking on it reveals 2 sliders that correspond to the size of the board.
And another for the number of mines on the board.
Each slider says their value above them.
Clicking the face to reset will change the size of the board. 
(May freeze for a split second rendering more tiles)
On the top left there is text that says "Exit"
Clicking that text will close the window for mines, ending the game.


Command to compile the code in Linux: 
  
``gcc `pkg-config --cflags --libs gtk+-3.0` Tile.c Board.c main.c``

To run the compiled code:
  
`./a.out`
  
To create a symlink of the executable so that the game can be run as shell command:
  
`ln -s /path_to_executable/a.out /usr/local/bin/mines`

Then you can run the game by typing the following into the terminal:

`mines`
