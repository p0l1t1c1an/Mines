# Mines
## Minesweeper clone using SVG images built in C using GTK+ 3.0
Initially a high school project for a CS course, but rewritten during the `Covid-19` pandemic.
Also note that I am no artist, so my SVG images might not be the nicest things to look at.
As of right now, I have only tested this on Arch Linux, but I will be testing it on FreeBSD.
<br/><br/>
## How to play
### The Basics
For some reason, people don't know how to play this amazing game, so I am leaving a basic set of instuctions.
Left click a tile to reveal the what is underneath it and Right click to flag the tile. 
Left clicking a nonbomb tile will reveal a number that represents the number of bombs surronding that space.
Left click a tile with no bombs around it will reveal everything around it as well, aka Recursion.
Left clicking a tile with a bomb makes you lose the game.
You can restart the game by clicking the face at the top of the window.
You win by revealing every tile that is not a mine.
When you win the face at the top puts on shades. 
(I plan on adding a small pop window to tell you that you won.)

### Secret move
Middle click! Some people, even some that I know play the game, don't know about this move in minesweeper.
You middle click an already revealed number tile to reveal everything around it that isn't flagged.
This only works if there are the same amount of flags around it as the number of bombs around it, i.e. the number shown.
So if a 3 is shown, you need 3 flags around the space to middle click. 

### The General Logic of the Game
Your ultimate goal in the game is to clear all tiles that are do not contain a mine. 
Each revealed tile will tell you how many mines surrond that block.
Using that information and some deduction, you can determine which tiles are a mine and those that are not.

### Useful Shortcuts

<img align="right" src="https://github.com/p0l1t1c1an/Mines/blob/assets/basic1.png?raw=true">

I have played this game quite a lot, but sadly I haven't prestiged yet. \
I have figured out some uses shortcut to look for when playing to mark. \
The image to the right shows a generic area on a game of mines, but if \
you have never played before you may have no idea what to do.

<img align="right" src="https://github.com/p0l1t1c1an/Mines/blob/assets/corner.png?raw=true">

Now, the image to the left reveals the 2 shortcuts, a simple one on the corner, and another I call a 121.
I think the corner is one is obvious, the 1 tile is only touching a single unrevealed tile, so it must be a mine.
As for the 121, this always works if nothing is interfering with the pattern. We know the tiles under the 1's are mines and the tile under the 2 is not, since it is the only possible way to satisfy both the 2 and the two 1's.  
<br/>

<img align="right" src="https://github.com/p0l1t1c1an/Mines/blob/assets/basic2.png?raw=true">
<br/><br/>

For the next shortcut, look at the game of Mines to the right.\
You can see both the corner and a 121 shortcuts. \
However, there is another one.

<img align="right" src="https://github.com/p0l1t1c1an/Mines/blob/assets/1_2_2_1.png?raw=true">

I call the shortcut shown in the left image a 1221. \
It works just like the 121 but instead the mines are located underneath the 2s and not the 1s.
It's the only possible solution without interferences like a wall next to the 1221.
There are definitely some other shortcuts in Mines, but these are what I find to be the backbound of what I use.
Many combination can be simplified to these, if you use your deduction!

<br/><br/>
## The Top Bar
### Menu
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

#### Make a directory to store the Images and theme.css

`mkdir /usr/local/share/Mines`

#### Then copy the data into the said directory

`cp -R /path/to/Mines/Images /path/to/Mines/theme.css /usr/local/share/Mines`

#### If you decide a to use a different path you will need to change this line in `Game.h`

`#define dir "/usr/local/share/Mines/"`

#### Then use this command to compile the code
  
``gcc `pkg-config --cflags --libs gtk+-3.0` Tile.c Board.c Game.c main.c``
<br/><br/>
## Known Issues
### Valgrind Invalid Read Size of 1
I have tested my program multiple times with different optimization levels.
Each time valgrind shows an invalid read size of 1 sourced to `reset` or `setup_num` in `Board.c`.
Optimization levels fast or s (in gcc) seem to have the most of these, and no optimization normally gives one or two. 
I'm not sure the cause behind this, and will be looking into it. 

### Memory Leaks 
When using valgrind, it shows that I have tens of thousands of bytes lost by termination, hundreds of thousands are possibly lost, and millions are still reachable.
My guess is that most of these are something to do with the Gtk and how it frees the memory it has been using for its widgets. 

### End Note
If you find any issues, the cause to one of the known issues above, or have an idea for a new feature, 
Please let me by submitting an issue or pull request. 
I would especially welcome a fix to my SVG images, in particular the faces, and any memory leaks that I could control.\
All are welcome to collaboration.
