//
//  USDTypes.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/18/20.
//  Copyright © 2020 Robert Crosby. All rights reserved.
//

#include "USDTypes.h"


using namespace fx;
using namespace std;


void USDAttribute::setToPath(Path &path, StringVector &tokens) {
  name = path.token;
  reps = path.spec.reps;
  if (reps.count("typeName") > 0 && reps["typeName"].type == USD_TOKEN)
    typeName = tokens[reps["typeName"].payload];
}

void USDAttribute::print(ostream &os, string indent) const {
  os << indent << "-" << typeName << " " << name << endl;
  
  // Print Attribute Meta Data
  for (auto itr = reps.begin(); itr != reps.end(); ++itr)
    os << indent << "  " << itr->first << ": " << itr->second.type << endl;
}


void USDItem::setToPath(Path &path, StringVector &tokens) {
  name = path.token;
  reps = path.spec.reps;
  if (reps.count("typeName") > 0 && reps["typeName"].type == USD_TOKEN)
    typeName = tokens[reps["typeName"].payload];
}

void USDItem::print(ostream &os, string indent) const {
  os << indent << "+" << typeName << " " << name << endl;
  
  // Print Meta Data
  for (auto itr = reps.begin(); itr != reps.end(); ++itr)
    os << indent << "   " << itr->first << ": " << itr->second.type << endl;
  
  // Print Attributes
  for (auto itr = attributes.begin(); itr != attributes.end(); ++itr)
    itr->second.print(os, indent + "  ");
  
  // Print Children
  for (auto itr = children.begin(); itr != children.end(); ++itr)
    itr->print(os, indent + "  ");
}
