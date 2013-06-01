#include "../inc/config.h"

#include "../inc/typedefs.h"

#include "../inc/log.h"

MODULE_LICENSE( "Dual BSD/GPL" );

static int kernel_module_init( void )
{
	int32_t iRetCode = -1;
	
	enable_log( 1 );
	set_log( LOG_TYPE_CONSOLE, NULL, 0 );

	log_print( "kernel_module_init:------------------------->" );

	//printk( KERN_INFO "hello world!\r\n" );
	
	iRetCode = 0;

	return iRetCode;
}

static void kernel_module_exit( void )
{

	log_print( "kernel_module_exit:-------------------------->" );
}

module_init( kernel_module_init );
module_exit( kernel_module_exit );


