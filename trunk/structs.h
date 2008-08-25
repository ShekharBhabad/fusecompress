/** @file
 *
 * FuseCompress
 * Copyright (C) 2005 Milan Svoboda <milan.svoboda@centrum.cz>
 *
 */
#ifndef STRUCTS_H
#define STRUCTS_H

#include <pthread.h>
#include <sys/types.h>

#include "list.h"

#define FALSE 0
#define TRUE 1
#define FAIL -1

#ifdef DEBUG
# define LOCK(lock) assert(pthread_mutex_lock(lock) == 0)
# define LOCK_RD(lock) assert(pthread_rwlock_rdlock(lock) == 0)
# define LOCK_WR(lock) assert(pthread_rwlock_wrlock(lock) == 0)

# define UNLOCK(lock) assert(pthread_mutex_unlock(lock) == 0)
# define UNLOCK_RW(lock) assert(pthread_rwlock_unlock(lock) == 0)

# define NEED_LOCK(lock) assert(pthread_mutex_lock(lock) == EDEADLK)
# define NEED_WR_LOCK(lock) assert(pthread_rwlock_wrlock(lock) == EDEADLK)
# define NEED_RD_LOCK(lock) assert(pthread_rwlock_rdlock(lock) == EDEADLK)
#else
# define LOCK(lock) pthread_mutex_lock(lock)
# define LOCK_RD(lock) pthread_rwlock_rdlock(lock)
# define LOCK_WR(lock) pthread_rwlock_wrlock(lock)

# define UNLOCK(lock) pthread_mutex_unlock(lock)
# define UNLOCK_RW(lock) pthread_rwlock_unlock(lock)

# define NEED_LOCK(lock)
# define NEED_WR_LOCK(lock)
# define NEED_RD_LOCK(lock)
#endif

typedef struct
{
	char type;		// ID of compression module
	const char *name;
	off_t (*compress)(void *cancel_cookie, int fd_source, int fd_dest);
	off_t (*decompress)(int fd_source, int fd_dest);

	void *(*open) (int fd, const char *mode);
	int (*close)(void *file);
	int (*read)(void *file, void *buf, unsigned int len);
	int (*write)(void *file, void *buf, unsigned int len);

} compressor_t;

typedef struct
{
	char id[3];		// ID of FuseCompress format
	unsigned char type;	// ID of used compression module
	off_t size;		// Uncompressed size of the file in bytes

} __attribute__((packed)) header_t;

#define READ		(1 << 1)
#define WRITE		(1 << 2)

#define COMPRESSING	(1 << 1)
#define DECOMPRESSING	(1 << 2)
#define CANCEL		(1 << 3)

/**
 * Used in database
 */
typedef struct {
	char		*filename;
	unsigned int	 filename_hash;

	ino_t		 ino;		/**< inode */
	nlink_t		 nlink;		/**< number if hard links */

	int		 deleted;	/**< Boolean, if set file no longer exists */
	int		 accesses;	/**< Number of accesses to this file (number of descriptor_t in
					     the `list` */
	off_t		 size;		/**< Filesize, if 0 then not read */
	compressor_t	*compressor;	/**< NULL if file isn't compressed */
	off_t		 skipped;	/**< Number of bytes read and discarded while seeking */
	int		 dontcompress;
	int		 type;
	int		 status;
	
	void**           cache;
	int              cache_size;

	pthread_mutex_t	lock;
	pthread_cond_t cond;

	struct list_head	head;	/**< Head of descriptor_t. This is needed
					     because truncate has to close all active fd's
					     and do direct_close */
	struct list_head	list;
} file_t;

typedef struct {
	file_t		*file;		// link back to file_t, can't be free'd until accesses = 0

	int		 fd;		// file descriptor
	int		 flags;		// Needed when we have to reopen fd, this is fi->flags

	void		*handle;	// for example gzFile
	off_t		 offset;	// offset in file (this if for compression data)

	struct list_head list;
} descriptor_t;

/**
 * Used in comp_database
 */
typedef struct {
	file_t *file;		/**< Pointer to the file_t of the file
				     scheduled to be compressed in the safe time */
	struct list_head list;
} compress_t;

/**
 * Main database structure
 */
typedef struct {
	pthread_mutex_t lock;
	pthread_cond_t cond;
	int entries; 			/**< Number of entries in the database */
	struct list_head head;		/**< Head of the file_t or compress_t */
} database_t;

#endif
