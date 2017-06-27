#include <map>
#include <fstream>
#include <digi/Utility/ArrayUtility.h>
#include <digi/Utility/VectorUtility.h>
#include <digi/Utility/StringUtility.h>
#include <digi/Utility/Convert.h>
#include <digi/System/FileSystem.h>
#include <digi/CodeGenerator/CodeWriter.h>
#include <boost/range.hpp>
#include <digi/Utility/foreach.h>

// include system headers for virtual keycodes
#ifdef _WIN32
	#include <windows.h>
#endif
#ifdef __APPLE__
	#import <Carbon/Carbon.h>
#endif

using namespace digi;

struct Attribute
{
	const char* name;
	int code;
};

int main()
{
	typedef std::pair<const std::string, int> AttributePair;
	std::map<std::string, int> attributes;
	
	static const Attribute attributeList[] =
	{
	#if defined(_WIN32)

		{"back", VK_BACK},
		{"tab", VK_TAB},
		{"return", VK_RETURN},
		{"space", VK_SPACE},
		{"escape", VK_ESCAPE},

		{"shift", VK_SHIFT},
		{"leftShift", VK_LSHIFT},
		{"rightShift", VK_RSHIFT},
		{"control", VK_CONTROL},
		{"leftControl", VK_LCONTROL},
		{"rightControl", VK_RCONTROL},
		{"alt", VK_MENU},
		{"leftAlt", VK_LMENU},
		{"rightAlt", VK_RMENU},
		{"command", -1},
		{"leftCommand", VK_LWIN},
		{"rightCommand", VK_RWIN},

	//	{"pause", VK_PAUSE},
		{"pageUp", VK_PRIOR},
		{"pageDown", VK_NEXT},
		{"home", VK_HOME},
		{"end", VK_END},
		{"left", VK_LEFT},
		{"right", VK_RIGHT},
		{"up", VK_UP},
		{"down", VK_DOWN},
		{"insert", VK_INSERT},
		{"delete", VK_BACK},
		{"forwardDelete", VK_DELETE},

		{"add", VK_ADD},
		{"subtract", VK_SUBTRACT},
		{"multiply", VK_MULTIPLY},
		{"divide", VK_DIVIDE},
		{"decimal", VK_DECIMAL},
		//{"clear", },
		{"enter", },
		//{"equals", },

	#elif defined(__APPLE__)
		
		{"back", kVK_Delete},
		{"tab", kVK_Tab},
		{"return", kVK_Return},
		{"space", kVK_Space},
		{"escape", kVK_Escape},
		
		{"shift", -1},
		{"leftShift", -1},
		{"rightShift", -1},
		{"control", -1},
		{"leftControl", -1},
		{"rightControl", -1},
		{"alt", -1},
		{"leftAlt", -1},
		{"rightAlt", -1},
		{"command", -1},
		{"leftCommand", -1},
		{"rightCommand", -1},

	//	{"pause", VK_PAUSE},
		{"pageUp", kVK_PageUp},
		{"pageDown", kVK_PageDown},
		{"home", kVK_Home},
		{"end", kVK_End},
		{"left", kVK_LeftArrow},
		{"right", kVK_RightArrow},
		{"up", kVK_UpArrow},
		{"down", kVK_DownArrow},
//!		{"insert", kVK_Help},
		{"delete", kVK_Delete},
		{"forwardDelete", kVK_ForwardDelete},

		{"a", kVK_ANSI_A},
		{"s", kVK_ANSI_S},
		{"d", kVK_ANSI_D},
		{"f", kVK_ANSI_F},
		{"h", kVK_ANSI_H},
		{"g", kVK_ANSI_G},
		{"z", kVK_ANSI_Z},
		{"x", kVK_ANSI_X},
		{"c", kVK_ANSI_C},
		{"v", kVK_ANSI_V},
		{"b", kVK_ANSI_B},
		{"q", kVK_ANSI_Q},
		{"w", kVK_ANSI_W},
		{"e", kVK_ANSI_E},
		{"r", kVK_ANSI_R},
		{"y", kVK_ANSI_Y},
		{"t", kVK_ANSI_T},
		{"1", kVK_ANSI_1},
		{"2", kVK_ANSI_2},
		{"3", kVK_ANSI_3},
		{"4", kVK_ANSI_4},
		{"6", kVK_ANSI_6},
		{"5", kVK_ANSI_5},
		{"9", kVK_ANSI_9},
		{"7", kVK_ANSI_7},
		{"8", kVK_ANSI_8},
		{"0", kVK_ANSI_0},
		{"o", kVK_ANSI_O},
		{"u", kVK_ANSI_U},
		{"i", kVK_ANSI_I},
		{"p", kVK_ANSI_P},
		{"l", kVK_ANSI_L},
		{"j", kVK_ANSI_J},
		{"k", kVK_ANSI_K},
		{"n", kVK_ANSI_N},
		{"m", kVK_ANSI_M},

		{"f1", kVK_F1},
		{"f2", kVK_F2},
		{"f3", kVK_F3},
		{"f4", kVK_F4},
		{"f5", kVK_F5},
		{"f6", kVK_F6},
		{"f7", kVK_F7},
		{"f8", kVK_F8},
		{"f9", kVK_F9},
		{"f10", kVK_F10},
		{"f11", kVK_F11},
		{"f12", kVK_F12},			

		{"add", kVK_ANSI_KeypadPlus},
		{"subtract", kVK_ANSI_KeypadMinus},
		{"multiply", kVK_ANSI_KeypadMultiply},
		{"divide", kVK_ANSI_KeypadDivide},
		{"decimal", kVK_ANSI_KeypadDecimal},
		//{"clear", kVK_ANSI_KeypadClear},
		{"enter", kVK_ANSI_KeypadEnter},
		//{"equals", kVK_ANSI_KeypadEquals},
		{"num0", kVK_ANSI_Keypad0},
		{"num1", kVK_ANSI_Keypad1},
		{"num2", kVK_ANSI_Keypad2},
		{"num3", kVK_ANSI_Keypad3},
		{"num4", kVK_ANSI_Keypad4},
		{"num5", kVK_ANSI_Keypad5},
		{"num6", kVK_ANSI_Keypad6},
		{"num7", kVK_ANSI_Keypad7},
		{"num8", kVK_ANSI_Keypad8},
		{"num9", kVK_ANSI_Keypad9},
	#else
		{"dummy", 0},
	#endif
	};

	// add attributes from static list
	foreach (const Attribute& attribute, attributeList)
	{
		attributes[attribute.name] = attribute.code;
	}

	// generate attributes
#if defined(_WIN32)

	// 0 - 9 and numpad 0 - 9
	for (char ch = '0'; ch <= '9'; ++ch)
	{
		attributes[arg("%0", ch)] = ch;
		attributes[arg("num%0", ch)] = VK_NUMPAD0 + ch - '0';
	}

	// A - Z
	for (char ch = 'a'; ch <= 'z'; ++ch)
	{
		attributes[arg("%0", ch)] = 'A' + ch - 'a';
	}

	// F1 - F12
	for (int i = 1; i <= 12; ++i)
	{
		attributes[arg("f%0", i)] = VK_F1 + i - 1;
	}
	
#elif defined(__APPLE__)


#else

#endif

	// write attributes and generate mapping from key code to attribute index
	int key2attributeIndex[256];
	fill(key2attributeIndex, -1);
	{
		std::ofstream w("keyAttributeInfos.inc.h");
		int attributeIndex = 0;
		foreach (AttributePair& attribute, attributes)
		{
			w << "{\"." << attribute.first << "\", InputDevice::T_BOOL, offsetof(KeyState, keys[" << attributeIndex << "])}," << std::endl;

			if (attribute.second >= 0 && attribute.second < boost::size(key2attributeIndex))
				key2attributeIndex[attribute.second] = attributeIndex;
			attribute.second = attributeIndex;
			++attributeIndex;
		}
		w.close();
	}
	{
		std::ofstream w("numKeyAttributes.inc.h");
		w << "enum" << std::endl;
		w << "{" << std::endl;
		w << "\tNUM_ATTRIBUTES = " << attributes.size() << "," << std::endl;
		
		w << "\tSHIFT_INDEX = " << attributes["shift"] << "," << std::endl;
		w << "\tLEFT_SHIFT_INDEX = " << attributes["leftShift"] << "," << std::endl;
		w << "\tRIGHT_SHIFT_INDEX = " << attributes["rightShift"] << "," << std::endl;
		w << "\tCONTROL_INDEX = " << attributes["control"] << "," << std::endl;
		w << "\tLEFT_CONTROL_INDEX = " << attributes["leftControl"] << "," << std::endl;
		w << "\tRIGHT_CONTROL_INDEX = " << attributes["rightControl"] << "," << std::endl;
		w << "\tALT_INDEX = " << attributes["alt"] << "," << std::endl;
		w << "\tLEFT_ALT_INDEX = " << attributes["leftAlt"] << "," << std::endl;
		w << "\tRIGHT_ALT_INDEX = " << attributes["rightAlt"] << "," << std::endl;
		w << "\tCOMMAND_INDEX = " << attributes["command"] << "," << std::endl;
		w << "\tLEFT_COMMAND_INDEX = " << attributes["leftCommand"] << "," << std::endl;		
		w << "\tRIGHT_COMMAND_INDEX = " << attributes["rightCommand"] << "," << std::endl;		
		w << "};" << std::endl ;
		
		w.close();
	}

	// write mapping from key code to attribute index
	{
		std::ofstream w("key2AttributeIndex.inc.h");
		int i = 0;
		foreach (int attributeIndex, key2attributeIndex)
		{
			w << attributeIndex;
			if (i % 16 == 15)
				w << "," << std::endl;
			else
				w << ", ";
			++i;
		}
		w.close();
	}

	return 0;
}
