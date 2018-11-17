#include "Presenter.h"

int main()
{
	Presenter presenter("sampleDataset");
	presenter.get_input();
	presenter.sort();
	presenter.print();
	return 0;
}
