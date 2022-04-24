/*
 * TimerId.h
 *
 *  Created on: 2022Äê4ÔÂ24ÈÕ
 *      Author: daniel
 */

#ifndef H4D3BAE1F_744E_4DD5_B495_0AEC8A90CBCB
#define H4D3BAE1F_744E_4DD5_B495_0AEC8A90CBCB

class Timer;

///
/// An opaque identifier, for canceling Timer.
///
class TimerId
{
 public:
  explicit TimerId(Timer* timer)
    : value_(timer)
  {
  }

  // default copy-ctor, dtor and assignment are okay

 private:
  Timer* value_;
};



#endif /* H4D3BAE1F_744E_4DD5_B495_0AEC8A90CBCB */
