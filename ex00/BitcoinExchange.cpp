#include "BitcoinExchange.hpp"

BitcoinExchange::BitcoinExchange()
{
}

BitcoinExchange::BitcoinExchange(const BitcoinExchange& other)
{
	*this = other;
}

BitcoinExchange& BitcoinExchange::operator=(const BitcoinExchange& other)
{
	if (this != &other)
		_rates = other._rates;
	return (*this);
}

BitcoinExchange::~BitcoinExchange()
{
}

void BitcoinExchange::loadDatabase(const std::string& filename)
{
    // input file stream
	std::ifstream file;

    file.open(filename.c_str());

	if (!file.is_open())
		throw std::runtime_error("Error: could not open database.");

	std::string line;

    // skip 1st line (header)
	std::getline(file, line);

	while (std::getline(file, line))
	{
		std::stringstream ss(line);
		std::string date;
		std::string rateStr;

        // Extracts the date and checks if the read was successful
		if (!std::getline(ss, date, ','))
			continue;
		if (!std::getline(ss, rateStr))
			continue;

		std::stringstream rateStream(rateStr);
		double rate;

        // Converts the exchange rate to a double and checks if the conversion succeeded
		if (!(rateStream >> rate))
			continue;

		_rates[date] = rate;
	}

	file.close();
}

std::string BitcoinExchange::trim(const std::string& str) const
{
    // 1st character that is not a space or tab
	size_t start = str.find_first_not_of(" \t");
	size_t end = str.find_last_not_of(" \t");

	if (start == std::string::npos) // if the string is empty or contains only whitespace
		return ("");

	return (str.substr(start, end - start + 1)); // including the last character
}

bool BitcoinExchange::isValidDate(const std::string& date) const
{
	if (date.length() != 10)
		return (false);
	if (date[4] != '-' || date[7] != '-')
		return (false);

	for (size_t i = 0; i < date.length(); i++)
	{
		if (i == 4 || i == 7)
			continue;
		if (!std::isdigit(static_cast<unsigned char>(date[i])))
	        return (false);
	}

	int year;
	int month;
	int day;

	std::stringstream(date.substr(0, 4)) >> year;
	std::stringstream(date.substr(5, 2)) >> month;
	std::stringstream(date.substr(8, 2)) >> day;

	if (month < 1 || month > 12)
		return (false);

	int daysInMonth[12] = {
		31, 28, 31, 30, 31, 30,
		31, 31, 30, 31, 30, 31
	};

	bool leap = (year % 4 == 0 && year % 100 != 0)
		|| (year % 400 == 0);

	if (leap)
		daysInMonth[1] = 29;

	if (day < 1 || day > daysInMonth[month - 1]) // month - 1 because the array is 0-indexed
		return (false);

	return (true);
}

bool BitcoinExchange::parseValue(const std::string& valueStr, double& value) const
{
	std::stringstream ss(valueStr);

	if (!(ss >> value))
		return (false);

    // Check for any extra characters after the number
	char extra;
	if (ss >> extra)
		return (false);

	return (true);
}

double BitcoinExchange::getRate(const std::string& date) const
{
	std::map<std::string, double>::const_iterator it;

    // Finds the first date greater than or equal to the requested date
	it = _rates.lower_bound(date);

    // exact date found
	if (it != _rates.end() && it->first == date)
		return (it->second);
    // no earlier date found
	if (it == _rates.begin())
		throw std::runtime_error(
			"Error: no exchange rate available for this date.");

	--it; // exact date not found, use the closest earlier date
	return (it->second);
}

void BitcoinExchange::processLine(const std::string& line)
{
	std::stringstream	ss(line);
	std::string			date;
	std::string			valueStr;

    // Extracts the date and value separated by '|', and checks that both are present
	if (!std::getline(ss, date, '|') || !std::getline(ss, valueStr))
	{
		std::cerr << "Error: bad input => " << line << std::endl;
		return;
	}

	date = trim(date);
	valueStr = trim(valueStr);
    if (!isValidDate(date))
    {
	    std::cerr << "Error: bad input => " << line << std::endl;
	    return;
    } 
    
    double value;

    if (!parseValue(valueStr, value))
    {
	    std::cerr << "Error: bad input => " << line << std::endl;
	    return;
    }

    if (value < 0)
    {
        std::cerr << "Error: not a positive number." << std::endl;
        return;
    }

    if (value > 1000)
    {
        std::cerr << "Error: too large a number." << std::endl;
        return;
    }

    double rate;

    try
    {
        rate = getRate(date);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return;
    }

    std::cout << date << " => " << value
        << " = " << value * rate << std::endl;
}

void BitcoinExchange::processFile(const std::string& filename)
{
	std::ifstream file(filename.c_str());

	if (!file.is_open())
		throw std::runtime_error("Error: could not open file.");

	std::string line;

	std::getline(file, line);

	while (std::getline(file, line))
		processLine(line);

	file.close();
}
