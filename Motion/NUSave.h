/*! @file NUSave.h
    @brief Declaration of a ball blocking class
 
    @class NUSave
    @brief A module to block the ball using the legs (suitable for both goal keeper and field player)
 
    @author Jason Kulk
 
  Copyright (c) 2010 Jason Kulk
 
    This file is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This file is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with NUbot.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef NUSAVE_H
#define NUSAVE_H

class NUSensorsData;
class NUActionatorsData;
class SaveJob;
class NUWalk;

class NUSave
{
public:
    NUSave(NUWalk* walk);
    ~NUSave();
    
    void process(NUSensorsData* data, NUActionatorsData* actions);
private:
public:
private:
    NUWalk* m_walk;
};

#endif

