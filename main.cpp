/////////////////////
// Matthew Barham
// 2022-03-19
///////////////////////////////////////////////////////////////////
// Long-Distance Call Cost Calculator
//
// Calls made between 8am and 6pm on Mon-Fri are rated at $0.40/min
// Calls started before 8am or after 6pm Mon-Fri are $0.25/min
// Calls on Sat and Sun are $0.15/min
//
// Handles a lot of edge cases, but some functions are not
// very sophisticated. Letters instead of numbers can cause
// the program to wait for valid numerical input at times.
///////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include <iomanip>
#include <limits>

enum class weekday : int
{
	None = 0,
	Monday,
	Tuesday,
	Wednesday,
	Thursday,
	Friday,
	Saturday,
	Sunday
};

/**
 * Data class used to store all user input
 */
class long_distance_call_data
{
public:
	long_distance_call_data() = default;

	const char* get_text_for_enum(const int enum_val) const
	{
		return weekday_strings_[enum_val];
	}

	// Setters
	void set_weekday(const weekday in_weekday) { weekday_ = in_weekday; }
	void set_hour(const int in_hour) { hour_ = in_hour; }
	void set_minute(const int in_minute) { minutes_ = in_minute; }
	void set_length_of_call(const int in_length) { length_of_call_ = in_length; }

	// Getters
	weekday get_weekday() const { return weekday_; }
	int get_hour() const { return hour_; }
	int get_minutes() const { return minutes_; }
	int get_length_of_call() const { return length_of_call_; }

	// Outputs time in a 24:00 format
	std::string get_time_formatted() const
	{
		// Handle double zero case
		if (minutes_ == 0)
		{
			return std::to_string(hour_) + ':' + std::to_string(minutes_) + std::to_string(minutes_);
		}
		return std::to_string(hour_) + ':' + std::to_string(minutes_);
	}

private:
	weekday weekday_{weekday::Monday};
	int hour_{24};
	int minutes_{00};
	int length_of_call_{};

	// Used to output enum values as strings
	const char* weekday_strings_[8]
	{
		"None",
		"Monday",
		"Tuesday",
		"Wednesday",
		"Thursday",
		"Friday",
		"Saturday",
		"Sunday"
	};
};

/**
 * Takes a two-letter input for the day of the week.
 * If invalid letters are entered it will continue to loop.
 * Expects at least two letters before it checks input.
 */
void get_day_of_week(long_distance_call_data& in_data)
{

	bool valid_weekday{false};
	while (!valid_weekday)
	{
		std::cout << "\nPlease enter the day of the week you made your call (e.g. Mo, Tu, etc.): ";

		char first_letter;
		char second_letter;
		std::cin >> first_letter >> second_letter;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		if (!std::isalpha(first_letter) || !std::isalpha(second_letter)) continue;

		switch (std::tolower(first_letter))
		{
		case 'm':
			in_data.set_weekday(weekday::Monday);
			valid_weekday = true;
			break;
		case 't':
			if (std::tolower(second_letter) == 'u')
			{
				in_data.set_weekday(weekday::Tuesday);
				valid_weekday = true;
				break;
			}
			if (std::tolower(second_letter) == 'h')
			{
				in_data.set_weekday(weekday::Thursday);
				valid_weekday = true;
				break;
			}
			break;
		case 'w':
			in_data.set_weekday(weekday::Wednesday);
			valid_weekday = true;
			break;
		case 'f':
			in_data.set_weekday(weekday::Friday);
			valid_weekday = true;
			break;
		case 's':
			if (std::tolower(second_letter) == 'a')
			{
				in_data.set_weekday(weekday::Saturday);
				valid_weekday = true;
				break;
			}
			if (std::tolower(second_letter) == 'u')
			{
				in_data.set_weekday(weekday::Sunday);
				valid_weekday = true;
				break;
			}
			break;
		default:
			std::cout << "You entered an invalid weekday.\n";
		}
	}
}

