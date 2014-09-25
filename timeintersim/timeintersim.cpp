/********************************************************************
*
*   timeintersim.cpp
*
*   - based on findings in timetest.cpp, timeinter.cpp, and VirtualMethodStudyThree.cpp;
*
*   - creates a file where each entry is a string - result of the
*     following concatenation:
*
*   |---------------------------timestamp
*	|
*	|	        |---------------remainder of "division"
* (format)      |
*	|           |        |-----"mystr" (placeholder text)
*	|           |        |
*	|           |        |
* |-------|  |-----| |------ - - ---|
* |_______|, |_____| |______ _ _ ___|
* |__________________________ _ _ ___|
*             /|\
*              |
*	           |_________ string - entry in the file =
*
*  = <Year - Month - Day hour24:min:sec> <placeholder>
*
*   Copyright Eugene Berenshteyn 2014
*
****************************************************************************************/
/* *
 * 1.	Configfile.txt: 15 3.5 3
 *
 * The first parameter, 15, when rounded to nearest greater even number, becomes 16.
 * Then 16 / 2 = 8 exam record to be created in the log.
 * There are 5 elements in array Scantype. Hence the first three types "add"
 * 2 "exams" each in the logfile (total 6), and the two remaining types "add"
 * 1 "exams" each in the logfile (total 2), so that 6 + 2 = 8:
 *
 * 	       |   |   |   |
 *       A | B | C | D | E            -   elements of array scantype
 *       ------------------
 *       1 | 2 | 3 | 4 | 5
 *      -------------------           -   iteration on which the type is used
 *       6 | 7 | 8
 *
 *
 * In order to use all scan types evenly, the first entry in the configfile.txt,
 * in this example 15, should be used by the program in the following way:
 *
 * 	The actual number of iterations =
 * 			smallest multiple of (2 * number_of_entries_in_scantype)
 * 			greater than or equal first parameter in configfile.txt
 *
 * For the given example (first parameter: 15, number of scantypes: 5),
 * 	The actual number of iterations = smallest multiple of (2 * 5) greater than
 * 		or equal 15. The number is 20
 *
 *************************************************************************************
 *
 *  2.  Report based on data accumulated in the log file is created by running the
 *  Octave script logtest5.m
 *  The script is [typically] located in
 *
 *     c:\eugene\testspfgnuplotandoctave\logtest5.m
 *
 *  The logfile created as output of timeintersim.cpp should be placed into
 *  the folder
 *
 *     c:\eugene\testspfgnuplotandoctave\Logs\
 *
 *  ATTENTION: logtest5.m generates report based on ALL files in the latter
 *     folder
 *
 *******************************************************************************************************
 *
 *  3.	For the configfile.txt shown in Comment 1 above, Logtest5.m produced the report stating duration
 *    of a test equaling 6 seconds:
 *
 * 	1 + 0.5 * 3 + 3.5 = 6
 * 	|    |    |    |________  * second parameter from Configfile.txt; this adds one for
 * 	|    |    |		          each interval between opening and closing statements of an exam
 *  |    |    |
 * 	|    |    |_____________  * third parameter from Configfile.txt; number of place holding
 * 	|    |                    strings within each interval between opening and closing statements
 *  |    |
 * 	|    |__________________  * hardcoded interval before each place holding string
 * 	|
 * 	|
 * 	|_______________________  * hardcoded interval, one per exam, before each "group"
 * 	                            of place holding strings
 *
 ******************************************************************************************************
 *
 *  4.	A configfile.txt, like the one shown in Comment 1 above, should be preserved for future re-use.
 * A possible file name may comply with the following format:
 *
 * 	Configfile_plh_<number of placeholding strings>_avrt_<average time for an exam>.txt
 *
 * For the example describe above, the file configfile.txt should be saved as
 *
 * 	Configfile_plh_3_avrt_6.txt
 *
 */

#include <sys/stat.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <ctime>
#include <sstream>
#include <Windows.h>      // for FreeConsole()


#pragma comment(lib, "Ws2_32.lib")

using namespace std;

/*****************************************
*
*   class Pause
*
*   intended to "mock up" the duration
*   of an exam;
*
*   having two (overloaded) constructors
*   suffices for the intended use of
*   this class
*
******************************************/
class Pause
{
    public:
        Pause(int iDuration);

        Pause(double fDuration);

