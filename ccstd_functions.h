#pragma once

// CADECRAFT STANDARD HEADERS
// FUNCTIONS AND CLASSES
// Several utility classes and functions that I use in my C++ programs

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <random>
#include <map>
#ifdef _WIN32
#include <stdio.h>
#include <wchar.h>
#include <windows.h>
#endif

// Consts
const std::string CCSTD_RECENTVERSION = "0.2.3";
const std::string CCSTD_EDITDATE = "2023/12/29";
const std::string CCSTD_CREATEDATE = "2022/06/27";

// Console "ccConsole"; "cccon"; "cons"
// Displays and manages console messages as well as the REM init sequence
class ccConsole {
private:
	// Defaults
	std::string appName;
	std::string appVer;
	std::string fileToDump;
	bool logDbgs;
	int spacing;
	std::vector<std::string> loggedMsgs;
public:
	// Constructor
	ccConsole(std::string inappName, std::string inappVer, std::string infileToDump = "[none]", bool inlogDbgs = true, int inspacing = 4, bool sendInitMsg = true, std::string editDate = "[none]", std::string creator = "[none]", std::string createDate = "[none]") {
		appName = inappName;
		appVer = inappVer;
		fileToDump = infileToDump;
		logDbgs = inlogDbgs;
		spacing = inspacing;
		if (sendInitMsg) {
			// REM init sequence; check for morning star, then print
			std::string morningStar = "[successful]";
			consLog("SYS", "REM init sequence [console] for '" + appName + "' v" + appVer);
			//consLog("SYS", "- programName:    '" + appName + "'");
			consLog("SYS", "- recentVersion:  v" + appVer);
			consLog("SYS", "- editDate:       " + editDate);
			consLog("SYS", "- morningStar:    " + morningStar);
			if (creator != "[none]") { consLog("SYS", "- creator:        " + creator); }
			if (createDate != "[none]") { consLog("SYS", "- createDate:     " + createDate); }
			consBlank();
		}
	}
	// Console log function
	bool consLog(std::string type = "DBG", std::string contents = "<Log Contents>", std::string options = "[none]") {
		// Msg types: DBG=debug, GAME=game, STAT=status, SYS=system
		bool success = false;
		int uniqueSpacing = spacing;
		uniqueSpacing -= (type.length()-3);
		if (uniqueSpacing < 0) { uniqueSpacing = 0; }
		// Determine msg
		std::string newMsg = type + ":";
		for (int i = 0; i < spacing; i++) {
			newMsg += " ";
		}
		newMsg += contents;
		// Log msg to console, if it can (DBG only allowed with options)
		if (type != "DBG" || logDbgs) {
			std::cout << newMsg << "\n";
			success = true;
		}
		// Add to vector, if it can (DBG never allowed)
		if (type != "DBG") {
			loggedMsgs.push_back(newMsg);
		}
		/* // Try to dump msg to file
		if (fileToDump != "[none]") {
			try {
				// Try to dump msg to file
				std::ofstream 
			}
			catch (const std::exception& e) {
				// Failed to dump
			}
		} */
		return success;
	}
	// Console blank function
	bool consBlank() {
		std::cout << "\n";
		loggedMsgs.push_back("\n");
		return true;
	}
	// Get one char inputted by the user, between validMin and validMax
	char inputChar(
		char validMin = '1',
		char validMax = '9',
		std::string invalidMsg = "Input invalid; please try again: "
	) {
		// Until valid
		while (true) {
			// Get the inputted character
			char c;
			try {
				std::cin >> c;
				// Check validity
				if (c >= validMin && c <= validMax) return c;
			}
			catch (std::exception& e) {}
			// Invalid: display msg asking for re-input
			std::cout << invalidMsg;
		}
	}
	// Get one decimal number inputted by the user, between validMin and validMax
	double inputDecimal(
		double validMin = 0.0,
		double validMax = 10.0,
		std::string invalidMsg = "Input invalid; please try again: "
	) {
		// Until valid
		while (true) {
			// Get the inputted string
			std::string s;
			try {
				std::getline(std::cin, s);
				// Check validity
				double parsedDouble = std::stod(s);
				if (parsedDouble >= validMin && parsedDouble <= validMax) return parsedDouble;
			}
			catch (std::exception& e) {}
			// Invalid: display msg asking for re-input
			std::cout << invalidMsg;
		}
	}
	// Get one line of text inputted by the user, NOT trimmed for whitespace (can be blank)
	std::string inputLine() {
		std::string s;
		std::getline(std::cin, s);
		return s;
	}
	// Wait for the user to press enter
	void awaitEnter(std::string message = "", std::string after = "") {
		std::cout << message;
		std::string s;
		std::getline(std::cin, s);
		if (after.size() > 0) std::cout << after;
	}
	// Enable the Virtual Console for rendering ANSI codes, such as ccColor (Windows only)
	int enableVirtualConsole() {
#ifdef _WIN32
		// Windows: the Virtual Console can be enabled
		// Set output mode to handle virtual terminal sequences
		// Source: https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#example-of-enabling-virtual-terminal-processing
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hOut == INVALID_HANDLE_VALUE) return false;
		HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
		if (hIn == INVALID_HANDLE_VALUE) return false;
		DWORD dwOriginalOutMode = 0;
		DWORD dwOriginalInMode = 0;
		if (!GetConsoleMode(hOut, &dwOriginalOutMode)) return false;
		if (!GetConsoleMode(hIn, &dwOriginalInMode)) return false;
		DWORD dwRequestedOutModes = ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
		DWORD dwRequestedInModes = ENABLE_VIRTUAL_TERMINAL_INPUT;
		DWORD dwOutMode = dwOriginalOutMode | dwRequestedOutModes;
		if (!SetConsoleMode(hOut, dwOutMode)) {
			// we failed to set both modes, try to step down mode gracefully.
			dwRequestedOutModes = ENABLE_VIRTUAL_TERMINAL_PROCESSING;
			dwOutMode = dwOriginalOutMode | dwRequestedOutModes;
			if (!SetConsoleMode(hOut, dwOutMode)) {
				// Failed to set any VT mode, can't do anything here.
				return -1;
			}
		}
		DWORD dwInMode = dwOriginalInMode | dwRequestedInModes;
		if (!SetConsoleMode(hIn, dwInMode)) {
			// Failed to set VT input mode, can't do anything here.
			return -1;
		}
		return 0;
#else
		// Not Windows: the Virtual Console cannot be enabled
		return -1;
#endif
	}
};

