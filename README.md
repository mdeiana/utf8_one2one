# utf8_one2one, index array of UTF-8 multibyte chars 1:1
Access Nth Unicode Multibyte Character in `string` as `string[N]`.

## Quick, how do I use this lib?
```c
#include "utf8_one2one.h"
int main(void){

	uint32_t *string;
	string = fileto32(FILE_NAME);	// read string from file and get pointer

	string[8] = 0xE282AC;		// or whatever operation on chars
	fprintf8(0, NULL);      	// print string to stdout as UTF-8

	free32(-1);			// param -1 frees *all* strings and library objects.
	return 0;
}
```

### A few more details
```c
#include "utf8_one2one.h"
#define FILE_NAME "Those darn UTF-8s!.txt"   // Example file name

int main(void){

	uint32_t *string;
	string = fileto32(FILE_NAME);			// string contains content of file

	/* ...
	 * do whatever operation with individual UTF-32 encoded chars (up to 4 bytes),
	 * e.g. encode the whole UTF-32 Universal Codepoint to hex/binary value and assign;
	 * if the original UTF-8 character uses X < 4 bytes (as is frequent), the
	 * uint32_t will have the 4 - X least significant bytes set to 0 (small-endian)
	 * ...
	*/

	string[8] = 0xE282AC;			// to exemplify, change the 9th element to '€'
									// if necessary, you can print to stdout; support
	fprintf8(0, NULL);				// for choosing output stream is to be implemented

	free32(0);					// remember to free with free32, this
								// takes care of the string and then
	return 0;					// also frees the objects used.
}
```

#### Freeing memory
Each new loaded file will exist as a uint32_t string loaded in memory and be given an incremental ID starting from 0.
You can run `-1` as an argument to `free32()` to free all objects and strings:

```c
free32(-1); // all strings and objects are gone
```

WIP: Right now, calling `fileto32` more than once simply frees the previous string and re-constructs the object that's used to keep track of it.


#### Future additions
Multiple object instances are halfway there, but there still is a dynamically-resized array to keep track of, for when deleting one random object instance `x` among `N` instances will require you to move `N-1` other instances.
For now, when you try to read more than one string, the library will free the old `uint32_t string` and re-use the one instance.

Dynamic allocation of the internal uint32_t array is **not** there yet. So we just read the size of the file and allocate 4x it in memory.

This means that if you're logging relatively large logfiles (in UTF-8 for whatever reason), say 250 MB, it will take roughly a GB in your RAM.

Another memory optimization strategy could be to keep an internal table to keep track of which UTF-8 characters use 3 bytes or more (the vast majority uses 1 or 2) to cull the empty `00000000` bytes in memory, drastically reducing memory consumption even with a static approach.

## Why does this exist?
I was coding up a quick TUI game with a student as a learning aid but it became non-trivial to manage a 2D array of UTF-8 chars which represented a top-down map of the playable area, especially when chars need to be shifted or swapped.
The simplest solution is to change the encoding to UTF-32 where 4 bytes always represent one on-screen character, and simply encode back to UTF-8 when outputting to stdout.

That is one of the reasons you'd want to use 1:1 indexing, I'm sure.
I later re-adapted that part of the game logic as a library, in the "object-oriented" style of C you see in the Linux kernel.

Was fun.




##### Quick poem to end the day
when UTF-8 chars ain't no fun :(
cut them up with one2one! :D
if UTF-8 ain't fun to count >:(
one2one's the right amount! :D
