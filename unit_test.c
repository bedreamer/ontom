#include <stdio.h>
#include "config.h"

void config_test()
{
	config_initlize("cfg.fg");
	config_print();
//	config_write("a", "123");
//	config_write("b", NULL);
//	config_write(NULL, "123");
//	config_write("xmlsrv_port", "123");
//	config_print();
//	config_write("xmlsrv_port", "sadfsaf");
//	config_print();
}

int main()
{
	config_test();
	return 0;
}