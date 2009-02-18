#include <iostream>
#include <fileout.h>

int main() {
	std::string s = "hallo";
	std::string x = "hallo";



	FileOut *fo = new FileOut();
	fo->setParameter<std::string>("out_file_name", "/tmp/out.txt");

	fo->init(0, 7200, 300);
	fo->f(0, 300);
	std::string *fout = fo->getParameter<std::string>("out_file_name");
	std::cout << *fout << std::endl;
	return 0;
}