/**
 * Asks the user for a time in international format.
 * Checks for invalid hours and minutes outside of expected.
 * Cannot handle character inputs elegantly. Just waits for numbers.
 */
void get_start_time(long_distance_call_data& in_data)
{
	bool valid_time{ false };
	while (!valid_time)
	{
		std::cout << "Please enter the time you began your call (e.g. 16:32): ";

		int hour;
		int minutes;
		char c;
		std::cin >> hour >> c >> minutes;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		if (hour < 0 || hour > 24)
		{
			std::cout << "You entered an invalid hour." << std::endl;
			continue;
		}
		if (minutes < 0 || minutes > 59 || (minutes > 0 && hour == 24))
		{
			std::cout << "You entered an invalid minutes." << std::endl;
			continue;
		}
		in_data.set_hour(hour);
		in_data.set_minute(minutes);
		valid_time = true;
	}
}

/**
 * Asks the user for a total call length in minutes.
 * Checks for non-digit input.
 * Converts string input to integer and stores it in provided reference argument.
 */
void get_length_of_call(long_distance_call_data& in_data)
{
	bool is_valid{ false };
	while (!is_valid)
	{
		char length_of_call[256];
		std::cout << "Please enter the length of your call in minutes: ";

		std::cin >> length_of_call;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		if (!std::isdigit(length_of_call[0]))
		{
			std::cout << "You did not enter a valid number.\n";
			continue;
		}

		in_data.set_length_of_call(std::stoi(length_of_call));

		is_valid = true;
	}
}

/**
 * Calculates cost of call based on length of call, start time, and day of week.
 * MON - FRI, before 8:00 or after 18:00 : $0.25/min
 * MON - FRI, between 8:00 and 18:00 : $0.40/min
 * SAT / SUN, $0.15/min
 */
float calculate_cost_of_call(const long_distance_call_data& in_data)
{
	float total_cost{};
	constexpr float weekend_rate{ 0.15f };
	constexpr float weekday_edge_rate{ 0.25f };
	constexpr float weekday_normal_rate{ 0.4f };

	if (in_data.get_weekday() == weekday::Saturday ||
		in_data.get_weekday() == weekday::Sunday)
	{
		total_cost = static_cast<float>(in_data.get_length_of_call()) * weekend_rate;
		return total_cost;
	}

	if (in_data.get_hour() < 8 || in_data.get_hour() > 18)
	{
		total_cost = static_cast<float>(in_data.get_length_of_call()) * weekday_edge_rate;
		return total_cost;
	}

	total_cost = static_cast<float>(in_data.get_length_of_call()) * weekday_normal_rate;
	return total_cost;
}

int main()
{
	bool quit{ false };
	while (!quit)
	{
		long_distance_call_data input;
		std::cout << "Hello. Let's calculate the cost of your long-distance call.\n";

		// Get the day of the week from the user
		get_day_of_week(input);

		std::cout << "You entered: " << input.get_text_for_enum(static_cast<int>(input.get_weekday()));
		std::cout << std::endl;
		std::cout << std::endl;

		// Get the starting time of the call from the user
		get_start_time(input);

		std::cout << "You entered the time: " << input.get_time_formatted();
		std::cout << std::endl;
		std::cout << std::endl;

		// Get length of call in minutes from user
		get_length_of_call(input);

		std::cout << "You entered: " << input.get_length_of_call() << " minutes.";
		std::cout << std::endl;
		std::cout << std::endl;

		// Set output format and calculate total cost
		std::cout << std::setprecision(2);
		std::cout << std::fixed;
		std::cout << "The cost of your call is $" << calculate_cost_of_call(input) << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;

		// Handle loop
		char quit_input{};
		std::cout << "Would you like to calculate another call? (y or n): ";
		std::cin >> quit_input;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		switch (std::tolower(quit_input))
		{
		case 'n':
			quit = true;
			break;
		case 'y':
			break;
		default: 
			std::cout << "I'll take that as a yes\n\n";
		}
	}
	
	std::cout << std::endl;
	return 0;
}