// Math class "ccMath"
// Stores custom math functions for graphics, games, or other calculations
class ccMath {
public:
	// Get rotation in degrees from point 1 to point 2
	static double faceToPoint(double x1, double y1, double x2, double y2, double rotCurrent = -1, double rotLatency = 0) {
		// Calculate new rotation
		double rotNew = std::atan2(y2 - y1, x2 - x1) * 180 / std::_Pi_val - 90;
		// Apply latency (lerp between a and b): 0.0=instant; 0.9=slow; 1.0=no new rot applied 
		double rotNew2 = (rotNew) * (1 - rotLatency) + (rotCurrent) * (rotLatency);
		// Account for 360deg rotations in either direction (passing 0)
		if (rotCurrent - rotNew >= 300) {
			rotNew2 = (rotNew) * (1 - rotLatency) + (rotCurrent - 360) * (rotLatency);
		}
		else if (rotNew - rotCurrent >= 300) {
			rotNew2 = (rotNew - 360) * (1 - rotLatency) + (rotCurrent) * (rotLatency);
		}
		// Return
		return rotNew2;
	}
	// Get distance from point 1 to point 2
	static double distance(double x1, double y1, double x2, double y2) {
		// Calculate distance
		double resDist = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
		// Return
		return resDist;
	}
	// Get a random number 0 <= n < 1
	static double randd() {
		return ((double) rand() / RAND_MAX);
	}
};

// String utilities class "ccStringUtil"
// Stores simple string utility functions
class ccStringUtil {
public:
	// Trim/strip whitepsace off a string
	static std::string trim(std::string s) {
		return trimLeft(trimRight(s));
	}
	static std::string trimLeft(std::string s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
			return !std::isspace(ch);
		}));
		return s;
	}
	static std::string trimRight(std::string s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
			return !std::isspace(ch);
		}).base(), s.end());
		return s;
	}
	static std::string lowerCase(std::string s) {
		for (int i = 0; i < s.length(); i++) s[i] = std::tolower(s[i]);
		return s;
	}
	static std::string upperCase(std::string s) {
		for (int i = 0; i < s.length(); i++) s[i] = std::toupper(s[i]);
		return s;
	}
	static std::vector<std::string> split(std::string s, char delim = ' ') {
		std::vector<std::string> res;
		std::string curr = "";
		for (int i = 0; i < s.length(); i++) {
			if (s[i] == delim) {
				if (curr != "") {
					res.push_back(curr);
					curr = "";
				}
			} else {
				curr += s[i];
			}
		}
		if (curr != "") res.push_back(curr);
		return res;
	}
	static bool contains(std::string s, std::string substring) {
		return s.find(substring) != std::string::npos;
	}
};

