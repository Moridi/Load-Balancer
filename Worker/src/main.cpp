#include "Worker.h"

using namespace std;

int main()
{
	Worker worker;
	worker.set_filters();
	worker.set_file_names();
	worker.read_from_file();
	return 0;
}
