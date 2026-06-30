#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <unordered_map>
#include <windows.h>
using namespace std;


//---------PROTOTYPES----------
bool databaseCheck(string usernameFile, string passwordFile);
bool databaseCreation(string usernameFile, string passwordFile);
bool isDBEmpty(string usernameFile, string passwordFile);
bool loadDBData(string usernameFile, string passwordFile);
void userAdding(string username, string password);
bool openReactorControlPanel(string exeName);



//--------GLOBAL VARIABLES--------
string usernameFile = "usernameDB.txt";
string passwordFile = "passwordDB.txt";
string exeName = "reactorControlPanel.exe";
unordered_map<string, string> databaseMap;//We create a map for the access logic for fixing a bug



//This class is used for "debug" or supervisor settings. Not accessible from normal operator.
//Before using, see the manual.txt
class Supervisor {
	private:
		string databaseWord = "dbCreate--";
		string databaseUserAddingWordDebug = "userAdding--";
	public:
		bool databaseIns(string supervisorText){
			if(supervisorText == databaseWord){
				return true;
			} else {
				return false;
			}
		}
		bool databaseUserAdd(string supervisorText){
			if(supervisorText == databaseUserAddingWordDebug){
				return true;
			} else {
				return false;
			}
		}
};



//-------MAIN-------
int main() {

	//Object creation of the Supervisor class
	Supervisor workerSupervisor;

	string username, password, nameCheck, passwCheck, supervisorText;
	bool userFound = false;
	bool passFound = false;

	//With this function we put our credentials in a map, for not creating a bug in the access logic
	loadDBData(usernameFile, passwordFile);

	cout << "Welcome to the nuclear control panel. First, Log In with your credetials.";
	cout << "\nIf you are a new worker, please, contact your supervisor and let them put your new credetials in the database!";


	//If both username and password .txt files are not found, you can first create them
	if(!databaseCheck(usernameFile, passwordFile)){
		cout<<"\n\n\n\nThe username database and password database are not found.";
		cout<<"\nIf you are the owner of the company, insert the secret word to create the database.";

		cout<<"\n\nIf you are a worker, and you encounter this messages, please contact your supervisor.";
		cout<<"\n\nPlease, write the secret word: ";
		getline(cin, supervisorText);//All secret words are in the Supervisor class


		//We call a function in the class by using the object created before, and typing the name of the function we need to take from the class with the correct parameteres
		if(workerSupervisor.databaseIns(supervisorText)){
			databaseCreation(usernameFile, passwordFile);
			cout<<"Both username and password databases have been created!"<<endl;
		} else{
			cout<<"Couldn't create databases file for an unknown error."<<endl;
			return 1;
		}
	}
		

	if(isDBEmpty(usernameFile, passwordFile)){
		cout<<"\n\nBoth databases are empty, write the secret word to type in new users: "<<endl;
	}
	while(!userFound || !passFound){
		cout << "\nUsername: ";
		getline(cin, username);
		if(workerSupervisor.databaseUserAdd(username)){
			userAdding(username, password);
			cin.get();
			return 1;
		}
		cout << "\nPassword: ";
		getline(cin, password);
		if(databaseMap.count(username)){
			userFound = true;

			if(databaseMap[username] == password){
				passFound = true;
			} else {
				passFound = false;
				cout<<"Username or password are incorrect."<<endl;
			}
		} else {
			userFound = false;
			passFound = false;
			cout<<"Username or password are incorrect."<<endl;
		}
	}

	cout<<"Welcome back " << username << "!" <<endl;
	cout<<"Launching "<<exeName <<"..."<<endl;

	if(openReactorControlPanel(exeName)){
		cout<<"The program is now open."<<endl;
		cout<<"Press enter to close this window...";
		cin.get();
	} else {
		cout<<"ERROR: Couldn't open "<< exeName <<"."<<endl;
		cout<<"Please, ensure the .exe file is is present and in the same folder of this program.";
	}
	return 0;
}




//-------FUNCTIONS-------

bool databaseCheck(string usernameFile, string passwordFile){
	bool userDB = true;
	bool psswdDB = true;

	ifstream userFile(usernameFile);
	if(!userFile){
		userDB = false;
	}
	userFile.close();

	ifstream passwdFile(passwordFile);
	if(!passwdFile){
		psswdDB = false;
	}
	passwdFile.close();

	if(!userDB || !psswdDB){
		return false;
	}
	return true;
}


bool databaseCreation(string usernameFile, string passwordFile){
	ofstream userDB(usernameFile);
	if(!userDB){
		return false;
	}
	userDB.close();
	ofstream psswdDB(passwordFile);
	if(!psswdDB){
		return false;
	}
	psswdDB.close();
	return true;
}

bool isDBEmpty(string usernameFile, string passwordFile){
	bool userDBC = false;
	bool psswdDBC = false;
	
	ifstream userDB(usernameFile);
	if(userDB.peek() == ifstream::traits_type::eof()){
		userDBC = true;
	}
	userDB.close();

	ifstream psswdDB(passwordFile);
		if(psswdDB.peek() == ifstream::traits_type::eof()){
			psswdDBC = true;
		}
	psswdDB.close();

	if(userDBC || psswdDBC){
		return true;
	} else {
		return false;
	}
}

bool loadDBData(string usernameFile, string passwordFile){
	ifstream userDB(usernameFile);
	ifstream psswDB(passwordFile);
	if(!psswDB || !userDB){
		return false;
	}

	string user, pass;
	while(getline(userDB, user) && getline(psswDB, pass)){
		databaseMap[user] = pass;
	}
	userDB.close();
	psswDB.close();
	return true;
}

void userAdding(string username, string password){
	cout<<"You typed a secret word for debug. You are adding new users."<<endl;
			cout<<"Please, write they're name and password they've choosed: "<<endl;
			cout<<"Username: "<<endl;
			getline(cin, username);
			cout<<"Password: "<<endl;
			getline(cin, password);
			ofstream userDB(usernameFile, ios::app);
			userDB << username <<endl;
			userDB.close();
			ofstream passwDB (passwordFile, ios::app);
			passwDB << password <<endl;
			passwDB.close();
			cout<<"Restarting the program for applying changes..."<<endl;
}

bool openReactorControlPanel(string exeName){
    // FORZIAMO la struttura dati in versione ANSI usando STARTUPINFOA
    STARTUPINFOA si; 
    PROCESS_INFORMATION pi;

    // Azzera la memoria per evitare dati spazzatura
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Convertiamo la stringa in un puntatore a caratteri modificabile
    char* reactorControlPanel = &exeName[0];

    // Chiamata esplicita alla funzione ANSI
    if (CreateProcessA(
        NULL,                   // Nessun percorso assoluto fisso
        reactorControlPanel,    // Il nome del file .exe (char*)
        NULL,                   // Attributi di sicurezza del processo di default
        NULL,                   // Attributi di sicurezza del thread di default
        FALSE,                  // Non ereditare i descrittori
        CREATE_NEW_CONSOLE,     // Flag per aprire un altra finestra di terminale
        NULL,                   // Usa le variabili d'ambiente del programma padre
        NULL,                   // Usa la stessa cartella corrente
        &si,                    // Ora il tipo (STARTUPINFOA*) coincide perfettamente!
        &pi                     // Informazioni sul processo creato
    )) {
        // Chiudiamo i canali (handle) per non sprecare memoria RAM
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return true; 
    }

    return false;
}