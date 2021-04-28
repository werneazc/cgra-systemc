#ifndef UTILS_H_
#define UTILS_H_

#include <sstream>
#include <vector>
#include <cstring>


namespace cgra {
//Global cache for dynamicallly created port and signal names.
extern std::vector<char*> gsysc_renaming_strings;


/*!
 * \brief Create name string for GsysC hierarchy viewer
 *
 * \tparam T Datatype for first name part
 * \tparam K Datatype of last name part
 * \param[in] part1 First name part
 * \param[in] part2 Last name part
 *
 * \return
 * Concatenated string parts <part1><part2> as a C-string
 */
template<typename T, typename K = T>
char* create_name(T part1, K part2)
{
		std::stringstream t_ss{""};
		std::string t_str{""};

		t_ss << part1 << part2;
		t_str = t_ss.str();

		char* t_name = new char[t_str.length()];
		strncpy(t_name, t_str.c_str(),t_str.length());

		gsysc_renaming_strings.push_back(t_name);

		return gsysc_renaming_strings.back();
}

} // namespace CGRA

#endif // UTILS_H_
