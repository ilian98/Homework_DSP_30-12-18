#include<iostream>
#include<algorithm>
#include<fstream>
#include<vector>
#include<string>
using namespace std;
struct student { // structure for the data of the students in the binary file
    int fn;
    char name[128]; // the size if smaller than the constraint because the binary file was too large for moodle with size of 257
    char family_name[128]; // the size if smaller than the constraint because the binary file was too large for moodle with size of 257
    int grade;
};
struct ind { // structure for the index file containing the faculty number and position in the original file
    int fn;
    int pos;
    bool operator< (ind const& rhs) {
        if (fn<rhs.fn) return true;
        return false;
        }
};
int check_file (ifstream& file) {
    if (file.is_open()==0) { // if the file is not open, we open it
       file.open("StudentsGrades.db",ios::in|ios::binary);
       if (file.is_open()==0) return -1; // if the file is still not open, this means it does not exist and we cannot search
       }
    return 0;
}
void build_index (ifstream& file, fstream& index) {
    if (check_file(file)==-1) return ;
    index.open("FacultyNumber.ids",ios::in|ios::out|ios::trunc|ios::binary); // truncate the file everytime
    vector <ind> indices; // vector storing the indices for the index file
    student st;
    ind temp;
    int i;
    file.clear(); // we clear the flags
    file.seekg(0,ios_base::beg); // we position the stream at the beginning of the file
    for (i=0; ; i++) { // cycle for iterating through the students in the original file
        if (!(file.read((char*)&st,sizeof(student)))) break; // this statement checks if we are at the end of the file
        temp.fn=st.fn; temp.pos=i;
        indices.push_back(temp);
        }
    sort(indices.begin(),indices.end()); // sorting the indices by the operator< in the structure ind
    for (i=0; i<indices.size(); i++) { // cycle for saving the indices in the index file
        index.write((char*)&indices[i],sizeof(ind));
        }
}
int main () {
    int input,l,r,mid; // variables for storing faculty number and binary search
    ifstream file; // variable for the binary file
    fstream index; // variable for the index file
    string command; // variable for storing command
    student st; // variable for the student
    bool flag;
    ind temp;
    for (;;) {
        cin >> command ;
        if (command=="exit") break; // if command is exit, the program has to finish
        else if (command=="sequentialSearch") { // if command is sequentialSearch we search for a person in the file
                if (check_file(file)==-1) continue;
                cin >> input ; // we input a faculty number for the search
                file.clear(); // we clear the flags
                file.seekg(0,ios_base::beg); // we position the stream at the beginning of the file
                flag=0; // variable to know if the faculty number is found
                for (;;) {
                    if (!(file.read((char*)&st,sizeof(student)))) break; // this statement checks if we are at the end of the file
                    if (input==st.fn) { // if the current faculty number is the same as the input faculty number, we found the student
                       cout << st.fn << " " << st.name << " " << st.family_name << " " << st.grade << "\n";
                       flag++;
                       break;
                       }
                    }
                if (flag==0) cout << "Record not found!\n";
                }
        else if (command=="buildIndex") { // if command is buildIndex, we use the function build_index
                build_index(file,index);
                cout << "Index is built!\n";
                }
        else if (command=="search") { // if command is search, we do binary search for the student using the index file
                cin >> input ; // we input a faculty number for the search
                if (index.is_open()==0) build_index(file,index); // we check if the index file is created, and if it isn't - we created with the function build_index
                index.clear(); // we clear the flags
                index.seekg(0,ios_base::end); // we position the stream at the end in order to calculate the number of indices
                l=-1; r=index.tellg()/sizeof(ind); // we set the left and right positions of the binary search and we will also store the answer in l
                for (;;) {
                    if (l==r-1) break; // if the positions of the binary search are consecutive numbers, we have finished the search
                    mid=(l+r)/2; // we calculate the middle position
                    index.clear(); // we clear the flags
                    index.seekg(mid*sizeof(ind),ios_base::beg); // we position the stream to read the index with position (mid+1)
                    index.read((char*)&temp,sizeof(ind));
                    if (temp.fn<=input) l=mid;
                    else r=mid;
                    }
                index.seekg(l*sizeof(ind),ios_base::beg); // we position the stream to read the index with position (l+1) to check if we have found the faculty number
                index.read((char*)&temp,sizeof(ind));
                if (temp.fn!=input) cout << "Record not found!\n";
                else { // we use the position in the index file to have direct access to the student in the original file
                    file.clear(); // we clear the flags
                    file.seekg(temp.pos*sizeof(student),ios_base::beg);
                    file.read((char*)&st,sizeof(student));
                    cout << st.fn << " " << st.name << " " << st.family_name << " " << st.grade << "\n";
                    }
                }
        }
    file.close(); index.close(); // we close the files because we finished the work with it
    cout << endl ;
    return 0;
}
