#include "Worker.h"

int main(int argc, char* argv[])
{
	Worker worker;
	worker.set_filters(argv);
	worker.set_files_name(argc, argv);
	worker.read_from_file();
	worker.send_data_to_presenter();
	return 0;
}
