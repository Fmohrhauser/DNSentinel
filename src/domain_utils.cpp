#include "domain_utils.h"
#include <Arduino.h>

String normalizeDomain(String line)
{
  line.trim();

  if(line.length() ==0)
    return "";

  //skip comments
  if(line.startsWith("#"))
    return "";

  if(line.startsWith("!"))
    return "";

  //Remove AdBlock syntax
  if(line.startsWith("||"))
  {
    line.remove(0,2);
  }

  if(line.endsWith("^"))
  {
    line.remove(line.length() - 1);
  }

  //Remove hosts file IPs
  if(line.startsWith("0.0.0.0 "))
  {
    line.remove(0,8);
  }

  if(line.startsWith("127.0.0.1 "))
  {
    line.remove(0,10);
  }

  line.trim();
  line.toLowerCase();

  if(line == "localhost")
    return "";

  for(char c : line)
  {
    if(
      !(isalnum(c) ||
      c == '.' ||
      c == '-')
    )
    {
      return "";
    }
  }

  return line;
}

bool validDomain(const String &domain)
{
  if(domain.length() == 0)
    return false;

  if(domain.length() > 253)
    return false;

  if(domain.endsWith("."))
    return false;

  int labelStart = 0;

  for(int i = 0; i<= domain.length(); i++)
  {
    if(i == domain.length() || domain[i] == '.')
    {
      int labelLength = i - labelStart;

      if(labelLength <= 0 || labelLength > 63)
        return false;

      if(domain[labelStart] == '-' ||
        domain[i-1] == '-')
        {
          return false;
        }

        for(int j = labelStart; j < i; j++)
        {
          char c = domain[j];

          bool validChar =
            (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            c == '-';

          if(!validChar)
            return false;
        }

        labelStart = i + 1;
    }
  }

  return true;
}