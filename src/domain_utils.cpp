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