    //~Pause();

};

/****************************************
*
*   class Pause
*
*   implementation
*
*****************************************/

/*     constructor
 *
 *     @param: int, duration of the pause
 */
Pause::Pause(int iDuration)
{
    int temp;

	temp = time(NULL) + iDuration;

	while(time(NULL) < temp);
}

/*
 *     constructor
 *
 *     @param: double, duration of pause
 *
 */
Pause::Pause(double dDuration)
{
   int temp;

    //cout << "\n  ===== double duration: " << dDuration << endl;
	temp = clock() + (int)(dDuration * CLOCKS_PER_SEC);
	//cout << "\n  ===== temp: " << temp << endl;

	while(clock() < temp);
}

// for future use
// TARGETFILE for Cirrus 4000
#define TARGETFILE "c:\\Documents and settings\\All Users\\Application Data\\Carl Zeiss Meditec\\Cirrus background processing service\\Logs\\Log.txt"

// TARGETFILE for Cirrus 5000
//#define TARGETFILE "c:\\ProgramData\\Carl Zeiss Meditec\\Cirrus background processing service\\Logs\\Log.txt"

// TARGETFILE on Windows 7 PC
//#define TARGETFILE "c:\\Eugene\\testsofgnuplotandoctave\\logs\\Cirrus_log.txt"

// Building blocks of log file entries
const string scantype [] = {"MacularCombo","FrameAverageRaster", "GlaucomaCube", "AnteriorSegmentRaster", "AnteriorSegmentCombo"};
const string middlepart = " DEBUG WorkItem..ctor - Add work item: ";
const string closingpart = " DEBUG Context.Close - Closing database connection.";

// output file: mocked log file
//ofstream protolog("c:\\Eugene\\cpp\\protologs\\current_one.txt");
ofstream protolog(TARGETFILE);

// configuration files:
// file to be used for creation of well structured log file
// (intended mostly to assure correctness of this program : timeintersim)
ifstream conf("C:\\Eugene\\cpp\\configfile.txt",ios::in);

// file to be used for creation of log files with "elements of randomness"
// (supposed to close resemble actual log files)
ifstream configrun("C:\\Eugene\\cpp\\configruntime.txt",ios::in);

// global variables
clock_t ct, ct_start;

// forward declarations of functions outside any class
void addFewPlaceHoldingStrings(int numberOfStrings);
string makeTimePrefix();

///////////////////////////////////////////////////////////////////////////
// class Configurator
/***********************************
*
*       Abstract Class Configurator
*
*       (Base class)
*
*       definition
*
************************************/
class Configurator
{
    public:
        int iNumOfKnownScantypes;
        Configurator();                 // base class default constructor

        // all functions below are pure virtual:
        // the classes derived from this one
        // must implement each of these functions (methods)
        virtual int getNumOfItearations() = 0;
        virtual double getPauseAddend() = 0;
        virtual int getNumOfPlaceHolderStrings() = 0;
        virtual int getScanTypeIdx() = 0;
};

/***********************************
*
*       Class Configurator
*
*       implementation
*
************************************/
Configurator::Configurator()
{
   iNumOfKnownScantypes = sizeof(scantype)/sizeof(char *);
   //cout << "\n Constructor of Configurator, the base class: number of known scan types: " << iNumOfKnownScantypes << endl;
}

//  end of class Configurator
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ConfigFromFile
/***********************************
*
*       Class ConfigFromFile
*
*       definition
*
************************************/
class ConfigFromFile : public Configurator
{
    private:

        // total number of iteration of the main loop
        // in main routine; the number of "forged" entries
        // of the exams equals
        // if iNumberOfIterations is odd: (iNumberOfIterations + 1)/2
        // if iNumberOfIterations is even: iNumberOfIterations/2
        int iNumberOfIterations;

        // random component of the total duration of a
        // pause; pause "mock up" the time of an exam
        double dPauseAddend;

        // the number of entries between the
        // entries signifying start and and
        // of a "mocked up" exam
        int iNumberOfPlaceHolderStrings;

        // this counter is used to assure
        // successive selection of types of scan
        int iCallCount;

    public:

        ConfigFromFile();               //derived class' default constructor

        int getNumOfItearations() override;
        void setNumOfIterations();

        double getPauseAddend() override;
        void setPauseAddend();

