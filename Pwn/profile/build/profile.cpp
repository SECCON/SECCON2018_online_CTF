#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <stdio.h>
#include <malloc.h>

using namespace std;

int getn(char *buf, unsigned size);

class Profile {
	private:
		string msg;
		string name;
		int age;

	public:
		Profile(){
			name = "";
			age = 0;
			msg = "";
		};

		void set_name(string n){ name = n; };
		void set_age(int a){ age = a; };
		void set_msg(string m){ msg = m; };

		void update_msg(void);
		void show(void);
};

void Profile::update_msg(void){
	char *buf;
	size_t size;

	buf = (char*)msg.c_str();
	if(!(size = malloc_usable_size(buf))){
		cout << "Unable to update message." << endl;
		return;
	}

	cout << "Input new message >> ";
	getn(buf, size);
}

void Profile::show(void){
	cout << "Name : " << name << endl;
	cout << "Age  : " << age << endl;
	cout << "Msg  : " << msg << endl;
}

__attribute__((constructor))
void init(void){
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
}

int main(void){
	Profile p;

	string s;
	int n;

	cout << "Please introduce yourself!" << endl;
	cout << "Name >> ";
	cin >> s;
	p.set_name(s);

	cout << "Age >> ";
	cin >> n;
	p.set_age(n);

	cout << "Message >> ";
	cin >> s;
	p.set_msg(s);

	do {
		cout << endl
			 << "1 : update message" << endl
			 << "2 : show profile" << endl
			 << "0 : exit" << endl
			 << ">> ";
		cin >> n;
		getchar();
		switch(n){
			case 1:
				p.update_msg();
				break;
			case 2:
				p.show();
				break;
			default:
				cout << "Wrong input..." << endl;
		}
	} while(n);
}

int getn(char *buf, unsigned size){
	char c;
	unsigned i;

	for(i=0; i<size; i++){
		read(STDIN_FILENO, &c, 1);
		if(!(c^'\n'))
			break;
		buf[i] = c;
	}

	return i;
}
