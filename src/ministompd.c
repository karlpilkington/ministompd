#include <sys/types.h>  // open()
#include <sys/stat.h>   // open()
#include <fcntl.h>      // open()
#include <stdio.h>
#include <unistd.h>  // STDIN_FILENO
#include "ministompd.h"

void parse_file(char *filename);

int main(int argc, char *argv[])
{
  for (int i = 1; i < argc; i++)
    parse_file(argv[i]);

  return 0;
}

void parse_file(char *filename)
{
  int fd = open(filename, O_RDONLY);
  if (fd < 0)
  {
    perror("open()");
    exit(1);
  }

  frameparser *fp = frameparser_new();

  buffer *b = buffer_new(4096);

  while (buffer_in_from_fd(b, fd, 4096) > 0)
  {
    printf("Read loop...\n");

    frameparser_outcome outcome = frameparser_parse(fp, b);
    printf("Parse: %d\n", outcome);

    if (outcome == FP_OUTCOME_ERROR)
    {
      printf("-- Parse error: ");
      bytestring_dump(frameparser_get_error(fp));
      break;
    }
    else if (outcome == FP_OUTCOME_FRAME)
    {
      frame *f = frameparser_get_frame(fp);
      printf("-- Completed frame: ");
      frame_dump(f);
      frame_free(f);
    }
  }

  close(fd);

  buffer_dump(b);

  buffer_free(b);
  frameparser_free(fp);
}