        int getNumOfPlaceHolderStrings() override;
        void setNumOfPlaceHolderStrings();

        int getScanTypeIdx() override;
};

/***********************************
*
*       Class ConfigFromFile
*
*       implementation
*
************************************/
/*  constructor:
 *
 *  @param:  none
 *
 *  Uses the member functions to read the values
 *  from the configiration file; initializes
 *  iCallCount to 0
 */
ConfigFromFile::ConfigFromFile()
{
    //cout << "\n constructor ConfigFromFile" << endl;

    iCallCount = 0;

    //cout << "Number of scan types in constructor: " << iNumOfKnownScantypes << endl;

    setNumOfIterations();

    setPauseAddend();

    setNumOfPlaceHolderStrings();

    //cout << "iNumberOfIterations: " << getNumOfItearations() << ", dPauseAddend: " << getPauseAddend() << ", iNumberOfPlaceHolderStrings: " << getNumOfPlaceHolderStrings() << endl;
}
/*
 *     method getNumOfIterations
 *
 *     ordinary getter
 */
int ConfigFromFile:: getNumOfItearations()
{
    return iNumberOfIterations;
}

/*
 *     method setNumOfIterations
 *
 *     reads the parameter from configuration file
 *     and adjust that number accordingly to Comment 1 above:
 *
 *    Example: Configfile.txt: 15 3.5 3
 *  In order to use all scan types evenly, the first entry in the configfile.txt,
 *  in this example 15, should be used by the program in the following way:
 *
 * 	The actual number of iterations =
 * 			smallest multiple of (2 * number_of_entries_in_scantype) which is
 * 			greater than or equal to the first parameter in configfile.txt
 *
 * For the given example (first parameter: 15, number of scantypes: 5),
 * 	    The actual number of iterations = smalllest multiple of (2 * 5) greater than
 * 		or equal 15. The number is 20
 */
void ConfigFromFile::setNumOfIterations()
{
    conf >> iNumberOfIterations;

    int iTwiceNumOfTypes = 2 * iNumOfKnownScantypes;

    //cout << "twice number of types:" << iTwiceNumOfTypes << endl;

    div_t QuoteAndRem = div(iNumberOfIterations,iTwiceNumOfTypes);

    //cout << "quotient of division: " << QuoteAndRem.quot << endl;

    iNumberOfIterations = iTwiceNumOfTypes * (QuoteAndRem.quot + 1);

    //cout << "number of iterations: " << iNumberOfIterations << endl;

}

/*
 *     method getPauseAddend
 *
 *     ordinary getter
 */
double ConfigFromFile::getPauseAddend()
{
    return dPauseAddend;
}

/*
 *     method setPauseAddend
 *
 *     reads the parameter from configuration file
 *
 */
void ConfigFromFile::setPauseAddend()
{
    conf >> dPauseAddend;
}

/*
 *     method setNumOfPlaceHolderStrings
 *
 *     reads the parameter from configuration file
 *
 */
void ConfigFromFile::setNumOfPlaceHolderStrings()
{
    conf >> iNumberOfPlaceHolderStrings;
}

/*
 *     method getNumOfPlaceHoldingStrings
 *
 *     ordinary getter
 *
 */
int ConfigFromFile::getNumOfPlaceHolderStrings()
{
    return iNumberOfPlaceHolderStrings;
}

/*     method getScanTypeIdx
 *
 *     @param: none
 *
 *     @return: an integer to be used by calling routine
 *              as index of an entry in array scantype
 *
 *     Assures successive selection of all integer numbers
 *     in interval 0...(number_of_entries_in_array_scantype - 1)
 */
int ConfigFromFile::getScanTypeIdx()
{
    //cout << "This is method getScanType of class ConfigFromFile: " << sizeof(scantype)/sizeof(char *) << endl;

    // divisor in the calculation of itoreturn below
    //   ====   sizeof(scantype)/sizeof(char *) ===
    //determines actual size of array scantype

    int itoreturn = (iCallCount++) % iNumOfKnownScantypes;

    // the size of array of types scantype is 5
    // therefore this method returns an integer
    // 0, 1, 2, 3, or 4
    return itoreturn;
}
// end of class ConfigFromFile
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ConfigRunTime
/***********************************
*
*       Class ConfigRunTime
*
*       definition
*
************************************/
class ConfigRunTime:public Configurator
{
private:
    // total number of iteration of the main loop
    // in main routine; the number of "forged" entries
    // of the exams equals
    // if iNumberOfIter is odd: (iNumberOfIter + 1)/2
    // if iNumberOfIter is even: iNumberOfIter /2
    int iNumOfIter;

