//Arshya Sharifian, CPSC 121, Program#8
/*
This program demonstrates filling, sorting, randomizing, and outputting values
stored in arrays.  First, we ask the user how many values they would like to
store in an array; next, we fill the array using the Fibonacci Sequence; then,
we randomize the values stored in the array; next, we duplicate the values
in the array to another array; then, we sort the values in the duplicated array
according in increasing order; next, we conduct a sequential and binary search
for a value requested by the user; finally, we output the values stored in the
duplicated array.
*/
///////////////////////////////////////////////////////////////////////////////
//Pre-Processing
#include <iostream>
using namespace std;
///////////////////////////////////////////////////////////////////////////////
//Function Prototypes
void chooseSize(int&);
void generateValues(int[], int);
void duplicate_arr(int[], int[], int);
void sort(int[], int);
void sequential(int[], int, int&, int&, int&, bool&);
void binary(int[], int, int&, int&, bool &);
void output(int[], int);
///////////////////////////////////////////////////////////////////////////////
int main() {
	//VARIABLE DECLARATION AND INTIALIZATION
	int original[20], 
		duplicate[20], 
		user_value,
		foundLoc = -1, 
		srchVal, 
		loc = 0;
	bool found = false;
///////////////////////////////////////////////////////////////////////////////
	//FUNCTION CALL
	chooseSize(user_value);
	generateValues(original, user_value);
	duplicate_arr(original, duplicate, user_value);
	sort(duplicate, user_value);
	sequential(duplicate, user_value, foundLoc, srchVal, loc, found);
	binary(duplicate, user_value, foundLoc, srchVal, found);
	output(duplicate, user_value);
///////////////////////////////////////////////////////////////////////////////
return 0;
//END MAIN
}
///////////////////////////////////////////////////////////////////////////////
/*
The chooseSize functions asks the user how many values they would like to store
in the array.  The user must enter a value greater than or equal to 5 or less
than or equal to 20.
*/
void chooseSize(int & user_value) {
	cout << "How many values would you like to enter into the array" << endl
		<< "Please choose a value between 5 and 20 inclusive" << endl;
	cin >> user_value;
	//while the value does not meet the criteria
	while (user_value < 5 || user_value > 20) {
		cout << "Sorry, that value is not within the range specified" << endl
			<< "How many values would you like to enter into the array" << endl
			<< "Please choose a value between 5 and 20 inclusive" << endl;
		cin >> user_value;
	//end while user_value
	}
//end chooseSize function
}
///////////////////////////////////////////////////////////////////////////////
/*
The generateValues function enters values into the original array, then 
randomizes those values.
*/
void generateValues(int original[], int user_value) {
	//variables
	int counter = 0,
	    fiboSeq[20] = { 1 };
///////////////////////////////////////////////////////////////////////////////
	//filling the array with values and printing those values
	cout << "\nNon-Random Fibonacci Sequence:  " << endl;
	//storing fibonacci sequence's first value
	original[0] = 1;
	for (int i = 0; i < user_value; i++) {
		//Doing this for aesthetic purposes
		if (i == 0) {
		//end if i is 0
		}
		else if (i == 1) {
			fiboSeq[i] = fiboSeq[i - 1] + fiboSeq[i - 1];
		//end if i is 1
		}
		else {
			fiboSeq[i] = fiboSeq[i - 1] + fiboSeq[i - 2];
		//end else
		}
		//storing values into the array
		original[i] = fiboSeq[i];
		//printing the value
		cout << original[i] << "\t";
		counter++;
		//printing a new line for every 5 numbers
		if (counter == 5) {
			cout << endl;
			counter = 0;
		//end if counter is 5
		}
	//end for loop
	}
	//resetting counter
	counter = 0;
///////////////////////////////////////////////////////////////////////////////
	//randomizing values in the original array and printing it
	cout << "\n\nRandomized:" << endl;
	for (int i = user_value - 1; i >= 0; i--) {
		swap(original[i], original[rand() % (i + 1)]);
		cout << original[i] << "\t";
		counter++;
		if (counter == 5) {
			cout << endl;
			counter = 0;
		//end if counter is 5
		}

	//end for loop
	}
//End generateValues function
}
///////////////////////////////////////////////////////////////////////////////
/*
The duplicate function simply copies values from the original array and stores
those values in the duplicate array.
*/
void duplicate_arr(int original[], int duplicate[], int user_value) {
	for (int i = 0; i < user_value; i++) {
		duplicate[i] = original[i];
	//end for loop
	}
//end duplicate_arr function
}
///////////////////////////////////////////////////////////////////////////////
/*
The sort function simply sorts the values in the duplicate array in increasing
order.
*/
void sort(int duplicate[], int user_value) {
	//variables
	int top = 0, bottom = user_value - 1;
	bool swapped = true;
	//Bubble Sort
	while (swapped) {
		swapped = false;
		for (int loc = top; loc < bottom; loc++) {
			if (duplicate[loc] > duplicate[loc + 1]) {
				swap(duplicate[loc], duplicate[loc + 1]);
				swapped = true;
			//end if
			}
		//end for
		}
		//decrementing
		bottom--;
	//end while
	}
///////////////////////////////////////////////////////////////////////////////
	//Printing those values sorted
	cout << "\n\nSorted:  " << endl;
	int counter = 0;
	for (int i = 0; i < user_value; i++) {
		cout << duplicate[i] << "\t";
		counter++;
		//For aesthetic purposes
		if (counter == 5) {
			cout << endl;
		//end if counter is 5
		}
	//end for loop
	}
//end sort function
}
///////////////////////////////////////////////////////////////////////////////
/*
The Sequential function searches for values sequentially, starting from the
beginning of the array and stopping when it finds the value requested by the
user
*/
void sequential(int duplicate[], int user_value, int & foundLoc, 
				int & srchVal, int & loc, bool & found){
	//asking the user to enter the value they would like to find in the array
	cout << "\n\nPlease enter the value you would like to find:  " << endl;
	cin >> srchVal;
	//sequential search
	while (loc < user_value && !found) {
		if (duplicate[loc] == srchVal) {
			found = true;
			foundLoc = loc;
		//end if value is found
		}
		else {
			loc++;
		//end else
		}
	//end while searching
	}
	//printing the array location of the found value in the array
	cout << "\nIn the Sequential search Function \nFound the value here:  " 
		 << endl 
		 << foundLoc 
		 << endl;

//end sequential function
}
///////////////////////////////////////////////////////////////////////////////
/*
The binary function finds the same value requested by the user in the
sequential function but employs a different process.
*/
void binary(int duplicate[], int user_value, int & foundLoc, 
			int & srchVal, bool & found) {
	//variables
	found = false;
	int first = 0, last = user_value - 1, mid;
	//while the value is not found and there are still values
	while (!found && first < last) {
		mid = (first + last) / 2;
		if (srchVal < duplicate[mid]) {
			last = mid - 1;
		//end if
		}
		else if (srchVal > duplicate[mid]) {
			first = mid + 1;
		//end else if
		}
		else {
			foundLoc = mid;
			found = true;
		//end else
		}
	//end while
	}
	//printing the array location of the value found
	cout << "\nIn the Binary search function\nFound the value here:  " << endl << foundLoc << endl;
//end binary function
}
///////////////////////////////////////////////////////////////////////////////
/*
The output function prints the array
*/
void output(int duplicate[], int user_value) {
	//variables
	int counter = 4;
	//printing the values
	cout << "\n\nFinal output:" << endl;
	for (int i = 0; i < user_value; i++) {
		cout << duplicate[i] << "\t";
		if (i % counter == 0 && i != 0) {
			cout << endl;
			counter += 5;
		//end if
		}
	//end for loop
	}
	//for aesthetic purposes
	cout << endl;
//end output function
}