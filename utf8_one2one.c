#include "utf8_one2one.h"

/* Chunk size is a tuning parameter for an unimplemented feature.
 * UEOF is (uint8_t)EOF, which evaluates to 255.
 * It's used to check for EOF, duh.
*/
#define CHUNK_SIZE 128
#define UEOF 255

/* ================== Objects ==================== */

struct chunkObj;                            // class forward declaration

struct chunkOps {                           // vtable for class.
    void (*open)(struct chunkObj*, char*);  // technically unnecessary
    int (*scan)(struct chunkObj*);          // since we only have one class
    int (*close)(struct chunkObj*);         // but it's good practice  
    int (*free_buffer)(struct chunkObj*);

    int (*extend)(struct chunkObj*);
    int (*print_buffer)(struct chunkObj*);
};

struct chunkObj {
    uint32_t id;
    uint32_t size;                          // size of the file, we need to
    uint32_t (*buffer);                     // wastefully alloc size*4

    FILE *fs;

    struct chunkOps *vtbl;
};

static struct chunkOps vtable;              // declaring static-storage objects
static struct chunkObj *instances = NULL;   // we'll need to handle multiple
static int instances_num = 0;               // instances of class

/* ================== Functions ==================== */

int8_t bytesInChar(uint8_t ch);
void printb(uint32_t n, uint8_t size);
int chunkOps_scan(struct chunkObj*);

// get file stream, set chunk.fs to NULL if failed
void chunkOps_open(struct chunkObj *chunk, char *file_name){

    // open file, check success
    FILE *fs = fopen(file_name, "r");
    if(!fs){
        perror("Error opening file: ");
        return;
    }

    fseek(fs, 0L, SEEK_END);
    chunk->size = ftell(fs) + 1; // add 1 byte for null-termination
    rewind(fs);

    chunk->buffer = calloc((chunk->size), 4);
    chunk->fs = fs;

    return;
}

int chunkOps_close(struct chunkObj* chunk){
    fclose(chunk->fs);
    return 0;
}

int chunkOps_freeBuf(struct chunkObj* chunk){
    free(chunk->buffer);
    chunk->buffer = NULL;
    return 0;
}

/* Frees chunk instance with id `id`.
 * If `id == -1` free all instances.
 * Returns the number of freed instances.
 * Note that it quickly gets messy when downsizing
 * dynamic contiguous arrays. Will need to add
 * that to the to-do list. How great.
*/
uint32_t free32(int id){
    int i = 0;
    // insert id check here
    if(id != -1){
        id = 0; // lmao
        instances[id].vtbl->free_buffer(instances + id);
        free(instances + id);
        i = 1;
        // array resizing goes here
    }
    else{
        for(i = 0; i < instances_num; i++){
            instances[i].vtbl->free_buffer;
        }
        free(instances);
    }

    return i;
}


/* File pointer must be correct to use function.
 * Also, this isn't implemented yet and might never be.
 * Lol.
*/
int chunkOps_extend(struct chunkObj *chunk, FILE *file){
    // open file
    printf("To be implemented for progressive "
            "reallocation strategy");
    
    return 0;
}

void chunk_constructor(struct chunkOps* vtable, struct chunkObj* chunk){
    //vtable->extend = chunkOps_extend;
    //vtable->print_buffer = chunkOps_print;
    vtable->print_buffer = NULL;
    vtable->extend = NULL;
    vtable->open = chunkOps_open;
    vtable->scan = chunkOps_scan;
    vtable->close = chunkOps_close;
    vtable->free_buffer = chunkOps_freeBuf;
    
    chunk->id = 0;
    chunk->size = 0;
    chunk->buffer = NULL;
    chunk->vtbl = vtable;
}

