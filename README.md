# utf8_one2one, index array of UTF-8 multibyte chars 1:1
Access Nth Unicode Multibyte Character in `string` as `string[N]`.

## Quick, how do I use this lib?
```c
int main(void){

	uint32_t *string;
	string = fileto32(FILE_NAME);	// read string from file and get pointer

	string[8] = 0xE282AC;		// or whatever operation on chars
	fprintf8(0, NULL);      	// print string to stdout as UTF-8

	free32(-1);			// free all strings and library objects.
	return 0;
```

### Okay, a bit less quick with a bit more comments
```c
#include "utf8_one2one.h"
#define FILE_NAME "Those darn UTF-8s!.txt"      	// Get the name however you please

int main(void){

	uint32_t *string;
	string = fileto32(FILE_NAME);			// string contains content of file

	/* ...
	 * do whatever operation with single chars (up to 4 bytes)
	 * encode the whole UTF-32 Universal Codepoint to hex/binary value and assign
	 * ...
	*/

	string[8] = 0xE282AC;				// to exemplify, change the 9th element to €
							// if necessary, you can print, will soon
	fprintf8(0, NULL);				// support choosing output stream for sure

	free32(0);					// remember to free with free32, this
							// takes care of the string and then
	return 0;					// also frees the objects used.
}
```

Remember to free. Seriously. You can run `-1` as an argument to `free32` to free all objects and strings, like so:
```c
free32(-1); // all strings and objects are gone
```
This brings us to support for having multiple strings at once, all kept track of for the purpose of freeing the memory properly afterwards. Right now, calling `fileto32` more than once simply frees the previous string and re-constructs the object that's used to keep track of it.

Multiple object instances are halfway there, but there still is a dynamically-resized array to keep track of, for when deleting one random object instance `x` among `N` instances will require you to move `N-1` other instances.

Plus there's the whole thing with freeing the respective buffers, which is implemented, even when you try to read more than one string, it will free the old `uint32_t string` and re-use the one instance.

The library *should be* memory safe. I paid it some mind, but if you find anything let me know.
As long as, you know, you `free32(-1);`.

And you'd better, because dynamic allocation of the internal uint32_t array is **not** there yet. So we just read the size of the file and 4x it. Yeah.

But to be honest, even if you're logging relatively large logfiles (in UTF-8 for whatever reason), say 250 MB, it will take roughly a GB in your RAM.

That's pretty terrible but what's the average text file size? Besides, I'm not even sure 1GB is that much for RAM anymore...

## The reason?
I was coding up a quick ASCII game for fun but for days I stood there trying to get the damn UTF-8 characters to render properly on the terminal.

While that turned out to be a config issue, I had to devise a way to turn a small drawing in a text file encoded in pesky UTF-8 into the background for the little game.

That is one of the reasons you'd want to use 1:1 indexing, I'm sure.
So one day I had to put something up on GitHub so I could link it in a resume (almost) without shame. And I guess I readapted that as a library, in the style of C you see in the Linux kernel.

Was fun.

But remember:
# when UTF-8 chars ain't no fun :(
# cut them up with one2one! :D
# if UTF-8 ain't fun to count >:(
# one2one's the right amount! :D
