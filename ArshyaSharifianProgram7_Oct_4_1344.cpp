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

	cout << "please enter the name of the file you wish to open:  " << endl;
	cin >> file_name;
	fin.open(file_name.c_str());

	if (!fin) {
		cout << "The file failed to open" << endl;
		exit(0);
	}

	while (getline(fin, read_file)) {
		cout << read_file << endl;
		arrsize++;
	}

	fin.close();
	///////////////////////////////////////////////////////////////////////////////	

	fin.open(file_name.c_str());

	//array of structures
	profile person[10];

	int i = 0;

	cout << "second loop" << endl;
	while (fin.is_open()) {

		getline(fin, read_file);
		if (isalpha(read_file[0])) {
			person[i].name = read_file;
			cout << "inside the looop:  " << person[i].name << endl;
			//end if
		}
		else if (isdigit(read_file[0])) {
			person[i].age = stoi(read_file);
			cout << read_file << endl;
			i++;
		}
		else {
			//There are some extra lines at the end of the file
			break;
		//end else
		}

	//end while
	}

	int max_age = 0,
		max_age_index;

	for (int k = 0; k <= 6; k++) {
		if (person[k].age > max_age) {
			max_age = person[k].age;
			max_age_index = k;
		}
	}

	cout << "The oldest person is " << person[max_age_index].name 
		 << " and this person is " 
		 << person[max_age_index].age << " years old" << endl;

	fin.close();

	///////////////////////////////////////////////////////////////////////////////	
	string name[10];
	int age[10];
	i = 0;
	
	fin.open(file_name.c_str());
	
	while (getline(fin, read_file)) {

		if (isalpha(read_file[0])) {
			name[i] = read_file;
		}
		else if (isdigit(read_file[0])) {
			age[i] = stoi(read_file);
			i++;
		}
		else {
			cout << "somethings wrong" << endl;
			break;
		}
		
	//end while
	}

	cout << "SECOND TIME AROUND" << endl;
	cout << "The oldest person is " << name[max_age_index]
		<< " and this person is "
		<< age[max_age_index] << " years old" << endl;


	

	for (int i = 0; i <= 6; i++) {

	}










	



	return 0;
}