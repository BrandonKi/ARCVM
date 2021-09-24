/**
 * @file cprint.h
 * @author Brandon Kirincich
 * @brief single header library for printing text to the 
 * console with the specified format (color, underline, bold, etc.)
 * 
 * NOT ALL FEATURES ARE SUPPORTED BY ALL TERMINALS
 * 
 * @example 
 * 
 * // check the value of cprint_err to see if the current terminal/console supports virtual terminal processing
 * // This is only needed on Windows, but should be checked just in case
 * 
 * println("test", UNDERLINE, BOLD, RED);
 * 
 * println(fmt("I'm blue", BLUE) + " " + fmt("I'm red and bold", RED, BOLD));
 * 
 * println("r,g,b values are also supported! They can be bold, underlined, etc.", rgb(255, 100, 0), BOLD);
 * 
 * println("to set background color pass a color to the bg() function.", bg(RED));
 * 
 * print("rgb can also be passed to bg", bg(rgb(255, 100, 0)));
 * 
 * print("this also work", bg(255, 100, 0));
 * 
 * @version 0.1
 * @date 2020-10-16
 * 
 * MIT License
 *
 * @copyright (c) 2020 Brandon Kirincich
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 */

#ifndef cprint_HEADER_H
#define cprint_HEADER_H
#include <string>
#include <iostream>
#include <cstdint>
#include <type_traits>

#ifdef _WIN32
    #include <windows.h>
    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
        #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif
#endif


namespace cprint {

    static const std::string _cprint_preamble_("\033[");

    // Some macros share values with others
    // this is to give them more common names for ex. "gray/grey" instead of "bright black"

    #ifdef CPRINT_PREFIX
    constexpr auto CP_CLEAR = "\033[0m";
    constexpr auto CP_BOLD = "1;";
    constexpr auto CP_FAINT = "2;";
    constexpr auto CP_ITALICIZE = "3;";
    constexpr auto CP_UNDERLINE = "4;";
    constexpr auto CP_SLOW_BLINK = "5;";
    constexpr auto CP_FAST_BLINK = "6;";
    constexpr auto CP_REVERSE = "7;";
    constexpr auto CP_CONCEAL = "8;";
    constexpr auto CP_STRIKETHROUGH = "9;";
    constexpr auto CP_DEFAULT_FONT = "10;";
    constexpr auto CP_ALT_FONT_1 = "11;";
    constexpr auto CP_ALT_FONT_2 = "12;";
    constexpr auto CP_ALT_FONT_3 = "13;";
    constexpr auto CP_ALT_FONT_4 = "14;";
    constexpr auto CP_ALT_FONT_5 = "15;";
    constexpr auto CP_ALT_FONT_6 = "16;";
    constexpr auto CP_ALT_FONT_7 = "17;";
    constexpr auto CP_ALT_FONT_8 = "18;";
    constexpr auto CP_ALT_FONT_9 = "19;";
    constexpr auto CP_FRAKTUR = "20;";

    constexpr auto CP_BLACK = "30;";
    constexpr auto CP_RED = "31;";
    constexpr auto CP_GREEN = "32;";
    constexpr auto CP_YELLOW = "33;";
    constexpr auto CP_BLUE = "34;";
    constexpr auto CP_PINK = "35;";
    constexpr auto CP_MAGENTA = "35;";
    constexpr auto CP_CYAN = "36;";
    constexpr auto CP_WHITE = "37";

    constexpr auto CP_FRAME = "51;";
    constexpr auto CP_ENCIRCLE = "52;";
    constexpr auto CP_OVERLINE = "53;";

    constexpr auto CP_BRIGHT_BLACK = "90;";
    constexpr auto CP_GRAY = "90;";
    constexpr auto CP_GREY = "90;";
    constexpr auto CP_BRIGHT_RED = "91;";
    constexpr auto CP_BRIGHT_GREEN = "92;";
    constexpr auto CP_BRIGHT_YELLOW = "93;";
    constexpr auto CP_BRIGHT_BLUE = "94;";
    constexpr auto CP_BRIGHT_PINK = "95;";
    constexpr auto CP_BRIGHT_MAGENTA = "95;";
    constexpr auto CP_BRIGHT_CYAN = "96;";
    constexpr auto CP_BRIGHT_WHITE = "97;";
    #else
    constexpr auto CLEAR = "\033[0m";
    constexpr auto BOLD = "1;";
    constexpr auto FAINT = "2;";
    constexpr auto ITALICIZE = "3;";
    constexpr auto UNDERLINE = "4;";
    constexpr auto SLOW_BLINK = "5;";
    constexpr auto FAST_BLINK = "6;";
    constexpr auto REVERSE = "7;";
    constexpr auto CONCEAL = "8;";
    constexpr auto STRIKETHROUGH = "9;";
    constexpr auto DEFAULT_FONT = "10;";
    constexpr auto ALT_FONT_1 = "11;";
    constexpr auto ALT_FONT_2 = "12;";
    constexpr auto ALT_FONT_3 = "13;";
    constexpr auto ALT_FONT_4 = "14;";
    constexpr auto ALT_FONT_5 = "15;";
    constexpr auto ALT_FONT_6 = "16;";
    constexpr auto ALT_FONT_7 = "17;";
    constexpr auto ALT_FONT_8 = "18;";
    constexpr auto ALT_FONT_9 = "19;";
    constexpr auto FRAKTUR = "20;";

