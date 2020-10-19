#include "helpers.h"

//////CONVERT VECTOR OF INTS TO STRING//////
string vectorToString(vector<int> tempVec) {
        stringstream ss;
        copy( tempVec.begin(), tempVec.end(), ostream_iterator<int>(ss)); //mod
        string currentString = ss.str();
        currentString = currentString.substr(0, currentString.length());  // get rid of the trailing space
        return currentString;
}

