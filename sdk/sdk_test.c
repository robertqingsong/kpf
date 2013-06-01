#include "log.h"

int main( int argc, char **argv )
{
	enable_log( 0 );
	set_log( LOG_TYPE_CONSOLE, NULL, 0 );

	log_print( "Hello world");

	return 0;
}

