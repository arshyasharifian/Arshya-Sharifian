/*
 run: line print out something weird
 might need to change rtf into txt
*/

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

class words{
public:
    string word;
    int position = 1;
    words(){
        word = "yo";
        position = 1;
    }
};



void get_dict_word (string line);
void get_word_sounds (string line, int word_index);
void print_phonetics(string word);

int main() {

    
    string line;
    vector <words> poem;
    string word = "";
    int word_position = 1;
    words w;
    int poem_index = 0;
    //open dictionary
    //open poem
    fstream fin_poem ("ex_poem.txt");

    if (fin_poem.is_open()) cout << "poem is open" << endl;
    //if (fin_dict.is_open()) cout << "dict is open" << endl;
    
    getline(fin_poem, line);
    
    while (poem_index<=line.length()){
        if (line[poem_index] == 32){
            w.word = word;
            w.position = word_position;
            word = "";
            word_position++;
            poem.push_back(w);
        }
        else if (poem_index == line.length()){
            word+=line[poem_index];
            w.word = word;
            w.position = word_position;
            word = "";
            word_position++;
            poem.push_back(w);
        }
        else{
            word+=line[poem_index];
        }
        poem_index++;
    }
    
    for (int i = 0; i < poem.size(); i++){
        print_phonetics(poem[i].word);
    }
    
    //get phonetics for the words in the poem
    
    
    
    
    
    exit(0);
    
    
    /*
    //string dict_word;
    string sounds;
    while (getline(fin_dict, line)){
        if (isalpha(line[0])){
            get_dict_word(line);
        }//end if
    }//end while
    */
    return 0;
}//end main
/////////////////////////////////////////////////////////////////////
//START FUNCTIONS
void print_phonetics(string word){
    string line;
    string dict_word ="";
    int line_index = 0;
    fstream fin_dict ("sound_dict_.rtf");
    //going through dictionary
    while (getline(fin_dict, line)){
        cout << line << endl;
        dict_word = "";
        line_index=0;
        //if first character is a letter
        if (isalpha(line[0])){
            //while we are in the line
            while (line_index<=line.length()){
                //if the character is a letter
                if (isalpha(line[line_index])){
                    //add this character to the word
                    dict_word+=line[line_index];
                    //increment the index
                    line_index++;
                }
                //otherwise if the dict word contains letters
                else if (dict_word.length() > 0) {
                    //check if it matches poem word
                    if (dict_word == word){
                        cout << "match" << "\t" << dict_word << "\t" << word << endl;
                    }
                }
                else{
                    line_index = line.length() + 1;
                }
            }//end while still in line
            //get_dict_word(line);
        }//end if
    }
}


//////////////////////////////////////////////////////////////////
void get_dict_word (string line){
    string word = "";
    int word_index = 0;
    
    while (line[word_index]!=32){
        if (line[word_index] == '('){
            cout << word << "\t";
            get_word_sounds (line, word_index);
        }//end if
        word+=line[word_index];
        word_index++;
    }//end while
    cout << word << "\t";
    get_word_sounds (line, word_index);
    
}//end function
////////////////////////////////////////////////////////////////////////
void get_word_sounds (string line, int word_index){
    vector <string> sounds;
    string sound = "";
    while (word_index < line.length()){
        if (((line[word_index] == 32)) || (word_index + 1 == line.length())){
            if (sound.length() > 0){
                sounds.push_back(sound);
            }//end if length greater than 0
            sound = "";
        }//end if space
        else{
            sound+=line[word_index];
        }
        word_index++;
    }//end while
    
    for (int i = 0; i < sounds.size(); i++){
        cout << sounds[i] << '\t';
    }
    sounds.clear();
    cout << endl;
}//end function


