/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef GPSPATH_ITEM_H
#define GPSPATH_ITEM_H

#include <string>
#include <array>

using namespace std;

class GPSPathItem;

class GPSPathItem {
public:

    double lat_;
    double lng_;
    Wt::WString time_;

    Wt::WString provider;

};

#endif // GPSPATH_ITEM_H
