#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// This structure will make the elements of the table that will contain the chars with their codes
typedef struct {
    char c;
    char code[25];
}Codes;

// this one for the elements of the table where we keep track of the chars and their frequency
typedef struct {
    char c;
    int freq;
}tableCell;


//this structure is for the nodes of Huffman's tree
typedef struct {
    tableCell element;
    struct TreeNode *right;
    struct TreeNode *left;
}TreeNode;

// Huffman Tree Methods
TreeNode *createNode(){ // creates a node and returns its address
    return (TreeNode *)malloc(sizeof(TreeNode));
}
  // assigns a left child to the node
 void assLC(TreeNode *root, TreeNode *child){
    root->left = (struct TreeNode *)child;
 }
    // assigns a right child to the node
 void assRC(TreeNode *root, TreeNode *child){
    root->right = (struct TreeNode *)child;
 }
    // returns the @ of the right child
 TreeNode *RC(TreeNode *root){
    return (TreeNode *)root->right;
 }
    // returns the @ of the left child
 TreeNode *LC(TreeNode *root){
    return (TreeNode *)root->left;
 }
    // assigns a char to the struct
 void assChar(TreeNode *root,char c){
    root->element.c = c;
 }
    //assigns the frequency associated with that char
 void assFreq(TreeNode *root, int f){
    root->element.freq = f;
 }
  // here we return the frequency of some char
int getFreq(TreeNode *root){
    return root->element.freq;
}
    // and here we return the character itself
char getChar(TreeNode *root){
    return root->element.c;
}

 // here we will create the function that will traverse the tree looking for the leaves while keeping track of the path
 // and hence the Huffman coding of each character;
    void getEncoding(TreeNode *root,char *code, int level,Codes *CodesTable){
        if(root != NULL){
            if(LC(root)== NULL && RC(root) == NULL){ // if the node is a leaf, we remove the last char and that's the code for that char
                code[level] = '\0';
                CodesTable[getChar(root)].c = getChar(root);
                strcpy(CodesTable[getChar(root)].code,code);


            }
            code[level] = '0';
            code[level + 1] = '\0'; // we add a "0" to the code string before going left
            getEncoding(LC(root),strcat(code,"0"),level + 1,CodesTable);
            code[level] = '1';
            code[level + 1] = '\0'; // we add "1" to the code string before we go right
            getEncoding(RC(root),strcat(code,"1"),level + 1,CodesTable);
        } else {
            code[strlen(code) - 1] = '\0'; // if we find null,  we terminate the string
        }
    }

    // this function frees the nodes of the tree
    void freeTree(TreeNode *root){ // just an in-order traversal where we free each node we visit
        if(root != NULL){
            freeTree(LC(root));
            TreeNode *temp = RC(root);
            free(root);
            freeTree(temp);
        }
    }
    //---------------------------------------------------------------------------------------------------------
// here table methods
tableCell *createTable(int length){
    tableCell *table = (tableCell *)malloc(length * sizeof(tableCell));
    return table;
}

    // initialize the table
void tableInit(tableCell *table, int tableLen){
    for(int i = 0; i < tableLen; i++){
        table[i].c = i;
        table[i].freq = 0;
    }
}
    //fill the table based on the chars we read
