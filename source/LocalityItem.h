/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef LOCALITY_ITEM_H_
#define LOCALITY_ITEM_H_
#include <Wt/WString>
#include <string>
#include <array>

using namespace std;

class LocalityItem;

class LocalityItem {
public:

    Wt::WString localName;
    Wt::WString localAbbrv;
    Wt::WString localId;

};

#endif // LOCALITY_ITEM_H_