    // helper method dealing with random numbers in given interval
    int randivrem(int);

    // setter used from within the constructor
    void setNumOfIterations();
public:
    ConfigRunTime();        //derived class default constructor

    int getNumOfItearations() override;
    double getPauseAddend() override;
    int getNumOfPlaceHolderStrings() override;
    int getScanTypeIdx() override;
};

/***********************************
*
*       Class ConfigRunTime
*
*       implementation
*
************************************/

/*     constructor
 *
 *     @param: none
 *
 *     uses the setter for number of iterations;
 *     that number is the only parameter set during
 *     creation of an object
 */
ConfigRunTime::ConfigRunTime()
{
    //cout << "\n constructor ConfigRunTime" << endl;
    setNumOfIterations();
    //cout << "iNumberOfIterations: " << getNumOfItearations() << ", dPauseAddend: " << getPauseAddend() << ", iNumberOfPlaceHolderStrings: " << getNumOfPlaceHolderStrings() << endl;
}

/*     method setNumOfIterations
 *
 *     @param: none
 *
 *     @return: none
 *
 *     if there if the configuration file,
 *         read the number of iteration from there
 *     otherwise
 *         assume predetermined value
 *
 */
void ConfigRunTime::setNumOfIterations()
{
    if (configrun)
    {
        configrun >> iNumOfIter;
    }
    else
    {
        iNumOfIter = 25;
    }
}

/*
 *     method getNumOfIterations
 *
 *     ordinary getter
 *
 */
int ConfigRunTime::getNumOfItearations()
{
    return iNumOfIter;
}

/*     method getPauseAddend
 *
 *     @param: none_of
 *
 *     @return: double, random component of
 *              total duration of a pause
 *
 */
double ConfigRunTime::getPauseAddend()
{
    double denom = randivrem(3);
    if (denom == 0.0)
    {
        denom = 0.5;
    }

    double dtoreturn = (double)(randivrem(4))/denom;

    return dtoreturn;
}

/*
 *     method getNumOfPlaceHolderStrings
 *
 *     @return: an integer, randomly selected
 *     from interval 0 ... 3
 */
int ConfigRunTime::getNumOfPlaceHolderStrings()
{
    return randivrem(4);
}

/*     method getScanTypeIdx
 *
 *     @param: none
 *
 *     @return: integer, to be used by calling routine
 *              as index of entry in array scantype
 *
 *     Assures random selection of integers from interval
 *     0...(number_of_entries_in_array_scantype - 1)
 *
 */
int ConfigRunTime::getScanTypeIdx()
{
//    return randivrem(sizeof(scantype)/sizeof(char *));
        return randivrem(iNumOfKnownScantypes);
}

/*     method randivrem
 *
 *     @param: integer, divisor
 *
 *     @return: integer, remainder of division
 *              of a random number by the divisor
 *
 *     Assures random selection of an integer in the interval
 *     0 ... (divisor-1)
 *
 */
int ConfigRunTime::randivrem(int divisor)
{
    return (rand() % divisor);
}
// end of class ConfigRunTime
///////////////////////////////////////////////////////////////////////////
/* *
 *     main routine
 *
 *     detaches itself from the console
 *
 *     selects "source" of configuration parameters:
 *     if config file is present
 *         create the configurator capable reading the values from that file
 *     else
 *         create configurator that calculates most of vital parameter during execution
 *         of main loop
 *
 *     main loop runs pre-configured number of times;
 *         it can be interrupted by creating the file c:\temp\stopsim.txt
 *
 *     each iteration of the main loop either
 *         creates mock up entry signifying start of an exam
 *         or
 *         creates mock up entry signyfying end of the exam
 *         or
 *         creates placeholding entry
 */

