/*
 *  ti-pga2310
 *  v 1.0.0
 *  github.com/kashev/ti-pga2310
 *      
 *      An Arduino / Energia Library for control of a Texas Instruments
 *      PGA2310 Stereo Volume Control.
 *  
 *  Kashev Dalmia - kashev.dalmia@gmail.com
 *
 *  PGA2310.cpp
 */

/*
The MIT License (MIT)

Copyright (c) 2013 Kashev Dalmia

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "PGA4311.h"

PGA4311::PGA4311 (uint8_t pinCS,
                  uint8_t pinSDATA,
                  uint8_t pinSCLK,
                  uint8_t pinZCEN,
                  uint8_t pinMUTE)
: _pinCS     (pinCS),
  _pinSDATA  (pinSDATA),
  _pinSCLK   (pinSCLK),
  _pinZCEN   (pinZCEN),
  _pinMUTE   (pinMUTE),
  _hard_mute (1),
  _muted     (0),
  _v_first    (0),
  _v_second    (0),
  _v_third    (0),
  _v_fourth   (0),
  _pv_first   (0),
  _pv_second   (0),
  _pv_third   (0),
  _pv_fourth  (0)
{
    /* Intentionally Empty */
}

PGA4311::PGA4311 (uint8_t pinCS,
                  uint8_t pinSDATA,
                  uint8_t pinSCLK,
                  uint8_t pinZCEN)
: _pinCS     (pinCS),
  _pinSDATA  (pinSDATA),
  _pinSCLK   (pinSCLK),
  _pinZCEN   (pinZCEN),
  _hard_mute (0),
  _muted     (0),
  _v_first    (0),
  _v_second    (0),
  _v_third    (0),
  _v_fourth   (0),
  _pv_first   (0),
  _pv_second   (0),
  _pv_third   (0),
  _pv_fourth  (0)
{
    /* Intentionally Empty */
}

/* By Default, Enable Zero Crossing Detection */
void
PGA4311::begin (uint8_t zcen_enable)
{
    pinMode(_pinCS,    OUTPUT);
    pinMode(_pinSDATA, OUTPUT);
    pinMode(_pinSCLK,  OUTPUT);

    digitalWrite(_pinCS,    HIGH);
    digitalWrite(_pinSDATA, HIGH);
    digitalWrite(_pinSCLK,  HIGH);

    if (_hard_mute)
    {
        pinMode(_pinMUTE, OUTPUT);
    }

    pinMode(_pinZCEN,  OUTPUT);

    if (zcen_enable)
    {
        digitalWrite(_pinZCEN, HIGH);
    }
    else
    {
        digitalWrite(_pinZCEN, LOW);
    }
}

uint8_t
PGA4311::getFirstVolume (void)
{
    if (_muted)
    {
        return 0;
    }
    else
    {
        return _v_first;
    }
}

uint8_t
PGA4311::getSecondVolume (void)
{
    if (_muted)
    {
        return 0;
    }
    else
    {
        return _v_second;
    }
}

uint8_t
PGA4311::getThirdVolume (void)
{
    if (_muted)
    {
        return 0;
    }
    else
    {
        return _v_third;
    }
}

uint8_t
PGA4311::getFourthVolume (void)
{
    if (_muted)
    {
        return 0;
    }
    else
    {
        return _v_fourth;
    }
}

uint8_t
PGA4311::getVolume (void)
{
    return getFirstVolume();
}

boolean
PGA4311::isMuted (void)
{
    return (boolean)_muted;
}

void
PGA4311::SPIWrite (uint8_t b)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        digitalWrite(_pinSCLK, LOW);
        
        if (0x80 & b)
        {
            digitalWrite(_pinSDATA, HIGH);
        }
        else
        {
            digitalWrite(_pinSDATA, LOW);
        }
        
        digitalWrite(_pinSCLK, HIGH);
        
        b <<= 1;
    }
}

