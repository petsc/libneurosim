/*
 *  connection_generator.h
 *
 *  This file is part of libneurosim.
 *
 *  Copyright (C) 2013, 2016 INCF
 *
 *  libneurosim is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  libneurosim is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef CONNECTION_GENERATOR_H
#define CONNECTION_GENERATOR_H

#include <neurosim/connection_generator_V2_0.h>

//#define CONNECTION_GENERATOR_DEBUG 1

#include <vector>
#include <string>

/**
 * Pure abstract base class for connection generators.
 *
 * TODO: Extend with a means of specifying the order of iteration
 * (sources or targets first)
 */
class ConnectionGenerator {
 public:
  class ClosedInterval {
  public:
    ClosedInterval (int _first, int _last) : first (_first), last (_last) { }
    int first;
    int last;
  };

  class IntervalSet {
    std::vector<ClosedInterval> ivals;
    int _skip;
  public:
    IntervalSet (int skip = 1) : ivals (), _skip (skip) { }
    typedef std::vector<ClosedInterval>::iterator iterator;
    iterator begin () { return ivals.begin (); }
    iterator end () { return ivals.end (); }
    int skip () { return _skip; }
    void setSkip (int skip) { _skip = skip; }
    void insert (int first, int last) {
      ivals.push_back (ClosedInterval (first, last));
    }
  };

  class Mask {
  public:
    Mask (int sourceSkip = 1, int targetSkip = 1)
      : sources (sourceSkip), targets (targetSkip) { }
    IntervalSet sources;
    IntervalSet targets;
  };

  virtual ~ConnectionGenerator ();

  /**
   * Return the number of values associated with this generator
   */
  virtual int arity () = 0;

  /**
   * Inform the generator of which source and target indexes exist
   * (must always be called before any of the methods below)
   *
   * skip can be used in round-robin allocation schemes.
   */
  virtual void setMask (Mask& mask);

  /**
   * For a parallel simulator, we want to know the masks for all ranks
   */
  virtual void setMask (std::vector<Mask>& masks, int local) = 0;

  /**
   * Return number of connections represented by this generator
   */
  virtual int size ();

  /**
   * Start an iteration (must be called before first next)
   */
  virtual void start () = 0;

  /**
   * Advance to next connection or return false
   */
  virtual bool next (int& source, int& target, double* value) = 0;

  static void selectCGImplementation (std::string tag, std::string library);

  static ConnectionGenerator* fromXML (std::string xml);

  static ConnectionGenerator* fromXMLFile (std::string fileName);

 private:

  /**
   * Parse an XML file or string and return the first tag's name. This
   * function skips the opening tag (<?xml ...>) and comments (<!--
   * ... -->) up to the first "real" tag and returns the name of
   * that. If there are no tags other than comments and the start tag,
   * the function returns an empty string as a result.
   */
  static std::string ParseXML(std::stringstream& xmlStream);
};

#ifdef CONNECTION_GENERATOR_DEBUG
ConnectionGenerator* makeDummyConnectionGenerator ();
#endif

class ConnectionGeneratorClosure {
public:
  static ConnectionGeneratorClosure* fromXML (std::string xml);
  static ConnectionGeneratorClosure* fromXMLFile (std::string fname);
  virtual ConnectionGenerator* operator() () { return 0; }
  virtual ConnectionGenerator* operator() (double) { return 0; }
  virtual ConnectionGenerator* operator() (double, double) { return 0; }
  virtual ConnectionGenerator* operator() (double, double, double) { return 0; }
  virtual ConnectionGenerator* operator() (double, double, double, double)
  { return 0; }
};

typedef ConnectionGenerator* (*ParseCGFunc) (std::string);
typedef ConnectionGeneratorClosure* (*ParseCGCFunc) (std::string);

void registerConnectionGeneratorLibrary (std::string library,
					 ParseCGFunc pcg,
					 ParseCGFunc pcgFile,
					 ParseCGCFunc pcgc,
					 ParseCGCFunc pcgcFile);

// Local Variables:
// mode:c++
// End:

#endif /* #ifndef CONNECTION_GENERATOR_H */
