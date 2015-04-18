#ifndef LOGGER_HPP
#define LOGGER_HPP

/* DEBUG LOG */

void log(const std::string& s) {
	std::cout << s << std::endl;
}

template <typename T>
void log(const std::string& s, T v) {
	std::cout << s << ": " << v << std::endl;
}

template <typename T1, typename T2>
void log(const std::string& s1, T1 v1, const std::string& s2, T2 v2) {
	std::cout << s1 << ": " << v1 << ", " << s2 << ": " << v2 << std::endl;
}


/* ERROR LOG */

void logErr(const std::string& s) {
	std::cerr << s << std::endl;
}

template <typename T>
void logErr(const std::string& s, T v) {
	std::cerr << s << ": " << v << std::endl;
}

template <typename T1, typename T2>
void logErr(const std::string& s1, T1 v1, const std::string& s2, T2 v2) {
	std::cerr << s1 << ": " << v1 << ", " << s2 << ": " << v2 << std::endl;
}


#endif // LOGGER_HPP
