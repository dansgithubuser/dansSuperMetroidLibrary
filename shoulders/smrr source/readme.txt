This is a game maker file. you will need game maker 7 pro to run this properly
and a fair amount of knowledge of game maker.

Its up to you what you wanted to try with this but if you want to make your
own stuff and plan on showing it to me to put it then just make sure you
use a script for it and not touch the original code.

'binaryfile' is the modified rom used for the creation of the rom. it is imported in the EXE.
If you modify it you need to replace the file in the gmk file in the global setting menu.
make sure to set the options to what they were.
WARNING!! The file with the name 'binaryfile' is deleted everytime you run the EXE
so make sure that your backup is not name that.

----------------
scripts info
----------------

script0
	All the data for all the rooms and doors.

Script1
	All the data for all the item locations and what they are.

init
	Turn all the string data of script0 to a data structure.
	(grid data structure variable name 'data_bank')

init_item
	Turn all the string data of script1 to a data structure.
	(grid data structure variable name 'item_bank')

scr_hex_to_dig
	Will turn a 2 character string hex into a digit number.

add_point
	Script used for adding points/power up when a room is added 
	to the randomization.

get_a_door
	Return a random door within the room or in another room.(info in the script) 
	
get_a_room
	return a random room with the right amount of doors and a door in the right direction
	to fit in the previous room. also optionnal: look for a second door to connect into specifict room.

scr_reset
	unused

scr_room_restriction
	Check if some weird room restriction were met such as always coming from a
	certain direction, hell run stuff, etc

scr_rand_double
	script used to add a random room with 2 doors.(also possible to set it to add a 1 door room)

scr_rand_multi
	script used to add a random room with 3+ doors.

scr_add_freebie
	script used to add 1 or 2 random room with 1 door in linear mode.(power ups room)

scr_escape
	script used to generate the final escape

scr_ceres
	script used to generate ceres

scr_animals
	script used to add the animal room to the escape

scr_dig_to_hex
	turn a number from 0 to 255 to a hex number (return a string)

scr_text_file
	script used for debug mode when creating a text file.

readbit
	script used to read a bit within a byte (0s and 1s withing 0-255)

buildbyte
	script used to turn 0s and 1s into a 1 byte number

scr_read_seed
	script used to read the seed that is being used

scr_branch_set
	script used to decide what are the 4 branch in branch mode are going to be

scr_branch_restriction
	script used to check special room restriction that are only applied in branching mode

scr_saveroom
	script used when turning save room on

scr_route
	dont remember

scr_add_branch_end
	script used to conclud the end of a branch with the pre selected end.

scr_add_branch_restriction
	script used to add the room that will corespond to the last given branch end to force  the player to obtain that.

rbo_ridley
	script for adding ridley or torizo to hell run lvl 3

scr_hear_room_check
	script that return if a room is heated or not

scr_branch_point
	script used when adding a room with 3+ doors in branching mode

scr_switch_branch
	script used at the end of a branch to move back to a previous branching point and continue from there.

scr_debug
	script used for debug mode(put it where you want to test)

scr_button_check
	script used to clicking button in the program... dont worry bout it

scr_shortcut
	script used to add a short cut that leads back to the landing zone in branching mode

scr_no_monster
	return is a room as a way to refill hp or not(used for hell run)

scr_buggy_door
	make sure the red brinstar elevator top door is never anything else but blue or gray door
	(since it always shows up blue anyway.)

scr_item_location_rand
	script used to randomize the location of items

item_random
	randomize what the items are

scr_check_rooms
	make sure nothing is wrong with the item that were randomized

scr_major_random
	randomize major item and make sure nothing is wrong with they location

scr_check_item_enable
	makes sure a room with an item isnt used if it was disabled and make sure they are not in a room that
	requires that item to go in that room.

scr_item_disable_fix
	make sure an item that is forced isnt in a room that required a disabled item


	
----------------
objects info
----------------

object0
	the main item where all the generation happen

obj_create_from_seed
	object used for the button press to create from a seed

obj_create_random
	object used for the button to create from a random seed

obj_paste
	object used for the button to paste a seed from clip board

obj_extract
	object used for the button to extract seed from a rom

obj_seed
	object used for the button to the seed manual typing

obj_copy
	object used for the button to copy the seed to the clipboard

obj_prog
	object used for the button to change progressive mode

obj_extra_button
	object used for the button to open the extra menu

object_rand_location
	object used for the button to randomize the item location

obj_seed_name
	object used for the button to add the seed at the end of the file

obj_hell_run
	object used for the button to change the hell run level

obj_notyet
	was used to show that the feature was no yet implement

obj_branch
	object used for the button to switch branch mode

obj_rand_item
	object used for the button to switch random item

obj_file
	object used for the button to change file name

about
	object button to show the about and change log

