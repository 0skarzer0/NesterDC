NesterDC SE
Version: 1.0
Author: Scherzo
Released: 2005-12-10
http://www.sch3rz0.com/nesterdcse/

QUICK NOTE: This README has been formatted to 60 characters
a row instead of the typical 80. This allows it to be easily
readable when shown using the Dreamcast on a typical TV.

*** Description ***
NesterDC SE is an Nintendo Entertainment System emulator for
the Sega Dreamcast. It is based on NesterDC 7.1 most
recently worked on by Takayama Fumihiko. Other credits can
be found at the end of this docment.

*** Requirements ***
 - Sega Dreamcast console capable of booting CDRs.
 - Windows XP (Linux and OSX support for CD builder coming
   soon)
 - Blank CDR
 - A collection of NES ROMs (do not ask me how to get these)
 - An internet connection
 - Plenty of hard drive space for temporary files, CD
   images, etc.. A couple of gigbytes should be enough.

*** Preface from the Author ***
Many questioned me when I decided to work on an emulator
many considered more than adequate. I had two main purposes
for the creation of this newest incarnation of NesterDC.

First, improve compatibility and speed. The release of
NesterDC 7.1 brought seemingly fullspeed emuation, but
somehow compatibility dropped a couple of notches. Many
games no longer worked that were fine in version 6.

Second, I wanted to see an emulator that had many useful
extras such as screen shots, NSF music, cartridge scans, a
favorites list, and more.

In the end, I have accomplished my goals. The result is
an emulator that is, at the core, functionally an
improvement over its predecessors but has a uniqueness
that can easily be appreciated.

*** Features Overview ***
 - Fullspeed emulation on 99.9% of games
 - The most compatible Nester based emulator on any platform
   including PC
 - Various game related images by which you can use to
   browse the ROM libary
 - Configurable stereo separation of each NES sound channel
 - An NSF player
 - Insanely configurable controllers
 - Full screen sretched and pixel-for-pixel video modes
 - Built in Game Genie codes for many games
 - Full color instruction manuals for many games
 - User definable "favorites" lists
 - Zipped ROMs
 - Multiple save state slots (with the first slot being
   backwards compatible)
 - VS games correctly work
 - Ability to enter your own personal notes about a game
   and save these notes on your VMU

*** Building a Selfboot CD ***
Although NesterDC SE has many feautres it still happens to
be very easy to get up and running. None of the features
such as screenshots, Game Genie codes, etc. require you to
have anything other than NES ROMs. All other extra content
is downloaded directly from the NesterDC SE website.
Furthermore, only the content for the games you actually
have is downloaded. In the end, you have a NES library
customized to your likings.

Unzip the NesterDC SE zip archive if you haven't already
done so. In the resulting folder you will find a file named
"NesterDCSEBuilderWx.exe". Double-click this file to
start the CD builder interface and follow these
instructions:

1. Locate your ROMs folder. Your ROMs must not be in
   subdirectories of the folder you choose. Only ROMs in
   that folder itself will be scanned. Your ROMs may be
   zipped or unzipped. The CD builder will automatically zip
   your uncompressed ROMs for you.

2. Ignore the "Master Library URL" unless you have the URL
   of a known master library mirror. Check the NesterDC SE
   website periodically for official mirrors.

3. Uncheck the unwanted features. By default, all are
   chosen. If you are worried about not being able to fit
   all your ROMs and these features, don't worry. You can
   easily fit all USA releases on one CD with room to spare.

4. Select the CD image format you want the builder to
   finally produce, DiscJuggler (CDI) or Nero (NRG).

5. Click the "Start Build" button. 
   
   Asuming you are loading your CD to the limits with games,
   be prepared for this process to take a fairly long time.
   Keep in mind that each of your ROMs is being scanned,
   identified, perhaps zipped if not already, and then all
   the extra goodies are being downloaded from the internet.
   Be patient. The length of time needed to build the CD is
   mostly dependant on the speed of your internet connection.
   
   If you must stop it in the middle of its work, just click
   the same button you used to start the process, which
   should now read "Stop Build". Wait for the program to
   gracefully end its current task.

6. Once the process is complete, the builder will attempt
   to open the resulting CD image in the associated burning
   program. This might be Nero, DiscJuggler, or whatever
   program handles the kind of image format you chose.

7. Burn the CD and pop it in your Dreamcast. If all went
   well then you should see the NesterDC SE loading screen
   after a short wait.

*** Using NesterDC SE ***
Once NesterDC SE has finished booting and loading you'll
be presented with two main options: "Browse Library" and
"Options And More". In general, the A button makes a
selection and the B button goes to the previous screen.

*** Options and More ***
--- Input ---
First, choose the controller you want to configure, 1, 2,
3 or 4.

Use Analog: If set to Yes, the DC analog controller is used
	to control the emulated D-Pad for the NES. Otherwise the
	DC's D-Pad is used.

Auto-Fire (A, B) Speed: Change the speed of the rapid fire
	buttons. Your options are Slow, Medium, Fast and Insane.

