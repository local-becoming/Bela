
#include <Bela.h>
#include <typeinfo> 
#include <libraries/Gui/Gui.h>
#include <algorithm>
#include <string>
#include <iterator>
#include <sstream>
#include <math.h>

#include <time.h>

Gui gui;	

std::vector<int> dateTimeComponents;
float gTimePeriod = 0.01;

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::vector<std::string> currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    std::vector<std::string> dateTime;
    char       date[50];
    char       time[50];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(date, sizeof(date), "%Y-%m-%d", &tstruct);
    strftime(time, sizeof(time), "%T", &tstruct);
	dateTime.emplace_back(date);
	dateTime.emplace_back(time);
    return dateTime;
}

std::vector<std::string> split(const std::string& s, char delimiter)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}

bool setup(BelaContext *context, void *userData)			
{
	std::vector<std::string> dateComponents = split(currentDateTime()[0], '-');
	for(unsigned int i=0; i<dateComponents.size(); i++) 
		dateTimeComponents.push_back(atoi(dateComponents[i].c_str()));
	
	std::vector<std::string> timeComponents = split(currentDateTime()[1], ':');
	for(unsigned int i=0; i<timeComponents.size(); i++) 
		dateTimeComponents.push_back(atoi(timeComponents[i].c_str()));
		
	dateTimeComponents.push_back(0);// millisedcons
	
	gui.setup(5555, "gui", context->projectName);

	return true;
}

void calcDate(unsigned int samplesElapsed, unsigned int sampleRate, std::vector<int> &date)
{
	unsigned int miliseconds = floor(1000 * samplesElapsed / sampleRate) + date[6];
	date[6] = miliseconds % 1000;
	unsigned int seconds = floor(miliseconds / 1000) + date[5];
	date[5] = seconds%60;
	unsigned int minutes = floor(seconds/60) + date[4];
	date[4] = minutes%60;
	unsigned int hours = floor(minutes/60) + date[3];
	date[3] = hours%24;
	unsigned int days = floor(hours/60) + date[2];
	date[2] = days%30; // This is wrong
	unsigned int months = floor(days/30) + date[1];
	date[1] = months%12;
	date[0] = floor(months/12) + date[0];
}

void render(BelaContext *context, void *userData)
{	
	static unsigned int count = 0;
	
	for(unsigned int n = 0; n < context->audioFrames; n++) {
		if (count >= gTimePeriod*context->audioSampleRate)
		{
			calcDate(count, context->audioSampleRate, dateTimeComponents);
			gui.sendBuffer(0, dateTimeComponents);
			count = 0;
		}
		count++;
	}
	
}

void cleanup(BelaContext *context, void *userData)
{

}
