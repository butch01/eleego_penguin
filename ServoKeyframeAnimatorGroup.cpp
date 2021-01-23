/*
 * ServoKeyframeAnimatorGroup.cpp
 *
 *  Created on: 19.01.2021
 *      Author: butch
 */

#include "ServoKeyframeAnimatorGroup.h"
#include <arduino.h>
#include "globalDefines.h"
#include <ArduinoLog.h>


ServoKeyframeAnimatorGroup::ServoKeyframeAnimatorGroup(unsigned char numberOfServos)
{
	ServoKeyframeAnimator* _keyframeAnimators=NULL;
	_keyframeAnimators = new ServoKeyframeAnimator[numberOfServos];

	// _numbeOfServos=numberOfServos;
	_timePreviousKeyframe=0;
	_isInMove=false;
	_duration=0;

//	Log.trace(F("ServoKeyframeAnimatorGroup::ServoKeyframeAnimatorGroup (numberOfServos=%d) - _timePreviousKeyframe=%l _isInMove=%T _duration=%d" CR), numberOfServos,_timePreviousKeyframe, _isInMove, _duration);
}

/**
 * returns the number of servos
 */
unsigned char ServoKeyframeAnimatorGroup::getNumberOfServos()
{
	return sizeof _keyframeAnimators ;
}


/**
 * this returns a child ServoKeyframeAnimator. ID is the position in the array
 */
ServoKeyframeAnimator ServoKeyframeAnimatorGroup::getServoKeyframeAnimator (unsigned char id)
{
	return _keyframeAnimators[id];
}


ServoKeyframeAnimatorGroup::~ServoKeyframeAnimatorGroup() {
	// TODO Auto-generated destructor stub
}

/**
 * get complete time which is targeted for the complete move.
 * It is the time between previous and next keyframe
 */

unsigned int ServoKeyframeAnimatorGroup::getMoveDuration()
{
	return _duration;
}

/**
 * sets the time / duration which is used to move the servo from previous keyframe to target keyframe
 */
void ServoKeyframeAnimatorGroup::setServoMoveDuration(unsigned int duration)
{
	_duration = duration;
}


/**
 * calculate which position all servos of the groups should have now. Update the current servo position.
 * this does not move the servo. This needs to be done externally. Here is only the calculation.
 */
void ServoKeyframeAnimatorGroup::calculateServoPositions()
{
	if (!_isInMove)
	{
		// not in move jet, so lets start it
		_isInMove=true;
		// remember the time when the move starts for later calucation
		_timePreviousKeyframe=millis();

		#if DEBUG_SERVO_KEYFRAME_ANIMATOR_GROUP_CALCULATE_SERVO_POSITIONS == 1
			Log.trace(F("ServoKeyframeAnimatorGroup::calculateServoPositions : set _isInMove=%T at _timePreviousKeyframe=%l"CR), _isInMove, _timePreviousKeyframe );
		#endif

	}
	unsigned long currentTime = millis();


	// check if time is reached
	if (currentTime > _timePreviousKeyframe + _duration)
	{
		// time is reached -> end the move and set servo to final position
		_isInMove=false;

		#if DEBUG_SERVO_KEYFRAME_ANIMATOR_GROUP_CALCULATE_SERVO_POSITIONS == 1
			Log.trace(F("ServoKeyframeAnimatorGroup::calculateServoPositions : time is up (%d > %d + %d = %d, set _isInMove=%T, setting servos to final position"CR), currentTime, _timePreviousKeyframe, _duration, _timePreviousKeyframe + _duration,  _isInMove);
		#endif

		for (unsigned char s=0; s < sizeof (_keyframeAnimators); s++)
		{
			_keyframeAnimators[s].setServoAbsolutePosition(_keyframeAnimators[s].getServoTargetPositon());
		}
	}
	else
	{
		#if DEBUG_SERVO_KEYFRAME_ANIMATOR_GROUP_CALCULATE_SERVO_POSITIONS == 1
			Log.trace(F("ServoKeyframeAnimatorGroup::calculateServoPositions : time is NOT up calculating positions."CR));
		#endif

		// time is not up, we are within the move. calculate the new servo position and set it as current position.
		for (unsigned char s=0; s < sizeof _keyframeAnimators; s++)
		{
			_keyframeAnimators[s].setServoAbsolutePosition(_keyframeAnimators[s].getCalculatedServoPosition(_timePreviousKeyframe, _duration));
		}
	}
}

/**
 * get the servo calculated position for a _specific_ servo.
 * calculation needs to be done before.
 */
unsigned char ServoKeyframeAnimatorGroup::getCalculatedServoPositionById (unsigned char id)
{
	return _keyframeAnimators[id].getServoCurrentPositon();
}


/**
 * sets the target position for next keyframe for a _specific_ servo identified by id
 */
void ServoKeyframeAnimatorGroup::setServoPositionNextKeyframeById (unsigned char id, unsigned char targetPosition)
{
	_keyframeAnimators[id].setServoPositionNextKeyframe(targetPosition);
}


/**
 * sets the target position for next keyframe for _all_ servos, given as array
 */
void ServoKeyframeAnimatorGroup::setServoPositionsNextKeyframe (unsigned char* targetPositions)
{
	for (unsigned char s=0; s < sizeof targetPositions; s++)
	{
		_keyframeAnimators[s].setServoPositionNextKeyframe(targetPositions[s]);
	}
}


/**
 *
 * returns if we are within a move
 */
bool ServoKeyframeAnimatorGroup::isInMove()
{
	return _isInMove;
}