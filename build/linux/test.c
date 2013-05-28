#include "log.h"

int main( int argc, char **argv )
{
  enable_log(1);
  log_print( "%s:%d %s, %d", __FILE__, __LINE__, "hello", 100 );

  return 0;
}
