#include "Presenter.h"

int main(int argc, char* argv[])
{
	Presenter presenter;
	presenter.get_sorting_value(argc, argv);
	presenter.get_input();
	presenter.print();
	return 0;
}
