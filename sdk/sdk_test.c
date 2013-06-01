#include "log.h"

#include "queue.h"

int main( int argc, char **argv )
{
	CQueue queue;

	enable_log( 1 );
	set_log( LOG_TYPE_CONSOLE, NULL, 0 );

	log_print( "Hello world");

	if ( init_queue( &queue ) >= 0 )
	{
		CQueueNode tempQNode;

		set_queue_water_level( &queue, 100 );		

		memset( &tempQNode, 0x00, sizeof(tempQNode) );
		log_print("&tempQNode-->%u.\r\n", &tempQNode );
		if ( en_queue( &queue, &tempQNode ) >= 0 )
		{
			CQueueNode *pDelQNode = NULL;

			log_print( "en queu ok." );
			
			pDelQNode = de_queue( &queue );
			if ( pDelQNode )
			{
				log_print( "de queue ok..........." );

				log_print( "pDelQNode-->%u", pDelQNode );
			}
			else
				log_print( "de queue failed?????????????" );
		}
		else
			log_print( "en queu failed???????????" );
	}

	return 0;
}