void
PGA4311::setVolume (uint8_t first, uint8_t second, uint8_t third, uint8_t fourth)
{
    if ((first > MAX_GAIN) || (second > MAX_GAIN) || (third > MAX_GAIN) || (fourth > MAX_GAIN))
    {
        return; /* don't allow gains above MAX_GAIN */
    }

    digitalWrite(_pinCS, LOW);
    SPIWrite(fourth);
    SPIWrite(third);
    SPIWrite(second);
    SPIWrite(first);
    digitalWrite(_pinCS, HIGH);

    _pv_first = _v_first; _pv_second = _v_second; _pv_third = _v_third; _pv_fourth = _v_fourth;
    _v_first = first; _v_second = second; _v_third = third; _v_fourth = fourth;

    if (!(_hard_mute) && (_v_first == 0) && (_v_second == 0) && (_v_third == 0) && (_v_fourth == 0))
    {
        _muted = 1;
    }
    else
    {
        _muted = 0;
    }
}

void
PGA4311::setVolume (uint8_t vol)
{
    setVolume(vol, vol, vol, vol);
}

void
PGA4311::restoreVolume (void)
{
    uint8_t t4, t3, t2, t1;

    digitalWrite(_pinCS, LOW);
    SPIWrite(_pv_fourth);
    SPIWrite(_pv_third);
    SPIWrite(_pv_second);
    SPIWrite(_pv_first);
    digitalWrite(_pinCS, HIGH);

    t4 = _v_fourth; t3 = _v_third; t2 = _v_second; t1 = _v_first;

    _v_fourth = _pv_fourth; _v_third = _pv_third; _v_second = _pv_second; _v_first = _pv_first;
    _pv_fourth = t4; _pv_third = t3; _pv_second = t2; _pv_first = t1;
}

void
PGA4311::mute (void)
{
    if (_hard_mute)
    {
        digitalWrite(_pinMUTE, LOW);
    }
    else
    {
        setVolume(0, 0, 0, 0);
    }
    _muted = 1;
}

void
PGA4311::toggleMute (void)
{
    if (_muted)
    {   
        /* unmute */
        if (_hard_mute)
        {
            digitalWrite(_pinMUTE, HIGH);
        }
        else
        {
            restoreVolume();
        }    
        _muted = 0;
    }
    else
    {
        /* mute */
        if (_hard_mute)
        {
            digitalWrite(_pinMUTE, LOW);
        }
        else
        {
            setVolume(0, 0, 0, 0);
        }
        _muted = 1;
    }
}


void
PGA4311::incVolume (void)
{
    incVolume(1);
}

void
PGA4311::decVolume (void)
{
    decVolume(1);
}

void
PGA4311::incVolume (uint8_t step)
{
    int16_t n1, n2, n3, n4;

    if (_muted)
    {
        if (_hard_mute)
        {
            digitalWrite(_pinMUTE, HIGH);
        }
        _muted = 0;
    }

    n1 = _v_first + step; n2 = _v_second + step; n3 = _v_third + step; n4 = _v_fourth + step;

    n1 = n1 > MAX_GAIN ? MAX_GAIN : n1;
    n2 = n2 > MAX_GAIN ? MAX_GAIN : n2;
    n3 = n3 > MAX_GAIN ? MAX_GAIN : n3;
    n4 = n4 > MAX_GAIN ? MAX_GAIN : n4;

    if ((n1 != _v_first) || (n2 != _v_second) || (n3 != _v_third) || (n4 != _v_fourth))
    {
        /* minimize writes to device */
        setVolume((uint8_t)n1, (uint8_t)n2, (uint8_t)n3, (uint8_t)n4);
    }   
}

void
PGA4311::decVolume (uint8_t step)
{
    int16_t n1, n2, n3, n4;

    if (_muted)
    {
        if (_hard_mute)
        {
            digitalWrite(_pinMUTE, HIGH);
        }
        _muted = 0;
    }

    n1 = _v_first - step; n2 = _v_second - step; n3 = _v_third - step; n4 = _v_fourth - step;

    n1 = n1 < 0 ? 0 : n1;
    n2 = n2 < 0 ? 0 : n2;
    n3 = n3 < 0 ? 0 : n3;
    n4 = n4 < 0 ? 0 : n4;

    if ((n1 != _v_first) || (n2 != _v_second) || (n3 != _v_third) || (n4 != _v_fourth))
    {
        /* minimize writes to device */
        setVolume((uint8_t)n1, (uint8_t)n2, (uint8_t)n3, (uint8_t)n4);
    }   
}
