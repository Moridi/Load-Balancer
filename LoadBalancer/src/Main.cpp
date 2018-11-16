#include "LoadBalancer.h"

int main()
{
	LoadBalancer load_balancer;
	load_balancer.get_input();
	load_balancer.iterate_on_directory();
//	load_balancer.print();
	load_balancer.allot_files();
	return 0;
}
