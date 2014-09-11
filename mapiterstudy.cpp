
///////////////////////////////////////////////////////////////////////
//
//   mapiterstudy.cpp
//
//   workbench for experiments with maps, templates, and iterators
//
///////////////////////////////////////////////////////////////////////

#include <iostream>
#include <map>
#include <string>

using namespace std;

class ClientInfo 
{
	public:
		long lBlockIdx;
		string additionalInfo;
};

class ClientRegister 
{

	public:
		string transactId;
		ClientInfo ci;
		
		ClientRegister(string tid)
		{
			transactId = tid;
			ci.lBlockIdx = 0;
			ci.additionalInfo = "";
			
		};
		~ClientRegister(){};
		

};



int main(int argv, char** argc)
{
	ClientRegister cr("1018521191263717267");
	
	cout << "Cr: \nID: " << cr.transactId << "\nBlock Number: " << cr.ci.lBlockIdx << "\nExtraInfo: " << cr.ci.additionalInfo << endl;
	
	
	//// the map
	std::map < string, ClientInfo> connectReg;
	
	ClientInfo first, second, third;
	
	
	//// test "empty()"
	cout << "map connectReg is empty: " << connectReg.empty() << endl;
	
	//// fill in the values of an object of class ClientInfo
	first.lBlockIdx = 11;
	first.additionalInfo = "special one";
	
	//// insert the created object into the map
	connectReg["M110"] = first;
	
	//// another object of class ClientInfo
	second.lBlockIdx = 22;
	second.additionalInfo = "very special one";
	
	//// yet another one, of class ClientInfo
	third.lBlockIdx = 22;
	third.additionalInfo = "outstanding";
	
	//// now insert the two latest objects
	connectReg["2200N"] = second;
	connectReg["330K00"] = third;
	
	
	//////////////
	// test a simple update of an entry in the map
	cout << "Before: " << connectReg["2200N"].lBlockIdx << ", " << connectReg["2200N"].additionalInfo << endl;
	connectReg["2200N"].lBlockIdx += 1;
	connectReg["2200N"].additionalInfo += string(", almost exceptional");
	cout << "After: " << connectReg["2200N"].lBlockIdx << ", " << connectReg["2200N"].additionalInfo << endl;
	
	// test  "empty()": is expected to display either 1 for "true", 0 for "false" 
	cout << "\nmap connectReg is empty: " << connectReg.empty()<< "\n----------------\n" << endl;
	
	// find() and end()
	cout << " Testing function find() " << endl;
	cout << "Record 330K00 not found: " << (connectReg.find("330K00") == connectReg.end()) << endl;
	cout << "Record 365 not found: " << (connectReg.find("365") == connectReg.end()) << "\n----------------\n" << endl;
	
	// Test of iterator over the map:see usage of specifiers "first" and "second'
	// of the members of a pair "designated by the iterator via "begin" and "end"	
	cout << "TEST OF ITERATOR OVER MAP\n\n" << endl;
	cout << "Test function begin: " << (connectReg.begin()->second).additionalInfo << endl;
	cout << "Test function begin: " << connectReg.begin()->first<< endl;
	cout << "Test function begin: " << (connectReg.begin()->second).lBlockIdx << endl;

	cout << "Test function end: " << (connectReg.end()->second).additionalInfo << endl;
	cout << "Test function end: " << connectReg.end()->first << endl;
	cout << "Test function end: " << (connectReg.end()->second).additionalInfo << endl;
	return 0;
}