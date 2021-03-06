#include <iostream>
#include "SerialTimeout.h"

using namespace std;
using namespace boost;

int main(int argc, char* argv[])
{
	try {

		SerialTimeout serial("/dev/tty.usbserial-A501B6XH",9600);
		serial.setTimeout(posix_time::seconds(5));

		//Text test
		serial.writeString("Hello world\r\n");
		cout<<serial.readStringUntil("\r\n")<<endl;

		//Binary test
		char values[]={0xde,0xad,0xbe,0xef};
		serial.write(values,sizeof(values));
		serial.read(values,sizeof(values));
		for(unsigned int i=0;i<sizeof(values);i++)
		{
			cout<<static_cast<int>(values[i])<<endl;
		}

		serial.close();

	} catch(boost::system::system_error& e)
	{
		cout<<"Error: "<<e.what()<<endl;
		return 1;
	}
}

