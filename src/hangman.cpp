// This is the code of evil hangman !-_-!

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <cstdlib>
using namespace std;
// welcome information.
void welcome ();
// read the input file and put all the words of given length into a vector of strings.
void fileInput (vector<string>& wordList, int wordLength);
// prompt the player three information: word length, # guesses allowed, whether to show the number of words remaining.
bool userInput(vector<string>& wordList, int& wordLength, int& numGuess);
// Cited from class to prompt the user for the right integer.
int getInteger(string prompt);
// game start information.
void gameStart ();
// print the information on the console.
void printScreen(int numGuess,int numWords, bool showRemain, set<string> usedLetter, string wordHint);
// prompt the player a guess of one letter.
void letterInput (string& letter, set<string>& usedLetter);
// partition the words into different groups according to the words structure.
// return true if the player finds one letter, return false if the player does not find a letter.
bool partitionWords (vector<string>& wordList, string letter, string& wordHint);

int main() {
    welcome();
    bool play = true;
    while (play){
        vector<string> wordList;
        set<string> usedLetter;
        int wordLength;
        int numGuess;
        bool findOneLetter;
        string letter;
        string wordHint;
        bool showRemain = userInput(wordList, wordLength, numGuess);
        for (int i=0; i<wordLength; i++){
            wordHint.push_back('-');
        }
        gameStart();
        printScreen(numGuess, wordList.size(), showRemain, usedLetter, wordHint);
        // the main logic of this game. The player continues to guess letters.
        while ( true ){
            letterInput(letter, usedLetter);
            findOneLetter = partitionWords(wordList, letter, wordHint);
            if ( wordList.size()==1 && wordList[0]==wordHint){
                cout << endl << "CORRECT!" << endl << endl;
                cout << "Congratulations! You guessed the word: " << wordHint;
                break;
            }
            if (findOneLetter){
                cout << endl << "CORRECT!" << endl;
            } else {
                usedLetter.insert(letter);
                numGuess--;
                cout << endl << "WRONG!" << endl << endl;
            }
            if ( numGuess == 0 ){
                // generate a random integer as index and output the word with that index in wordList as secret word.
                int n = rand() % wordList.size();
                cout << "Sorry, but you lost. The word was: " << wordList[n] << endl;
                break;
            }
            printScreen(numGuess, wordList.size(), showRemain, usedLetter, wordHint);
        }
        // prompt the player whether to play again.
        while (true){
            cout << "Would you like to play again? [Y/N]: ";
            string playAgain;
            getline (cin, playAgain);
            if (playAgain == "Y" || playAgain == "y" ){
                break;
            } else if ( playAgain == "N" || playAgain == "n" ){
                play = false;
                break;
            } else {
                cout << "Invalid response; please try again." << endl;
            }
        }
    }
    return 0;
}

void welcome () {
    cout << "-------------------" << endl;
    cout << "Welcome to Hangman!" << endl;
    cout << "-------------------" << endl;
}

void gameStart (){
    cout << "----------" << endl;
    cout << "GAME START" << endl;
    cout << "----------" << endl;
}

// read the input file and put all the words of given length into a vector of strings.
void fileInput (vector<string>& wordList, int wordLength) {
    ifstream input;
    string currWord;
    input.open("dictionary.txt");

    while ( getline(input, currWord) ){
        int length = currWord.size();
        if ( length == wordLength ){
            wordList.push_back(currWord);
        }
    }
    input.close();
}

// prompt the player three information: word length, # guesses allowed, whether to show the number of words remaining.
bool userInput(vector<string>& wordList, int& wordLength, int& numGuess) {
    string showNumWords;

    while(true){
        wordLength = getInteger("Enter a word length: ");
        fileInput(wordList, wordLength);
        if ( wordList.size()==0 ){
            cout << "Sorry, there are no words of that length." << endl;
        } else {
            break;
        }
    }

    while(true){
        numGuess = getInteger("Enter the number of guesses allowed: ");
        if ( numGuess <= 0 ){
            cout << "Sorry, the guess count must be positive." << endl;
        } else {
            break;
        }
    }

    while(true){
        cout << "Should I show the number of possible words left each round? [Y/N]: ";
        getline (cin, showNumWords);
        if ( showNumWords == "Y" || showNumWords == "y" ){
            return true;
        } else if ( showNumWords == "N" || showNumWords == "n" ){
            return false;
        } else {
            cout << "Invalid response; please try again." << endl;
        }
    }
}