obj_extra
	object to set the extra stuff when done

obj_cancel
	object used for the button to cancel seed generation

object22
	object used to show more info with progressive off


----------------
object0 variables info(usefull for file generating)
----------------


minor_random
	variable to tell if minor items are randomize 0=no 1=yes

major_random
	variable to tell if major items are randomize 0=no 1=yes

random_location
	variable to tell if item location are randomize 0=no 1=yes

hard
	variable to tell if progressive is on or off 0=on 1=off

branching
	variable to tell what type of run this is 0=linear 1=branching 2=50 room 3=150 or 135 rooms

hell_run
	variable to tell what level of hell run this is  0-3

save_on
	variable to tell if saves are on or off 0=no 1=yes

force_heat
	variable used to force heated room in branching mode after a branch with varia is completed.

shuffling
	variable to tell the program that it is currently in the process of generating a new file

button_on
	used in the debug script i beleive	

no_gravity
	variable to make sure the gravity doesnt show up at the beginning of a branch in branching mode

global.no
	variable to make sure you can not press any button at the same time or while generating.

add_seed_name
	variable to tell if need to add seed to the file name

show_text
	variable of the text that needs to be shown in the box

check_plasma
check_spring
check_space
check_spazer
check_varia
check_ice
check_boots
check_grapple
check_speed
check_wave
check_screw
check_gravity
	variable to tell if item are forced or not  0=enable  1=disable 2=forced

goto_door1 to 7
	variable used to tell if a door was used in the room or not -1=nothing 0+= that door is used

from_door
	same then goto_door but shows what door you came from

d_room
m_room
s_room
	number for double/multiple/single doored room used (dont worry bout it)

no_power no_super
	dont remember

max_room
	the maximum number of room to be used during linear mode

branch_count
	number of branchable rooms using during branching mode

ridley
	used for hell run lvl 3 to tell if ridley was put in or not 0=no  1=yes

torizo
	used for hell run lvl 3 to tell if golden torizo was put in or not 0=no 1=yes

plasma
	used to tell if plasma was given yet

spazer
	used to tell if spazer was given yet

gravity_suit
	used to tell if gravity suit was given yet

real_varia
	used to tell if varia was given yet(used to check if enough e tank at mother brain)

varia
	used to tell if gravity or varia was given (used to open heated room)

screw
	used to tell if screw attack was given

grapple
	used to tell if grapple beam was given

wave
	used to tell if wave beam was given

ice
	used to tell if ice beam was given

speed_booster
	used to tell if speed booster was given

boots
	used to tell if high jump boots was given

space jump
	used to tell if space jump was given

missile
	used to tell how many missile pack were given

bombs
	used to tell if morph,bombs and charge was given

super
	used to tell how many super missiles were given

power
	used to tell how many power bombs were given

door_slot
	used to tell what door code to use next (for colored doors)

door_free[array]
	code for the door to use in conjonction with door_slot

energy
	used to tell how many energy tank+ reserve was given reserve count has .5

real_energy
	used to tell how many energy tank was given (not counting reserve tank)

add_super_door
	force the next branch door to be a super missile door during branch mode
	(if super missile was an option during branch mode)

add_power_door
	force the next branch door to be a power bomb door during branch mode
	(if power bomb was an option during branch mode)

spring_ball
	used to tell if spring ball was given

hell_room
	used to tell if some heated room were given recently

current_room
	current room number at hand

room_to
	next room to be connected to current room

used_stuff
	list data structure variable to tell what room was used

data_bank
	grid data structure of all the rooms + doors

item_bank
	grid data structure of all the items

door_color[2d array]
	what code to used for different color and direction  first number 0=missile 1=super 2=power second number 0=left door 1=right 2=up 3=down

item_hexcode[2d array]
	code of all the item for different position.

free_gravity_room
	list data structure for rooms that are ok for gravity suit when it is used as an option in branching mode and major item randomization

free_grapple_room
	same as free_gravity_room but grapple

free_item_room
	same as free_gravity_room but for other major items

file
	current file id to be writting on

super_room1 super_room2
	super missile room for branch mode

power_room1 power_room2
	power bomb room for branch mode

speed_room
	speed booster room for branching mode

speed_dir
	speed booster room door direction

space_room
	space jump room for branching mode

space_dir
	space jumpp room door direction

spring_ball_room
	spring ball room for branching mode

spring_ball_dir
	spring ball room door direction

wave_room
	wave beam room for branching mode

wave_dir
	wave beam room door direction

varia_room
	varia room for branching mode

varia_dir
	varia room door direction

gravity_room
	gravity suit room for branching mode

grapple_room
	grapple beam room for branching mode

grapple_door1 grapple_door2
	grapple beam door direction 1 and 2

item_list
	list data structure for branching item list (only first 4 matter)



if you have any question... twitch.tv/lioran