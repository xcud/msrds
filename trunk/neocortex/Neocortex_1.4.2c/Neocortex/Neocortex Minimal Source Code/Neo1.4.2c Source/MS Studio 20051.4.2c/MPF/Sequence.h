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

#ifndef SequenceH
#define SequenceH

class Sequence{
  private:
    unsigned Length;         //current number of patterns stored

    void CreateData();
    void DeleteData();
  protected:
    unsigned SequenceLength; //number of steps that form a sequence (time)
    unsigned InputCount;     //number of unsigned inputs (space)
    unsigned **Data; //dimensions SequenceLength x InputCount    
  public:
    Sequence(unsigned sl, unsigned ic);
    virtual ~Sequence();
    virtual void Init(unsigned sl, unsigned ic);
    void AddPattern(unsigned *pattern);
    void Clear();
    unsigned GetLength(){return Length;}
    bool Complete(){return Length == SequenceLength;}
    double Compare(Sequence &s);
    double Compare(Sequence &s, unsigned length);
    virtual void Assign(Sequence &s);
    unsigned GetElement(unsigned patternIndex, unsigned elementIndex);
};


class LearnedSequence : public Sequence{
  private:
    unsigned Frequency;
  public:
    LearnedSequence();
    LearnedSequence(unsigned sl, unsigned ic);
    virtual ~LearnedSequence(){};
    virtual void Init(unsigned sl, unsigned ic);
    void IncFrequency(){Frequency++;}
    virtual void Assign(Sequence &s);
    void Assign(LearnedSequence &s);
    bool CanDiscard();
    unsigned GetFrequency(){return Frequency;}
};


#endif