uint32_t* fileto32(char* file_name){
    
    /* check if instance of chunkObj already exists
     * if it does, just print a warning. In the future,
     * we'll refactor this code to support an undefined
     * number of object instances.
     * lines starting with ! represent future changes
    */
    if(instances != NULL){
        printf("\nWarning: multiple file streams aren't currently supported.\n"
                "uint32_t string is deallocated and stream reinitialized.");
        instances[0].vtbl->free_buffer(instances);
        printf("\nNevermind, I got it. Lol");
        free(instances);
        instances = NULL;
        instances_num--;
    }
    // get new instance
    struct chunkObj *new_instance = calloc(1, sizeof(struct chunkObj));
    if(!new_instance){
        printf("\nInternal error: failed allocating memory for chunk object.");
        return NULL;
    }
    // ! new instance = realloc(instances, instances_num+1, sizeof(struct chunkObj));
    // ! if (instances_num == 0)
    instances = new_instance;
    // ! else do nothing. from now on, use instances[instances_num].
    instances_num++;

    // construct both instance and vtable
    chunk_constructor(&vtable, &instances[0]);

    // open file, allocate memory, check failure
    (instances[0].vtbl)->open(&instances[0], file_name);
    if(instances[0].fs == NULL){
        return NULL;
    }

    // start scanning file into string, don't leak memory
    // in case of exception!
    if(instances[0].vtbl->scan(&instances[0])){
        instances[0].vtbl->free_buffer(&instances[0]);
        return NULL;
    }

    instances[0].vtbl->close(&instances[0]);
    return instances[0].buffer;
}

int chunkOps_scan(struct chunkObj* chunk){
    uint8_t ch = 0;
    int8_t byteN = 0;
    int i = 0;

    // fetch first byte
    if((ch = fgetc(chunk->fs)) == UEOF){
        printf("\nError: file appears to be empty.\n");
        return 4;
    }

    for(i = 0; i < (chunk->size); i++){

        (chunk->buffer)[i] = 0;
        if(((byteN = bytesInChar(ch)) != -1) && (byteN != 0)){
            for(int c = 0; c < byteN; c++){
                (chunk->buffer)[i] += ch;
                // see if this check is even necessary
                if(byteN != (c+1))
                    (chunk->buffer)[i] <<= 8;

                if((ch = fgetc(chunk->fs)) == UEOF){
                    i++;
                    goto succ;
                }
            }
        } else {
            if(byteN == 0){
                printf("\nWarning: null character 0x0 found at Iteration %d.", i);
                printf("\nString will terminate here. Remove null character from file"
                        " if you wish to read further.\n");
                goto succ;
            }
            printf("\nError: misalignment in input file at Index %d", i);
            printf("\nRead byte is: "); printb((uint32_t)ch, sizeof(ch));
            printf("\nAborting...\n");
            return 1;
        }
    }

    // null-terminate string
    succ:
    (chunk->buffer)[i] = (uint32_t)0;
    return 0;
}

/* The id parameter will be needed in the future, once
 * multiple instances of chunkObj are supported, to select
 * which buffer to print, if more files have been opened.
 * Also, fprintf8 should be front facing, and should just
 * call instances[id].vtbl->print_buffer(&instances[id])
*/
uint32_t fprintf8(int id, char* file_name){
    id = 0; // lmao
    uint8_t ch[4] = {0, 0, 0, 0};
    int8_t byteN = 0;
    int8_t firstByte = 0;
    uint32_t charsPrinted = 0;

    for(int i = 0; i < instances[id].size; i++){
        // read bytes, see where to start
        for(int c = 0; c < 4; c++){
            ch[c] = instances[id].buffer[i] >> (3-c)*8;
            if(ch[c] == 0)
                firstByte++;
        }
        // if all 4 bytes are null, we're done
        if(firstByte == 4){
            return charsPrinted;
        }
        // skip empty bytes, print the rest
        byteN = bytesInChar(ch[firstByte]);
        for(int p = firstByte; p < 4; p++){
            putchar(ch[p]);
        }
        firstByte = 0;
        charsPrinted++;
    }
    return charsPrinted;
}

/* takes first byte of UTF-32 character and returns how many bits are
 * used by that character (according to UTF-8 rules)
 * returns -1 if sequence is not valid UTF-8 first byte
*/
int8_t bytesInChar(uint8_t ch){
    if(ch == 0) return 0;
    if(ch < 128) return 1;
    if(ch < 224) return 2;
    if(ch < 240) return 3;
    if(ch > 247) return -1;
    return 4;
}

/* takes unsigned int up to 32 bits long and prints it as binary
 * can't believe it's not a standard format specifier
*/
void printb(uint32_t n, uint8_t size){
    int i = 8 * size;
    while(i--){
        putchar('0' + ((n >> i) & 1));
    }
}