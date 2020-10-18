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
  if (reps.count("default")) {
    Rep rep = reps["default"];
    if (rep.type == USD_TOKEN && !rep.array && rep.inlined)
      setTokenValue(tokens[rep.payload]);
    else if (rep.type == USD_ASSET && !rep.array && rep.inlined)
      setAssetValue(tokens[rep.payload]);
    else if (rep.type == USD_FLOAT && !rep.array && rep.inlined)
      setValue((float)rep.payload);
  }
}

void USDAttribute::print(ostream &os, string indent) const {
  os << indent << "-" << typeName << " " << name;
  if (dataType != USD_INVALID && data.size() > 0) {
    if (dataType == USD_TOKEN)
      os << " = \"" << tokenValue() << "\"" << endl;
    else if (dataType == USD_ASSET)
      os << " = <" << assetValue() << ">" << endl;
    else if (dataType == USD_FLOAT)
      os << " = " << floatValue() << endl;
  }
  else {
    os << endl;
  }
  
  // Print Attribute Meta Data
//  for (auto itr = reps.begin(); itr != reps.end(); ++itr) {
//    os << indent << "  " << itr->first << ": " << itr->second.type;
//    if (itr->second.array)
//      os << " []";
//    if (itr->second.inlined)
//      os << " inlined: " << itr->second.payload;
//    os << endl;
//  }
}

void USDAttribute::setTokenValue(const string &token) {
  dataType = USD_TOKEN;
  data.resize(token.size()+1);
  memcpy(&data[0], &token[0], token.size());
  data[token.size()] = '\0';
}

string USDAttribute::tokenValue() const {
  return &data[0];
}

void USDAttribute::setAssetValue(const string &asset) {
  dataType = USD_ASSET;
  data.resize(asset.size()+1);
  memcpy(&data[0], &asset[0], asset.size());
  data[asset.size()] = '\0';
}

string USDAttribute::assetValue() const {
  return &data[0];
}

void USDAttribute::setValue(float value) {
  dataType = USD_FLOAT;
  data.resize(sizeof(float));
  memcpy(&data[0], &value, sizeof(float));
}

float USDAttribute::floatValue() const {
  return (float)*(&data[0]);
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
//  for (auto itr = reps.begin(); itr != reps.end(); ++itr)
//    os << indent << "   " << itr->first << ": " << itr->second.type << endl;
  
  // Print Attributes
  for (auto itr = attributes.begin(); itr != attributes.end(); ++itr)
    itr->second.print(os, indent + "  ");
  
  // Print Children
  for (auto itr = children.begin(); itr != children.end(); ++itr)
    itr->print(os, indent + "  ");
}