void fillTable(char *buff,tableCell *table, int *heapLen){
    int n = strlen(buff);
    for(int i = 0; i < n; i++){
            if(table[buff[i]].freq == 0){
                (*heapLen) += 1;
            }
        table[buff[i]].freq += 1;

    }
}
// from here we begin programming heap methods
    void HeapInit(tableCell *table,int tableLen, TreeNode **heap,int heapLen){
        int j = 0;
        TreeNode *temp = NULL;
         for(int i = 0; i < tableLen; i++){
            if(table[i].freq > 0){  // everywhere we find a char with more than 1 frequency we add it to the heap
                temp = createNode(); // create the Node
                assChar(temp,table[i].c); // assign the char to it
                assFreq(temp,table[i].freq); // then assign its frequency
                assLC(temp, NULL); // assign left to be null (this node will be a leaf)
                assRC(temp, NULL); // same as the left side
                heap[j] = temp; // assign that pointer to the jth element of the heap
                j++; // increment j
            }
         } // sort the heap in a descending order based on the frequency
         for(int i = 0; i < j; i++){  // I just used the bubble sort, even though it's time complexity is O(n^2), the number of characters we have is constant and can't surpass 256 if we include all ASCII elements, even unprintable ones
            for(int k = i; k < j - 1; k++){
                if(heap[k]->element.freq < heap[k+1]->element.freq){
                    TreeNode *temp = heap[k];
                    heap[k] = heap[k+1];
                    heap[k+1] = temp;
                }
            }
         }
    }
 // and now, we will create the huffman tree based on the obtained heap;
   TreeNode *HuffmanTree(TreeNode **heap, int heapLen){
    TreeNode *temp = NULL;          // a temporary variable to hold the @s of new nodes
    for(int i = heapLen - 1; i > 0; i--){ // here we assign the leaves to the node, with the frequency being the sum of the children
        temp = createNode();
        assFreq(temp, getFreq(heap[i]) + getFreq(heap[i-1]));
        assLC(temp,heap[i-1]);
        assRC(temp,heap[i]);
        int j = i - 1;
        while (j > 0 && getFreq(temp) > getFreq(heap[j-1])) { // if the frequency is higher than any node before it, we keep shifting until the heap is sorted again
            heap[j] = heap[j-1];                        // here I went for the implementation of the priority queue as an array, I only included methods I needed in this case
            j--;
        }
         heap[j] = temp;
    }
    return temp; // we return the last pointer we're left with as it represents the root of our tree
  }
 // the following function encodes the file
 void EncodeText(char *src,char *dest,Codes *list){
    int size = strlen(src);
    for(int i = 0; i < size; i++){
        strcat(dest,list[src[i]].code);
    }
 }

 void writeCodeMap(TreeNode *root,FILE *codeMap){
    if(root != NULL){
        if(LC(root) == NULL && RC(root) == NULL){
            fwrite(root,sizeof(TreeNode),1,codeMap);
        }
        writeCodeMap(LC(root),codeMap);
        writeCodeMap(RC(root),codeMap);
    }
 }
 //-----------------------------------------------------------------------------------------------------------------------------------
 //-----------------------------------------------------------------------------------------------------------------------------------------------
int main(){
    int heapLen = 0;
    int tableLen = 256;
    tableCell *charsTable = createTable(tableLen); // create a table where we store chars with their frequency
    tableInit(charsTable,tableLen);

    char fileName[25];  // file Name!

    printf("enter the file name: "); // input the file name
    scanf("%s", &fileName);

    FILE *file = fopen(fileName,"r");  //open in read mode
        if(file == NULL){    // if couldn't open we close the program
            perror("something went wrong!");
            return 1;
        }
        char buff[512]; // string to take text from the file

    while(fgets(buff,512,file) != NULL){
        fillTable(buff,charsTable,&heapLen); // we fill our table
    }
     // now we allocate an array of pointers to TreeNode structure
     // it will play the role of the heap
    TreeNode **heap = (TreeNode **)malloc(sizeof(TreeNode *) * heapLen);
    HeapInit(charsTable,tableLen,heap,heapLen); // we initialize

    free(charsTable); // free the table of chars & frequencies as we don't need it anymore

    TreeNode *root = HuffmanTree(heap,heapLen); // the root of huffman tree is left here

    char code[30] = "";

    Codes *listOfCodes = (Codes *)malloc(sizeof(Codes) * tableLen); // here we allocate a table of chars & codes to be used when encoding the file
    memset(listOfCodes, 0,sizeof(Codes)); // initializing it to 0

    getEncoding(root,code,0,listOfCodes); //we get codes

    // at this point we have our huffman tree working, we created a table containing all chars with their codes
    // now we shall start encoding our file
    rewind(file); // we rewind the file to go back to the beginning of the file
    char encodedFile[40] = "encoded_";
    strcat(encodedFile,fileName);
    FILE *encoded = fopen(encodedFile,"a"); // we create a file where we will store the encoded file

    char encodedStr[2048];
    while(fgets(buff,512, file) != NULL){
        EncodeText(buff,encodedStr,listOfCodes);
    }
    encodedStr[strlen(encodedStr)] = '\0'; // we set the end of the encoded string

   //after this, we should also write another file that includes the code map so that the receiver can decode the text sent
     // to do this we need a function to traverse the tree, and write the leaf nodes in the binary file we send
     char LeavesToDecode[40] = "codeMap_";
     strcat(LeavesToDecode,fileName);
     FILE *codeMap = fopen(LeavesToDecode,"wb"); // open a file that will contain the leaves of Huffman's tree

    writeCodeMap(root,codeMap); // here we wrote leaf nodes into the file, including pointers right & left,
                                    // decomposing the structure would have been bothering,and on the other hand the decoder will have leaves ready
    fprintf(encoded,encodedStr);  // we wrote the final code here
    //now we shall free the remaining allocated structures
    freeTree(root);
    free(heap);

    // and finally, we close all the opened files and we terminate the program
    fclose(codeMap);
    fclose(encoded);
    fclose(file);
    printf("File encoded successfully, Encoded file: %s\nLeaves For decoding: %s",encoded,LeavesToDecode);
    return 0;
}
