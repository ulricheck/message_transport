//
// Created by Ulrich Eck on 11/05/16.
// copied from Ubitrack utCore
//

#ifndef MESSAGE_TRANSPORT_TIMESTAMP_H
#define MESSAGE_TRANSPORT_TIMESTAMP_H

#include <string>
#include "message_transport/message_transport_macros.h"

namespace message_transport {

/**
 * Holds execution for the given number of milliseconds.
 * boost::thread::sleep sucks, that's why we are providing our own version.
 *
 * @param ms number of milliseconds to sleep
 * @param ns number of additional nanoseconds to sleep (in case finer-grained sleeping should be necessary)
 */
MSGT_API_DECL void sleep(unsigned ms, unsigned ns = 0);

/** retrieves the high performance counter value */
MSGT_API_DECL long long getHighPerformanceCounter();

/** retrieves the high performance counter frequency in Hz */
MSGT_API_DECL double getHighPerformanceFrequency();

/// Timestamp: nanoseconds since epoch (UNIX birth)
typedef unsigned long long int Timestamp;

/// retrieve the current system time as Timestamp
MSGT_API_DECL Timestamp now();

/// convert a Timestamp to a string (returns something like "Fri Mar 02 11:41:41 2007 UTC")
MSGT_API_DECL std::string timestampToString( Timestamp );

/// convert a Timestamp to a shorter string (returns something like "11:41:41.521021")
MSGT_API_DECL std::string timestampToShortString( Timestamp );

/**
 * Class that handles synchronization between two different clocks.
 *
 * Can be used e.g. to synchronize a sensors's internal clock to the computer's local clock.
 * The sensors native clock is assumed to be precise whereas the local timestamp can have considerable
 * jitter when not using a real-time operating system. The shift and scaling between the clocks is
 * computed online using a kalman filter.
 *
 *  author: Danel Pustka  <daniel.pustka@in.tum.de>
 */
class MSGT_API_DECL TimestampSync
        {
                public:
                /**
                 * Constructor.
                 *
                 * The timestamp synchronization needs to be initialized with approximate values of the local
                 * clock and the native clock resolution. The values need not be correct, but the order of
                 * magnitude should be.
                 *
                 * @param approxNativeFreq frequency resolution of the sensor's native clock.
                 * @param approxLocalFrequency resolution of the local clock.
                 */
                TimestampSync( double approxNativeFreq, double approxLocalFreq = 1e9 )
                : m_events( 0 )
                , m_outlierBudget( -100 ) // no outlier detection the first 100 measurements
                {
                    setFrequency(approxNativeFreq, approxLocalFreq);
                }

                /** Defines the units of native and local. Values need not be precise. */
                void setFrequency( double approxNativeFreq, double approxLocalFreq = 1e9 );

                /**
                 * Add a sensor timestamp and relate it to the current system clock.
                 *
                 * @param native native sensor clock value
                 * @return the sensor time converted to a local time
                 */
                Timestamp convertNativeToLocal( double native )
                {
                    return convertNativeToLocal(native, now());
                }

                /**
                 * Add a sensor timestamp and relate it to the system clock at an arbitrary time.
                 *
                 * @param native native sensor clock value
                 * @param corresponding system clock value
                 * @return the sensor time converted to a local time
                 */
                Timestamp convertNativeToLocal( double native, Timestamp local );

                /**
                 * returns the number of timestamps processed
                 */
                unsigned getEventCount() const
                { return m_events; }

                protected:

                // number of treated events
                unsigned m_events;

                // time of last received native time
                double m_lastNative;

                // process & measurement noise (depends on timer resolution)
                double m_fLocalNoise;
                double m_fGainNoise;
                double m_fMeasurementNoise;

                // currently estimated local time
                Timestamp m_estLocal;

                // currently estimated gain
                double m_estGain;

                // current covariance matrix = { { p1, p2 }, { p2, p3 } }
                double m_p1;
                double m_p2;
                double m_p3;

                // variables for outlier detection
                int m_outlierBudget;
                double m_avgDeviationSquare;
        };

}

#endif //MESSAGE_TRANSPORT_TIMESTAMP_H
