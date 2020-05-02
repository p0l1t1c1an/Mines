# Mines
## Minesweeper clone using SVG images built in C using GTK+ 3.0.
Initially a high school project for a CS course, but rewritten during the `Covid-19` pandemic.
Also note that I am no artist, so my SVG images might not be the nicest things to look at.
As of right now, I have only tested this on Arch Linux, but I will be testing it on FreeBSD.
<br/><br/>
## How to play:
### The Basics:
For some reason, people don't know how to play this amazing game, so I am leaving a basic set of instuctions.
Left click a tile to reveal the what is underneath it and Right click to flag the tile. 
Left clicking a nonbomb tile will reveal a number that represents the number of bombs surronding that space.
Left click a tile with no bombs around it will reveal everything around it as well, aka Recursion.
Left clicking a tile with a bomb makes you lose the game.
You can restart the game by clicking the face at the top of the window.
You win by revealing every tile that is not a mine.
When you win the face at the top puts on shades. 
(I plan on adding a small pop window to tell you that you won.)

### Secret move:
Middle click! Some people, even some that I know play the game, don't know about this move in minesweeper.
You middle click an already revealed number tile to reveal everything around it that isn't flagged.
This only works if there are the same amount of flags around it as the number of bombs around it, i.e. the number shown.
So if a 3 is shown, you need 3 flags around the space to middle click. 

### The General Logic of the Game
Your ultimate goal in the game is to clear all tiles that are do not contain a mine. 
Each revealed tile will tell you how many mines surrond that block.
Using that information and some deduction, you can determine which tiles are a mine and those that are not.

### Useful shortcuts
(I will add these after uploading images of them)
<br/><br/>
## The Top Bar
### Menu: 
On the top right there is text that says `Menu`
Clicking on it reveals 2 sliders that correspond to the size of the board.
And another one for the number of mines on the board.
Each slider says their value above them.
The default values are 10 tiles for the width and height and 20 mines in the game. A 1:5 ratio.
Clicking the button toggles visibility. 

### Face
Clicking the face will reset the size of the board and restarts the game. 
This updates with the values of the sliders in the menu, changing the size of the board and adding more mines.

### Exit
On the top left there is text that says `Exit`
Clicking that text will close the window for `Mines` to end the game.

### Clock
In between the Exit button and Face, there is a clock counting up each second that the current game.
This value is reset with every press on the face. 

### Counter
In between the Face and Menu button, there is a counter with the number of bombs.
Each time you place a flag, the counter goes does down and when you remove a flag, the counter goes up. 
This is to keep track of how many mines aren't flagged. 
<br/><br/>
## Installation
I rewrote this from my original class project, because I wanted it to run on my FreeBSD system. 
As of right now, there is no Makefile to install the project.
So for the time being follow the following steps.

#### Make a directory to store the Images and theme.css:

`mkdir /usr/local/share/Mines`

#### Then copy the data into the said directory:

`cp -R /path/to/Mines/Images /path/to/Mines/theme.css /usr/local/share/Mines`

#### If you decide a to use a different path you will need to change this line in `Game.h`:

`#define dir "/usr/local/share/Mines/"`

#### Then use this command to compile the code: 
  
``gcc `pkg-config --cflags --libs gtk+-3.0` Tile.c Board.c Game.c main.c``

