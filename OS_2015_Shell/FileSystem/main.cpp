#include <iostream>
#include <sstream>
#include <cassert>

#include "Entry.h"
#include "Directory.h"
#include "File.h"
#include "RAMDrive.h"
#include "Utils.h"

int main()
{
	//file_system::RAMDisk disk;
	//file_system::RAMDrive drive;
	//disk.mount(drive);file_system::Directory(nullptr, "root")
	file_system::RAMDrive drive;
	file_system::Directory& root = drive.root();

	root.create_directory("TestDir");
	root.create_directory("TestDir");
	root.create_directory("TestDir2");
	root.create_directory("TestDir3");
	root.create_file("Test.txt");


	std::cout << "\n---------------------\n";
	std::cout << "Printing directory " << root.name() << ":" << std::endl;

	root.print_contents(std::cout);

	file_system::directoryAt(root, "TestDir2")->rename("TestDirRenamed");
	file_system::fileAt(root, "Test.txt")->rename("TestFile.txt");

	std::cout << "\n---------------------\n";
	std::cout << "Printing directory " << root.name() << ":" << std::endl;

	root.print_contents(std::cout);

	std::cout << "\n---------------------\n";
	std::string tested_name = "TestDir";
	assert(root[tested_name] != nullptr);
	std::cout << "Finding child " << tested_name << ": " << "SUCCEED" << std::endl;
	tested_name = "wrong_name";
	assert(root[tested_name] == nullptr);
	std::cout << "Finding child " << tested_name << ": " << "SUCCEED" << std::endl;

	std::stringstream str;
	file_system::File* f = static_cast<file_system::File*>(root["TestFile.txt"]);
	assert(f != nullptr);
	std::stringstream& file = f->open();
	file << "Hello world";

	std::cout << "\n---------------------\n";
	std::cout << "File " << f->name() << "contents are:\n" << f->open().str() << std::endl;

	file_system::Directory* dir = file_system::directoryAt(root, "TestDir");
	dir->create_file("TestSubFile.txt");
	std::stringstream* file2 = file_system::openFile(root, "TestDir/TestSubFile.txt");
	assert(file2 != nullptr);
	*file2 << "Hello from folder";


	std::cout << "File " << file_system::fileAt(root, "TestDir/TestSubFile.txt")->name() << " at " << "TestDir/TestFile.txt" << " contents are:\n" << file_system::openFile(root, "TestDir/TestSubFile.txt")->str() << std::endl;

	std::cin.get();
	return 0;
}