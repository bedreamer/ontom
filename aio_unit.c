#include <stdlib.h>
       #include <unistd.h>
       #include <stdio.h>
       #include <errno.h>
       #include <aio.h>
       #include <signal.h>

int main()
{
  int fd, ret;
  struct aiocb my_aiocb;
	char buff[100];

  fd = open( "/dev/ttyO3", O_RDONLY );
  if (fd < 0) perror("open");

  /* Zero out the aiocb structure (recommended) */
  bzero( (char *)&my_aiocb, sizeof(struct aiocb) );

  /* Allocate a data buffer for the aiocb request */
  my_aiocb.aio_buf = buff;
  if (!my_aiocb.aio_buf) perror("malloc");

  /* Initialize the necessary fields in the aiocb */
  my_aiocb.aio_fildes = fd;
  my_aiocb.aio_nbytes = 16;
  my_aiocb.aio_offset = 0;

  ret = aio_read( &my_aiocb );
  if (ret < 0) perror("aio_read");

  while ( aio_error( &my_aiocb ) == EINPROGRESS ) ;

  if ((ret = aio_return( &my_aiocb )) > 0) {
    /* got ret bytes on the read */
  } else {
    /* read failed, consult errno */
  }

	return 0;
}
