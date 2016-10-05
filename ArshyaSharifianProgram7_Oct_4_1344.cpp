#include <iostream>
#include <string>
#include <fstream>
using namespace std;

struct profile {
	int age;
	string name;
};

int main() {
	
	ifstream fin;
	string file_name, read_file;
	int arrsize = 0;

	//asking the user to enter the name of the file they wish to open
	cout << "please enter the name of the file you wish to open:  " << endl;
	cin >> file_name;
	fin.open(file_name.c_str());

	//if the file was not found
	if (!fin) {
		cout << "The file failed to open" << endl;
		exit(0);
	}

///////////////////////////////////////////////////////////////////////////////	

	//defining the number of lines in the file
	while (getline(fin, read_file)) {
		if (isalpha(read_file[0]) || isdigit(read_file[0])){
		arrsize++;
		//end if
		}
	//end while
	}

	arrsize = arrsize / 2;
	fin.close();
	///////////////////////////////////////////////////////////////////////////////	

	//array of structures
	profile person[10];
	int profile_index = 0;

	//opening the file again
	fin.open(file_name.c_str());

	while (getline(fin, read_file)) {
		//cout << "HERE" << endl;
		if (isalpha(read_file[0])) {
			person[profile_index].name = read_file;
			cout << "at index " << profile_index << "; This is the name:  " << person[profile_index].name << endl;
			//end if
		}
		else if (isdigit(read_file[0])) {
			person[profile_index].age = stoi(read_file);
			cout << "at index " << profile_index << "; This is the name:  " << person[profile_index].age << endl;

			//incrementing index by 1
			profile_index++;
		}
		else {
			//There are some extra lines at the end of the file
			break;
		//end else
		}
	//end while
	}

	//determining the oldest person's age and position in the index
	int max_age = 0,
		max_age_index;

	for (int k = 0; k < arrsize; k++) {
		if (person[k].age > max_age) {
			max_age = person[k].age;
			max_age_index = k;
		}
	}

	cout << "The oldest person is " << person[max_age_index].name
		<< " and this person is "
		<< person[max_age_index].age << " years old" << endl;

	//closing the file
	fin.close();

	///////////////////////////////////////////////////////////////////////////////	
	string name[10];
	int age[10];
	profile_index = 0;

	fin.open(file_name.c_str());

	while (getline(fin, read_file)) {
		if (isalpha(read_file[0])) {
			name[profile_index] = read_file;
		}
		else if (isdigit(read_file[0])) {
			age[profile_index] = stoi(read_file);
			profile_index++;
		}
		else {
			break;
		}
	//end while
	}

	cout << "SECOND TIME AROUND" << endl;
	cout << "The oldest person is " << name[max_age_index]
		<< " and this person is "
		<< age[max_age_index] << " years old" << endl;

	return 0;
}