The rest of the controller options are the mappings from
NES/Emulator functionality to DC controller. Pressing the A
button on one of these options will allow you to edit that
mapping. During this time you are given 2 seconds to press
the button(s) you want to use for this NES/Emulator
function. You can press any combination of buttons to equal
that function. For instance, you may desire the "Quick Save"
function to be a combination of the L + R + A and the
"Quick Load" function to be a L + R + B. That's fine.
Whatever feels most comfortable to you.

--- Video ---
Fullscreen Stretch: If enabled, the NES image of 256x224	is
	stretched across the full DC's resolution of 640x480.
	If disabled the NES image of 256x224 is doubled and
	placed within the center of the DC's resolution of
	640x480. This results in a black border around the NES
	image. Those who are adverse to the way bilinear
	filtering looks may find this a clearer and more
	pleasant image, especially on a VGA monitor.

Bilinear Filtering: If the Fullscreen Stretch option is
	enabled, this option will use the DC's hardware bilinear
	filtering to interpolate the NES's 256x224 image over
	the DC's 640x480 resolution. If disabled a
	nearest-neighbor method is used which produces a
	perceivably less accurate rendition of the NES screen.
	
Show FPS: If enabled, the frame rate is shown at the
	top-left while playing games. This is given as a ratio
	of the number of rendered NES frames to refresh rate
	of the DC screen, which is 60 for NTSC and 50 for PAL.

NES Type: This setting determines the refresh rate for the
	emulated NES. Basically, if want to play a European
	PAL game, set this to PAL. If you want to play an NTSC
	game, set this to NTSC. This ensures that the game
	plays at the correct speed, not too slow or too fast.

Adjust Screen: Pressing A on this option will allow the
	user to adjust the position, width and height of the
	displayed screen. This affects the menu as well as the
	Fullscreen Stretch mode of gameplay.

--- Lists ---
NesterDC SE has the ability to create your own personal
lists that you can use to categorize your game collection
by various criteria you choose. By default, NesterDC SE
comes with one list called "Favorites". However, you might
choose to create a list for games you've beaten, games you
hate, action game, RPGs and so on. It's up to you.

Add List: Upon choosing this option you will be presented
	with an on-screen keyboard. Use this to type out the
	name of your new list.

Delete List: Choosing this option will allow you get rid of
	a configured list, even the default "Favorites" list.

-- View README ---
Read this document on your DC.

Leaving the options menu saves the current options to the
first VMU connected to the Dreamcast.

NOTE ABOUT VMU SAVING IN GENERAL:
All VMU actions are always carried out on the first and
left-most VMU. This means that if you have VMUs in both
controller A and B, the VMU in controller A will always be
used. In addition, the VMU in slot 1 will always be used
over anothter in slot 2.

*** Browse Library ***
The library browser uses images of various types to traverse
the your games collection. These include Title Screens,
In-Game Screens, Cartridge Scans, and Box Scans. To switch
the type you are currently viewing press up or down on the
D-Pad.

To browse the library a game at a time press left or right
on the D-Pad to change the current game. The L and R
triggers can be used to rapidly change games. When using
the triggers the current letter of the alphabet is
displayed instead of the game images. This allows you to
roughly gauge your point in the collection.

Pressing the Y button will cycle through the various lists
you have defined in the Lists section in the Options area.

Pressing the B button will take you back to the main menu.

Press the A button when you found a game whose library
entries you want to view.

*** Library Actions Menu ***
Once you have selected a game you will be presented will
various actions. The number of actions available can differ
from game to game depending on what content was available
on the NesterDC SE website at the time of building your CD.

Play Game: Choose this to start emulation of the game.

Add To / Remove From (List): The number of List actions
	available depends on the number of list you have
	configured in Options. This is where you add or remove
	the selected game from the various lists.

View Manual: If available, this action allows you to view
	the game's instruction manual as a series of high
	quality images. Read the "View Manual" section later
	in this document for information on how to function
	the Manual Viewer.

Listen to NSF: If available, this action will start the
	NSF Player feature and play this game's sondtrack.
	Read the "NSF Player" section later in this document
	for more information on how to function the NSF Player.

Pressing the B button while on the actions menu will take
you back to the game selection view.

*** Playing a Game ***
Once you have chosen to play a game from the actions menu
NesterDC SE will read the game into memory and emulation
will start. Controlling the NES game will depend on how you
have configured the controller in the Input section of the
Options menu and each game's individual operating
instructions. If you have trouble understand a given game
read its instruction manual, if available, or look on the
internet for documentation for that game. A good site to
look first is GameFAQs.com

Although compatibility has been vastly improved it is still
not perfect. There are several games that do not work for
various reasons. If you encounter a game that doesn't
function or functions incorrectly, please contact the author
of NesterDC SE by looking at the Contact section of the
NesterDC SE website. The URL is available at the top of
this document.

*** In-Game Menu ***
After you have started playing a game, the only way to
return to the menu system is to first access the In-Game
menu. You do this either one of two ways.

