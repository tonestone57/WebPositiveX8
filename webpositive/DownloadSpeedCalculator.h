/*
 * Copyright (C) 2010 Stephan Aßmus <superstippi@gmx.de>
 *
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef DOWNLOAD_SPEED_CALCULATOR_H
#define DOWNLOAD_SPEED_CALCULATOR_H


#include <SupportDefs.h>
#include <stddef.h>


class DownloadSpeedCalculator {
public:
								DownloadSpeedCalculator();

			void				Reset(off_t initialSize,
									bigtime_t startTime);

			void				Update(off_t currentSize,
									bigtime_t currentTime);

			double				CurrentSpeed() const;
			double				AverageSpeed(off_t currentSize,
									bigtime_t currentTime) const;

private:
	static	const size_t		kBytesPerSecondSlots = 10;
	static	const bigtime_t		kSpeedReferenceInterval = 500000LL;

			off_t				fLastSpeedReferenceSize;
			bigtime_t			fLastSpeedReferenceTime;

			off_t				fEstimatedFinishReferenceSize;
			bigtime_t			fEstimatedFinishReferenceTime;

			size_t				fCurrentBytesPerSecondSlot;
			double				fBytesPerSecondSlot[kBytesPerSecondSlots];
			double				fBytesPerSecond;
};


#endif // DOWNLOAD_SPEED_CALCULATOR_H