// print the information on the console.
void printScreen (int numGuess, int numWords, bool showRemain, set<string> usedLetter, string wordHint){
    cout << "You have " << numGuess << " incorrect guesses remaining." << endl;
    cout << "Letters guessed so far: ";
    // print used letters.
    set<string>::iterator i= usedLetter.begin();
    while ( i!=usedLetter.end() ){
        cout << *i << " ";
        ++i;
    }
    cout << endl;
    cout << "Word hint: " << wordHint << endl;
    if (showRemain){
        cout << "There are " << numWords << " eligible words left." << endl;
    }
    cout << endl;
}

// prompt the player a guess of one letter.
void letterInput (string& letter, set<string>& usedLetter){
    cout << "Guess a letter: ";
    while ( true ){
        getline (cin, letter);
        auto search = usedLetter.find(letter);
        if (letter.size() == 0){
            cout << "Input invalid. Retry: ";
        }
        else if (letter.size() > 1){
            cout << "Unexpected character found: '" << letter[1] << "'. Retry: ";
        }
        else if ( search!= usedLetter.end() ){
            cout << "You have already guessed that. Guess a letter: ";
        }
        else if (letter[0]>='A' && letter[0]<='Z'){
            letter[0] = letter[0] - ('A'-'a');
            break;
        }
        else if (letter[0]>='a' && letter[0]<='z'){
            break;
        }
        else {
            cout << "Invalid guess. Guess a letter: ";
        }
    }

}

// partition the words into different groups according to the words structure.
// return true if the player finds one letter,
// return false if the player does not find a letter.
bool partitionWords(vector<string>& wordList, string letter, string& wordHint){
    // store all the groups which contain the letter user guesses.
    map <set<int>, vector<string> > wordFamily;
    // This is the big group of words which contain the letter. first partition.
    vector<string> containLetter;
    // this notContain group contains all the words which donot contain the given letter.
    vector<string> notContain;
    vector<string> currGroup;
    vector<string> mostCommonGroup;
    string word;
    int max = 0;

    vector<string>::iterator i= wordList.begin();
    while ( i!=wordList.end() ){
        word = *i;
        if (word.find(letter)!=string::npos ){
            containLetter.push_back(word);
        } else {
            notContain.push_back(word);
        }
        ++i;
    }
    // If the number of words which don't contain the letter user guesses, is more than half of the total number
    // of words in wordList, then we choose this notContain group and return false which means the user guesses wrong.
    if ( notContain.size() >= (wordList.size() / 2) ){
        wordList = notContain;
        return false;
    }
    else {
        wordList = containLetter;
        vector<string>::iterator j= wordList.begin();
        // put all the words in wordList in the right group.
        while ( j!=wordList.end() ){
            // store the index of a word, word[index] = letter.
            set<int> index;
            word = *j;
            for (int m=0; m<word.size(); m++){
                if ( word[m]==letter[0] ){
                    index.insert(m);
                }
            }
            currGroup = wordFamily[index];
            currGroup.push_back(word);
            wordFamily[index] = currGroup;
            ++j;
        }
        // find the group with the largest number of words. All groups are stored in the map with index as keys.
        map <set<int>, vector<string> >::iterator k= wordFamily.begin();
        while ( k!=wordFamily.end() ){
            currGroup = (*k).second;
            if (currGroup.size()>max){
                mostCommonGroup = currGroup;
                max = currGroup.size();
            }
            ++k;
        }
        // Compare the size of this group with the group with no such letter "notContain".
        if (mostCommonGroup.size() >= notContain.size()){
            wordList = mostCommonGroup;
            word = wordList[0];
            for (int i=0; i<word.size(); i++){
                if (word[i]==letter[0]){
                    wordHint[i] = letter[0];
                }
            }
            return true;
        }
        else{// The notContain group has the largest number of words. The player did not guess the right letter.
            wordList = notContain;
            return false;
        }
    }
}

// This function is cited from the class.
int getInteger(string prompt) {
    int value;
    string line;
    cout << prompt;
    while (true) {
        getline(cin, line);
        istringstream stream(line);
        stream >> value >> ws;
        if (!stream.fail() && stream.eof()) break;
        cout << "Input invalid. Retry: ";
    }
    return value;
}




