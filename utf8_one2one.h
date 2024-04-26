/* utf8_one2one by Michele Deiana (github.com/mdeiana)
 * 
 * read UTF-8-encoded file and store contents in uint32_t
 * string in which the Nth UTF-8 character is placed as
 * the string's Nth element string[N].
*/

#ifndef UTF8_ONE2ONE_H
#define UTF8_ONE2ONE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* Each read string has an ID which can be used
 * to selectively print or free a string.
 * 
 * RETURNS uint32_t* pointer to string with contents of file
 * TAKES   char*     pointer to file name or path
*/
uint32_t *fileto32(char *file_name);

/* RETURNS uint32_t number of printed UTF-8 characters
 * TAKES   int      id of the string to print
 *         char*    NULL to print to stdout,
 *                  provide filename to print to file.
*/
uint32_t fprintf8(int id, char *file_name);

/* RETURNS uint32_t number of freed strings
 * TAKES   int      id of the string to free
*/
uint32_t free32(int id);

#endif