If you are using the D-Pad for NES D-Pad input, pressing
Left on the analog stick then pressing Start will bring
up the In-Game menu.

If you are using the analog stick for NES D-Pad input,
pressing Left on the D-Pad then pressing Start will bring
up the In-Game menu.

Here are the different actions that can be taken on the
In-Game menu:

Return To Game: Selection this action will take you back to
	the execution of your currently loaded game. This is
	same as pressing the B button while in the In-Game menu.

Reset: Preforms a soft-reset of the emulated NES system.
	The game's SRAM remains intact.

Save SRAM: Choosing this action will save the game's SRAM
	to the VMU.

Save Slot: Select the slot, 0-9, with slot 0 being backwards
	compatible with NesterDC 7.1 and perhaps version 6 as
	well.

Save State: Save the current execution state of the game to
	the VMU to the currently selected slot number.

Load State: Load the current execution state of the game
	from the VMU from the currently selected slot number.

Game Genie: Enter the Game Genie edit screen. For
	information on how to use this screen read the Game
	Genie section later in this document.

Edit Notes: Chosing this action will allow you to edit your
	own personal notes for a given game. This is a great
	place to keep passwords and other various information
	about a game.

Main Menu: Return to NesterDC SE's main menu. Once in the
	main menu, pressing B will take you back to the In-Game
	menu.

*** Instruction Manual Viewer ***
Working the manual viewer is quite easy.

D-Pad Right: Next Image
D-Pad Left:  Previous Image
D-Pad Up:    Rotate Left
D-Pad Down:  Rotate Right
X Button:    Reset Rotation
L Trigger:   Zoom Out
R Trigger:   Zoom In
Y Button:    Toggle Filename
B Button:    Exit Viewer

*** NSF Player ***
If you're not in quite the mood for playing a game and just
feel like jamming to some NES musical goodness, the NSF
Player feature is for you.

Control-wise, the NSF Player consists of various buttons
and sliders that can be selected by using the D-Pad.
A selected control pulses.

When the player first starts it begins playing the first
track of the selected game. The buttons on the bottom
work like those on a typical media player, CD player, etc.
Pressing up or down switches between the sliders on the
upper right of the screen and the buttons at the bottom.

The sliders at the upper right of the screen represent the
left/right pan setting for each of the NES' 6 sound
channels. When you have a slider selected pressing left or
right on the D-Pad will adjust that channel's pan. Pressing
A will center the channel and pressing Y will set it to the
default.

At the bottom right of the screen you see the current VU
levels for each of the NES channels.

While you are listening to an NSF the player will cycle
through the game's different images, giving you something
to look at while bobbing your head to the beat.

You exit the NSF Player by pressing the B button. If you
exit the player while a track is playing it will continue to
play throughout the menu system. This allows you to do
things such as read the game's manual or find another game
while listening to music. One thing to note: the graphical
performance of the menu system suffers while an NSF is
playing in the background.

*** Game Genie ***
The Game Genie edit screen is broken into three parts.
The upper left list the codes that exist for this game.
The uperr right lets you add and remove your own custom
codes. The bottom right is the Game Genie keyboard used for
entering your custom codes.

NesterDC SE automatically provides codes for many games.
Scroll through the list of codes and locate one you want to
use. Press the A button. This enablesthat codes and this is
indicated with a green check mark placed next to that code.
Pressing A again will turn off that code.

To add your own codes press right to reach the action menu.
Choose Enter Code. You now may use the lower keyboard to
enter the code. Instructions are provided on the left next
to the keyboard. When done, press Start. Keep in mind that
NES Game Genie codes are always 6 or 8 characters long.

You now may enter a description for the code. Again follow
the directions on the left. When done, press Start. You
will now see your new code listed with the other existing
codes and already enabled.

To exit the Game Genie screen, press B.

*** Credits ***
Now for the long history of people involved in the
development of Nester from beginning to end.

Darren Ranalli - Original author of Nester for Windows
	http://nester.dranalli.com/

Takeda Toshiya - Unofficial Nester
	http://www.emulation9.com/takeda/

Screw & Mikami Kana - NesterJ
	http://www.dcn.ne.jp/~otim2/nesterj/en/

Ken Friece - NesterDC versions 1 - 3
	http://nesterdc.emulation64.com/

Takayama Fumihiko - NesterDC versions 4 - 7.1
	http://www.pqrs.org/~tekezo/nesterdc/

Warmtoe - NesterDC 7.1+B (Lightgun Support)

Menu graphics by braze

Special thanks for advice and help from these devers
and testers:

Quzar
BlackAura
GyroVorbis
DaMadFiddler
Demonious

Credits to libraries, tools and sites used in development:

KOS
	http://gamedev.allusion.net/softprj/kos/

KJS JavaScript Engine
	http://developer.kde.org/language-bindings/js/

Screenshot Archive
	http://www.screenshotarchive.com/

wxWidgets
	http://www.wxwidgets.org/

JPEG Slideshow by Jon Reeser

Thanks to everyone at the DCEmulation.com message boards
for all the support and ideas.

This document was last edited 2005-12-05