    constexpr auto BLACK = "30;";
    constexpr auto RED = "31;";
    constexpr auto GREEN = "32;";
    constexpr auto YELLOW = "33;";
    constexpr auto BLUE = "34;";
    constexpr auto PINK = "35;";
    constexpr auto MAGENTA = "35;";
    constexpr auto CYAN = "36;";
    constexpr auto WHITE = "37";

    constexpr auto FRAME = "51;";
    constexpr auto ENCIRCLE = "52;";
    constexpr auto OVERLINE = "53;";

    constexpr auto BRIGHT_BLACK = "90;";
    constexpr auto GRAY = "90;";
    constexpr auto GREY = "90;";
    constexpr auto BRIGHT_RED = "91;";
    constexpr auto BRIGHT_GREEN = "92;";
    constexpr auto BRIGHT_YELLOW = "93;";
    constexpr auto BRIGHT_BLUE = "94;";
    constexpr auto BRIGHT_PINK = "95;";
    constexpr auto BRIGHT_MAGENTA = "95;";
    constexpr auto BRIGHT_CYAN = "96;";
    constexpr auto BRIGHT_WHITE = "97;";
    #endif

    /**
     * @brief Initializes cprint. This is mostly for windows platforms but should be called anyway.
     *
     * @return a bool representing whether or not cprint could be initialized
     */
    inline bool init_cprint(){
        #ifdef _WIN32
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            if (hOut == INVALID_HANDLE_VALUE)
                return false;
            DWORD dwMode = 0;
            if (!GetConsoleMode(hOut, &dwMode))
                return false;
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            if (!SetConsoleMode(hOut, dwMode))
                return false;
            return true;
        #else
            return true;
        #endif
    }

    /**
     * @brief return a string representing rgb value
     *
     * @param r value for red (0 - 255)
     * @param g value for green (0 - 255)
     * @param b value for blue (0 - 255)
     * @return formatted string representing rgb value
     */
    inline std::string rgb(const uint8_t r, const uint8_t g, const uint8_t b){
        return "38;2;" + std::to_string(+r) + ';' + std::to_string(+g) + ';' + std::to_string(+b) + ';';
    }

    /**
     * @brief return string representing a color to set background to
     *
     * @param color color to set background to can
     * be a predefined color macro or rgb() string
     * @return formatted string representing background color
     */
    inline std::string bg(const std::string& color){
        if(color.length() > 3)
            return std::string("48;2;" + color.substr(5));   // if color is in rgb() format
        return std::to_string(std::stoi(color) + 10) + ';';
    }

    /**
     * @brief return string representing a color to set background to from r, g, b values
     *
     * @param r value for red (0 - 255)
     * @param g value for green (0 - 255)
     * @param b value for blue (0 - 255)
     * @return formatted string representing background color
     */
    inline std::string bg(uint8_t r, uint8_t g, uint8_t b){
        return std::string("48;2;" + std::to_string(+r) + ';' + std::to_string(+g) + ';' + std::to_string(+b) + ';');
    }

    /**
     * @brief base case function for unpacking var args
     *
     * @return an empty string
     */
    inline std::string fstring(){
        return std::string("");
    }

    /**
     * @brief Helper function for unpacking var args.
     * This function takes all var args which are assumed to be valid ansii args
     * and combines them into a single string .
     *
     * @param var1 first arg in var args
     * @param var2 var args
     * @return all args combined into a single string
     */
    template <typename T, typename... Types>
    inline std::string fstring(T var1, Types... var2){
        return std::move(std::string(var1) + fstring(var2...));
    }

