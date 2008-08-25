/*
    FuseCompress
    Copyright (C) 2005 Milan Svoboda <milan.svoboda@centrum.cz>
*/

extern int min_filesize_background;
extern int root_fs;
extern int read_only;

extern int cache_decompressed_data;
extern int decomp_cache_size;
extern int max_decomp_cache_size;
#define DC_PAGE_SIZE (4096)

extern pthread_t pt_comp;

extern pthread_mutexattr_t locktype;

extern compressor_t *compressor_default;
extern compressor_t *compressors[5];
extern char *incompressible[];
extern char *mmapped_dirs[];

extern database_t database;
extern database_t comp_database;

void *thread_compress(void *arg);

#define TEMP "._.tmp"		/* Template is: ._.tmpXXXXXX */
#define FUSE ".fuse_hidden"	/* Temporary FUSE file */

extern char compresslevel[];
#define COMPRESSLEVEL_BACKGROUND (compresslevel) /* See above, this is for background compress */

// Gcc optimizations
//
#if __GNUC__ >= 3
# define likely(x)	__builtin_expect (!!(x), 1)
# define unlikely(x)	__builtin_expect (!!(x), 0)
#else
# define likely(x)	(x)
# define unlikely(x)	(x)
#endif