// Dbg formatter "ccFormat"
// Formats arrays, items, etc. for debugging
class ccFormat {
public:
	// Format items passed in, such as arrays, etc.
	static std::string format(std::string inarr[], size_t inarrsize, bool brackets = true) { // arr: strings
		std::string res = "";
		if (brackets) res += "[";
		for (int i = 0; i < inarrsize; i++) { res += inarr[i]; if (i != inarrsize - 1) res += ","; }
		if (brackets) res += "]";
		return res;
	}
	static std::string format(int inarr[], size_t inarrsize, bool brackets = true) { // arr: ints
		std::string res = "";
		if (brackets) res += "[";
		for (int i = 0; i < inarrsize; i++) { res += std::to_string(inarr[i]); if (i != inarrsize - 1) res += ","; }
		if (brackets) res += "]";
		return res;
	}
	static std::string format(double inarr[], size_t inarrsize, bool brackets = true) { // arr: doubles
		std::string res = "";
		if (brackets) res += "[";
		for (int i = 0; i < inarrsize; i++) { res += std::to_string(inarr[i]); if (i != inarrsize - 1) res += ","; }
		if (brackets) res += "]";
		return res;
	}
	static std::string format(char inarr[], size_t inarrsize, bool brackets = true) { // arr: chars
		std::string res = "";
		if (brackets) res += "[";
		for (int i = 0; i < inarrsize; i++) { res += inarr[i]; if (i != inarrsize - 1) res += ","; }
		if (brackets) res += "]";
		return res;
	}
	static std::string format(int in) { // val: int
		return std::to_string(in);
	}
	static std::string format(double in) { // val: double
		return std::to_string(in);
	}
	static std::string format(char in) { // val: chars
		std::string res = "";
		res += in;
		return res;
	}
	static void trimr(std::string s) {
		s.erase(s.find_last_not_of(" \n\r\t"));
	}
};

// Color printer "ccColor"
// For console ANSI codes (ccConsole::enableVirtualTerminal may need to be called for Windows support)
class ccColor {
public:
	// Color enum
	enum class Color {
		Reset,
		Black,
		Gray,
		Red,
		Green,
		Yellow,
		Blue,
		Pink,
		Cyan,
		CyanMiku,
		PinkSakura,
		BlueRem,
		RedL,
		CcBlue,
		CcBlueDark,
		CcGray,
		CcWarn
	};
	// Get console ANSI code for an inputted color (ccColor::Color::Reset or no args to clear) ("pretty" to display code and name)
	static std::string c(Color color = Color::Reset, bool pretty = false) {
		// Color map (\033 will be appended on return)
		std::map<Color, std::string> colors = colorToCode();
		// Determine color chosen
		std::string chosen;
		if (colors.find(color) == colors.end()) {
			chosen = colors.find(Color::Reset)->second;
		}
		else {
			chosen = colors.find(color)->second;
		}
		if (pretty) {
			return "\033" + chosen + "\\033" + chosen + " - " + colorToString().find(color)->second + "\n";
		}
		else {
			return "\033" + chosen;
		}
	}
	// Print console ANSI code for an inputted color (ccColor::Color::Reset or no args to clear) ("pretty" to display code and name)
	static void cSet(Color color = Color::Reset, bool pretty = false) {
		// Print
		std::cout << c(color, pretty);
	}
	// Print console ANSI code for an inputted color ("reset" or no args to clear) (pretty to display code and name) (from string)
	// Print all colors pretty (for reference)
	static void cPrintAll() {
		// Color map
		std::map<Color, std::string> allColors = colorToCode();
		// Iterate through and pretty print (alphabetical order)
		std::map<Color, std::string>::iterator it;
		for (it = allColors.begin(); it != allColors.end(); it++) {
			cSet(it->first, true);
		}
		cSet(Color::Reset);
	}
private:
	// Get color map (all color names) from Color enum
	static std::map<ccColor::Color, std::string> colorToString() {
		const std::map<Color, std::string> colorStrings = {
			{Color::Reset, "Reset"},
			{Color::Black, "Black"},
			{Color::Gray, "Gray"},
			{Color::Red, "Red"},
			{Color::Green, "Green"},
			{Color::Yellow, "Yellow"},
			{Color::Blue, "Blue"},
			{Color::Pink, "Pink"},
			{Color::Cyan, "Cyan"},
			{Color::CyanMiku, "CyanMiku"},
			{Color::PinkSakura, "PinkSakura"},
			{Color::BlueRem, "BlueRem"},
			{Color::RedL, "RedL"},
			{Color::CcBlue, "CcBlue"},
			{Color::CcBlueDark, "CcBlueDark"},
			{Color::CcGray, "CcGray"},
			{Color::CcWarn, "CcWarn"}
		};
		return colorStrings;
	}
	// Get color map (all color ANSI codes) from Color enum
	static std::map<ccColor::Color, std::string> colorToCode() {
		const std::map<Color, std::string> colorCodes = {
			{Color::Reset, "[39m"}, // [39m\\033[49m
			{Color::Black, "[30m"},
			{Color::Gray, "[90m"},
			{Color::Red, "[91m"},
			{Color::Green, "[92m"},
			{Color::Yellow, "[93m"},
			{Color::Blue, "[94m"},
			{Color::Pink, "[95m"},
			{Color::Cyan, "[96m"},
			{Color::CyanMiku, "[38;2;41;194;191m"}, // Full color: [38;2; <r> ; <g> ; <b> m
			{Color::PinkSakura, "[38;2;255;140;171m"},
			{Color::BlueRem, "[38;2;144;193;252m"},
			{Color::RedL, "[38;2;166;10;10m"},
			{Color::CcBlue, "[38;2;28;160;203m"},
			{Color::CcBlueDark, "[38;2;18;100;158m"},
			{Color::CcGray, "[38;2;21;25;31m"},
			{Color::CcWarn, "[38;2;230;157;0m"}
		};
		return colorCodes;
	}
};

