/*
 * Timer.h
 *
 *  Created on: 2022Äê4ÔÂ24ÈÕ
 *      Author: daniel
 */

#ifndef H0E7930A5_A7CD_454E_8746_AE881AA73619
#define H0E7930A5_A7CD_454E_8746_AE881AA73619

#include "Noncopyable.h"
#include "Callbacks.h"
#include "Timestamp.h"

class Timer : Noncopyable
{
 public:
  Timer(const TimerCallback& cb, Timestamp when, double interval)
    : callback_(cb),
      expiration_(when),
      interval_(interval),
      repeat_(interval > 0.0)
  { }

  void run() const
  {
    callback_();
  }

  Timestamp expiration() const  { return expiration_; }
  bool repeat() const { return repeat_; }

  void restart(Timestamp now);

 private:
  const TimerCallback callback_;
  Timestamp expiration_;
  const double interval_;
  const bool repeat_;
};


#endif /* H0E7930A5_A7CD_454E_8746_AE881AA73619 */
