//
// Created by Ulrich Eck on 11/05/16.
// copied from Ubitrack utCore
//

#include "message_transport/message_transport_util.h"

#ifdef _MSC_VER
#include <sys/timeb.h>
#include <time.h>

//	#include "TimestampSync.h"

#else
#include <stdio.h>
#include <sys/time.h>
#endif

#ifdef WIN32
#include "message_transport/platform/clean_windows.h"
#else
#include <time.h>
#include <sys/time.h>
#endif

//#define DEBUG_TIMESTAMP_SYNC

#ifdef DEBUG_TIMESTAMP_SYNC
#include <iostream>
#include <iomanip>
#endif

#include <cmath>
#include <algorithm>


namespace message_transport {

namespace {
// parameters to tune for timestamp sync

/** process noise of absolute local time (in seconds/measurement) */
static const double g_fLocalNoise = 1e-3;

/** process noise of gain */
static const double g_fGainNoise = 1e-4;

/** measurement noise (seconds/measurement) */
static const double g_fMeasurementNoise = 3e-2;

/** weight of measurements in outlier detection */
static const double g_fDeviationWeight = 0.01;

} // anonymous namespace

void TimestampSync::setFrequency( double approxNativeFreq, double approxLocalFreq )
{
    m_estGain = approxLocalFreq / approxNativeFreq;

    // compute noise values
    m_fLocalNoise = std::pow( g_fLocalNoise * approxLocalFreq, 2 );
    m_fGainNoise = std::pow( g_fGainNoise * m_estGain, 2 );
    m_fMeasurementNoise = std::pow( g_fMeasurementNoise * approxLocalFreq, 2 );

    // compute initial error
    m_p1 = std::pow( approxLocalFreq, 2 );
    m_p2 = 0.0;
    m_p3 = std::pow( 1e1 * m_estGain, 2 );

    m_avgDeviationSquare = std::pow( 0.001 * approxLocalFreq, 2 );
}


Timestamp TimestampSync::convertNativeToLocal( double native, Timestamp local )
{
    // initialization on first event;
    if ( m_events == 0 )
    {
        m_lastNative = native;
        m_estLocal = local;
    }

    // time update
    double deltaN = native - m_lastNative;
    m_estLocal += static_cast< long long >( deltaN * m_estGain );
    m_p1 += 2.0 * deltaN * m_p2 + deltaN * deltaN * m_p3 + m_fLocalNoise;
    m_p2 += deltaN * m_p3;
    m_p3 += m_fGainNoise;

    // outlier detection
    double deltaL = double( static_cast< long long >( local - m_estLocal ) );
    double deviationSquare = deltaL * deltaL;
    double thresholdSquare = m_avgDeviationSquare * 9; // 3 sigma = 99.7% confidence (chebychev: 89% if not gaussian)

    if ( deviationSquare < thresholdSquare || m_outlierBudget < 0 )
    {
        // measurement update
        double k1 = m_p1 / ( m_fMeasurementNoise + m_p1 );
        double k2 = m_p2 / ( m_fMeasurementNoise + m_p1 );

        m_estGain += k2 * deltaL;
        m_estLocal += static_cast< long long >( k1 * deltaL );

        m_p1 = m_fMeasurementNoise * k1;
        m_p3 -= m_p2 * k2;
        m_p2 = m_fMeasurementNoise * k2;

        if ( m_events > 50 )
            m_avgDeviationSquare += ( deviationSquare - m_avgDeviationSquare ) * g_fDeviationWeight;

        m_outlierBudget = std::min( 40, m_outlierBudget + 1 );
    }
    else
        m_outlierBudget -= 2;

#ifdef DEBUG_TIMESTAMP_SYNC
    if ( native < 1e9  )
		std::cerr << std::setprecision( 15 ) << native << " " << local / 1000 << " " <<
			static_cast< long long >( local - m_estLocal ) / 1000 << " " <<
			m_estGain << " " << m_estLocal / 1000 << " " <<
			std::sqrt( m_avgDeviationSquare ) * 3 / 1000 << std::endl;
#endif

    m_lastNative = native;
    m_events++;

    return m_estLocal;
}


#ifdef _WIN32

void sleep( unsigned ms, unsigned )
{
    Sleep( ms );
}


long long getHighPerformanceCounter()
{
    LARGE_INTEGER counter;
    QueryPerformanceCounter( &counter );
    return counter.QuadPart;
}


double getHighPerformanceFrequency()
{
    LARGE_INTEGER counter;
    QueryPerformanceFrequency( &counter );
    return static_cast< double >( counter.QuadPart );
}

#else // unix

void sleep(unsigned ms, unsigned ns)
{
    timespec t;
    t.tv_sec = ms/1000;
    t.tv_nsec = (ms%1000)*1000000+ns;
    nanosleep(&t, NULL);
}

long long getHighPerformanceCounter()
{
    // TODO: read CPU counter
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec*static_cast< long long >( 1000000 )+tv.tv_usec;
}

double getHighPerformanceFrequency()
{
    return 1000000.0;
}

#endif





Timestamp now()
{
#ifdef _MSC_VER

    // Windows time has bad resolution (10ms), therefore we prefer to use the High Performance Counter
    // and synchronize it to the real time clock

    // read HPC
    long long hiPerf = Util::getHighPerformanceCounter();

    // read RTC
    struct _timeb wintime;
#if _MSC_VER < 1400
        _ftime( &wintime );
#else
        _ftime_s( &wintime );
#endif
    Timestamp rtc = Timestamp( wintime.time ) * 1000000000 + Timestamp( wintime.millitm ) * 1000000;

    // synchronization
    static TimestampSync synchronizer( 1e9 );
    static bool bUseHpc = true;
    static double lastHpcFreq;

    if ( bUseHpc )
    {
        // check if HPC is any good (has constant frequency)
        double hpcFreq = Util::getHighPerformanceFrequency();
        if ( synchronizer.getEventCount() > 0 )
            if ( hpcFreq != lastHpcFreq )
            {
                bUseHpc = false;
                LOG4CPP_WARN( logger, "Your CPU frequency is not constant (power save mode?). Timestamps will be unprecise." );
                return rtc;
            }
            else
            {}
        else
        {
            // initialization
            synchronizer.setFrequency( hpcFreq );
            lastHpcFreq = hpcFreq;
        }

        // convert hpc to rtc time
        return synchronizer.convertNativeToLocal( double( hiPerf ), rtc );
        //return Timestamp( hiPerf / hpcFreq * 1e9 );
    }
    else
        return rtc;

#else

    // Unix time
    struct timeval tv;
    gettimeofday(&tv, 0);
    return ((Timestamp)(tv.tv_sec))*1000000000+((Timestamp)(tv.tv_usec))*1000;

#endif
}

std::string timestampToString(Timestamp t)
{
    std::string result;
    time_t mytime = (time_t)(t/1000000000);
#if !defined( _MSC_VER ) || _MSC_VER>=1400
    char buffer[50]; // asctime requires at least 26 bytes
    struct tm temp;
#endif

#ifdef _MSC_VER
#if _MSC_VER >= 1400
    gmtime_s( &temp, &mytime );
    asctime_s( buffer, sizeof(buffer), &temp );
    result = buffer;
#else
    result = asctime( gmtime( &mytime ) );
#endif
#else
    gmtime_r(&mytime, &temp);
    asctime_r(&temp, buffer);
    result = buffer;
#endif

    result.replace(result.length()-1, 4, " UTC");
    return result;
}

std::string timestampToShortString(Timestamp t)
{
    time_t mytime = (time_t)(t/1000000000);
    struct tm temp;
    struct tm* pTm = &temp;

#ifdef _MSC_VER
#if _MSC_VER >= 1400
    localtime_s( &temp, &mytime );
#else
    pTm = localtime( &mytime );
#endif
#else
    localtime_r(&mytime, &temp);
#endif

    char buffer[32];
    sprintf(buffer, "%02d:%02d:%02d.%06d", pTm->tm_hour, pTm->tm_min, pTm->tm_sec, int((t/1000)%1000000));
    return std::string(buffer);
}

}
