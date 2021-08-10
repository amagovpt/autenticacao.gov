/*-****************************************************************************

 * Copyright (C) 2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2012 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2021 Adriano Campos - <adrianoribeirocampos@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

/*
 * StringOps.cpp
 *
 *  Created on: Dec 26, 2011
 *      Author: ruim
 */

#include "StringOps.h"
#include <string>
#include <cstring>

namespace eIDMW {

void replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

bool endsWith(const std::string& fullString, const std::string& ending){
    if (fullString.length() < ending.length())
        return false;

    return fullString.compare(fullString.length() - ending.length(), ending.length(), ending) == 0;
}

/*
 * Get the exact widths of the glyphs necessary to represent
 * the text in 'winansi_encoded_string', for a chosen font-face and fontsize
 * Units: postscript points (pts)
 */
double getStringWidth(const char *winansi_encoded_string, double font_size, MyriadFontType font)
{
    double total_width = 0;
    unsigned int w = 0;
    unsigned char code = 0;

    //Lookup each winansi char width
    for (unsigned int i = 0; i!= strlen(winansi_encoded_string); i++)
    {
       code = (unsigned char)winansi_encoded_string[i];
       w = getWidth(code, font);
       total_width += 0.001 * font_size * w;
    }

    return total_width;
}

std::string getFittingSubString(const std::string &str, double font_size, MyriadFontType font, double space_available)
{
    int i = str.length() - 1;
    const double reserved = getStringWidth("(...)", font_size, font);
    while(i >= 0)
    {
        std::string tmp = str.substr(0, i);
        if (getStringWidth(tmp.c_str(), font_size, font) <= (space_available - reserved))
            return tmp;
        i--;

    }
    return "";
}

std::vector<std::string> wrapString(const std::string& content, double available_space, double font_size, MyriadFontType font,
                                    int available_lines, double first_line_offset) {

  std::vector<std::string> result;
  std::string current_line;
  std::string word;

  double space_width = getStringWidth(" ", font_size, font);
  double suspension_points_width = getStringWidth("(...)", font_size, font);

  double space_left = available_space - first_line_offset;
  double word_width = 0;
  double needed_width = 0;

  std::istringstream iss(content, std::istringstream::in);
  while (iss >> word)
  {
    word_width = getStringWidth(word.c_str(), font_size, font);
    needed_width = word_width + space_width;

    if (available_lines == 1) // last line
      needed_width += suspension_points_width;

    //No more space in current line
    if (needed_width > space_left)
    {
      available_lines--;

      // if it' i's the first word of current line, and it is too long
      if (current_line.empty())
        current_line.append(getFittingSubString(word, font_size, font, space_left));

      if (available_lines == 0)
      {
        //No more available lines so its an early exit...
        current_line.append("(...)");
        result.push_back(current_line);
        return result;
      }

      // New line
      result.push_back(current_line);
      current_line = word;

      // Reset space_left
      space_left = available_space - word_width;
    }
    else
    {
      if (current_line.size() > 1)
        current_line.append(" ");

      // add word to current line
      current_line.append(word);

      // re-calculate space_left in current line
      space_left -= (word_width + space_width);
    }
  }

  result.push_back(current_line);
  return result;
}

/*
 * Determines maximum font size and number of lines
 * that fit in the available space and height left
 *
 * Used in signature seals with SCAP strings
 * font size varies from 8 to 4.
 */
WrapParams calculateWrapParams(const std::string& text, const std::string& label, int rect_height,
                               double rect_width)
{
    unsigned int max_font_size = 8;
    unsigned int min_font_size = 4;

    unsigned int line_height;
    unsigned int font_size;
    unsigned int lines;
    double label_width;

    std::vector<std::string> wrapped;
    for (unsigned int i = max_font_size; i >= min_font_size; --i) {
        font_size = i;
        line_height = font_size + 1;
        // Remove half line for safety
        lines = ( rect_height - (line_height / 2) ) / line_height ;

        label_width = getStringWidth(label.c_str(), font_size, MYRIAD_REGULAR);
        wrapped = wrapString(text, rect_width, font_size, MYRIAD_BOLD, lines, rect_width - label_width);

        if (!endsWith(wrapped.back(), "(...)"))
            break; // at current font size, wrapped strings fit on available space
    }

    WrapParams p;
    p.font_size = font_size;
    p.available_lines = lines;
    p.first_line_offset = label_width;

    return p;
}

FontParams calculateFontParams(bool small_signature_format, const std::string& reason, const std::string& name,
        bool haveNic, bool haveDate, const std::string& location, const std::string& entities, 
        const std::string& attributes,
        int rect_width, int rect_height)
{
    unsigned int max_font_size = 8;
    unsigned int min_font_size = 2;

    unsigned int line_height;
    unsigned int font_size;

    std::vector<std::string> wrapped;

    printf("Calculate FontParams w=%d h=%d\n", rect_width, rect_height);

    for (unsigned int i = max_font_size; i >= min_font_size; --i) {
        int countReason = 0;
        int countEntities = 0;
        int countAttributes = 0;
        int countLocation = 0;
        int linesFit = 0;
        int linesCount = 0;
        font_size = i;
        line_height = font_size > 5 ? font_size + 1 : font_size;

        // The first line have 1 px top margin
        linesFit = ( rect_height - 1 ) / line_height ;

        if (!small_signature_format && reason != "")
        {
          std::vector<std::string> wrappedReason;
          wrappedReason = wrapString(reason, rect_width, font_size, MYRIAD_ITALIC, 2);
          countReason = wrappedReason.size();
          /*for (std::string s: wrappedReason){
            printf("**** wrappedReason **** %s\n", s.c_str());
          }*/
          linesCount +=  countReason;
        }

        std::vector<std::string> wrappedName;
        double first_line_offset = 6 * font_size;
        wrappedName = wrapString(name, rect_width, font_size, MYRIAD_BOLD, 5, first_line_offset);
        int countName = wrappedName.size();
        /*for (std::string s: wrappedName){
          printf("**** wrappedName **** %s\n", s.c_str());
        }*/

        linesCount += countName;
        linesCount += haveNic ? 1 : 0;
        linesCount += haveDate ? 1 : 0;

        if (!small_signature_format && location != "")
        {
          std::vector<std::string> wrappedLocation;
          wrappedLocation = wrapString(location, rect_width, font_size, MYRIAD_ITALIC, 2);
          countLocation = wrappedLocation.size();
          /*for (std::string s: wrappedLocation){
            printf("**** wrappedLocation **** %s\n", s.c_str());
          }*/
          linesCount +=  countLocation;
        }

        if (!small_signature_format && entities != "")
        {
          std::vector<std::string> wrappedEntities;
          wrappedEntities = wrapString(entities, rect_width, font_size, MYRIAD_ITALIC, 2);
          countEntities = wrappedEntities.size();
          /*for (std::string s: wrappedEntities){
            printf("**** wrappedEntities **** %s\n", s.c_str());
          }*/
          linesCount +=  countEntities;
        }

        if (!small_signature_format && attributes != "")
        {
          std::vector<std::string> wrappedAttributes;
          wrappedAttributes = wrapString(attributes, rect_width, font_size, MYRIAD_ITALIC, 10);
          countAttributes = wrappedAttributes.size();
          /*for (std::string s: wrappedAttributes){
            printf("**** wrappedAttributes **** %s\n", s.c_str());
          }*/
          linesCount +=  countAttributes;
        }

        if (linesFit >= linesCount){
            printf("break i = %d countReason=%d countName=%d countEntities=%d countAttributes=%d countLocation=%d\n", 
              i, countReason, countName, countEntities, countAttributes,countLocation );
            break;
        }
    }

    FontParams p;
    p.font_size = font_size;
    p.line_height = line_height;

    return p;
}

} /* namespace eIDMW */