// REM data storage/printing "ccRemData"
// Used for storing recentVersion, editDate, and morningStar
class ccRemData {
private:
	// Members
	std::string recentVersion;
	std::string editDate;
	bool morningStar;

public:
	ccRemData(std::string recentVersion, std::string editDate, bool morningStar) {
		this->recentVersion = recentVersion;
		this->editDate = editDate;
		this->morningStar = morningStar;
	}
	std::string toString(bool separateLines = false) {
		std::string delim = "";
		if (separateLines) delim = "\n";
		else delim = "; ";
		return "R: v" + recentVersion + delim + "E: " + editDate + delim + "M: " + (morningStar ? "[successful]" : "[unsuccessful]");
	}
	std::string getRecentVersion() {
		return recentVersion;
	}
	std::string getEditDate() {
		return editDate;
	}
	std::string getMorningStarDisplay() {
		return morningStar ? "[successful]" : "[unsuccessful]";
	}
};

// Encryption class "ccEncryption"
// Currently has no real encryption algorithms; these merely obscure text for the human eye
class ccEncryption {
public:
	// Simple incrementation
	static std::string basic_encrypt(std::string input, int shift_by = 1) {
		std::string res = "";
		for (int i = 0; i < input.size(); i++) {
			char adjusted_here = (char)((int)input.at(i) + shift_by);
			res += adjusted_here;
		}
		return res;
	}
	static std::string basic_decrypt(std::string input, int shift_by = 1) {
		std::string res = "";
		for (int i = 0; i < input.size(); i++) {
			char adjusted_here = (char)((int)input.at(i) - shift_by);
			res += adjusted_here;
		}
		return res;
	}
};

// Date and time class "ccDate"
// Used for getting and formatting the current date/time
class ccDate {
private:
	static std::string toTwoDigitString(int i) {
		std::string res = std::to_string(i);
		if (res.size() <= 1) res = "0" + res;
		return res;
	}
public:
	static std::string currentDateToString() {
		time_t now = time(NULL);
		tm* local = localtime(&now);
		std::string res = "";
		res += std::to_string(1900 + local->tm_year);
		res += "/";
		res += toTwoDigitString(1 + local->tm_mon);
		res += "/";
		res += toTwoDigitString(local->tm_mday);
		return res;
	}
	static std::string currentTimeToString() {
		time_t now = time(NULL);
		tm* local = localtime(&now);
		std::string res = "";
		res += toTwoDigitString(local->tm_hour);
		res += ":";
		res += toTwoDigitString(local->tm_min);
		res += ":";
		res += toTwoDigitString(local->tm_sec);
		return res;
	}
	static std::string currentDateTimeToString() {
		std::string res = currentDateToString();
		res += " ";
		res += currentTimeToString();
		return res;
	}
};
