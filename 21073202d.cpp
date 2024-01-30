#include <iostream>
#include <time.h>
#include <math.h>
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
using namespace std;

enum WalkingDir {NORTH, EAST, SOUTH, WEST};
class Simulator;

class User{
    public:
        User(int X);
        
        void walk(Simulator* simulator, int X , int S);

        void report();

        void setContagious();

        bool getContagious();

        void setVaccinated();

        bool getVaccinated();

        void setWearMask();

        bool getWearMask();

        int getCoordinateX();

        void setCoordinateX(int xCoor);

        int getCoordinateY();

        void setCoordinateY(int yCoor);

        void setSpeed(int S);

        int getSpeed();

    private:
        int speed;
        int x = 0;
        int y = 0;
        bool contagious = false;
        bool vaccinated = false;
        bool wearMask = false;

};

class Simulator{

    public:
        Simulator(int N,int R,int V, int M, int X);

        void update(int X, int S, int C , int Tnn , int Tnm, int Tmm);

        void report();

        void checkTransmit(int C , int Tnn , int Tnm , int Tmm);

        void checkInfect(User* userA, User* userB, int C , int Tnn , int Tnm , int Tmm);

        void printLocation();

        bool transmitChance(int T);

        bool hasUserAt(int j , int i);

        bool isBoundary(int j , int i, int X);

        int getBoundary();

        string checkStatus(int i , int j);

    private:
        User** userList;
        int normalUser;
        int userLength;
        int boundary;

};

User::User(int X) {
    x = rand() % (X - 1) + 1;
    y = rand() % (X - 1) + 1; 
}

void User::walk(Simulator* simulator, int X , int S) {
    WalkingDir direction = (WalkingDir) (rand() % 4);
    int tempX = x;
    int tempY = y;
    switch (direction) {
        case NORTH:
            tempY += S;
            break;
        case EAST:
            tempX += S;
            break;
        case SOUTH:
            tempY -= S;
            break;
        case WEST:
            tempX -= S;
            break;
    }
    if (simulator->hasUserAt(tempX, tempY) || simulator->isBoundary(tempX, tempY, X)) {  
        return;  //if there are something infront of user, don't move.
    }
    else {
        this->x = tempX;
        this->y = tempY;
    }
}
    
void User::report() {
        cout << "coordiante is " << x << "," << y << endl;
}

void User::setContagious(){
    this->contagious = true;
}

bool User::getContagious(){
    return this->contagious;
}

void User::setVaccinated(){
    this->vaccinated = true;
}

bool User::getVaccinated(){
    return this->vaccinated;
}

void User::setWearMask(){
    this->wearMask = true;
}

bool User::getWearMask(){
    return this->wearMask;
}

int User::getCoordinateX(){
    return x;
}

void User::setCoordinateX(int xCoor){
    x = xCoor;
}

int User::getCoordinateY(){
    return y;
}

void User::setCoordinateY(int yCoor){
    y = yCoor;
}

void User::setSpeed(int S){
    cout << "Enter the Speed of User: ";
    speed = S;
}

int User::getSpeed(){
    return speed;
}


Simulator::Simulator(int N,int R,int V, int M, int X){
    userLength = N;
    normalUser = N - R - M - V;
    userList = new User*[N];
    boundary = X;
    for (int i = 0;i < normalUser;i++){
        userList[i] = new User(X);
    }
    for (int i = normalUser;i < normalUser+R;i++){
        userList[i] = new User(X);
        userList[i]->setContagious();
    }
    for(int i= normalUser+R;i< normalUser+R+M;i++){
        userList[i] = new User(X);
        userList[i]->setWearMask();
    }
    for (int i = normalUser+R+M;i < N;i++){
        userList[i] = new User(X);
        userList[i]->setVaccinated();
    }
}

void Simulator::update(int X, int S, int C , int Tnn , int Tnm, int Tmm){
    for (int i=0;i < userLength;i++){
        userList[i]->walk(this, X , S);
    }
    checkTransmit(C , Tnn , Tnm, Tmm);
    printLocation();
}

void Simulator::report(){
    for (int i=0;i < userLength;i++){
        cout << userList[i] << ": ";
        userList[i]->report();
    }
}

void Simulator::checkTransmit(int C , int Tnn , int Tnm , int Tmm){
    for(int i=0;i<userLength;i++){
        for(int j=0;j<userLength;j++){
            User* userA = userList[i];
            User* userB = userList[j];
            if(userList[i]->getVaccinated() == true || userList[j]->getVaccinated() == true) continue;
            else{
                checkInfect(userA, userB , C , Tnn , Tnm, Tmm);
            }
        }
    }
}