int main(int argv, char ** argc)
{

    FreeConsole();

	int idxIteration;
	double denom;
	stringstream timeprefix;

	ct_start = clock();
	srand(time(NULL));

    // create configurator;
	Configurator * ptrConf;
	if (conf)
    {
        ptrConf = new ConfigFromFile();
    }
    else
    {
        ptrConf = new ConfigRunTime();
    }

    bool bWorkItemAdded = false;
    int iNumberOfIterations = ptrConf->getNumOfItearations();
    if ((iNumberOfIterations % 2) != 0 )      // enforces even number of iterations
    {                                         // to make sure that each "opening entry"
        iNumberOfIterations += 1;             // in the created file is matched by a
    }                                         // closing entry

      // see comment below about possible implementation
/*    struct stat st;*/
    for (idxIteration = 1; idxIteration <= iNumberOfIterations; idxIteration++)
	{

        // a possible implementation; left here for
        // educational purposes; advantage (in comparison with
        // the implemented alternative:
        // does not entail creation and subsequent destruction
        // of an object: see stopsim below
/*         memset(&st,0,sizeof(struct stat));
 *         if (stat("C:\\temp\\stopsim.txt",&st) != 0)
 *         {
 *             cout << "stopsim does not exist" << endl;
 *         }
 */

        // an object of class fstream must be created on
        // each iteration of the loop and later destructed
        fstream stopsim("C:\\temp\\stopsim.txt");
        if (stopsim) break;

        //Pause p2((double)idxIteration + (ptrConf->getPauseAddend()));
        Pause p2(ptrConf->getPauseAddend());

		timeprefix.str("");
        timeprefix << makeTimePrefix();

        if (bWorkItemAdded)
        {
           protolog << timeprefix.str() << closingpart << endl;
           protolog.flush();
           bWorkItemAdded = false;
        }
        else
        {
            protolog << timeprefix.str() << middlepart << scantype[ptrConf->getScanTypeIdx()] << endl;
            protolog.flush();
            bWorkItemAdded = true;
        }

        addFewPlaceHoldingStrings(ptrConf->getNumOfPlaceHolderStrings());
        delete stopsim;
    }

   // ct = clock() - ct_start;

	return 0;

}

/*     function addFewPlaceHoldingStrings
 *
 *     @param: integer: number of place holding strings to be created
 *
 *     @return: none
 *
 *     adds to the output file the strings with timestamp
 *     compliant to timestamp format; the remaining part of
 *     the string does not bear any significance
 *
 */
void addFewPlaceHoldingStrings(int numberOfStrings)
{
    Pause plps(1);
    for (int i = 0; i < numberOfStrings; i++)
    {
        stringstream plhldrtime;
        plhldrtime << makeTimePrefix();
        protolog << plhldrtime.str() << " This is placeholder # " << i << endl;
        protolog.flush();
        Pause pse(0.5);
    }
}

/*
 *     method makeTimePrefix
 *
 *     @param: none
 *
 *     @return: string, timestamp compliant to timestamp format of actual logfile
 *
 *     Example: 2014-07-31 12:14:44,344
 */
string makeTimePrefix()
{
    char timestamp[32];

	time_t timeretrieved;
	static time_t oldtime;
	static div_t oldquote;

    memset(&timeretrieved,sizeof(time_t),0);

	time(&timeretrieved);

	memset(timestamp,'\0',sizeof(timestamp));

	strftime(timestamp,sizeof(timestamp) - 1,"%Y-%m-%d %X",localtime(&timeretrieved));

	ct = clock() - ct_start;
    div_t quote;

    quote = div(ct, CLOCKS_PER_SEC);

    int iFractionSeconds = quote.rem;

//    This part of code intends to prevent situations like the one shown below
//    where two consecutive entries have inversely ordered time stamps; it happened
//    because the fractions of the seconds following the decimal point are not
//    directly retrieved from the timestamp
//    .......................................
//    2014-07-29 15:07:29,875 This is placeholder # 0
//    2014-07-29 15:07:29,375 This is placeholder # 1
//    ........................................
    if (difftime(timeretrieved,oldtime) == 0)
    {
        //cout << "from MakeTimePrefix: dealing with difftime zero" << endl;
        if (oldquote.rem > iFractionSeconds)
        {
            //cout << "from MakeTimePrefix: dealing with bad fraction seconds" << endl;

            if (oldquote.rem < 999)
            {
                iFractionSeconds = oldquote.rem + 1;
            }
            else
            {
                iFractionSeconds = oldquote.rem;
            }
        }
    }
    stringstream timeprefix;
    timeprefix << timestamp << "," << iFractionSeconds;

	oldtime = timeretrieved;
	oldquote = quote;

	return timeprefix.str();
}