    /**
     * @brief function called to correctly format a string with all args given
     * for ex. fmt("test", UNDERLINE, RED, ...)
     *
     * @param str base string to add ansi escape args onto
     * @param modifiers any amount of ansi escape args
     * @return base string with ansi escape args added onto it
     */
    template <typename T, typename... Types>
    inline std::string fmt(T str, Types... modifiers){
        std::string&& color = fstring(modifiers...);
        if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, char>)
            return _cprint_preamble_ + color.substr(0,color.length()-1) + 'm' + str + CLEAR;
        else if constexpr(std::is_const_v<std::remove_pointer_t<T>>)
            return _cprint_preamble_ + color.substr(0,color.length()-1) + 'm' + std::string(str) + CLEAR;
        else
            return _cprint_preamble_ + color.substr(0,color.length()-1) + 'm' + std::to_string(str) + CLEAR;
    }


    /**
     * @brief print string to the console
     *
     * @param str string to print
     */
    inline void print(const std::string& str){
        std::cout << str;
    }

    /**
     * @overload
     *
     * @param str string to print
     */
    inline void print(const std::string&& str = ""){
        std::cout << str;
    }

    /**
     * @brief print string to the console with specified format as a string
     *
     * @param str string to print
     * @param fmt format to use
     */
    inline void print(const std::string& str, const std::string& fmt){
        if(fmt.empty())
            std::cout << str;
        else {
            std::cout << _cprint_preamble_ << fmt.substr(0, fmt.length()-1) << 'm' << str << CLEAR;
        }
    }

    /**
     * @overload
     *
     * @param str string to print
     * @param fmt format to use
     */
    inline void print(const std::string& str, const std::string&& fmt){
        if(fmt.empty())
            std::cout << str;
        else {
            std::cout << _cprint_preamble_ << fmt.substr(0, fmt.length()-1) << 'm' << str << CLEAR;
        }
    }

    /**
     * @overload
     *
     * @param str string to print
     * @param fmt format to use
     */
    inline void print(const std::string&& str, const std::string& fmt){
        if(fmt.empty())
            std::cout << str;
        else {
            std::cout << _cprint_preamble_ << fmt.substr(0, fmt.length()-1) << 'm' << str << CLEAR;
        }
    }

    /**
     * @overload
     *
     * @param str string to print
     * @param fmt format to use
     */
    inline void print(const std::string&& str, const std::string&& fmt){
        if(fmt.empty())
            std::cout << str;
        else {
            std::cout << _cprint_preamble_ << fmt.substr(0, fmt.length()-1) << 'm' << str << CLEAR;
        }
    }

    /**
     * @brief wrapper of print() that unpacks var args and formats them automatically
     *
     * @param str base string
     * @param modifiers list of ansi escape args
     */
    template <typename T, typename... Types>
    void print(T str, Types... modifiers){
        if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, char>)
            print(std::string("") + str, fstring(modifiers...));
        else if constexpr(std::is_const_v<std::remove_pointer_t<T>>)
            print(std::string(str), fstring(modifiers...));
        else
            print(std::to_string(str), fstring(modifiers...));
    }

    /**
     * @brief print a string to the console with a newline at the end
     *
     * @param str string to print
     */
    inline void println(const std::string& str = ""){
        std::cout << str << '\n';
    }

    /**
     * @overload
     *
     * @param str string to print
     */
    inline void println(const std::string&& str){
        std::cout << str << '\n';
    }

    /**
     * @brief print a string with the specified format
     * to the console with a newline at the end
     *
     * @param str string to print
     * @param fmt format string
     */
    inline void println(const std::string& str, const std::string& fmt){
        if(fmt.empty())
            std::cout << str;
        else {
            std::cout << _cprint_preamble_ << fmt.substr(0, fmt.length()-1) << 'm' << str << CLEAR;
        }
    }

    /**
     * @overload
     *
     *
     * @param str string to print
     * @param fmt format to use
     */
    inline void println(const std::string& str, const std::string&& fmt){
        if(fmt.empty())
            std::cout << str;
        else {
            std::cout << _cprint_preamble_ << fmt.substr(0, fmt.length()-1) << 'm' << str << CLEAR;
        }
    }

    /**
     * @overload
     *
     * @param str string to print
     * @param fmt format to use
     */
    inline void println(const std::string&& str, const std::string& fmt){
        if(fmt.empty())
            std::cout << str << '\n';
        else {
            std::cout << _cprint_preamble_ << fmt.substr(0, fmt.length()-1) << 'm' << str << CLEAR << '\n';
        }
    }

    /**
     * @overload
     *
     * @param str string to print
     * @param fmt format to use
     */
    inline void println(const std::string&& str, const std::string&& fmt){
        if(fmt.empty())
            std::cout << str << '\n';
        else {
            std::cout << _cprint_preamble_ << fmt.substr(0, fmt.length()-1) << 'm' << str << CLEAR << '\n';
        }
    }

    /**
     * @brief wrapper of println() that unpacks var args and formats them automatically
     *
     * @param str base string
     * @param modifiers list of ansi escape args
     */
    template <typename T, typename... Types>
    void println(T str, Types... modifiers){
        if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, char>)
            println(std::string("") + str, fstring(modifiers...));
        else if constexpr(std::is_const_v<std::remove_pointer_t<T>>)
            println(std::string(str), fstring(modifiers...));
        else
            println(std::to_string(str), fstring(modifiers...));
    }
}

inline auto cprint_err = cprint::init_cprint();

#endif