void Simulator::checkInfect(User* userA, User* userB, int C , int Tnn , int Tnm , int Tmm){
    int userACoordinateX = userA->getCoordinateX();
    int userBCoordinateX = userB->getCoordinateX();
    int userACoordinateY = userA->getCoordinateY();
    int userBCoordinateY = userB->getCoordinateY();
    int distanceOfX = userACoordinateX - userBCoordinateX;
    int distanceOfY = userACoordinateY - userBCoordinateY;
    int euclideanX = pow(distanceOfX , 2);
    int euclideanY = pow(distanceOfY , 2);
    int euclideanDistance = sqrt(euclideanX + euclideanY);
    if(euclideanDistance <= C && userA->getContagious() == true){
        if ((userA->getWearMask() == false && userB->getWearMask() == false) && transmitChance(Tnn)){
            userB->setContagious();
        }
        else if ((userA->getWearMask() == false && userB->getWearMask() == true) && transmitChance(Tnm)){
            userB->setContagious();
        }
        else if ((userA->getWearMask() == true && userB->getWearMask() == false) && transmitChance(Tnm)){
            userB->setContagious();
        }
        else if((userA->getWearMask() == true && userB->getWearMask() == true) && transmitChance(Tmm)){
            userB->setContagious();
        }
    }
}

void Simulator::printLocation(){
    for (int i =0;i <= boundary+1;i++){
        cout << endl;
        for (int j=0;j <= boundary+1;j++){
                if (hasUserAt(j , i) == true){
                    cout << checkStatus(i , j) << "U" << RESET;
                }
                else if (i == 0 || i == boundary+1){
                    cout << "#";
                }
                else if (j == 0 || j == boundary+1 ){
                    cout << "#";
                }
                else cout << " ";
        }
    }
    cout << endl;
}


bool Simulator::transmitChance(int T){
    int chance = T;
    if(chance > rand() % 101){
        return true;
    }
    else return false;
}

bool Simulator::hasUserAt(int j , int i){
    for(int u=0;u < userLength;u++){
        User* user = userList[u];
        int x = user->getCoordinateX();
        int y = user->getCoordinateY();
        if(i == x && j == y) return true;
        else continue;
    }
    return false;
}

bool Simulator::isBoundary(int j , int i , int X){
    if(i >= X || j >= X) return true;
    else if(i <= 0 || j <= 0) return true;
    else return false;
}

string Simulator::checkStatus(int i , int j){
    for(int u=0;u < userLength;u++){
        User* user = userList[u];
        int x = user->getCoordinateX();
        int y = user->getCoordinateY();
        if(i == x && j == y){
            if(user->getContagious() && user->getWearMask()) return MAGENTA;
            else if(user->getVaccinated()) return BLUE;
            else if(user->getWearMask()) return GREEN;
            else if (user->getContagious()) return RED;
            else return RESET;
        }
    }
    return RESET;
}

int Simulator::getBoundary(){
    return boundary;
}



int main(){    
    srand(time(NULL));
    int N = 10; // total number of user default = 10
    int R = 1;  // contagious user default = 1
    int V = 2;  // vaccinated user default = 2
    int M = 3;  // wear mask user default = 3
    int X = 20; // size of boundary default = 20
    int S = 2;  // speed default = 2
    int C = 1;  // transmit distance default = 1
    int Tnn = 90;  // default 90% transmit (normal to normal)
    int Tnm = 50;  // default 50% transmit (normal to mask or mask to normal)
    int Tmm = 30;  // default 30% transmit (mask to mask)
    int timesOfUpdate = 3;  //default Update 10 times
    cout << "Enter the size of boundary: (default = 20)" << endl;
    cin >> X;
    cout << "Enter the total number of user: (default = 10)" << endl;
    cin >> N;
    cout << "Enter the number of contagious user: (default = 1)" << endl;
    cin >> R;
    cout << "Enter the number of vaccinated user: (default = 2)" << endl;
    cin >> V;
    cout << "Enter the number of user who wear mask: (default = 3)" << endl;
    cin >> M;
    cout << "Enter the speed of user: (default = 5)" << endl;
    cin >> S;
    cout << "Enter the transmit distance: (default = 1)" << endl;
    cin >> C;
    cout << "Enter the probability normal to normal: (default = 90)" << endl;
    cin >> Tnn;
    cout << "Enter the probability normal to mask: (default = 50)" << endl;
    cin >> Tnm;
    cout << "Enter the probability mask to mask: (default = 30)" << endl;
    cin >> Tnn;
    cout << "Enter the number of times of update: (default = 5)" << endl;
    cin >> timesOfUpdate;
    Simulator* simulator = new Simulator(N, R , V, M, X); // run simulator
    for(int i=0;i < timesOfUpdate;i++){
        simulator->update(X , S , C , Tnn, Tnm, Tmm);
    }
}