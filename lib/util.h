#ifndef UTIL_H
#define UTIL_H

#include <string>

/**
 * @brief Remove all white space after the content.
 *
 * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
 */
void removeTrailingWhitespace(std::string &input);

/**
 * @brief Remove all white space before the content.
 *
 * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
 */
void removeLeadingWhitespace(std::string &input);

/**
 * @brief Remove all white space before and after the content.
 *
 * This uses the removeTrailingWhitespace and the removeLeadingWhitespace
 * methods together on the same string.
 *
 * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
 */
void removeWhitespace(std::string &input);

#endif
