#include "Worker.h"

int main()
{
	Worker worker;
	worker.set_filters();
	worker.set_file_names();
	worker.read_from_file();
	worker.print();
	return 0;
}
