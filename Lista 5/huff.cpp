#include "huffmanTrie.h"
using namespace std;

// Main Huffman Trie
struct MinHeapNode* Trie;

// Table created by tree
map<char,string> table;

// Encoded trie and message
string trieCode, compressedMessage, uncompressedMessage;


// Filename
string filename;
 
// Create table of binary 
void createTable(struct MinHeapNode* root, string str){
    // Return if root is NULL
    if (!root)
        return;
    
    if (root->data != '\0')
        table[root->data] = str;
    
    // Pre order construction
    createTable(root->left, str + "0");
    createTable(root->right, str + "1");
}

void encodeTrie(struct MinHeapNode* root){

    if(root->isLeaf()){
        // Add bit 1 if root is leaf
        trieCode += "1";

        // Get in binary char value
        bitset<8> y(root->data);

        // Transfor binary value to string
        trieCode += y.to_string();

        return;
    }

    // Add bit 0 if root is not leaf
    trieCode += "0";

    // Pre order call
    encodeTrie(root->left);
    encodeTrie(root->right);

}

// Create frequency and set of chars used on file
void create(vector<char> *carcteres, vector<int> *freq, int *n){
    ifstream file(filename);

    // String to read file
    string s;

    // Count freq
    map<char,int> mp;

    // Walk through file and walk through chars
    mp['\n'] = -1;
    while(getline(file,s)){
        for(auto x: s)
            mp[x]++;
        mp['\n']++;
    }

    // Adding representation of EOF char
    mp[4] = 1;
        
    
    // Walk through map and set vector of char and freq.
    for(auto x: mp){
        carcteres->push_back(x.first);
        freq->push_back(x.second);
    }

    // Set carcteres size
    *n = mp.size();

    // close file
    file.close();

}

void printMsg(struct MinHeapNode* root, int idx){
    // Stop recursion if don't have more binary string to read
    // If root is leaft then we achieved a char
    if(root->isLeaf()){
    	if(root->data == 4) return;
        uncompressedMessage += ((int)root->data == 13 ? '\n':root->data);
        printMsg(Trie, idx);
    }
    // If is not a root and is 1, so go right
    else if(compressedMessage[idx] == '1'){
        printMsg(root->right, idx+1);

    }

    // Go left otherwise
    else{
        printMsg(root->left, idx+1);
    }
}

string binaryString2bits(string total){
    string ret, aux;

    // Walk through all bits
    for(int i = 0; i < total.size(); i++){

        // Add bit to string aux
        aux += total[i];

        // Work only with 8 bits number
        if( aux.size() != 8) continue;

        // Transform to binary
        bitset<8> y(aux);

        // Get respective char according to binary value and save on compressed file
        ret += (char)y.to_ulong();

        // Reset numer;        
        aux.clear();
    }
    if(aux.size() == 8){
        bitset<8> y(aux);
        // Get respective char according to binary value and save on compressed file
        ret += (char)y.to_ulong();
    }
    return ret;
}

string char2binaryString(string message){

    // string to return
    string ret;
    for(int i = 0; i < message.size(); i++){        
        // Get binary value from byte value
        bitset<8> y(message[i]);
        // Add binary to the final string
        ret += y.to_string();
    }    

    // Return binary string
    return ret;

}

string transform(vector<char> bytes){
    string ret;
    for(auto x : bytes){
        bitset<8> y(x);
        // Add binary to the final string
        ret += y.to_string();
    }
    return ret;
}


string ReadAllBytes(string filename){
    ifstream ifs(filename, ios::binary|ios::ate);
    ifstream::pos_type pos = ifs.tellg();

    vector<char>  result(pos);

    ifs.seekg(0, ios::beg);
    ifs.read(&result[0], pos);

    return transform(result);
}

void createCompressedFile(){

    // Open message code 
    ifstream file(filename);

    // Open output file compressed
    ofstream saida("compressed.txt");

    // Strings to read file and final result
    string line, total;

    // Adding Trie binary code to the beginnig o compressed file

    // Walktrough file lines and create binary code with table
    while(getline(file,line)){
        for(auto x: line)
            total += table[x];
    }
    // Adding char that represent EOF
    total += table[4];

    total = trieCode + total;

    while(total.size()%8 != 0) total += "0";

    // Total binary form need to be multiple of 8
    string final = binaryString2bits(total);

    // Adding compressed to file
    saida << final;

    // Close message file
    file.close();

    // Close compressed file
    saida.close();
}

void compress(){

    // User interaction
    printf("Input file name to compress : ");

    // Read filename
    cin >> filename;

    // open message file
    ifstream file(filename);

    // Vetors size
    int n;

    // Chars on file
    vector<char> carcteres;

    // Frequency of each char
    vector<int> freq;

    // Create vector with chars and each frequency
    create(&carcteres,&freq,&n);

    struct MinHeapNode *left, *right;
    // Compress all data
    Trie = HuffmanCodes(left, right, Trie, carcteres, freq, n);
    
    // Create table of values for each char
    createTable(Trie,"");

    // Create trie binary code
    encodeTrie(Trie);

    // Print trie chars codes
    printTrie(Trie, "");

    // Create compressed file
    createCompressedFile();

    // Closing file
    file.close();
}


void decompress(){

	// Interaction with user
    printf("Input file name that want to decompress : ");

    // Read filename
    cin >> filename;

    compressedMessage = ReadAllBytes(filename);

    
    // Idx to walk in compressedMessage
    int idx = 0;
     
    // Build trie with this binary string
    Trie = buildTrie(compressedMessage, &idx);

    // Print trie
    printTrie(Trie, "");

    // Print compressed message
    printMsg(Trie, idx);

    // Open final result file
    ofstream out("uncompressed.txt");

    // Write message
    out << uncompressedMessage;

    // Close file;
    out.close();

}



 

int main(){
    
    // Interaction with user
    printf("1 - Compress file\n");
    printf("2 - Read compressed file\n");

    // Operation variable
    int op;

    // Read operation
    cin >> op;

    // Go to huffman compression
    if(op == 1)
        compress();

    // Decompress
    else if(op == 2)
        decompress();

    return 0;
}