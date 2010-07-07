/*
Neocortex - Memory-Prediction Framework
Copyright (C) 2007 Saulius J. Garalevicius
Modified by David Green (QT based GUI, event-model data sources, User interface enhancements)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SA_H
#define SA_H

// AccessKey is a class that provides the key upon which the array is sorted
class AccessKey
{
public:
   AccessKey(unsigned r, unsigned c)
   {
      //cPosition = new std::pair<unsigned, unsigned>;
      cPosition.first = r;
      cPosition.second = c;      //qint64 value = Q_UINT64_C(r);
   };

   ~AccessKey()
   {
      //delete cPosition;
   }

   void SetAccessKey(unsigned r, unsigned c)
   {
      cPosition.first = r;
      cPosition.second = c;
   };

   //unsigned GetCurrentRow( )
   //{
   //   unsigned rTemp = cPosition.first;
   //   unsigned cTemp = cPosition.second;
   //   return rTemp;
   //};

   // AccessKey::< is the sort algorithm
   bool operator<(const AccessKey rhs) const
   {
      if ( this->cPosition.first == rhs.cPosition.first )
      {
         // same row - so compare column number
         if ( this->cPosition.second < rhs.cPosition.second )
            return true;
         else
            return false;
      }
      else
      {
         // different rows - so compare row number
         if ( this->cPosition.first < rhs.cPosition.first )
            return true;
         else
         {
            return false;
         }
      }
   };

//private:
   std::pair<unsigned, unsigned>  cPosition;
};

#endif