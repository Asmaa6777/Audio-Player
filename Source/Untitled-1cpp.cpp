#include <iostream>
using namespace std;
class myfloat {
    private : 
    float value;
    public :
    myfloat(float v=0.0):value(v) { cout <<"myflaot cons called"; }
    friend istream& operator>> (istream& in, myfloat& f);
    friend ostream& operator<< (ostream& out, myfloat& f);
};
istream& operator>> (istream& in, myfloat &f){
  in >> f.value;
 return in ;
}
ostream& operator<< (ostream& out, myfloat& f){
  out << f.value;
 return out ;
}
int main(){
  myfloat f;  
  cin >> f;
  cout << f;
  return 0;
}
 
