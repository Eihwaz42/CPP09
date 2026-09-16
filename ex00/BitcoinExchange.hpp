#ifndef BITCOINEXCHANGE_HPP
# define BITCOINEXCHANGE_HPP

# include <iostream>
# include <fstream>
# include <sstream>
# include <string>
# include <map>
# include <cctype>
# include <stdexcept>

class BitcoinExchange
{
	private:
		std::map<std::string, double>	_rates;

		std::string	trim(const std::string& str) const;
		double		getRate(const std::string& date) const;
		bool		isValidDate(const std::string& date) const;
		bool		parseValue(const std::string& valueStr, double& value) const;
		void		processLine(const std::string& line);

	public:
		BitcoinExchange();
		BitcoinExchange(const BitcoinExchange& other);
		BitcoinExchange& operator=(const BitcoinExchange& other);
		~BitcoinExchange();

		void	loadDatabase(const std::string& filename);
		void	processFile(const std::string& filename);
};

#endif