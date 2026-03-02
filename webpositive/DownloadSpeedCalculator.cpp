/*
 * Copyright (C) 2010 Stephan Aßmus <superstippi@gmx.de>
 *
 * All rights reserved. Distributed under the terms of the MIT License.
 */


#include "BeOSCompatibility.h"
#include "DownloadSpeedCalculator.h"


DownloadSpeedCalculator::DownloadSpeedCalculator()
{
	Reset(0, 0);
}


void
DownloadSpeedCalculator::Reset(off_t initialSize, bigtime_t startTime)
{
	fLastSpeedReferenceSize = initialSize;
	fLastSpeedReferenceTime = startTime;
	fEstimatedFinishReferenceSize = initialSize;
	fEstimatedFinishReferenceTime = startTime;
	fCurrentBytesPerSecondSlot = 0;
	fBytesPerSecond = 0.0;
	for (size_t i = 0; i < kBytesPerSecondSlots; i++)
		fBytesPerSecondSlot[i] = 0.0;
}


void
DownloadSpeedCalculator::Update(off_t currentSize, bigtime_t currentTime)
{
	if (currentTime >= fLastSpeedReferenceTime + kSpeedReferenceInterval) {
		// update current speed every kSpeedReferenceInterval
		fCurrentBytesPerSecondSlot
			= (fCurrentBytesPerSecondSlot + 1) % kBytesPerSecondSlots;
		fBytesPerSecondSlot[fCurrentBytesPerSecondSlot]
			= (double)(currentSize - fLastSpeedReferenceSize)
				* 1000000LL / (currentTime - fLastSpeedReferenceTime);
		fLastSpeedReferenceSize = currentSize;
		fLastSpeedReferenceTime = currentTime;
		fBytesPerSecond = 0.0;
		size_t count = 0;
		for (size_t i = 0; i < kBytesPerSecondSlots; i++) {
			if (fBytesPerSecondSlot[i] != 0.0) {
				fBytesPerSecond += fBytesPerSecondSlot[i];
				count++;
			}
		}
		if (count > 0)
			fBytesPerSecond /= count;
	}
}


double
DownloadSpeedCalculator::CurrentSpeed() const
{
	return fBytesPerSecond;
}


double
DownloadSpeedCalculator::AverageSpeed(off_t currentSize, bigtime_t currentTime) const
{
	if (currentTime <= fEstimatedFinishReferenceTime)
		return 0.0;

	return (double)(currentSize - fEstimatedFinishReferenceSize)
		* 1000000LL / (currentTime - fEstimatedFinishReferenceTime);
}
